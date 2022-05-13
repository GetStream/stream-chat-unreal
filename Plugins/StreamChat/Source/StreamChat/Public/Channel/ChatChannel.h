// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelProperties.h"
#include "ChannelSortOption.h"
#include "ChannelState.h"
#include "ChatApi.h"
#include "ChatSocketEvents.h"
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/LatentActionManager.h"
#include "Filter.h"
#include "IChatSocket.h"
#include "Message.h"
#include "MessagePaginationOptions.h"
#include "PaginationOptions.h"
#include "User/UserPaginationOptions.h"
#include "User/UserRef.h"

#include "ChatChannel.generated.h"

class UStreamChatClientComponent;
struct FChannelEvent;
struct FChannelStateResponseDto;
struct FChannelStateResponseFieldsDto;
struct FMessageDeletedEvent;
struct FMessageEvent;
struct FMessageNewEvent;
struct FMessageReadEvent;
struct FMessageUpdatedEvent;
struct FNotificationMarkReadEvent;
struct FNotificationMessageNewEvent;
struct FReactionDeletedEvent;
struct FReactionNewEvent;
struct FReactionUpdatedEvent;
struct FTypingStartEvent;
struct FTypingStopEvent;

UENUM(BlueprintType)
enum class ETypingIndicatorState : uint8
{
    StartTyping,
    StopTyping
};

UENUM(BlueprintType, meta = (BitFlags))
enum class EPaginationDirection : uint8
{
    None UMETA(Hidden),

    /// Query earlier messages
    Top = 1 << 0,
    /// Query later messages
    Bottom = 1 << 1,
};
ENUM_CLASS_FLAGS(EPaginationDirection);

UENUM(BlueprintType)
enum class EHttpRequestState : uint8
{
    Started,
    Ended
};

/**
 * @brief The client-side representation of a Stream Chat channel
 * @ingroup StreamChat
 */
UCLASS(BlueprintType)
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    /// Create a new channel object
    static UChatChannel* Create(UObject* Outer, const TSharedRef<FChatApi>, const TSharedRef<IChatSocket>, const FChannelStateResponseFieldsDto&);

    /**
     * @brief Delete this channel from the server.
     * @param Callback Called when response is received.
     */
    void Delete(TFunction<void()> Callback = {}) const;

    /**
     * @brief Updates certain fields of the channel
     * @param Set Sets new field values
     * @param Unset Array of field names to unset
     * @param Callback Called when response is received.
     */
    void PartialUpdate(const TSharedRef<FJsonObject>& Set = {}, const TArray<FString>& Unset = {}, TFunction<void()> Callback = {});

    /**
     * @brief Change channel data. Any data that is present on the channel and not included in a full update will be deleted.
     * @param Data Properties to set on the channel
     * @param Message Optionally include a message object that will be used to populate a system message
     * @param Callback Called when response is received.
     */
    void Update(const FAdditionalFields& Data, const TOptional<FMessage>& Message = {}, TFunction<void()> Callback = {});

    /**
     * @brief List and paginate members for this channel.
     * @param Filter Conditions to use to filter the members
     * @param Sort The sorting used for the members matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param Pagination Limit, offset, and member selection by user id
     * GtId, GteId, LtId and LteId fields are for user IDs
     * @param Callback Called when response is received.
     */
    void QueryMembers(
        const FFilter& Filter = {},
        const TArray<FUserSortOption>& Sort = {},
        const FMessagePaginationOptions& Pagination = {},
        TFunction<void(const TArray<FMember>&)> Callback = {});

    /**
     * @brief Add given users as members
     * @param MemberIds IDs of users to add to the channel as members
     * @param Message Optionally include a message object that will be used to populate a system message
     * @param Callback Called when response is received.
     */
    void AddMembers(const TArray<FString>& MemberIds, const TOptional<FMessage>& Message = {}, TFunction<void()> Callback = {});

    /**
     * @brief Remove given users as members
     * @param MemberIds IDs of users to remove from the channel as members
     * @param Message Optionally include a message object that will be used to populate a system message
     * @param Callback Called when response is received.
     */
    void RemoveMembers(const TArray<FString>& MemberIds, const TOptional<FMessage>& Message = {}, TFunction<void()> Callback = {});

    /**
     * @brief Remove this channel from query channel requests for the current user until a new message is added.
     * Hiding a channel is only available to members of that channel.
     * @param bClearHistory Clear the entire message history of that channel for the user
     * @param Callback Called when response is received.
     */
    void Hide(bool bClearHistory = false, TFunction<void()> Callback = {}) const;

    /**
     * @brief Un-hide this channel. It will again appear in query channel requests for the current user.
     * @param Callback Called when response is received.
     */
    void Show(TFunction<void()> Callback = {}) const;

    /**
     * @brief Stop receiving channel events
     * @param Callback Called when response is received.
     */
    void StopWatching(TFunction<void()> Callback = {}) const;

    /// The local static properties of the channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel") FChannelProperties Properties;

    /// The local state of the channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FChannelState State;

