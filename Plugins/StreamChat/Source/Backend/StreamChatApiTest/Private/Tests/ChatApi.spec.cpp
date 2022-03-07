// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "ConstantTokenProvider.h"
#include "Dom/JsonObject.h"
#include "IChatSocket.h"
#include "Misc/AutomationTest.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "TokenManager.h"
#include "User/Jwt.h"
#include "User/User.h"

BEGIN_DEFINE_SPEC(FChatApiSpec, "StreamChat.ChatApi", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
const FString ApiKey = TEXT("kmajgxb2rk4p");
const FString Host = TEXT("chat.stream-io-api.com");
const FUserObjectDto User{{TEXT("TestUser")}};
const FString ChannelId = TEXT("test-channel");
const FString ChannelType = TEXT("messaging");
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
                        [=](const FChannelStateResponseDto& Dto)
                        {
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, TEXT("messaging:test-channel"));
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, ChannelId);
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            TestDone.Execute();
                        },
                        ChannelType,
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        {},
                        ChannelId);
                });

            LatentIt(
                "should find one channel",
                [=](const FDoneDelegate& TestDone)
                {
                    const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                    Filter->SetStringField(TEXT("id"), ChannelId);
                    Api->QueryChannels(
                        [=](const FChannelsResponseDto& Dto)
                        {
                            TestEqual("One channel in response", Dto.Channels.Num(), 1);
                            TestDone.Execute();
                        },
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        Filter);
                });

            // AfterEach are executed in reverse order oft ad declaration...

            // Check channel deleted
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                    Filter->SetStringField(TEXT("id"), ChannelId);
                    Api->QueryChannels(
                        [=](const FChannelsResponseDto& Dto)
                        {
                            TestEqual("No channels in response", Dto.Channels.Num(), 0);
                            TestDone.Execute();
                        },
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        Filter);
                });

            // Delete channel
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->DeleteChannel(
                        [=](const FDeleteChannelResponseDto& Dto)
                        {
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, TEXT("messaging:test-channel"));
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, TEXT("test-channel"));
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        },
                        ChannelType,
                        ChannelId);
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
                        [=](const FUsersResponseDto& Dto)
                        {
                            TestTrue("Users returned", Dto.Users.Num() > 0);
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
                        },
                        Socket->GetConnectionId(),
                        false);
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
                    const FUserObjectRequestDto GuestUserDto{{TEXT("test-guest-user")}};
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

    AfterEach([=] { Socket->Disconnect(); });
}
