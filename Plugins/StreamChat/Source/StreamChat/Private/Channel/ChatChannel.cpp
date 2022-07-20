// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "Algo/Transform.h"
#include "Blueprint/CallbackAction.h"
#include "Channel/Filter.h"
#include "ChatApi.h"
#include "Engine/World.h"
#include "Event/Channel/MessageDeletedEvent.h"
#include "Event/Channel/MessageNewEvent.h"
#include "Event/Channel/MessageReadEvent.h"
#include "Event/Channel/MessageUpdatedEvent.h"
#include "Event/Channel/ReactionDeletedEvent.h"
#include "Event/Channel/ReactionNewEvent.h"
#include "Event/Channel/ReactionUpdatedEvent.h"
#include "Event/Channel/TypingStartEvent.h"
#include "Event/Channel/TypingStopEvent.h"
#include "Event/Notification/NotificationMarkReadEvent.h"
#include "Event/Notification/NotificationMessageNewEvent.h"
#include "Event/User/UserBannedEvent.h"
#include "Event/User/UserUnbannedEvent.h"
#include "Reaction/Reaction.h"
#include "Request/Channel/UpdateChannelRequestDto.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Channel/MembersResponseDto.h"
#include "Response/Channel/TruncateChannelResponseDto.h"
#include "Response/Channel/UpdateChannelPartialResponseDto.h"
#include "Response/Channel/UpdateChannelResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/Moderation/MuteChannelResponseDto.h"
#include "Response/Reaction/GetReactionsResponseDto.h"
#include "StreamChatClientComponent.h"
#include "TimerManager.h"
#include "User/UserManager.h"
#include "Util.h"

namespace
{
FMessage PrepareMessage(const FMessage& Message)
{
    FMessage NewMessage = Message;
    NewMessage.State = EMessageSendState::Sending;
    if (NewMessage.Id.IsEmpty())
    {
        NewMessage.Id = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    }
    if (!NewMessage.User.IsValid())
    {
        NewMessage.User = UUserManager::Get()->GetCurrentUser().User;
    }
    if (NewMessage.CreatedAt.GetTicks() == 0)
    {
        NewMessage.CreatedAt = FDateTime::UtcNow();
    }
    if (NewMessage.UpdatedAt.GetTicks() == 0)
    {
        NewMessage.UpdatedAt = FDateTime::UtcNow();
    }
    return NewMessage;
}
}    // namespace

UChatChannel* UChatChannel::Create(
    UObject* Outer,
    const TSharedRef<FChatApi> Api,
    const TSharedRef<IChatSocket> Socket,
    const FChannelStateResponseFieldsDto& Dto)
{
    UChatChannel* Channel = Create(Outer, Api, Socket);
    Channel->Properties = FChannelProperties{Dto.Channel, Dto.Members, UUserManager::Get()};
    Channel->State = FChannelState{Dto, UUserManager::Get()};

    Channel->On<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Channel->On<FNotificationMessageNewEvent>(Channel, &UChatChannel::OnNotificationMessageNew);

    Channel->On<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Channel->On<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);
    Channel->On<FReactionNewEvent>(Channel, &UChatChannel::OnReactionNew);
    Channel->On<FReactionUpdatedEvent>(Channel, &UChatChannel::OnReactionUpdated);
    Channel->On<FReactionDeletedEvent>(Channel, &UChatChannel::OnReactionDeleted);
    Channel->On<FTypingStartEvent>(Channel, &UChatChannel::OnTypingStart);
    Channel->On<FTypingStopEvent>(Channel, &UChatChannel::OnTypingStop);
    Channel->On<FUserBannedEvent>(Channel, &UChatChannel::OnUserBanned);
    Channel->On<FUserUnbannedEvent>(Channel, &UChatChannel::OnUserUnbanned);

    if (Channel->Properties.Config.bReadEvents)
    {
        Channel->On<FMessageReadEvent>(Channel, &UChatChannel::OnMessageRead);
        Channel->On<FNotificationMarkReadEvent>(Channel, &UChatChannel::OnNotificationMessageRead);
    }

    Channel->MessagesUpdated.Broadcast();

    return Channel;
}

