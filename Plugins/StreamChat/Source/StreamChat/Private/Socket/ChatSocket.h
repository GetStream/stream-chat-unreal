#pragma once

#include "CoreMinimal.h"
#include "EventSubscription.h"
#include "Token/TokenManager.h"

struct FUser;
struct FHealthCheckEvent;
class IWebSocket;

class FChatSocket : public TSharedFromThis<FChatSocket>
{
public:
    explicit FChatSocket(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);
    ~FChatSocket();
    void Connect(TFunction<void()> Callback);
    void Disconnect();

    bool IsConnected() const;
    const FString& GetConnectionId() const;

    template <class TEvent>
    FDelegateHandle SubscribeToEvent(TEventReceivedDelegate<TEvent> Callback);

private:
    static FString BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);

    // WS event handlers
    void HandleWebSocketConnected();
    void HandleWebSocketConnectionError(const FString& Error);
    void HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean);
    void HandleWebSocketMessage(const FString& Message);

    void OnHealthCheckEvent(const FHealthCheckEvent&, TFunction<void()> Callback);

    template <class TEvent>
    TEventSubscription<TEvent>& GetSubscription();

    TMap<FName, TUniquePtr<IEventSubscription>> Subscriptions;

    /**
     * Provided by the server when the websocket connection is established
     */
    FString ConnectionId;

    TSharedPtr<IWebSocket> WebSocket;

    bool bClosePending = false;
};

template <class TEvent>
FDelegateHandle FChatSocket::SubscribeToEvent(TEventReceivedDelegate<TEvent> Callback)
{
    return GetSubscription<TEvent>().Delegate.Add(MoveTemp(Callback));
}

template <class TEvent>
TEventSubscription<TEvent>& FChatSocket::GetSubscription()
{
    // TODO don't like the double StaticCast
    const FName& EventType = TEvent::StaticType;
    if (const TUniquePtr<IEventSubscription>* Subscription = Subscriptions.Find(EventType))
    {
        return StaticCast<TEventSubscription<TEvent>&>(**Subscription);
    }
    const TUniquePtr<IEventSubscription>& Subscription =
        Subscriptions.Emplace(EventType, MakeUnique<TEventSubscription<TEvent>>());
    return StaticCast<TEventSubscription<TEvent>&>(*Subscription);
}
