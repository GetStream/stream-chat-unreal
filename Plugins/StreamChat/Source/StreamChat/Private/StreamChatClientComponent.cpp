// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Algo/Transform.h"
#include "Blueprint/CallbackAction.h"
#include "Channel/ChatChannel.h"
#include "Channel/Filter.h"
#include "ChatApi.h"
#include "ConstantTokenProvider.h"
#include "Event/Channel/MessageNewEvent.h"
#include "Event/Client/ConnectionRecoveredEvent.h"
#include "Event/User/UserPresenceChangedEvent.h"
#include "IChatSocket.h"
#include "Moderation/Ban.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Device/ListDevicesResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/Moderation/QueryBannedUsersResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "StreamChatSettings.h"
#include "Token.h"
#include "TokenManager.h"
#include "User/Device.h"
#include "User/Jwt.h"
#include "User/UserManager.h"
#include "Util.h"

UStreamChatClientComponent::UStreamChatClientComponent() : TokenManager(MakeShared<FTokenManager>())
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UStreamChatClientComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize in BeginPlay to ensure properties like ApiKey are loaded from BP, TODO I don't like this
    Api = FChatApi::Create(ApiKey, GetDefault<UStreamChatSettings>()->Host, TokenManager);
}

void UStreamChatClientComponent::ConnectUserInternal(const FUser& User, const TFunction<void(const FUserRef&)> Callback)
{
    const FUserRef UserRef = UUserManager::Get()->UpsertUser(User);
    Socket = IChatSocket::Create(TokenManager.ToSharedRef(), ApiKey, GetDefault<UStreamChatSettings>()->Host, FUserObjectDto(User));
    Socket->Connect(
        [Callback](const FOwnUserDto& OwnUser)
        {
            const FUserRef OwnUserRef = UUserManager::Get()->UpsertUser(OwnUser);
            UUserManager::Get()->SetCurrentUser(OwnUserRef);
            if (Callback)
            {
                Callback(OwnUserRef);
            }
        });

    On<FConnectionRecoveredEvent>(this, &UStreamChatClientComponent::OnConnectionRecovered);
    On<FUserPresenceChangedEvent>(this, &UStreamChatClientComponent::OnUserPresenceChanged);
    On<FMessageNewEvent>(this, &UStreamChatClientComponent::OnNewMessage);
}

UChatChannel* UStreamChatClientComponent::CreateChannelObject(const FChannelStateResponseFieldsDto& Dto)
{
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), Dto);
}

void UStreamChatClientComponent::OnConnectionRecovered(const FConnectionRecoveredEvent&)
{
    // Fetch data for known channels
    TArray<FString> Cids;
    Algo::Transform(Channels, Cids, [](const UChatChannel* Channel) { return Channel->Properties.Cid; });
    QueryChannels(FFilter::In(TEXT("cid"), Cids));
}

void UStreamChatClientComponent::OnUserPresenceChanged(const FUserPresenceChangedEvent& Event)
{
    UUserManager::Get()->UpsertUser(Event.User);
}

void UStreamChatClientComponent::OnNewMessage(const FMessageNewEvent& Event)
{
    // Maintain order by most recent message
    const int32 Index = Channels.IndexOfByPredicate([&Event](const UChatChannel* Channel) { return Channel->Properties.Id == Event.ChannelId; });
    if (Index != 0)
    {
        // We could sort the entire array, but moving the channel to the first position should be enough
        UChatChannel* Channel = Channels[Index];
        Channels.RemoveAt(Index, 1, false);
        Channels.Insert(Channel, 0);
        ChannelsUpdated.Broadcast(Channels);
    }
}

void UStreamChatClientComponent::AddChannels(const TArray<UChatChannel*>& InChannels)
{
    Channels.Append(InChannels);
    ChannelsUpdated.Broadcast(Channels);
}

void UStreamChatClientComponent::ConnectUser(
    const FUser& User,
    const FString& Token,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectUser(User, Token, Callback); });
}

void UStreamChatClientComponent::ConnectAnonymousUser(const UObject* WorldContextObject, const FLatentActionInfo LatentInfo, FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectAnonymousUser(Callback); });
}

void UStreamChatClientComponent::ConnectGuestUser(const FUser& User, const UObject* WorldContextObject, const FLatentActionInfo LatentInfo, FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectGuestUser(User, Callback); });
}