UChatChannel* UChatChannel::Create(UObject* Outer, const TSharedRef<FChatApi> Api, const TSharedRef<IChatSocket> Socket)
{
    UChatChannel* Channel = NewObject<UChatChannel>(Outer);
    check(Socket->IsConnected());

    Channel->Api = Api;
    Channel->Socket = Socket;

    return Channel;
}

bool UChatChannel::IsValid() const
{
    return !Properties.Cid.IsEmpty();
}

void UChatChannel::Delete(TFunction<void()> Callback) const
{
    Api->DeleteChannel(
        Properties.Type,
        Properties.Id,
        [Callback](const TResponse<FDeleteChannelResponseDto>&)
        {
            if (Callback)
            {
                Callback();
            }
        });
}

void UChatChannel::PartialUpdate(const TSharedRef<FJsonObject>& Set, const TArray<FString>& Unset, TFunction<void()> Callback)
{
    Api->PartialUpdateChannel(
        Properties.Type,
        Properties.Id,
        Set,
        Unset,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FUpdateChannelPartialResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties.Merge(Dto->Channel, Dto->Members, UUserManager::Get());
                }
                if (Callback)
                {
                    Callback();
                }
            }
        });
}

void UChatChannel::Update(const FAdditionalFields& Data, const TOptional<FMessage>&, TFunction<void()> Callback)
{
    FUpdateChannelRequestDto RequestDto;
    RequestDto.AdditionalFields = Data;
    Api->UpdateChannel(
        Properties.Type,
        Properties.Id,
        RequestDto,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FUpdateChannelResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties = FChannelProperties{Dto->Channel, Dto->Members, UUserManager::Get()};
                    WeakThis->State.AddMessage(FMessage{Dto->Message, UUserManager::Get()});
                }
                if (Callback)
                {
                    Callback();
                }
            }
        });
}

void UChatChannel::Truncate(
    const bool bHardDelete,
    const TOptional<FDateTime>& TruncatedAt,
    const TOptional<FMessage>& Message,
    const bool bSkipPush,
    TFunction<void()> Callback)
{
    const auto OptionalMessage =
        Message.IsSet() ? TOptional<FMessageRequestDto>{PrepareMessage(Message.GetValue()).ToRequestDto(Properties.Cid)} : TOptional<FMessageRequestDto>{};

    Api->TruncateChannel(
        Properties.Type,
        Properties.Id,
        bHardDelete,
        TruncatedAt,
        OptionalMessage,
        bSkipPush,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FTruncateChannelResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties.Merge(Dto->Channel, UUserManager::Get());
                    if (Callback)
                    {
                        Callback();
                    }
                }
            }
        });
}

void UChatChannel::QueryMembers(
    const FFilter& Filter,
    const TArray<FUserSortOption>& Sort,
    const FMessagePaginationOptions& Pagination,
    TFunction<void(const TArray<FMember>&)> Callback)
{
    Api->QueryMembers(
        Properties.Type,
        Filter.ToJsonObject(),
        Properties.Id,
        {},
        Util::Convert<FSortParamRequestDto>(Sort),
        Util::Convert<FMessagePaginationParamsRequestDto>(Pagination),
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FMembersResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties.AppendMembers(Dto->Members, UUserManager::Get());
                }
                if (Callback)
                {
                    Callback(Util::Convert<FMember>(Dto->Members, UUserManager::Get()));
                }
            }
        });
}

void UChatChannel::AddMembers(const TArray<FString>& MemberIds, const TOptional<FMessage>& Message, TFunction<void()> Callback)
{
    FUpdateChannelRequestDto RequestDto;
    Algo::Transform(
        MemberIds,
        RequestDto.AddMembers,
        [](auto&& Id)
        {
            FChannelMemberRequestDto Dto;
            Dto.UserId = Id;
            return Dto;
        });
    if (Message.IsSet())
    {
        RequestDto.SetMessage(Message.GetValue().ToRequestDto(Properties.Cid));
    }
    Api->UpdateChannel(
        Properties.Type,
        Properties.Id,
        RequestDto,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FUpdateChannelResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties = FChannelProperties{Dto->Channel, Dto->Members, UUserManager::Get()};
                }
                if (Callback)
                {
                    Callback();
                }
            }
        });
}

