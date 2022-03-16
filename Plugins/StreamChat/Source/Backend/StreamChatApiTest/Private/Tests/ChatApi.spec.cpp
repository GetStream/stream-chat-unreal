// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "Channel/Filter.h"
#include "ConstantTokenProvider.h"
#include "Dom/JsonObject.h"
#include "IChatSocket.h"
#include "Misc/AutomationTest.h"
#include "PushProvider.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Device/ListDevicesResponseDto.h"
#include "Response/Moderation/BanResponseDto.h"
#include "Response/Moderation/QueryBannedUsersResponseDto.h"
#include "Response/ResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "TokenManager.h"
#include "User/Jwt.h"
#include "User/User.h"

BEGIN_DEFINE_SPEC(FChatApiSpec, "StreamChat.ChatApi", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
const FString ApiKey = TEXT("kmajgxb2rk4p");
const FString Host = TEXT("chat.stream-io-api.com");
const FUserObjectDto User{FUserDto{TEXT("TestUser")}};
const FString ChannelType = TEXT("messaging");
const FString NewChannelId = TEXT("test-channel");
const FString NewCid = FString::Printf(TEXT("%s:%s"), *ChannelType, *NewChannelId);
const FString ChannelId = TEXT("unrealdevs");
const FString Cid = FString::Printf(TEXT("%s:%s"), *ChannelType, *ChannelId);
const FString DeviceId = TEXT("random-device-id");
const FString BanUserId = TEXT("tutorial-unreal");
const TSharedRef<FTokenManager> TokenManager = MakeShared<FTokenManager>();
const TSharedRef<FChatApi> Api = FChatApi::Create(ApiKey, Host, TokenManager);
TSharedPtr<IChatSocket> Socket;
END_DEFINE_SPEC(FChatApiSpec)

void FChatApiSpec::Define()
{
    const FToken Token = FToken::Jwt(Jwt::Development(User.Id));
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token), User.Id);

    // Connect WebSocket
    LatentBeforeEach(
        [=](const FDoneDelegate& TestDone)
        {
            Socket = IChatSocket::Create(TokenManager, ApiKey, Host, User);
            Socket->Connect([=](const FOwnUserDto&) { TestDone.Execute(); });
        });

    Describe(
        "Channel",
        [=]
        {
            // Create channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->QueryChannel(
                        ChannelType,
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        {},
                        NewChannelId,
                        {},
                        {},
                        {},
                        [=](const FChannelStateResponseDto& Dto)
                        {
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "should find one channel",
                [=](const FDoneDelegate& TestDone)
                {
                    const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                    Filter->SetStringField(TEXT("id"), NewChannelId);
                    Api->QueryChannels(
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        Filter,
                        {},
                        {},
                        {},
                        {},
                        {},
                        [=](const FChannelsResponseDto& Dto)
                        {
                            TestEqual("One channel in response", Dto.Channels.Num(), 1);
                            TestDone.Execute();
                        });
                });

            // AfterEach are executed in reverse order of declaration...

            // Check channel deleted
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                    Filter->SetStringField(TEXT("id"), NewChannelId);
                    Api->QueryChannels(
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        Filter,
                        {},
                        {},
                        {},
                        {},
                        {},
                        [=](const FChannelsResponseDto& Dto)
                        {
                            TestEqual("No channels in response", Dto.Channels.Num(), 0);
                            TestDone.Execute();
                        });
                });

            // Delete channel
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->DeleteChannel(
                        ChannelType,
                        NewChannelId,
                        [=](const FDeleteChannelResponseDto& Dto)
                        {
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Users",
        [=]
        {
            LatentIt(
                "should return some known users",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->QueryUsers(
                        Socket->GetConnectionId(),
                        false,
                        {},
                        {},
                        100,
                        {},
                        [=](const FUsersResponseDto& Dto)
                        {
                            TestTrue("Users returned", Dto.Users.Num() > 0);
                            AddInfo(FString::Printf(TEXT("%s"), *Dto.Users[0].Id));
                            const FUserResponseDto* FoundUser =
                                Dto.Users.FindByPredicate([=](const FUserResponseDto& UserDto) { return UserDto.Id == User.Id; });
                            TestNotNull("User found", FoundUser);
                            if (FoundUser)
                            {
                                TestTrue("Online", FoundUser->bOnline);
                                TestEqual("Role", FoundUser->Role, TEXT("user"));
                                TestEqual("No additional fields", FoundUser->AdditionalFields.GetFields().Num(), 0);
                            }
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Guest",
        [=]
        {
            LatentIt(
                "should create guest user",
                [=](const FDoneDelegate& TestDone)
                {
                    const FUserObjectRequestDto GuestUserDto{FUserDto{TEXT("test-guest-user")}};
                    Api->CreateGuest(
                        GuestUserDto,
                        [=](const FGuestResponseDto& Dto)
                        {
                            // Check for "guest-{uuid}-test-guest-user
                            const FRegexPattern Pattern{TEXT("guest-[0-9a-f]{8}-[0-9a-f]{4}-[0-5][0-9a-f]{3}-[089ab][0-9a-f]{3}-[0-9a-f]{12}-test-guest-user")};
                            FRegexMatcher Regex{Pattern, Dto.User.Id};
                            TestTrue("Guest generated", Regex.FindNext());
                            TestFalse("Online", Dto.User.bOnline);
                            TestEqual("Role", Dto.User.Role, TEXT("guest"));
                            TestEqual("No additional fields", Dto.User.AdditionalFields.GetFields().Num(), 0);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Device",
        [=]
        {
            LatentIt(
                "should add device",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->AddDevice(
                        DeviceId,
                        EPushProvider::Firebase,
                        [=](const FResponseDto& Dto)
                        {
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // Remove device
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->RemoveDevice(
                        DeviceId,
                        [=](const FResponseDto& Dto)
                        {
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // List devices
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->ListDevices(
                        [=](const FListDevicesResponseDto& Dto)
                        {
                            TestEqual("Device exists", Dto.Devices[0].Id, DeviceId);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Ban",
        [=]
        {
            // Ban user
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->BanUser(
                        BanUserId,
                        ChannelType,
                        ChannelId,
                        {},
                        {},
                        {},
                        {},
                        [=](const FResponseDto& Dto)
                        {
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // User was banned
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->QueryBannedUsers(
                        FFilter::Equal(TEXT("channel_cid"), Cid).ToJsonObject(),
                        {},
                        {},
                        {},
                        {},
                        {},
                        {},
                        {},
                        [=](const FQueryBannedUsersResponseDto& Dto)
                        {
                            const FBanResponseDto* Ban = Dto.Bans.FindByPredicate([&](const FBanResponseDto& B) { return B.User.Id == BanUserId; });
                            TestNotNull("User was banned", Ban);
                            TestEqual("Banned in channel", Ban->Channel.Cid, Cid);
                            // TestTrue("Banned recently", FDateTime::UtcNow() - Ban->CreatedAt < FTimespan::FromMinutes(1.f));
                            TestDone.Execute();
                        });
                });

            // Unban user
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->UnbanUser(
                        BanUserId,
                        ChannelType,
                        ChannelId,
                        [=](const FResponseDto& Dto)
                        {
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // User was unbanned
            LatentIt(
                "should ban user",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->QueryBannedUsers(
                        FFilter::Equal(TEXT("channel_cid"), Cid).ToJsonObject(),
                        {},
                        {},
                        {},
                        {},
                        {},
                        {},
                        {},
                        [=](const FQueryBannedUsersResponseDto& Dto)
                        {
                            const FBanResponseDto* Ban = Dto.Bans.FindByPredicate([&](const FBanResponseDto& B) { return B.User.Id == BanUserId; });
                            TestNull("User was unbanned", Ban);
                            TestDone.Execute();
                        });
                });
        });

    AfterEach([=] { Socket->Disconnect(); });
}