void UStreamChatClientComponent::QueryChannels(
    const FFilter Filter,
    const TArray<FChannelSortOption>& SortOptions,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    TArray<UChatChannel*>& OutChannels,
    const int32 Flags)
{
    const TOptional<FFilter> OptionalFilter = Filter.IsValid() ? Filter : TOptional<FFilter>{};
    TCallbackAction<TArray<UChatChannel*>>::CreateLatentAction(
        WorldContextObject,
        LatentInfo,
        OutChannels,
        [&](auto Callback) { QueryChannels(OptionalFilter, SortOptions, static_cast<EChannelFlags>(Flags), {}, Callback); });
}

void UStreamChatClientComponent::WatchChannel(
    const FChannelProperties& ChannelProperties,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    UChatChannel*& OutChannel)
{
    TCallbackAction<UChatChannel*, UChatChannel*>::CreateLatentAction(
        WorldContextObject, LatentInfo, OutChannel, [&](auto Callback) { WatchChannel(ChannelProperties, Callback); });
}

void UStreamChatClientComponent::BanUserBP(const FUserRef& User, const FTimespan Timeout, const FString& Reason, const bool bIpBan) const
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    const TOptional<FString> OptionalReason = Reason.IsEmpty() ? TOptional<FString>{} : Reason;
    BanUser(User, OptionalTimeout, OptionalReason, bIpBan);
}

void UStreamChatClientComponent::BanUser(const FUserRef& User, const TOptional<FTimespan>& Timeout, const TOptional<FString>& Reason, const bool bIpBan) const
{
    Api->BanUser(User->Id, {}, {}, Timeout, Reason, false, bIpBan);
}

void UStreamChatClientComponent::UnbanUser(const FUserRef& User) const
{
    Api->UnbanUser(User->Id);
}

void UStreamChatClientComponent::ShadowBanUserBP(const FUserRef& User, const FTimespan Timeout) const
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    ShadowBanUser(User, OptionalTimeout);
}

void UStreamChatClientComponent::ShadowBanUser(const FUserRef& User, const TOptional<FTimespan>& Timeout) const
{
    Api->BanUser(User->Id, {}, {}, Timeout, {}, true);
}

void UStreamChatClientComponent::ShadowUnbanUser(const FUserRef& User) const
{
    Api->UnbanUser(User->Id);
}

void UStreamChatClientComponent::QueryBannedUsers(
    const FFilter& Filter,
    const TArray<FBanSortOption>& SortOptions,
    const TOptional<FBanPaginationOptions> PaginationOptions,
    TCallback<TArray<FBan>> Callback)
{
    TOptional<FDateTime> CreatedAtAfterOrEqual;
    TOptional<FDateTime> CreatedAtAfter;
    TOptional<FDateTime> CreatedAtBeforeOrEqual;
    TOptional<FDateTime> CreatedAtBefore;
    TOptional<uint32> Limit;
    TOptional<uint32> Offset;
    if (PaginationOptions.IsSet())
    {
        CreatedAtAfterOrEqual = PaginationOptions.GetValue().CreatedAtAfterOrEqual;
        CreatedAtAfter = PaginationOptions.GetValue().CreatedAtAfter;
        CreatedAtBeforeOrEqual = PaginationOptions.GetValue().CreatedAtBeforeOrEqual;
        CreatedAtBefore = PaginationOptions.GetValue().CreatedAtBefore;
        Limit = PaginationOptions.GetValue().Limit;
        Offset = PaginationOptions.GetValue().Offset;
    }
    Api->QueryBannedUsers(
        Filter.ToJsonObject(),
        Util::Convert<FSortParamRequestDto>(SortOptions),
        CreatedAtAfterOrEqual,
        CreatedAtAfter,
        CreatedAtBeforeOrEqual,
        CreatedAtBefore,
        Limit,
        Offset,
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FQueryBannedUsersResponseDto& Dto)
        {
            if (Callback)
            {
                const TArray<FBan> Bans = Util::Convert<FBan>(Dto.Bans, UUserManager::Get());
                Callback(Bans);
            }
        });
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, TUniquePtr<ITokenProvider> TokenProvider, const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    TokenManager->SetTokenProvider(MoveTemp(TokenProvider), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(FToken::Jwt(Token)), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectAnonymousUser(const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    const FToken Token = FToken::Anonymous();
    const FUser User{TEXT("anonymous")};
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectGuestUser(const FUser& User, TFunction<void(const FUserRef&)> Callback)
{
    const FToken AnonToken = FToken::Anonymous();
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(AnonToken), User.Id);

    Api->CreateGuest(
        FUserObjectRequestDto(User),
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FGuestResponseDto& Response)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            const FToken GuestToken = FToken::Jwt(Response.AccessToken);
            const FUser GuestUser = *UUserManager::Get()->UpsertUser(Response.User);
            WeakThis->TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(GuestToken), GuestUser.Id);
            WeakThis->ConnectUserInternal(GuestUser, Callback);
        });
}