void UChatChannel::RemoveMembers(const TArray<FString>& MemberIds, const TOptional<FMessage>& Message, TFunction<void()> Callback)
{
    FUpdateChannelRequestDto RequestDto;
    RequestDto.RemoveMembers = MemberIds;
    if (Message.IsSet())
    {
        RequestDto.SetMessage(Message.GetValue().ToRequestDto(Properties.Cid));
    }
    Api->UpdateChannel(
        Properties.Type,
        Properties.Id,
        RequestDto,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FUpdateChannelResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties = FChannelProperties{Dto->Channel, Dto->Members, UUserManager::Get()};
                }
                if (Callback)
                {
                    Callback();
                }
            }
        });
}

void UChatChannel::Hide(const bool bClearHistory, TFunction<void()> Callback) const
{
    Api->HideChannel(
        Properties.Type,
        Properties.Id,
        bClearHistory,
        [Callback](const TResponse<FResponseDto>&)
        {
            if (Callback)
            {
                Callback();
            }
        });
}

void UChatChannel::Show(TFunction<void()> Callback) const
{
    Api->ShowChannel(
        Properties.Type,
        Properties.Id,
        [Callback](const TResponse<FResponseDto>&)
        {
            if (Callback)
            {
                Callback();
            }
        });
}

void UChatChannel::StopWatching(TFunction<void()> Callback) const
{
    Api->StopWatchingChannel(
        Properties.Type,
        Properties.Id,
        Socket->GetConnectionId(),
        [Callback](const TResponse<FResponseDto>&)
        {
            if (Callback)
            {
                Callback();
            }
        });
}

void UChatChannel::SendMessageBP(const FMessage& Message, const UObject* WorldContextObject, const FLatentActionInfo LatentInfo, bool& bSuccess)
{
    TCallbackAction<bool>::CreateLatentAction(WorldContextObject, LatentInfo, bSuccess, [&](auto Callback) { SendMessage(Message, Callback); });
}

void UChatChannel::SendMessage(const FMessage& Message, const TFunction<void(const bool& bSuccess)> Callback)
{
    // TODO Wait for attachments to upload

    const FMessage NewMessage = PrepareMessage(Message);
    const FMessageRequestDto Request = NewMessage.ToRequestDto(Properties.Cid);
    AddMessage(NewMessage);
    Api->SendNewMessage(
        Properties.Type,
        Properties.Id,
        Request,
        false,
        [Callback](const TResponse<FMessageResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                // No need to add message here as the backend will send a websocket message
                UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Dto->Message.Id);
            }
            if (Callback)
            {
                Callback(Response.IsSuccessful());
            }
        });
    MessageSent.Broadcast(NewMessage);

    // TODO Retry logic
}

void UChatChannel::GetMessageBP(const FString& MessageId, const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FMessage& OutMessage)
{
}

void UChatChannel::GetMessage(const FString& MessageId, const TFunction<void(const FMessage&)> Callback)
{
    Api->GetMessage(
        MessageId,
        [Callback](const TResponse<FMessageResponseDto> Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (Callback)
                {
                    Callback(Util::Convert<FMessage>(Dto->Message, UUserManager::Get()));
                }
            }
        });
}

void UChatChannel::UpdateMessage(const FMessage& Message)
{
    // TODO Attachments

    FMessage UpdatedMessage = Message;
    UpdatedMessage.State = EMessageSendState::Updating;
    AddMessage(UpdatedMessage);

    const FMessageRequestDto Request = UpdatedMessage.ToRequestDto(Properties.Cid);
    Api->UpdateMessage(
        Request,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this)](const TResponse<FMessageResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->AddMessage(FMessage{Dto->Message, UUserManager::Get()});
                    UE_LOG(LogTemp, Log, TEXT("Updated message [Id=%s]"), *Dto->Message.Id);
                }
            }
        });
    // TODO retry?
}

