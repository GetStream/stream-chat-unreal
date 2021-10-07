﻿#include "ChatSocket.h"

#include "Dto/Event/HealthCheckEvent.h"
#include "Dto/Request/ConnectRequest.h"
#include "IWebSocket.h"
#include "StreamChatSettings.h"
#include "StreamJson/Public/StreamJson.h"
#include "WebSocketsModule.h"

FChatSocket::FChatSocket(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager)
{
    const FString Url = BuildUrl(ApiKey, User, TokenManager);
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT(""));
    UE_LOG(LogTemp, Log, TEXT("Websocket configured with URL: %s"), *Url);
}

FChatSocket::~FChatSocket()
{
    if (WebSocket)
    {
        if (IsConnected())
        {
            UE_LOG(LogTemp, Warning, TEXT("Websocket destroyed without closing"));
            Disconnect();
        }

        WebSocket->OnConnected().RemoveAll(this);
        WebSocket->OnConnectionError().RemoveAll(this);
        WebSocket->OnClosed().RemoveAll(this);
        WebSocket->OnRawMessage().RemoveAll(this);
    }
}

void FChatSocket::Connect(TFunction<void()> Callback)
{
    WebSocket->OnConnected().AddSP(this, &FChatSocket::HandleWebSocketConnected);
    WebSocket->OnConnectionError().AddSP(this, &FChatSocket::HandleWebSocketConnectionError);
    WebSocket->OnClosed().AddSP(this, &FChatSocket::HandleWebSocketConnectionClosed);
    WebSocket->OnMessage().AddSP(this, &FChatSocket::HandleWebSocketMessage);

    UE_LOG(LogTemp, Log, TEXT("Initiating websocket connection"));
    WebSocket->Connect();

    OnHealthCheckEvent.AddLambda(Callback);
}

void FChatSocket::Disconnect()
{
    if (IsConnected())
    {
        UE_LOG(LogTemp, Log, TEXT("Closing websocket connection"));
        bClosePending = true;
        WebSocket->Close(/* going away */ 1001);
    }
}

FString FChatSocket::BuildUrl(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager)
{
    const FString Domain = GetDefault<UStreamChatSettings>()->Host;
    const FString Token = TokenManager.LoadToken();
    const FConnectRequest Request = {
        true,
        User.Id,
        User,
    };
    const FString Json = Json::Serialize(Request);
    return FString::Printf(
        TEXT("wss://%s/connect?json=%s&api_key=%s&authorization=%s&stream-auth-type=jwt"),
        *Domain,
        *Json,
        *ApiKey,
        *Token);
}

bool FChatSocket::IsConnected() const
{
    return WebSocket && WebSocket->IsConnected() && !bClosePending;
}

const FString& FChatSocket::GetConnectionId() const
{
    return ConnectionId;
}

void FChatSocket::HandleWebSocketConnected()
{
    UE_LOG(LogTemp, Log, TEXT("Websocket connected"));
}

void FChatSocket::HandleWebSocketConnectionError(const FString& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Websocket error :%s"), *Error);
}

void FChatSocket::HandleWebSocketConnectionClosed(int32 Status, const FString& Reason, bool bWasClean)
{
    bClosePending = false;

    // TODO Human readable status codes
    if (bWasClean)
    {
        UE_LOG(LogTemp, Log, TEXT("Websocket connection closed [Status=%d, Reason=%s]"), Status, *Reason);
    }
    else
    {
        UE_LOG(
            LogTemp, Warning, TEXT("Websocket connection closed unexpectedly [Status=%d, Reason=%s]"), Status, *Reason);
    }
}

void FChatSocket::HandleWebSocketMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Websocket received message: %s"), *Message);

    // TODO Support more than just health.check
    FHealthCheckEvent Event = Json::Deserialize<FHealthCheckEvent>(Message);
    if (Event.Type == TEXT("health.check"))
    {
        ConnectionId = Event.ConnectionId;
        OnHealthCheckEvent.Broadcast();
    }
}