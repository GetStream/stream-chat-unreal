// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "Channel/Filter.h"
#include "ConstantTokenProvider.h"
#include "Dom/JsonObject.h"
#include "IChatSocket.h"
#include "Misc/AutomationTest.h"
#include "PushProvider.h"
#include "Request/Channel/UpdateChannelRequestDto.h"
#include "Request/Message/MessageRequestDto.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Channel/UpdateChannelPartialResponseDto.h"
#include "Response/Channel/UpdateChannelResponseDto.h"
#include "Response/Device/ListDevicesResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Moderation/BanResponseDto.h"
#include "Response/Moderation/FlagResponseDto.h"
#include "Response/Moderation/MuteChannelResponseDto.h"
#include "Response/Moderation/MuteUserResponseDto.h"
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
const FString ChannelId = TEXT("unrealdevs");
const FString Cid = FString::Printf(TEXT("%s:%s"), *ChannelType, *ChannelId);
const FString NewChannelId = TEXT("test-channel");
const FString NewCid = FString::Printf(TEXT("%s:%s"), *ChannelType, *NewChannelId);
const FString DeviceId = TEXT("random-device-id");
const FString BanUserId = TEXT("tutorial-unreal");
const FString MsgText{TEXT("My test message!")};
FString MessageId;

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
                            TestFalse("Not hidden", Dto.Channel.bHidden);
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
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

            // Mute channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->MuteChannel(
                        {NewCid},
                        {},
                        [=](const FMuteChannelResponseDto& Dto)
                        {
                            TestEqual("Correct channel muted", Dto.ChannelMute.Channel.Cid, NewCid);
                            TestEqual("No channel mute expiry", Dto.ChannelMute.Expires.GetTicks(), 0);
                            TestTrue(
                                "Channel muted",
                                Dto.OwnUser.ChannelMutes.ContainsByPredicate([&](const FChannelMuteDto& M) { return M.Channel.Cid == NewCid; }));
                            TestDone.Execute();
                        });
                });

            // Unmute channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->UnmuteChannel(
                        {NewCid},
                        [=](const FResponseDto& Dto)
                        {
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Partial update channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    const FString NewName = TEXT("New channel name");
                    const TSharedRef<FJsonObject> Set = MakeShared<FJsonObject>();
                    Set->SetStringField(TEXT("name"), NewName);
                    Api->PartialUpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Set,
                        {},
                        [=](const FUpdateChannelPartialResponseDto& Dto)
                        {
                            TestEqual("Channel name partial updated", Dto.Channel.AdditionalFields.GetString(TEXT("name")), {NewName});
                            TestDone.Execute();
                        });
                });

            // Update channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    const FString NewName = TEXT("MY CHANNEL");
                    FUpdateChannelRequestDto Data;
                    Data.Data.AdditionalFields.SetString(TEXT("name"), NewName);
                    Data.Data.AdditionalFields.SetString(TEXT("random"), NewName);
                    Api->UpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Data,
                        [=](const FUpdateChannelResponseDto& Dto)
                        {
                            const FString ResponseName = Dto.Channel.AdditionalFields.GetString(TEXT("name")).GetValue();
                            AddInfo(ResponseName);
                            TestEqual("Name updated", ResponseName, NewName);

                            TestDone.Execute();
                        });
                });

            // Add members
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    FUpdateChannelRequestDto Data;
                    FChannelMemberRequestDto MemberRequestDto;
                    MemberRequestDto.bIsModerator = true;
                    MemberRequestDto.User.Id = User.Id;
                    Data.AddMembers.Add(MemberRequestDto);
                    Api->UpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Data,
                        [=](const FUpdateChannelResponseDto& Dto)
                        {
                            const FChannelMemberDto* Found = Dto.Members.FindByPredicate([&](const FChannelMemberDto& A) { return A.UserId == User.Id; });
                            TestNotNull("User added", Found);
                            TestTrue("User is moderator", Found->bIsModerator);
                            TestTrue("No message", Dto.Message.Id.IsEmpty());
                            AddInfo(FString::FromInt(Dto.Channel.Cooldown));
                            TestEqual("No cooldown", Dto.Channel.Cooldown, TNumericLimits<uint32>::Max());
                            TestDone.Execute();
                        });
                });

            // Hide channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->HideChannel(
                        ChannelType,
                        NewChannelId,
                        false,
                        [=](const FResponseDto& Dto)
                        {
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Show channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->ShowChannel(
                        ChannelType,
                        NewChannelId,
                        [=](const FResponseDto& Dto)
                        {
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Remove members
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    FUpdateChannelRequestDto Data;
                    FMessageRequestDto MessageRequest;
                    MessageRequest.Cid = Cid;
                    MessageRequest.Text = MsgText;
                    Data.SetMessage(MessageRequest);
                    Data.RemoveMembers.Add(User.Id);
                    Api->UpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Data,
                        [=](const FUpdateChannelResponseDto& Dto)
                        {
                            const FChannelMemberDto* Found = Dto.Members.FindByPredicate([&](const FChannelMemberDto& A) { return A.UserId == User.Id; });
                            TestNull("User removed", Found);
                            TestEqual("Message sent", Dto.Message.Text, MsgText);
                            TestDone.Execute();
                        });
                });

            // Delete channel
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->DeleteChannel(
                        ChannelType,
                        NewChannelId,
                        [=](const FDeleteChannelResponseDto& Dto)
                        {
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Check channel deleted
            LatentIt(
                "should have deleted test channel",
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
        "Mute",
        [=]
        {
            // Mute user
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->MuteUser(
                        {BanUserId},
                        {},
                        [=](const FMuteUserResponseDto& Dto)
                        {
                            TestEqual("User muted", Dto.Mute.Target.Id, BanUserId);
                            /*
                            TestTrue(
                                "User muted in own user",
                                Dto.OwnUser.Mutes.ContainsByPredicate([&](const FUserMuteDto& Mute) { return Mute.Target.Id == BanUserId; }));
                            */
                            TestDone.Execute();
                        });
                });

            // Unmute user
            LatentIt(
                "should unmute user",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->UnmuteUser(
                        {BanUserId},
                        [=](const FResponseDto& Dto)
                        {
                            TestTrue("Response received", Dto.Duration.Len() > 0);
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

    Describe(
        "Flag",
        [=]
        {
            // Create message
            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Cid = Cid;
                    Request.Text = MsgText;
                    Api->SendNewMessage(
                        ChannelType,
                        ChannelId,
                        Request,
                        false,
                        [=](const FMessageResponseDto& Dto)
                        {
                            TestEqual("Message text is same as input", Dto.Message.Text, MsgText);
                            MessageId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "Flag message",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->Flag(
                        MessageId,
                        {},
                        [=](const FFlagResponseDto& Dto)
                        {
                            TestEqual("Message ID matches query", Dto.Flag.TargetMessageId, MessageId);
                            TestDone.Execute();
                        });
                });

            // Delete message
            LatentAfterEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Api->DeleteMessage(
                        MessageId,
                        true,
                        [=](const FMessageResponseDto& Dto)
                        {
                            TestEqual("Message text is same as input", Dto.Message.Text, MsgText);
                            TestEqual("Message is deleted", Dto.Message.Type, EMessageTypeDto::Deleted);
                            TestDone.Execute();
                        });
                });
        });

    AfterEach([=] { Socket->Disconnect(); });
}
