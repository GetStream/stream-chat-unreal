#pragma once

#include "CoreMinimal.h"
#include "Detail/ChatSocketDetail.h"
#include "EventSubscription.h"
#include "Token/TokenManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogChatSocket, Verbose, All);

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
    // WS event handlers
    void HandleWebSocketConnected();
    void HandleWebSocketConnectionError(const FString& Error);
    void HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean);
    void HandleWebSocketMessage(const FString& Message);

    void OnHealthCheckEvent(const FHealthCheckEvent&, TFunction<void()> Callback);
    void OnConnect(TFunction<void()> Callback);

    bool KeepAlive(float DeltaTime) const;

    static FString BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);

    TMap<FName, TUniquePtr<IEventSubscription>> Subscriptions;

    /// Provided by the server when the websocket connection is established
    FString ConnectionId;

    TSharedPtr<IWebSocket> WebSocket;

    bool bClosePending = false;
    FDelegateHandle KeepAliveTickerHandle;
};

template <class TEvent>
FDelegateHandle FChatSocket::SubscribeToEvent(TEventReceivedDelegate<TEvent> Callback)
{
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Callback);
}
