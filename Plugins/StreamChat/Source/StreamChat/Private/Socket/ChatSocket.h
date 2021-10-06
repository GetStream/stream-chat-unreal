#pragma once

#include "CoreMinimal.h"
#include "Token/TokenManager.h"

struct FUser;
class IWebSocket;

class FChatSocket : public TSharedFromThis<FChatSocket>
{
public:
    explicit FChatSocket(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);
    ~FChatSocket();
    void Connect(TFunction<void()> Callback);
    void Disconnect();

private:
    static FString BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager);
    bool IsConnected() const;

    // WS event handlers
    void HandleWebSocketConnected();
    void HandleWebSocketConnectionError(const FString& Error);
    void HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean);
    void HandleWebSocketMessage(const FString& Message);

    TSharedPtr<IWebSocket> WebSocket;

    bool bClosePending = false;
};
