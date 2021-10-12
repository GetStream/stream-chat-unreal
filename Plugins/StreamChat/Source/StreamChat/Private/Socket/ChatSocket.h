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
    bool IsHealthy() const;
    const FString& GetConnectionId() const;

    template <class TEvent>
    FDelegateHandle SubscribeToEvent(TEventReceivedDelegate<TEvent> Callback);
    template <class TEvent>
    bool UnsubscribeFromEvent(FDelegateHandle);

private:
    enum class EConnectionState
    {
        NotConnected,
        Connecting,
        Connected,
        Healthy,
        Disconnecting,
        Reconnecting,
    };

    void InitWebSocket();
    void CloseWebSocket();

    // WS event handlers
    void BindEventHandlers();
    void UnbindEventHandlers();
    void HandleWebSocketConnected();
    void HandleWebSocketConnectionError(const FString& Error);
    void HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean);
    void HandleWebSocketMessage(const FString& Message);

    void OnHealthCheckEvent(const FHealthCheckEvent&);
    void OnHealthyConnect();

    void StartMonitoring();
    void StopMonitoring();
    bool KeepAlive(float DeltaTime) const;
    bool CheckNeedToReconnect(float DeltaTime);
    void Reconnect();

    static FString BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);

    TMap<FName, TUniquePtr<IEventSubscription>> Subscriptions;

    /// Provided by the server when the websocket connection is established
    FString ConnectionId;

    TSharedPtr<IWebSocket> WebSocket;

    FSimpleDelegate PendingOnConnectCallback;
    EConnectionState ConnectionState = EConnectionState::NotConnected;
    FDelegateHandle HealthCheckEventDelegateHandle;
    FDelegateHandle KeepAliveTickerHandle;
    FDelegateHandle ReconnectTickerHandle;
    TOptional<FDateTime> LastEventTime;
    uint32 ReconnectAttempt = 0;
};

template <class TEvent>
FDelegateHandle FChatSocket::SubscribeToEvent(TEventReceivedDelegate<TEvent> Callback)
{
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Callback);
}

template <class TEvent>
bool FChatSocket::UnsubscribeFromEvent(const FDelegateHandle DelegateHandle)
{
    return Detail::UnsubscribeFromEvent<TEvent>(Subscriptions, DelegateHandle);
}