private:
    void MergeState(const FChannelStateResponseFieldsDto&);

    TSharedPtr<FChatApi> Api;
    TSharedPtr<IChatSocket> Socket;

#pragma region Event
    /** @name Event
     *  Send, subscribe and unsubscribe to events
     *  @{
     */

public:
    /// Send a custom event on this channel
    template <class TEvent>
    void SendEvent(const TEvent&);

    /// Subscribe to a channel event using your own delegate object
    template <class TEvent>
    FDelegateHandle On(TEventDelegate<TEvent> Callback);

    /**
     * @brief Subscribe to a channel event using a UObject-based member function delegate.
     * @tparam TEvent Event type
     * @param Obj UObject with given method
     * @param Method Method to call when event occurs. Should have signature: void OnMyEvent(const TEvent& Event)
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, class UserClass>
    typename TEnableIf<TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type On(
        UserClass* Obj,
        TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method);

    /**
     * @brief Subscribe to a channel event using a shared pointer-based (fast, not thread-safe) member function
     * delegate.
     * @tparam TEvent Event type
     * @param Obj Object with given method
     * @param Method Method to call when event occurs. Should have signature: void OnMyEvent(const TEvent& Event)
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, class UserClass>
    typename TEnableIf<!TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type On(
        UserClass* Obj,
        TEventDelegateSpMethodPtr<TEvent, UserClass> Method);

    /**
     * @brief Subscribe to a channel event using a C++ lambda
     * @tparam TEvent Event type
     * @param Functor Lambda to execute when event occurs. Should have signature similar to: [](const TEvent& Event){}
     * @param Vars Additional variables to pass to the lambda
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, typename FunctorType, typename... VarTypes>
    typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type On(FunctorType&& Functor, VarTypes... Vars);

    /**
     * @brief Unsubscribe from a previously subscribed channel event
     *
     * @tparam TEvent Event type
     * @return Whether anything was unsubscribed from
     */
    template <class TEvent>
    bool Unsubscribe(FDelegateHandle) const;

    ///@}
#pragma endregion Event

#pragma region Message
    /** @name Message
     *  Paginate, send, update and delete messages.
     *  @{
     */

public:
    /**
     * @brief Send a message on this channel
     *
     * @param Message A message struct containing information of the message to be sent
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Message")
    void SendMessage(const FMessage& Message);

    /**
     * @brief Update (user might see "Edit") a message which exists on this channel
     *
     * @param Message Message which already exists in this channel
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Message")
    void UpdateMessage(const FMessage& Message);

    /**
     * @brief Soft delete a message in this channel
     *
     * @param Message Message which already exists in this channel
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Message")
    void DeleteMessage(const FMessage& Message);

    /**
     * @brief Fetch more messages from the server.
     *
     * @attention Some messages must already have been fetched for this to do anything.
     * @param Direction Top if the user is scrolling up, Bottom if the user is scrolling down
     * @param Limit Number of messages returned is limited by this value. Maximum 200.
     * @param Callback Called when a response is received from the backend
     */
    void QueryAdditionalMessages(EPaginationDirection Direction = EPaginationDirection::Top, int32 Limit = 20, TFunction<void()> Callback = {});

    /**
     * @brief Manually mark all messages in this channel as read by the current user
     * @param Message Optionally provide a Message that is considered last read
     */
    void MarkRead(const TOptional<FMessage>& Message = {});
    /**
     * @brief Manually mark all messages in this channel as read by the current user
     * @param Message Optionally provide a Message that is considered last read
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Message", meta = (AutoCreateRefTerm = Message))
    void MarkRead(const FMessage& Message);

    /**
     * @brief Get messages, members or other channel fields.
     * @param Callback Called when response is received.
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param MessagePagination Pagination details for returned messages.
     * @see https://getstream.io/chat/docs/unreal/channel_pagination/
     * @param MemberPagination Pagination details for returned members.
     * @param WatcherPagination Pagination details for returned watchers/
     */
    void Query(
        EChannelFlags Flags = EChannelFlags::State,
        const TOptional<FMessagePaginationOptions> MessagePagination = {},
        const TOptional<FUserPaginationOptions> MemberPagination = {},
        const TOptional<FUserPaginationOptions> WatcherPagination = {},
        TFunction<void()> Callback = {});

    /**
     * @brief Search the messages in this channel
     *
     * All parameters are optional, but a minimum of either a query or message filter is required
     *
     * @param Query Search phrase
     * @param MessageFilter Message filter conditions
     * @param Sort Sort parameters. Cannot be  used with non-zero offset.
     * @param MessageLimit Number of messages to return
     * @param Callback Called when response is received
     */
    void SearchMessages(
        const TOptional<FString>& Query = {},
        const TOptional<FFilter>& MessageFilter = {},
        const TArray<FMessageSortOption>& Sort = {},
        TOptional<uint32> MessageLimit = {},
        TFunction<void(const TArray<FMessage>&)> Callback = {}) const;

    /**
     * @brief Search the messages in this channel
     *
     * All parameters are optional, but a minimum of either a query or message filter is required
     *
     * @param Query Search phrase (Optional)
     * @param MessageFilter Message filter conditions (Optional)
     * @param Sort Sort parameters (Optional)
     * @param MessageLimit Number of messages to return (Optional)
     * @param OutMessages The result of the search
     */
    UFUNCTION(
        BlueprintCallable,
        Category = "Stream Chat|Channel",
        meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo, AutoCreateRefTerm = "Sort,MessageFilter"))
    void SearchMessages(
        const FString& Query,
        const FFilter& MessageFilter,
        const TArray<FMessageSortOption>& Sort,
        int32 MessageLimit,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        TArray<FMessage>& OutMessages);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMessagesUpdatedDelegate);
    /// Fired when any of the messages we have locally change
    UPROPERTY(BlueprintAssignable, Category = "Stream Chat|Channel")
    FMessagesUpdatedDelegate MessagesUpdated;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageDelegate, const FMessage&, Messages);
    /// Fired when a message is first sent client-side
    UPROPERTY(BlueprintAssignable, Category = "Stream Chat|Channel")
    FMessageDelegate MessageSent;

    /// Fired when a message is first received from the server (but not updated, deleted, etc)
    UPROPERTY(BlueprintAssignable, Category = "Stream Chat|Channel")
    FMessageDelegate MessageReceived;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnreadChangedDelegate, int32, UnreadCount);
    /// Fired when the unread count changes for the current user
    UPROPERTY(BlueprintAssignable, Category = "Stream Chat|Channel")
    FUnreadChangedDelegate UnreadChanged;