void UStreamChatClientComponent::DisconnectUser()
{
    if (Socket)
    {
        Socket->Events().UnsubscribeAll(this);
        Socket->Disconnect();
    }
    TokenManager->Reset();
    UUserManager::Get()->ResetCurrentUser();
}

UChatChannel* UStreamChatClientComponent::NewChat()
{
    Channels.RemoveAll([](const UChatChannel* C) { return !C->IsValid(); });
    UChatChannel* NewChannel = UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef());
    Channels.Insert(NewChannel, 0);
    ChannelsUpdated.Broadcast(Channels);
    return NewChannel;
}

void UStreamChatClientComponent::CancelNewChat()
{
    Channels.RemoveAll([](const UChatChannel* C) { return !C->IsValid(); });
    ChannelsUpdated.Broadcast(Channels);
}

void UStreamChatClientComponent::QueryChannels(
    const TOptional<FFilter> Filter,
    const TArray<FChannelSortOption>& SortOptions,
    const EChannelFlags Flags,
    const FPaginationOptions& PaginationOptions,
    TFunction<void(const TArray<UChatChannel*>&)> Callback)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    TOptional<TSharedRef<FJsonObject>> FilterJson;
    if (Filter.IsSet())
    {
        FilterJson.Emplace(Filter.GetValue().ToJsonObject());
    }

    Api->QueryChannels(
        Socket->GetConnectionId(),
        Flags,
        FilterJson,
        Util::Convert<FSortParamRequestDto>(SortOptions),
        {},
        {},
        PaginationOptions.GetLimitAsOptional(),
        PaginationOptions.GetOffsetAsOptional(),
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FChannelsResponseDto& Response)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            // TODO Perf?
            TArray<UChatChannel*> NewChannels;
            for (const FChannelStateResponseFieldsDto& ResponseChannel : Response.Channels)
            {
                const auto Pred = [&ResponseChannel, &NewChannels](const UChatChannel* Channel)
                {
                    return Channel->Properties.Cid == ResponseChannel.Channel.Cid;
                };
                if (!WeakThis->Channels.ContainsByPredicate(Pred))
                {
                    NewChannels.Add(WeakThis->CreateChannelObject(ResponseChannel));
                }
            }
            if (NewChannels.Num() > 0)
            {
                WeakThis->AddChannels(NewChannels);
            }

            if (Callback)
            {
                Callback(NewChannels);
            }
        });
}

void UStreamChatClientComponent::CreateChannel(const FChannelProperties& ChannelProperties, const TFunction<void(UChatChannel*)> Callback)
{
    QueryChannel(ChannelProperties, EChannelFlags::None, Callback);
}

void UStreamChatClientComponent::WatchChannel(const FChannelProperties& ChannelProperties, const TFunction<void(UChatChannel*)> Callback)
{
    QueryChannel(ChannelProperties, EChannelFlags::State | EChannelFlags::Watch, Callback);
}

void UStreamChatClientComponent::QueryChannel(const FChannelProperties& ChannelProperties, const EChannelFlags Flags, TFunction<void(UChatChannel*)> Callback)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    const TOptional<FString> OptionalId = ChannelProperties.Id.IsEmpty() ? TOptional<FString>{} : ChannelProperties.Id;
    const FChannelRequestDto Data(ChannelProperties);
    Api->QueryChannel(
        ChannelProperties.Type,
        Socket->GetConnectionId(),
        Flags,
        Data,
        OptionalId,
        {},
        {},
        {},
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FChannelStateResponseDto& Dto)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            UChatChannel* Channel = WeakThis->CreateChannelObject(Dto);

            WeakThis->Channels.Insert(Channel, 0);
            WeakThis->ChannelsUpdated.Broadcast(WeakThis->Channels);
            if (Callback)
            {
                Callback(Channel);
            }
        });
}

