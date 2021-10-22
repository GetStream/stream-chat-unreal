// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChannelConfig.h"
#include "Channel/Message.h"
#include "ChatApi.h"
#include "ChatSocketEvents.h"
#include "CoreMinimal.h"
#include "IChatSocket.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

struct FChannelEvent;
class UStreamChatClientComponent;
struct FChannelStateResponseDto;
struct FChannelStateResponseFieldsDto;
struct FMessageDeletedEvent;
struct FMessageNewEvent;
struct FMessageUpdatedEvent;
struct FReactionDeletedEvent;
struct FReactionNewEvent;
struct FReactionUpdatedEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagesUpdatedDelegate, const TArray<FMessage>&, Messages);

/**
 * The client-side representation of a Stream Chat channel
 */
UCLASS(BlueprintType)
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel* Create(UStreamChatClientComponent&, const FString& Type, const FString& Id);
    static UChatChannel* Create(UStreamChatClientComponent&, const FChannelStateResponseFieldsDto&);

    void Watch(TFunction<void()> Callback = {});

    const FString& GetCid() const;

private:
    void InitializeState(const FChannelStateResponseFieldsDto&);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Id;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Name;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString ImageUrl;

    /// The cid of this channel
    FString Cid;

    FChannelConfig Config;
    FUser User;
    TSharedPtr<FChatApi> Api;
    TSharedPtr<IChatSocket> Socket;

#pragma region Event

public:
    template <class TEvent>
    void SendEvent(const TEvent&);

    /// Subscribe to a channel event using your own delegate object
    template <class TEvent>
    FDelegateHandle On(TEventDelegate<TEvent> Callback);

    /**
     * Subscribe to a channel event using a UObject-based member function delegate.
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
     * Subscribe to a channel event using a shared pointer-based (fast, not thread-safe) member function delegate.
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
     * Subscribe to a channel event using a C++ lambda
     * @tparam TEvent Event type
     * @param Functor Lambda to execute when event occurs. Should have signature similar to: [](const TEvent& Event){}
     * @param Vars Additional variables to pass to the lambda
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, typename FunctorType, typename... VarTypes>
    FORCEINLINE FDelegateHandle On(FunctorType&& Functor, VarTypes... Vars);

    template <class TEvent>
    bool Unsubscribe(FDelegateHandle) const;

#pragma endregion Event

#pragma region Message

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Message")
    void SendMessage(const FString& Text, const FUser& FromUser);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Message")
    void UpdateMessage(const FMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Message")
    void DeleteMessage(const FMessage& Message);

    UFUNCTION(BlueprintPure, Category = "Stream Chat Channel|Message")
    const TArray<FMessage>& GetMessages() const;

    UPROPERTY(BlueprintAssignable, Category = "Stream Chat Channel")
    FMessagesUpdatedDelegate MessagesUpdated;

private:
    void AddMessage(const FMessage&);

    void OnMessageNew(const FMessageNewEvent&);
    void OnMessageUpdated(const FMessageUpdatedEvent&);
    void OnMessageDeleted(const FMessageDeletedEvent&);

    TArray<FMessage> Messages;

#pragma endregion Message

#pragma region Reaction

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Reaction")
    void SendReaction(const FMessage& Message, const FName& ReactionType, bool bEnforceUnique = true);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Reaction")
    void DeleteReaction(const FMessage& Message, const FReaction& Reaction);

private:
    void OnReactionNew(const FReactionNewEvent&);
    void OnReactionUpdated(const FReactionUpdatedEvent&);
    void OnReactionDeleted(const FReactionDeletedEvent&);

public:
    /**
     * Should be called on every keystroke. Sends typing.start and typing.stop events accordingly.
     * @param ParentMessageId In the case of a thread, the ID of the parent message (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel|Typing", meta = (AdvancedDisplay = ParentMessageId))
    void KeyStroke(const FString& ParentMessageId = TEXT(""));

private:
    TOptional<FDateTime> LastKeystrokeAt;

#pragma endregion Reaction
};

template <class TEvent>
void UChatChannel::SendEvent(const TEvent& Event)
{
    Api->SendChannelEvent(Type, Id, Event);
}

template <class TEvent>
FDelegateHandle UChatChannel::On(TEventDelegate<TEvent> Callback)
{
    return Socket->Events().SubscribeLambda<TEvent>(
        [this, Callback](const TEvent& Event)
        {
            // TODO static assert with nice error message
            // https://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class
            if (Event.Cid == Cid)
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
FDelegateHandle UChatChannel::On(FunctorType&& Functor, VarTypes... Vars)
{
    const TEventDelegate<TEvent> Delegate =
        TEventDelegate<TEvent>::CreateLambda(Forward<FunctorType>(Functor), Vars...);
    return On<TEvent>(Delegate);
}

template <class TEvent>
bool UChatChannel::Unsubscribe(const FDelegateHandle Handle) const
{
    return Socket->Events().Unsubscribe<TEvent>(Handle);
}
