#pragma once

#include "ChatSocketEvents.h"
#include "CoreMinimal.h"
#include "IChatSocket.h"

struct FUserObjectDto;
struct FHealthCheckEvent;
class IWebSocket;

class FChatSocket final : public IChatSocket
{
public:
    FChatSocket() = delete;
    explicit FChatSocket(const FString& ApiKey, const FString& Token, const FString& Host, const FUserObjectDto& User);
    virtual ~FChatSocket() override;

    virtual void Connect(TFunction<void()> Callback) override;
    virtual void Disconnect() override;
    virtual bool IsConnected() const override;
    virtual const FString& GetConnectionId() const override;

    virtual FChatSocketEvents& Events() override;

private:
    enum class EConnectionState
    {
        NotConnected,
        Connecting,
        Connected,
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

    void SetConnectionState(EConnectionState);

    static FString
    BuildUrl(const FString& ApiKey, const FString& Token, const FString& Host, const FUserObjectDto& User);
    TUniquePtr<FChatSocketEvents> ChatSocketEvents;

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