void UChatChannel::DeleteMessage(const FMessage& Message)
{
    // TODO Attachments

    // Directly deleting the local messages which are not yet sent to server
    if (Message.State == EMessageSendState::Sending || Message.State == EMessageSendState::Failed)
    {
        FMessage DeletedMessage = Message;
        DeletedMessage.Type = EMessageType::Deleted;
        DeletedMessage.State = EMessageSendState::Sent;
        AddMessage(DeletedMessage);
        return;
    }

    FMessage DeletedMessage = Message;
    DeletedMessage.Type = EMessageType::Deleted;
    DeletedMessage.State = EMessageSendState::Deleting;
    if (!DeletedMessage.DeletedAt)
    {
        DeletedMessage.DeletedAt.Emplace(FDateTime::UtcNow());
    }
    AddMessage(DeletedMessage);

    Api->DeleteMessage(
        DeletedMessage.Id,
        false,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this)](const TResponse<FMessageResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->AddMessage(FMessage{Dto->Message, UUserManager::Get()});
                    UE_LOG(LogTemp, Log, TEXT("Deleted message [Id=%s]"), *Dto->Message.Id);
                }
            }
        });

    // TODO retry?
}

void UChatChannel::QueryAdditionalMessages(const EPaginationDirection Direction, const int32 Limit, const TFunction<void()> Callback)
{
    if (State.Messages.IsEmpty())
    {
        if (Callback)
        {
            Callback();
        }
        return;
    }

    const FMessagePaginationOptions MessagePagination = [&]
    {
        FMessagePaginationOptions Options;
        Options.Limit = Limit;
        if (Direction == EPaginationDirection::Top)
        {
            Options.IdLt = State.Messages.First().Id;
        }
        else
        {
            Options.IdGte = State.Messages.Last().Id;
        }
        return Options;
    }();

    Query(EChannelFlags::State, MessagePagination, {}, {}, Callback);
}

void UChatChannel::MarkRead(const TOptional<FMessage>& Message)
{
    const TOptional<FString> MessageId = Message.IsSet() ? Message.GetValue().Id : TOptional<FString>{};
    MarkRead(MessageId);
}

void UChatChannel::MarkRead(const FMessage& Message)
{
    const TOptional<FString> MessageId = Message.Id.IsEmpty() ? TOptional<FString>{} : Message.Id;
    MarkRead(MessageId);
}

void UChatChannel::MarkReadLocal()
{
    if (Properties.Config.bReadEvents && State.UnreadCount() > 0)
    {
        // Clear unread count straight away locally
        State.MarkRead();
        UnreadChanged.Broadcast(0);
    }
}

void UChatChannel::MarkRead(const TOptional<FString>& MessageId)
{
    if (Properties.Config.bReadEvents && State.UnreadCount() > 0)
    {
        Api->MarkChannelRead(Properties.Type, Properties.Id, MessageId);

        MarkReadLocal();
    }
}

void UChatChannel::Query(
    const EChannelFlags Flags,
    const TOptional<FMessagePaginationOptions> MessagePagination,
    const TOptional<FUserPaginationOptions> MemberPagination,
    const TOptional<FUserPaginationOptions> WatcherPagination,
    TFunction<void()> Callback)
{
    Api->QueryChannel(
        Properties.Type,
        Socket->GetConnectionId(),
        Flags,
        {},
        Properties.Id,
        Util::Convert<FMessagePaginationParamsRequestDto>(MessagePagination),
        Util::Convert<FPaginationParamsRequestDto>(MemberPagination),
        Util::Convert<FPaginationParamsRequestDto>(WatcherPagination),
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const TResponse<FChannelStateResponseDto>& Response)
        {
            if (!WeakThis.IsValid())
            {
                if (Callback)
                {
                    Callback();
                }
                return;
            }

            if (const auto* Dto = Response.Get())
            {
                WeakThis->MergeState(*Dto);

                if (Callback)
                {
                    Callback();
                }
            }
        });
}

