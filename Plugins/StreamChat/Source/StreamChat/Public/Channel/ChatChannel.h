// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
#include "User/UserRef.h"
#include "UserPaginationOptions.h"

#include "ChatChannel.generated.h"

class UStreamChatClientComponent;
struct FChannelEvent;
struct FChannelStateResponseDto;
struct FChannelStateResponseFieldsDto;
struct FMessageDeletedEvent;
struct FMessageNewEvent;
struct FMessageUpdatedEvent;
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

    /// The local static properties of the channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FChannelProperties Properties;

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
     */
    ///@{

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
     */
    ///@{

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
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Message")
    void QueryAdditionalMessages(EPaginationDirection Direction = EPaginationDirection::Top, int32 Limit = 20);

    /// Get all the messages in this channel which we have locally
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel|Message")
    const TArray<FMessage>& GetMessages() const;

    /**
     * @brief Get messages, members or other channel fields.
     * @param Callback Called when response is received.
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param MessagePagination Pagination details for returned messages.
     * @see https://getstream.io/chat/docs/other-rest/channel_pagination/
     * @param MemberPagination Pagination details for returned members.
     * @param WatcherPagination Pagination details for returned watchers/
     */
    void Query(
        TFunction<void()> Callback,
        EChannelFlags Flags = EChannelFlags::State,
        const TOptional<FMessagePaginationOptions> MessagePagination = {},
        const TOptional<FUserPaginationOptions> MemberPagination = {},
        const TOptional<FUserPaginationOptions> WatcherPagination = {});

    /**
     * @brief Search the messages in this channel
     *
     * All parameters are optional, but a minimum of either a query or message filter is required
     *
     * @param Callback Called when response is received
     * @param Query Search phrase
     * @param MessageFilter Message filter conditions
     * @param Sort Sort parameters. Cannot be  used with non-zero offset.
     * @param MessageLimit Number of messages to return
     */
    void SearchMessages(
        TFunction<void(const TArray<FMessage>&)> Callback,
        const TOptional<FString>& Query = {},
        const TOptional<FFilter>& MessageFilter = {},
        const TArray<FMessageSortOption>& Sort = {},
        TOptional<uint32> MessageLimit = {}) const;

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
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo, AutoCreateRefTerm = "Sort,MessageFilter"))
    void SearchMessages(
        const FString& Query,
        const FFilter& MessageFilter,
        const TArray<FMessageSortOption>& Sort,
        int32 MessageLimit,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        TArray<FMessage>& OutMessages);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagesUpdatedDelegate, const TArray<FMessage>&, Messages);
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

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPaginatingMessagesDelegate, EPaginationDirection, Direction, EHttpRequestState, RequestState);
    /// Fired during various stages of message pagination
    UPROPERTY(BlueprintAssignable)
    FPaginatingMessagesDelegate OnPaginatingMessages;

private:
    void AddMessage(const FMessage&);

    void OnMessageNew(const FMessageNewEvent&);
    void OnMessageUpdated(const FMessageUpdatedEvent&);
    void OnMessageDeleted(const FMessageDeletedEvent&);

    void SetPaginationRequestState(EHttpRequestState, EPaginationDirection);
    EPaginationDirection EndedPaginationDirections = EPaginationDirection::None;
    EHttpRequestState PaginationRequestState = EHttpRequestState::Ended;

    ///@}
#pragma endregion Message

#pragma region Reaction
    /** @name Reaction
     *  Send and delete reactions.
     */
    ///@{

public:
    /**
     * @brief Add a reaction to a given message
     *
     * @param Message The message in this channel to react to
     * @param ReactionType The type of reaction to send
     * @param bEnforceUnique Should all other reactions from this user be removed
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Channel|Reaction")
    void SendReaction(const FMessage& Message, const FName& ReactionType, bool bEnforceUnique = true);

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
     */
    ///@{

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

    TOptional<FDateTime> LastKeystrokeAt;
    FTimerHandle TypingTimerHandle;

    ///@}
#pragma endregion Typing
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
