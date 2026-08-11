// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "Channel/Filter.h"
#include "ConstantTokenProvider.h"
#include "Dom/JsonObject.h"
#include "IChatSocket.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "PushProvider.h"
#include "Request/Channel/UpdateChannelRequestDto.h"
#include "Request/Message/MessageRequestDto.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Channel/MembersResponseDto.h"
#include "Response/Channel/TruncateChannelResponseDto.h"
#include "Response/Channel/UpdateChannelPartialResponseDto.h"
#include "Response/Channel/UpdateChannelResponseDto.h"
#include "Response/Device/ListDevicesResponseDto.h"
#include "Response/Message/GetRepliesResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Moderation/BanResponseDto.h"
#include "Response/Moderation/BlockUserResponseDto.h"
#include "Response/Moderation/FlagResponseDto.h"
#include "Response/Moderation/GetBlockedUsersResponseDto.h"
#include "Response/Moderation/MuteChannelResponseDto.h"
#include "Response/Moderation/MuteUserResponseDto.h"
#include "Response/Moderation/QueryBannedUsersResponseDto.h"
#include "Response/ResponseDto.h"
#include "Response/Thread/GetThreadResponseDto.h"
#include "Response/Thread/QueryThreadsResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UpdateUsersResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "TokenManager.h"
#include "User/Jwt.h"
#include "User/User.h"