void UChatChannel::SearchMessages(
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
        FFilter::Equal(TEXT("cid"), Properties.Cid).ToJsonObject(),
        Query,
        MessageFilterJson,
        Util::Convert<FSortParamRequestDto>(Sort),
        MessageLimit,
        {},
        {},
        [Callback](const TResponse<FSearchResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                // Don't add the messages to this channel's state, just return
                if (Callback)
                {
                    Callback(Util::Convert<FMessage>(Dto->Results, UUserManager::Get()));
                }
            }
        });
}

void UChatChannel::SearchMessages(
    const FString& Query,
    const FFilter& MessageFilter,
    const TArray<FMessageSortOption>& Sort,
    const int32 MessageLimit,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    TArray<FMessage>& OutMessages)
{
    const TOptional<FString> OptionalQuery = Query.IsEmpty() ? TOptional<FString>{} : Query;
    const TOptional<FFilter> OptionalMessageFilter = MessageFilter.IsValid() ? MessageFilter : TOptional<FFilter>{};
    const TOptional<uint32> OptionalMessageLimit = MessageLimit > 0 ? static_cast<uint32>(MessageLimit) : TOptional<uint32>{};
    TCallbackAction<TArray<FMessage>>::CreateLatentAction(
        WorldContextObject,
        LatentInfo,
        OutMessages,
        [&](auto Callback) { SearchMessages(OptionalQuery, OptionalMessageFilter, Sort, OptionalMessageLimit, Callback); });
}

FReaction UChatChannel::SendReaction(const FMessage& Message, const FName& ReactionType, const int32 Score, const bool bEnforceUnique)
{
    FMessage NewMessage{Message};
    // Remove all previous reactions current user did
    if (bEnforceUnique)
    {
        NewMessage.Reactions.RemoveReactionWhere([](const FReaction& R) { return R.User.IsCurrent(); });
    }

    const FReaction NewReaction{ReactionType, UUserManager::Get()->GetCurrentUser().User, Message.Id, Score > 1 ? Score : 1};
    NewMessage.Reactions.AddReaction(NewReaction, true);

    AddMessage(NewMessage);

    const TOptional<uint32> OptionalScore = Score > 1 ? static_cast<uint32>(Score) : TOptional<uint32>{};
    Api->SendReaction(Message.Id, ReactionType, bEnforceUnique, OptionalScore);
    return NewReaction;
}

void UChatChannel::GetReactions(const FMessage& Message, const FPaginationOptions& Pagination, TFunction<void(const TArray<FReaction>&)> Callback)
{
    Api->GetReactions(
        Message.Id,
        Pagination.GetLimitAsOptional(),
        Pagination.GetOffsetAsOptional(),
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback, NewMessage = Message](const TResponse<FGetReactionsResponseDto>& Response) mutable
        {
            if (const auto* Dto = Response.Get())
            {
                const TArray<FReaction> Reactions = Util::Convert<FReaction>(Dto->Reactions, UUserManager::Get());
                if (WeakThis.IsValid())
                {
                    for (auto&& Reaction : Reactions)
                    {
                        NewMessage.Reactions.AddReaction(Reaction, false);
                    }
                    WeakThis->AddMessage(NewMessage);
                }
                if (Callback)
                {
                    Callback(Reactions);
                }
            }
        });
}

void UChatChannel::DeleteReaction(const FMessage& Message, const FReaction& Reaction)
{
    FMessage NewMessage{Message};
    NewMessage.Reactions.RemoveReactionWhere([&Reaction](const FReaction& R)
                                             { return R.User == Reaction.User && R.Type == Reaction.Type && R.MessageId == Reaction.MessageId; });

    AddMessage(NewMessage);
    Api->DeleteReaction(Message.Id, Reaction.Type);
}

