#pragma once

#include "CoreMinimal.h"
#include "Token/TokenManager.h"

struct FUser;
struct FNewMessageEvent;
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

    DECLARE_MULTICAST_DELEGATE_OneParam(FNewMessageEventDelegate, const FNewMessageEvent&);
    FNewMessageEventDelegate NewMessageEventDelegate;

private:
    static FString BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);

    // WS event handlers
    void HandleWebSocketConnected();
    void HandleWebSocketConnectionError(const FString& Error);
    void HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean);
    void HandleWebSocketMessage(const FString& Message);

    DECLARE_DELEGATE(FHealthCheckEventDelegate);
    FHealthCheckEventDelegate HealthCheckEventDelegate;

    /**
     * Provided by the server when the websocket connection is established
     */
    FString ConnectionId;

    TSharedPtr<IWebSocket> WebSocket;

    bool bClosePending = false;
};