private:
    void AddMessage(const FMessage&);
    void MarkRead(const TOptional<FString>& MessageId);

    void OnMessageUpdated(const FMessageUpdatedEvent&);
    void OnMessageDeleted(const FMessageDeletedEvent&);

    void OnMessageNew(const FMessageNewEvent&);
    void OnNotificationMessageNew(const FNotificationMessageNewEvent&);
    void HandleNewMessageEvent(const FMessageEvent&);

    void OnMessageRead(const FMessageReadEvent&);
    void OnNotificationMessageRead(const FNotificationMarkReadEvent&);
    void UpdateUnread(const FUserRef& User, int32 UnreadCount, const FDateTime& LastRead);

    ///@}
#pragma endregion Message

#pragma region Reaction
    /** @name Reaction
     *  Send and delete reactions.
     *  @{
     */

public:
    /**
     * @brief Add a reaction to a given message
     *
     * @param Message The message in this channel to react to
     * @param ReactionType The type of reaction to send
     * @param bEnforceUnique Should all other reactions from this user be removed
     * @param Score The score of the reaction. Defaults to 1.
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Reaction")
    void SendReaction(const FMessage& Message, const FName& ReactionType, int32 Score = 1, bool bEnforceUnique = false);

    /**
     * @brief Fetch or paginate more reactions for a given message
     *
     * @param Message The message in this channel to fetch reactions for
     * @param Pagination Limit and offset for pagination of reactions
     * @param Callback Called when response is received
     */
    void GetReactions(const FMessage& Message, const FPaginationOptions& Pagination = {}, TFunction<void(const TArray<FReaction>&)> Callback = {});

    /**
     * @brief Remove a reaction from a given message
     *
     * @param Message The message in this channel to react to
     * @param Reaction The reaction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Reaction")
    void DeleteReaction(const FMessage& Message, const FReaction& Reaction);

private:
    void OnReactionNew(const FReactionNewEvent&);
    void OnReactionUpdated(const FReactionUpdatedEvent&);
    void OnReactionDeleted(const FReactionDeletedEvent&);

    ///@}
#pragma endregion Reaction

#pragma region Typing
    /** @name Typing
     *  @{
     */