void UChatChannel::KeyStroke(const FString& ParentMessageId)
{
    if (!Properties.Config.bTypingEvents)
    {
        return;
    }

    const FDateTime Now = FDateTime::UtcNow();
    if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        UE_LOG(LogTemp, Log, TEXT("Start typing"));
        const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser().User;
        SendEvent(FTypingStartEvent{
            Now,
            Properties.Id,
            Properties.Type,
            Properties.Cid,
            ParentMessageId,
            Util::Convert<FUserObjectDto>(CurrentUser),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    GetWorld()->GetTimerManager().SetTimer(
        TypingTimerHandle,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), ParentMessageId]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            if (WeakThis->LastKeystrokeAt.IsSet() && (FDateTime::UtcNow() - WeakThis->LastKeystrokeAt.GetValue()).GetTotalSeconds() >= WeakThis->TypingTimeout)
            {
                WeakThis->SendStopTypingEvent(ParentMessageId);
            }
        },
        TypingTimeout,
        false);
}

void UChatChannel::StopTyping(const FString& ParentMessageId)
{
    if (!Properties.Config.bTypingEvents)
    {
        return;
    }

    if (TypingTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
    }
    if (LastKeystrokeAt.IsSet() && (FDateTime::UtcNow() - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        SendStopTypingEvent(ParentMessageId);
    }
}

void UChatChannel::SendStopTypingEvent(const FString& ParentMessageId)
{
    const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser().User;
    UE_LOG(LogTemp, Log, TEXT("Stop typing"));
    SendEvent(FTypingStopEvent{
        FDateTime::UtcNow(),
        Properties.Id,
        Properties.Type,
        Properties.Cid,
        ParentMessageId,
        Util::Convert<FUserObjectDto>(CurrentUser),
    });
}

void UChatChannel::OnTypingStart(const FTypingStartEvent& Event)
{
    const FUserRef User = UUserManager::Get()->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StartTyping, User);
}

void UChatChannel::OnTypingStop(const FTypingStopEvent& Event)
{
    const FUserRef User = UUserManager::Get()->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StopTyping, User);
}

void UChatChannel::BanMemberBP(const FUserRef& User, const FTimespan Timeout, const FString Reason, const bool bIpBan) const
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    const TOptional<FString> OptionalReason = Reason.IsEmpty() ? TOptional<FString>{} : Reason;
    BanMember(User, OptionalTimeout, OptionalReason, bIpBan);
}

void UChatChannel::BanMember(const FUserRef& User, const TOptional<FTimespan>& Timeout, const TOptional<FString>& Reason, const bool bIpBan) const
{
    Api->BanUser(User->Id, Properties.Type, Properties.Id, Timeout, Reason, false, bIpBan);
}

void UChatChannel::UnbanMember(const FUserRef& User) const
{
    Api->UnbanUser(User->Id, Properties.Type, Properties.Id);
}

void UChatChannel::ShadowBanMemberBP(const FUserRef& User, const FTimespan Timeout) const
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    BanMember(User, Timeout);
}

void UChatChannel::ShadowBanMember(const FUserRef& User, const TOptional<FTimespan>& Timeout) const
{
    Api->BanUser(User->Id, Properties.Type, Properties.Id, Timeout, {}, true);
}

void UChatChannel::ShadowUnbanMember(const FUserRef& User) const
{
    Api->UnbanUser(User->Id, Properties.Type, Properties.Id);
}

void UChatChannel::MuteBP(const FTimespan Timeout)
{
    const TOptional<FTimespan> OptionalTimeout = Timeout.IsZero() ? TOptional<FTimespan>{} : Timeout;
    Mute(OptionalTimeout);
}

void UChatChannel::Mute(const TOptional<FTimespan>& Timeout)
{
    Api->MuteChannels(
        {Properties.Cid},
        Timeout,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this)](const TResponse<FMuteChannelResponseDto>& Response)
        {
            if (const auto* Dto = Response.Get())
            {
                UUserManager::Get()->UpsertUser(Dto->OwnUser);
                if (WeakThis.IsValid())
                {
                    WeakThis->Properties.Merge(Dto->ChannelMute.Channel, UUserManager::Get());
                }
            }
        });
}