void UStreamChatClientComponent::QueryAdditionalChannels(const int32 Limit, const TFunction<void()> Callback)
{
    if (Channels.Num() == 0)
    {
        return;
    }

    const FPaginationOptions Pagination{Limit, Channels.Num()};

    QueryChannels(
        FFilter::In(TEXT("members"), {UUserManager::Get()->GetCurrentUser()->Id}),
        {{EChannelSortField::LastMessageAt, ESortDirection::Descending}},
        EChannelFlags::State | EChannelFlags::Watch,
        Pagination,
        [Callback](const TArray<UChatChannel*>)
        {
            if (Callback)
            {
                Callback();
            }
        });
}

void UStreamChatClientComponent::QueryUsers(
    const FFilter& Filter,
    const TArray<FUserSortOption>& Sort,
    const bool bPresence,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset,
    TFunction<void(const TArray<FUserRef>&)> Callback) const
{
    Api->QueryUsers(
        Socket->GetConnectionId(),
        bPresence,
        Filter.ToJsonObject(),
        Util::Convert<FSortParamRequestDto>(Sort),
        Limit,
        Offset,
        [Callback](const FUsersResponseDto& Dto)
        {
            if (Callback)
            {
                UUserManager* UserManager = UUserManager::Get();
                TArray<FUserRef> Users;
                Algo::Transform(Dto.Users, Users, [UserManager](const FUserResponseDto& UserResponseDto) { return UserManager->UpsertUser(UserResponseDto); });
                Callback(Users);
            }
        });
}

void UStreamChatClientComponent::SearchMessages(
    const FFilter& ChannelFilter,
    const TOptional<FString>& Query,
    const TOptional<FFilter>& MessageFilter,
    const TArray<FMessageSortOption>& Sort,
    const TOptional<uint32> MessageLimit,
    TFunction<void(const TArray<FMessage>&)> Callback) const
{
    TOptional<TSharedRef<FJsonObject>> MessageFilterJson;
    if (MessageFilter.IsSet())
    {
        MessageFilterJson.Emplace(MessageFilter.GetValue().ToJsonObject());
    }

    Api->SearchMessages(
        ChannelFilter.ToJsonObject(),
        Query,
        MessageFilterJson,
        Util::Convert<FSortParamRequestDto>(Sort),
        MessageLimit,
        {},
        {},
        [Callback](const FSearchResponseDto& Response)
        {
            if (Callback)
            {
                Callback(Util::Convert<FMessage>(Response.Results, UUserManager::Get()));
            }
        });
}

FString UStreamChatClientComponent::DevToken(const FString& UserId)
{
    return Jwt::Development(UserId);
}

void UStreamChatClientComponent::AddDevice(const FString& DeviceId, const EPushProvider PushProvider) const
{
    Api->AddDevice(DeviceId, PushProvider);
}

void UStreamChatClientComponent::RemoveDevice(const FString& DeviceId) const
{
    Api->RemoveDevice(DeviceId);
}

void UStreamChatClientComponent::ListDevices(TFunction<void(TArray<FDevice>)> Callback) const
{
    Api->ListDevices(
        [Callback](const FListDevicesResponseDto& Dto)
        {
            if (Callback)
            {
                const TArray<FDevice> Devices = Util::Convert<FDevice>(Dto.Devices, UUserManager::Get());
                Callback(Devices);
            }
        });
}

void UStreamChatClientComponent::FlagMessage(const FMessage& Message) const
{
    Api->Flag(Message.Id);
}

void UStreamChatClientComponent::FlagUser(const FUserRef& User) const
{
    Api->Flag({}, User->Id);
}

void UStreamChatClientComponent::MuteUserBP(const FUserRef& User, const FTimespan Timeout) const
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    MuteUser(User, OptionalTimeout);
}

void UStreamChatClientComponent::MuteUser(const FUserRef& User, const TOptional<FTimespan>& Timeout) const
{
    Api->MuteUsers({User->Id}, Timeout);
}

void UStreamChatClientComponent::UnmuteUser(const FUserRef& User) const
{
    Api->UnmuteUsers({User->Id});
}

const TArray<UChatChannel*>& UStreamChatClientComponent::GetChannels() const
{
    return Channels;
}