BEGIN_DEFINE_SPEC(FChatApiSpec, "StreamChat.ChatApi", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
const FString ApiKey = TEXT("kmajgxb2rk4p");
const FString Host = TEXT("chat.stream-io-api.com");
const FUserObjectDto User{FUserDto{TEXT("TestUser")}};
const FString ChannelType = TEXT("messaging");
const FString ChannelId = TEXT("unrealdevs");
const FString Cid = FString::Printf(TEXT("%s:%s"), *ChannelType, *ChannelId);
const FString NewChannelId = TEXT("test-channel_" + FGuid::NewGuid().ToString());
const FString NewCid = FString::Printf(TEXT("%s:%s"), *ChannelType, *NewChannelId);
const FString DeviceId = TEXT("random-device-id");
const FString BanUserId = TEXT("tutorial-unreal");
const FString MsgText{TEXT("My test message!")};
const FString UpdatedMsgText{TEXT("My new message")};
const FString ReplyText{TEXT("My test reply!")};
FString MessageId;
FString GuestUserId;
FString ThreadParentId;
FString ThreadReplyId;

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
        [=, this](const FDoneDelegate& TestDone)
        {
            Socket = IChatSocket::Create(TokenManager, ApiKey, Host, User);
            Socket->Connect([=, this](const FOwnUserDto&) { TestDone.Execute(); });
        });

    Describe(
        "Channel",
        [=, this]
        {
            // Create channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FChannelStateResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestFalse("Not hidden", Dto.Channel.bHidden);
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FChannelsResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("One channel in response", Dto.Channels.Num(), 1);
                            TestDone.Execute();
                        });
                });

            // Mute channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->MuteChannels(
                        {NewCid},
                        {},
                        [=, this](const TResponse<FMuteChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
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
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->UnmuteChannels(
                        {NewCid},
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Partial update channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    const FString NewName = TEXT("New channel name");
                    const TSharedRef<FJsonObject> Set = MakeShared<FJsonObject>();
                    Set->SetStringField(TEXT("name"), NewName);
                    Api->PartialUpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Set,
                        {},
                        [=, this](const TResponse<FUpdateChannelPartialResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Channel name partial updated", Dto.Channel.AdditionalFields.GetString(TEXT("name")), {NewName});
                            TestDone.Execute();
                        });
                });

            // Update channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    const FString NewName = TEXT("MY CHANNEL");
                    FUpdateChannelRequestDto Data;
                    Data.Data.AdditionalFields.SetString(TEXT("name"), NewName);
                    Data.Data.AdditionalFields.SetString(TEXT("random"), NewName);
                    Api->UpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Data,
                        [=, this](const TResponse<FUpdateChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FString ResponseName = Dto.Channel.AdditionalFields.GetString(TEXT("name")).GetValue();
                            AddInfo(ResponseName);
                            TestEqual("Name updated", ResponseName, NewName);

                            TestDone.Execute();
                        });
                });

            // Add members
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FUpdateChannelRequestDto Data;
                    FChannelMemberRequestDto MemberRequestDto;
                    MemberRequestDto.ChannelRole = TEXT("channel_moderator");
                    MemberRequestDto.User.Id = User.Id;
                    Data.AddMembers.Add(MemberRequestDto);
                    Api->UpdateChannel(
                        ChannelType,
                        NewChannelId,
                        Data,
                        [=, this](const TResponse<FUpdateChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FChannelMemberDto* Found = Dto.Members.FindByPredicate([&](const FChannelMemberDto& A) { return A.UserId == User.Id; });
                            TestNotNull("User added", Found);
                            TestEqual("User is moderator", Found->ChannelRole, TEXT("channel_moderator"));
                            TestTrue("No message", Dto.Message.Id.IsEmpty());
                            AddInfo(FString::FromInt(Dto.Channel.Cooldown));
                            TestEqual("No cooldown", Dto.Channel.Cooldown, TNumericLimits<uint32>::Max());
                            TestDone.Execute();
                        });
                });

            // Query members
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryMembers(
                        ChannelType,
                        {},
                        NewChannelId,
                        {},
                        {},
                        {},
                        [=, this](const TResponse<FMembersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FChannelMemberDto* Found = Dto.Members.FindByPredicate([&](const FChannelMemberDto& A) { return A.UserId == User.Id; });
                            TestNotNull("User queried", Found);
                            TestEqual("User is moderator", Found->ChannelRole, TEXT("channel_moderator"));
                            TestDone.Execute();
                        });
                });

            // Hide channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->HideChannel(
                        ChannelType,
                        NewChannelId,
                        false,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Show channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->ShowChannel(
                        ChannelType,
                        NewChannelId,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Remove members
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FUpdateChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FChannelMemberDto* Found = Dto.Members.FindByPredicate([&](const FChannelMemberDto& A) { return A.UserId == User.Id; });
                            TestNull("User removed", Found);
                            TestEqual("Message sent", Dto.Message.Text, MsgText);
                            TestDone.Execute();
                        });
                });

            // Delete channel
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->DeleteChannel(
                        ChannelType,
                        NewChannelId,
                        [=, this](const TResponse<FDeleteChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, NewCid);
                            TestEqual("Response.Channel.Id", Dto.Channel.Id, NewChannelId);
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Check channel deleted
            LatentIt(
                "should have deleted test channel",
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FChannelsResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("No channels in response", Dto.Channels.Num(), 0);
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "should send a message",
                [=, this](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Cid = Cid;
                    Request.Text = MsgText;
                    Api->SendNewMessage(
                        ChannelType,
                        ChannelId,
                        Request,
                        false,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message text is same as input", Dto.Message.Text, MsgText);
                            MessageId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });
            LatentIt(
                "should truncate channel",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->TruncateChannel(
                        ChannelType,
                        ChannelId,
                        {},
                        {},
                        {},
                        {},
                        [=, this](const TResponse<FTruncateChannelResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message is empty", Dto.Message.CreatedAt.GetTicks(), 0);
                            TestEqual("Correct channel", Dto.Channel.Cid, Cid);
                            TestEqual("Channel has no messages", Dto.Channel.LastMessageAt.GetTicks(), 0);
                            const double SecsSinceTrunc = (FDateTime::UtcNow() - Dto.Channel.TruncatedAt).GetTotalSeconds();
                            TestTrue("Truncated recently", SecsSinceTrunc < 10.);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Users",
        [=, this]
        {
            LatentIt(
                "should return some known users",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryUsers(
                        Socket->GetConnectionId(),
                        false,
                        FFilter::Autocomplete(TEXT("id"), User.Id).ToJsonObject(),
                        {},
                        100,
                        {},
                        [=, this](const TResponse<FUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Users returned", Dto.Users.Num() > 0);
                            const FUserResponseDto* FoundUser =
                                Dto.Users.FindByPredicate([=, this](const FUserResponseDto& UserDto) { return UserDto.Id == User.Id; });
                            TestNotNull("User found", FoundUser);
                            if (FoundUser)
                            {
                                TestTrue("Online", FoundUser->bOnline);
                                TestEqual("Role", FoundUser->Role, TEXT("user"));
                                // Querying yourself returns the own user representation, which carries fields that a
                                // plain user response does not model. Anything beyond those is genuine schema drift.
                                static const TSet<FName> OwnUserOnlyFields{
                                    TEXT("channel_mutes"),
                                    TEXT("devices"),
                                    TEXT("mutes"),
                                    TEXT("total_unread_count"),
                                    TEXT("unread_channels"),
                                    TEXT("unread_count"),
                                    TEXT("unread_threads")};
                                for (const auto& Field : FoundUser->AdditionalFields.GetFields())
                                {
                                    TestTrue(FString::Printf(TEXT("Field %s is modelled"), *Field.Key.ToString()), OwnUserOnlyFields.Contains(Field.Key));
                                }
                            }
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Guest",
        [=, this]
        {
            LatentIt(
                "should create guest user",
                [=, this](const FDoneDelegate& TestDone)
                {
                    const FUserObjectRequestDto GuestUserDto{FUserDto{TEXT("test-guest-user")}};
                    Api->CreateGuest(
                        GuestUserDto,
                        [=, this](const TResponse<FGuestResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            // Check for "guest-{uuid}-test-guest-user
                            const FRegexPattern Pattern{TEXT("guest-[0-9a-f]{8}-[0-9a-f]{4}-[0-5][0-9a-f]{3}-[089ab][0-9a-f]{3}-[0-9a-f]{12}-test-guest-user")};
                            FRegexMatcher Regex{Pattern, Dto.User.Id};
                            TestTrue("Guest generated", Regex.FindNext());
                            TestFalse("Online", Dto.User.bOnline);
                            TestEqual("Role", Dto.User.Role, TEXT("guest"));
                            TestEqual("No additional fields", Dto.User.AdditionalFields.GetFields().Num(), 0);
                            GuestUserId = Dto.User.Id;
                            TestDone.Execute();
                        });
                });

            // Partial update user
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    const FString NewName = TEXT("New user name");
                    const TSharedRef<FJsonObject> Set = MakeShared<FJsonObject>();
                    Set->SetStringField(TEXT("name"), NewName);
                    const FChatApi::FPartialUpdateUser Update{GuestUserId, Set, {}};
                    Api->PartialUpdateUsers(
                        {Update},
                        [=, this](const TResponse<FUpdateUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("User partial updated", Dto.Users[GuestUserId].AdditionalFields.GetString(TEXT("name")).GetValue(), NewName);
                            TestDone.Execute();
                        });
                });

            // Upsert users
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FUserObjectRequestDto Update;
                    Update.Id = GuestUserId;
                    Update.Teams.Push(TEXT("team1"));
                    const FString NewName = TEXT("Guest name");
                    Update.AdditionalFields.SetString(TEXT("name"), NewName);
                    Api->UpsertUsers(
                        {{GuestUserId, Update}},
                        [=, this](const TResponse<FUpdateUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("User upserted", Dto.Users[GuestUserId].AdditionalFields.GetString(TEXT("name")).GetValue(), NewName);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Device",
        [=, this]
        {
            LatentIt(
                "should add device",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->AddDevice(
                        DeviceId,
                        EPushProvider::Firebase,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // Remove device
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->RemoveDevice(
                        DeviceId,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // List devices
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->ListDevices(
                        [=, this](const TResponse<FListDevicesResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Device exists", Dto.Devices[0].Id, DeviceId);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Mute user",
        [=, this]
        {
            // Mute user
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->MuteUsers(
                        {BanUserId},
                        {},
                        [=, this](const TResponse<FMuteUserResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
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
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->UnmuteUsers(
                        {BanUserId},
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Block user",
        [=, this]
        {
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->BlockUser(
                        BanUserId,
                        [=, this](const TResponse<FBlockUserResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Blocked user", Dto.BlockedUserId, BanUserId);
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "should list the blocked user",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->GetBlockedUsers(
                        [=, this](const TResponse<FGetBlockedUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue(
                                "Blocked user listed",
                                Dto.Blocks.ContainsByPredicate([&](const FBlockedUserDto& Block) { return Block.BlockedUserId == BanUserId; }));
                            TestDone.Execute();
                        });
                });

            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->UnblockUser(
                        BanUserId,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            TestTrue("Response received", Response.GetRef().Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Ban user",
        [=, this]
        {
            // Ban user
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->BanUser(
                        BanUserId,
                        ChannelType,
                        ChannelId,
                        {},
                        {},
                        {},
                        {},
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // User was banned
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FQueryBannedUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FBanResponseDto* Ban = Dto.Bans.FindByPredicate([&](const FBanResponseDto& B) { return B.User.Id == BanUserId; });
                            TestNotNull("User was banned", Ban);
                            TestEqual("Banned in channel", Ban->Channel.Cid, Cid);
                            // TestTrue("Banned recently", FDateTime::UtcNow() - Ban->CreatedAt < FTimespan::FromMinutes(1.f));
                            TestDone.Execute();
                        });
                });

            // Unban user
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->UnbanUser(
                        BanUserId,
                        ChannelType,
                        ChannelId,
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestTrue("Response received", Dto.Duration.Len() > 0);
                            TestDone.Execute();
                        });
                });

            // User was unbanned
            LatentIt(
                "should ban user",
                [=, this](const FDoneDelegate& TestDone)
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
                        [=, this](const TResponse<FQueryBannedUsersResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FBanResponseDto* Ban = Dto.Bans.FindByPredicate([&](const FBanResponseDto& B) { return B.User.Id == BanUserId; });
                            TestNull("User was unbanned", Ban);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Message",
        [=, this]
        {
            // Create message
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Cid = Cid;
                    Request.Text = MsgText;
                    Api->SendNewMessage(
                        ChannelType,
                        ChannelId,
                        Request,
                        false,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message text is same as input", Dto.Message.Text, MsgText);
                            MessageId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            // Update message
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Id = MessageId;
                    Request.Text = UpdatedMsgText;
                    Api->UpdateMessage(
                        Request,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message text is same as input", Dto.Message.Text, UpdatedMsgText);
                            MessageId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            // Get message
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->GetMessage(
                        MessageId,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Fetched message", Dto.Message.Text, UpdatedMsgText);
                            MessageId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "Flag message",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->Flag(
                        MessageId,
                        {},
                        TEXT("spam"),
                        {{TEXT("source"), TEXT("automated test")}},
                        [=, this](const TResponse<FFlagResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message ID matches query", Dto.Flag.TargetMessageId, MessageId);
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "Unflag message",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->Unflag(
                        MessageId,
                        {},
                        [=, this](const TResponse<FFlagResponseDto>& Response)
                        {
                            TestTrue("Flag withdrawn", Response.IsSuccessful());
                            TestDone.Execute();
                        });
                });

            // Delete message
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->DeleteMessage(
                        MessageId,
                        true,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Message text is same as input", Dto.Message.Text, UpdatedMsgText);
                            TestEqual("Message is deleted", Dto.Message.Type, EMessageTypeDto::Deleted);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Thread",
        [=, this]
        {
            // Start a thread: send the message the thread hangs off
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Cid = Cid;
                    Request.Text = MsgText;
                    Api->SendNewMessage(
                        ChannelType,
                        ChannelId,
                        Request,
                        false,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Parent is a regular message", Dto.Message.Type, EMessageTypeDto::Regular);
                            TestEqual("Parent has no replies yet", static_cast<int32>(Dto.Message.ReplyCount), 0);
                            ThreadParentId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            // Reply in the thread, and ask for it to show in the channel too
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    FMessageRequestDto Request;
                    Request.Cid = Cid;
                    Request.Text = ReplyText;
                    Request.ParentId = ThreadParentId;
                    Request.bShowInChannel = true;
                    Api->SendNewMessage(
                        ChannelType,
                        ChannelId,
                        Request,
                        false,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Reply text is same as input", Dto.Message.Text, ReplyText);
                            // The write path is what was missing: without parent_id on the wire the
                            // backend takes this for a plain channel message instead of a reply.
                            TestEqual("Reply is attached to the parent", Dto.Message.ParentId, ThreadParentId);
                            TestTrue("Reply is shown in the channel", Dto.Message.bShowInChannel);
                            // A reply is identified by its parent, not by its type: the API reports it
                            // as regular despite EMessageTypeDto::Reply existing. Anything keying off
                            // the type to spot a reply would quietly never match.
                            TestEqual("Reply is reported as a regular message", Dto.Message.Type, EMessageTypeDto::Regular);
                            ThreadReplyId = Dto.Message.Id;
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->GetReplies(
                        ThreadParentId,
                        {},
                        [=, this](const TResponse<FGetRepliesResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FMessageDto* Reply = Dto.Messages.FindByPredicate([&](const FMessageDto& M) { return M.Id == ThreadReplyId; });
                            TestNotNull("Reply returned by the thread", Reply);
                            if (Reply)
                            {
                                TestEqual("Reply text", Reply->Text, ReplyText);
                            }
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    // Paginating back from the only reply leaves nothing older to return
                    FMessagePaginationParamsRequestDto Pagination;
                    Pagination.Limit = 1;
                    Pagination.IdLt = ThreadReplyId;
                    Api->GetReplies(
                        ThreadParentId,
                        Pagination,
                        [=, this](const TResponse<FGetRepliesResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("No replies before the first one", Dto.Messages.Num(), 0);
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryThreads(
                        {},
                        FFilter::Equal(TEXT("parent_message_id"), ThreadParentId).ToJsonObject(),
                        {},
                        {},
                        {},
                        {},
                        {},
                        {},
                        false,
                        [=, this](const TResponse<FQueryThreadsResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            const FThreadStateResponseDto* Thread =
                                Dto.Threads.FindByPredicate([&](const FThreadStateResponseDto& T) { return T.ParentMessageId == ThreadParentId; });
                            TestNotNull("Thread listed", Thread);
                            if (Thread)
                            {
                                TestEqual("Thread is in the right channel", Thread->ChannelCid, Cid);
                                TestEqual("Thread has one reply", Thread->ReplyCount, 1);
                                TestEqual("Thread was started by the test user", Thread->CreatedByUserId, User.Id);
                                TestEqual("Thread parent message", Thread->ParentMessage.Id, ThreadParentId);
                                TestTrue(
                                    "Latest replies previewed",
                                    Thread->LatestReplies.ContainsByPredicate([&](const FMessageDto& M) { return M.Id == ThreadReplyId; }));
                            }
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "should get a single thread",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->GetThread(
                        ThreadParentId,
                        {},
                        false,
                        {},
                        // Without a participant limit this endpoint returns no participants at all,
                        // rather than defaulting to some number of them
                        10,
                        {},
                        [=, this](const TResponse<FGetThreadResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Requested thread returned", Dto.Thread.ParentMessageId, ThreadParentId);
                            TestEqual("Thread has one reply", Dto.Thread.ReplyCount, 1);
                            TestEqual("Thread has one participant", Dto.Thread.ParticipantCount, 1);
                            const FThreadParticipantDto* Participant =
                                Dto.Thread.ThreadParticipants.FindByPredicate([&](const FThreadParticipantDto& P) { return P.UserId == User.Id; });
                            TestNotNull("Test user participates in the thread", Participant);
                            TestDone.Execute();
                        });
                });

            // Hard deleting the parent takes its replies with it
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->DeleteMessage(
                        ThreadParentId,
                        true,
                        [=, this](const TResponse<FMessageResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Parent message is deleted", Dto.Message.Type, EMessageTypeDto::Deleted);
                            TestDone.Execute();
                        });
                });
        });

    Describe(
        "Watch/unwatch channel",
        [=, this]
        {
            // TODO: Should be able to combine this and next query, once backend fixes race condition
            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryChannel(
                        ChannelType,
                        Socket->GetConnectionId(),
                        EChannelFlags::Watch,
                        {},
                        ChannelId,
                        {},
                        {},
                        {},
                        [=, this](const TResponse<FChannelStateResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, Cid);
                            TestDone.Execute();
                        });
                });

            LatentBeforeEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryChannel(
                        ChannelType,
                        Socket->GetConnectionId(),
                        EChannelFlags::State,
                        {},
                        ChannelId,
                        {},
                        {},
                        {{5, 0}},
                        [=, this](const TResponse<FChannelStateResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestEqual("Response.Channel.Cid", Dto.Channel.Cid, Cid);
                            TestTrue("Watched", Dto.Watchers.ContainsByPredicate([&](const FUserObjectDto& U) { return U.Id == User.Id; }));
                            TestDone.Execute();
                        });
                });

            LatentIt(
                "Stop watching channel",
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->StopWatchingChannel(
                        ChannelType,
                        ChannelId,
                        Socket->GetConnectionId(),
                        [=, this](const TResponse<FResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                            TestDone.Execute();
                        });
                });

            // Delete message
            LatentAfterEach(
                [=, this](const FDoneDelegate& TestDone)
                {
                    Api->QueryChannel(
                        ChannelType,
                        Socket->GetConnectionId(),
                        EChannelFlags::State,
                        {},
                        ChannelId,
                        {},
                        {},
                        {},
                        [=, this](const TResponse<FChannelStateResponseDto>& Response)
                        {
                            const auto& Dto = Response.GetRef();
                            TestFalse("Stopped watching", Dto.Watchers.ContainsByPredicate([&](const FUserObjectDto& U) { return U.Id == User.Id; }));
                            TestDone.Execute();
                        });
                });
        });

    AfterEach([=, this] { Socket->Disconnect(); });
}