void UChatChannel::Unmute() const
{
    Api->UnmuteChannels({Properties.Cid});
}

bool UChatChannel::IsMuted() const
{
    return UUserManager::Get()->GetCurrentUser().MutedChannels.ContainsByPredicate([&](const FMutedChannel& C) { return C.Cid == Properties.Cid; });
}

void UChatChannel::EnableSlowMode(const FTimespan Cooldown)
{
    const TSharedRef<FJsonValueNumber> CooldownInSecond = MakeShared<FJsonValueNumber>(Cooldown.GetTotalSeconds());
    const TSharedRef<FJsonObject> Set = MakeShared<FJsonObject>();
    Set->SetField(TEXT("cooldown"), CooldownInSecond);
    PartialUpdate(Set);
}

void UChatChannel::DisableSlowMode()
{
    const TSharedRef<FJsonValueNumber> CooldownInSecond = MakeShared<FJsonValueNumber>(0.);
    const TSharedRef<FJsonObject> Set = MakeShared<FJsonObject>();
    Set->SetField(TEXT("cooldown"), CooldownInSecond);
    PartialUpdate(Set);
}

void UChatChannel::OnUserBanned(const FUserBannedEvent& Event)
{
    Properties.GetMember(UUserManager::Get()->UpsertUser(Event.User))->bBanned = true;
}

void UChatChannel::OnUserUnbanned(const FUserUnbannedEvent& Event)
{
    Properties.GetMember(UUserManager::Get()->UpsertUser(Event.User))->bBanned = false;
}

void UChatChannel::MergeState(const FChannelStateResponseFieldsDto& Dto)
{
    check(IsValid());
    UUserManager* UserManager = UUserManager::Get();
    State.Append(Dto, UserManager);
    MessagesUpdated.Broadcast();
}

void UChatChannel::AddMessage(const FMessage& Message)
{
    State.AddMessage(Message);

    MessagesUpdated.Broadcast();
}

FMessage MakeMessage(const FMessageEvent& Event)
{
    return FMessage{Event.Message, UUserManager::Get()};
}

void UChatChannel::OnMessageNew(const FMessageNewEvent& Event)
{
    HandleNewMessageEvent(Event);
}

void UChatChannel::OnNotificationMessageNew(const FNotificationMessageNewEvent& Event)
{
    HandleNewMessageEvent(Event);
}

void UChatChannel::HandleNewMessageEvent(const FMessageEvent& Event)
{
    const FMessage Message = MakeMessage(Event);
    AddMessage(Message);

    MessageReceived.Broadcast(Message);
    UnreadChanged.Broadcast(State.UnreadCount());
}

void UChatChannel::OnMessageUpdated(const FMessageUpdatedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnMessageDeleted(const FMessageDeletedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnMessageRead(const FMessageReadEvent& Event)
{
    const FUserRef EventUser = UUserManager::Get()->UpsertUser(Event.User);
    UpdateUnread(EventUser, 0, Event.CreatedAt);
}

void UChatChannel::OnNotificationMessageRead(const FNotificationMarkReadEvent& Event)
{
    const FUserRef EventUser = UUserManager::Get()->UpsertUser(Event.User);
    const int32 Unread = Event.TotalUnreadCount;
    UpdateUnread(EventUser, Unread, Event.CreatedAt);
}

void UChatChannel::UpdateUnread(const FUserRef& User, const int32 UnreadCount, const FDateTime& LastRead)
{
    if (FRead* Read = State.Read.FindByPredicate([&User](const FRead& R) { return R.User == User; }))
    {
        Read->UnreadMessages = UnreadCount;
        Read->LastRead = LastRead;
    }
    else
    {
        State.Read.Add(FRead{User, UnreadCount, LastRead});
    }
    if (User.IsCurrent())
    {
        UnreadChanged.Broadcast(UnreadCount);
    }
}

void UChatChannel::OnReactionNew(const FReactionNewEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnReactionUpdated(const FReactionUpdatedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnReactionDeleted(const FReactionDeletedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}