public:
    /**
     * @brief Should be called on every keystroke. Sends `typing.start` and `typing.stop` events accordingly.
     * @param ParentMessageId In the case of a thread, the ID of the parent message (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Typing", meta = (AdvancedDisplay = ParentMessageId))
    void KeyStroke(const FString& ParentMessageId = TEXT(""));

    /**
     * @brief Should be called when the user sends the message (or aborts).
     * Manually sends the typing.stop event, and cancels any pending typing.stop event queued by Keystroke()
     * @param ParentMessageId In the case of a thread, the ID of the parent message (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Typing", meta = (AdvancedDisplay = ParentMessageId))
    void StopTyping(const FString& ParentMessageId = TEXT(""));

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTypingIndicatorDelegate, ETypingIndicatorState, TypingState, const FUserRef&, User);
    /// Fired whenever any user starts or stops typing
    UPROPERTY(BlueprintAssignable)
    FTypingIndicatorDelegate OnTypingIndicator;

private:
    void SendStopTypingEvent(const FString& ParentMessageId = TEXT(""));
    void OnTypingStart(const FTypingStartEvent&);
    void OnTypingStop(const FTypingStopEvent&);

    float TypingTimeout = 2.f;
    TOptional<FDateTime> LastKeystrokeAt;
    FTimerHandle TypingTimerHandle;

    ///@}
#pragma endregion Typing

#pragma region Moderation
    /** @name Moderation
     *  @{
     */

    /**
     * @brief Ban a member from this channel.
     * When a user is banned, they will not be allowed to post messages until the ban is
     * removed or expired but will be able to connect to Chat and to channels as before.
     * @param User User to ban
     * @param Timeout Timeout of ban in minutes. User will be unbanned after this period of time (unlimited if zero)
     * @param Reason Ban reason (optional)
     * @param bIpBan Whether to perform IP ban or not
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Channel|Moderation", DisplayName = "Ban Member")
    void BanMemberBP(const FUserRef& User, FTimespan Timeout, FString Reason, bool bIpBan) const;
    void BanMember(const FUserRef& User, const TOptional<FTimespan>& Timeout = {}, const TOptional<FString>& Reason = {}, bool bIpBan = false) const;

    /**
     * @brief Remove previously applied ban
     * @param User User to unban
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Moderation")
    void UnbanMember(const FUserRef& User) const;

    /**
     * @brief Shadow ban a member from this channel.
     * When a user is shadow banned, they will still be allowed to post messages,
     * but any message sent during the will only be visible to the messages author and invisible to other users of the App.
     * @param User User to shadow ban
     * @param Timeout Timeout of ban in minutes. User will be unbanned after this period of time (never unbanned if zero)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Channel|Moderation", DisplayName = "Shadow Ban Member")
    void ShadowBanMemberBP(const FUserRef& User, FTimespan Timeout) const;
    void ShadowBanMember(const FUserRef& User, const TOptional<FTimespan>& Timeout = {}) const;

    /**
     * @brief Remove previously applied shadow ban
     * @param User User to unban
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Channel|Moderation")
    void ShadowUnbanMember(const FUserRef& User) const;

    /**
     * @brief Mutes channel for current user
     * @param Timeout Duration of mute (never unmuted if left zero)
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Moderation", DisplayName = "Mute")
    void MuteBP(FTimespan Timeout);
    void Mute(const TOptional<FTimespan>& Timeout = {});

    /**
     * @brief Unmutes channel for current user
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Moderation")
    void Unmute() const;

    /**
     * @brief Is this channel muted for the current user
     */
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel|Moderation")
    bool IsMuted() const;

    ///@}
#pragma endregion Moderation
};

template <class TEvent>
void UChatChannel::SendEvent(const TEvent& Event)
{
    Api->SendChannelEvent(Properties.Type, Properties.Id, Event);
}

template <class TEvent>
FDelegateHandle UChatChannel::On(TEventDelegate<TEvent> Callback)
{
    return Socket->Events().SubscribeLambda<TEvent>(
        [this, Callback](const TEvent& Event)
        {
            // TODO static assert with nice error message
            // https://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class
            if (Event.Cid == Properties.Cid)
            {
                Callback.ExecuteIfBound(Event);
            }
        });
}

template <class TEvent, class UserClass>
typename TEnableIf<TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type UChatChannel::On(
    UserClass* Obj,
    TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateUObject(Obj, Method);
    return On<TEvent>(Delegate);
}

template <class TEvent, class UserClass>
typename TEnableIf<!TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type UChatChannel::On(
    UserClass* Obj,
    TEventDelegateSpMethodPtr<TEvent, UserClass> Method)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateSP(Obj, Method);
    return On<TEvent>(Delegate);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type UChatChannel::On(
    FunctorType&& Functor,
    VarTypes... Vars)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateLambda(Forward<FunctorType>(Functor), Vars...);
    return On<TEvent>(Delegate);
}

template <class TEvent>
bool UChatChannel::Unsubscribe(const FDelegateHandle Handle) const
{
    return Socket->Events().Unsubscribe<TEvent>(Handle);
}
