// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChatSocket.h"

#include "Containers/Ticker.h"
#include "Event/Client/ConnectionChangedEvent.h"
#include "Event/Client/ConnectionRecoveredEvent.h"
#include "Event/Client/HealthCheckEvent.h"
#include "IWebSocket.h"
#include "LogChatSocket.h"
#include "Request/ConnectRequestDto.h"
#include "Response/ErrorResponseDto.h"
#include "StreamChatWebSocketSettings.h"
#include "StreamJson.h"
#include "TokenManager.h"
#include "WebSocketCloseCodes.h"
#include "WebSocketsModule.h"

namespace
{
float GetReconnectDelay(const uint32 Attempt)
{
    // Try to reconnect in 0.25-25 seconds
    // Random to spread out the load from failures
    check(Attempt > 0);
    constexpr float HardMin = 0.25f;
    constexpr float HardMax = 25.f;
    const float Max = FMath::Min(0.5f + Attempt * 2.f, HardMax);
    const float Min = FMath::Min(FMath::Max(HardMin, (Attempt - 1) * 2.f), HardMax);
    return FMath::FRandRange(Min, Max);
}
}    // namespace

FChatSocket::FChatSocket(const TSharedRef<FTokenManager>& TokenManager, const FString& ApiKey, const FString& Host, const FUserObjectDto& User)
    : ChatSocketEvents(MakeUnique<FChatSocketEvents>()), TokenManager(TokenManager), ApiKey(ApiKey), Host(Host)
{
    const FConnectRequestDto Request = {true, User.Id, User};
    ConnectionRequestJson = Json::Serialize(Request);
    ConnectionRequestJson.ReplaceInline(TEXT(" "), TEXT("%20"));

    CreateUnderlyingWebSocket(false);
}

FChatSocket::~FChatSocket()
{
    Disconnect();
    UnbindEventHandlers();
}

void FChatSocket::Connect(const TFunction<void(const FOwnUserDto&)> Callback)
{
    if (ConnectionState == EConnectionState::Connecting)
    {
        UE_LOG(LogChatSocket, Error, TEXT("Attempted to connect while already connecting"));
        return;
    }

    UE_LOG(LogChatSocket, Log, TEXT("Initiating WebSocket connection"));
    SetConnectionState(EConnectionState::Connecting);

    PendingOnConnectCallback.BindLambda(Callback);

    ConnectUnderlyingWebSocket();
}

void FChatSocket::Disconnect()
{
    UE_LOG(LogChatSocket, Log, TEXT("Closing WebSocket connection"));
    SetConnectionState(EConnectionState::Disconnecting);

    CloseUnderlyingWebSocket();
    StopMonitoring();
}

FString FChatSocket::BuildUrl(const bool bRefreshToken) const
{
    const FString Token = TokenManager->LoadToken(bRefreshToken);
    return FString::Printf(TEXT("wss://%s/connect?json=%s&api_key=%s&authorization=%s&stream-auth-type=jwt"), *Host, *ConnectionRequestJson, *ApiKey, *Token);
}

bool FChatSocket::IsConnected() const
{
    return WebSocket->IsConnected() && ConnectionState == EConnectionState::Connected && !ConnectionId.IsEmpty();
}

const FString& FChatSocket::GetConnectionId() const
{
    return ConnectionId;
}

FChatSocketEvents& FChatSocket::Events()
{
    return *ChatSocketEvents;
}

void FChatSocket::CreateUnderlyingWebSocket(const bool bRefreshToken)
{
    const FString Url = BuildUrl(bRefreshToken);
    // Ensure WebSockets module is actually loaded
    FModuleManager::Get().LoadModule(TEXT("WebSockets"));
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT("wss"));
    UE_LOG(LogChatSocket, Log, TEXT("WebSocket configured with URL: %s"), *Url);
}

void FChatSocket::ConnectUnderlyingWebSocket()
{
    if (IsConnected())
    {
        UE_LOG(LogChatSocket, Error, TEXT("Trying to initialize a connected WebSocket"));
        return;
    }

    BindEventHandlers();
    WebSocket->Connect();
}

void FChatSocket::CloseUnderlyingWebSocket()
{
    UnbindEventHandlers();
    if (!WebSocket->IsConnected())
    {
        return;
    }

    WebSocket->Close(WebSocketCloseCode::GoingAway);
}

void FChatSocket::BindEventHandlers()
{
    // Reset any existing callbacks
    UnbindEventHandlers();
    WebSocket->OnConnected().AddSP(this, &FChatSocket::HandleWebSocketConnected);
    WebSocket->OnConnectionError().AddSP(this, &FChatSocket::HandleWebSocketConnectionError);
    WebSocket->OnClosed().AddSP(this, &FChatSocket::HandleWebSocketConnectionClosed);
    WebSocket->OnMessage().AddSP(this, &FChatSocket::HandleWebSocketMessage);

    HealthCheckEventDelegateHandle = Events().SubscribeSp<FHealthCheckEvent>(this, &FChatSocket::OnHealthCheckEvent);
}

void FChatSocket::UnbindEventHandlers()
{
    WebSocket->OnConnected().RemoveAll(this);
    WebSocket->OnConnectionError().RemoveAll(this);
    WebSocket->OnClosed().RemoveAll(this);
    WebSocket->OnMessage().RemoveAll(this);

    Events().Unsubscribe<FHealthCheckEvent>(HealthCheckEventDelegateHandle);
}

void FChatSocket::HandleWebSocketConnected()
{
    SetConnectionState(EConnectionState::Connected);
    UE_LOG(LogChatSocket, Log, TEXT("WebSocket connected"));
}

void FChatSocket::HandleWebSocketConnectionError(const FString& Error)
{
    SetConnectionState(EConnectionState::NotConnected);
    UE_LOG(LogChatSocket, Error, TEXT("Failed to connect to WebSocket [Error=%s]"), *Error);

    Reconnect(false);
}

void FChatSocket::HandleWebSocketConnectionClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
    SetConnectionState(EConnectionState::NotConnected);

    const TCHAR* Code = WebSocketCloseCode::ToString(Status);
    if (bWasClean)
    {
        UE_LOG(LogChatSocket, Log, TEXT("WebSocket connection closed [Status=%d (%s), Reason=%s]"), Status, Code, *Reason);
    }
    else
    {
        UE_LOG(LogChatSocket, Warning, TEXT("WebSocket connection closed unexpectedly [Status=%d (%s), Reason=%s]"), Status, Code, *Reason);
        Reconnect(false);
    }
}

void FChatSocket::HandleWebSocketMessage(const FString& JsonString)
{
    LastEventTime = FDateTime::UtcNow();
    UE_LOG(LogChatSocket, Verbose, TEXT("Websocket received message: %s"), *JsonString);

    TSharedPtr<FJsonObject> JsonObject;
    if (!JsonObjectDeserialization::JsonObjectStringToJsonObject(JsonString, JsonObject))
    {
        UE_LOG(LogChatSocket, Warning, TEXT("Unable to parse JSON [Json=%s]"), *JsonString);
        return;
    }

    FString Type;
    if (!JsonObject->TryGetStringField(TEXT("type"), Type))
    {
        if (const TSharedPtr<FJsonObject>* ErrorJsonObject; JsonObject->TryGetObjectField(TEXT("error"), ErrorJsonObject))
        {
            FErrorResponseDto ErrorResponse;
            if (JsonObjectDeserialization::JsonObjectToUStruct(ErrorJsonObject->ToSharedRef(), &ErrorResponse))
            {
                HandleChatError(ErrorResponse);
            }
            else
            {
                UE_LOG(LogChatSocket, Error, TEXT("Unable to deserialize WebSocket error event [Message=%s]"), *JsonString);
            }
        }
        else
        {
            UE_LOG(LogChatSocket, Error, TEXT("Trying to deserialize a WebSocket event with no type [JSON=%s]"), *JsonString);
        }
        return;
    }

    Events().ProcessEvent(FName{Type}, JsonObject.ToSharedRef());
}

void FChatSocket::HandleChatError(const FErrorResponseDto& Error)
{
    if (Error.IsTokenExpired())
    {
        UE_LOG(LogChatSocket, Warning, TEXT("WebSocket token expired [Message=%s]"), Error.Code, *Error.Message);
        Reconnect(true);
        return;
    }
    UE_LOG(LogChatSocket, Error, TEXT("WebSocket responded with error [Code=%d, Message=%s]"), Error.Code, *Error.Message);
    Reconnect(false);
}

void FChatSocket::OnHealthCheckEvent(const FHealthCheckEvent& HealthCheckEvent)
{
    UE_LOG(LogChatSocket, Verbose, TEXT("Health check received [ConnectionId=%s]"), *ConnectionId);

    ConnectionId = HealthCheckEvent.ConnectionId;

    const bool bIsConnectionEvent = !HealthCheckEvent.Me.Id.IsEmpty();
    if (bIsConnectionEvent)
    {
        OnHealthyConnect(HealthCheckEvent.Me);
    }
}

void FChatSocket::OnHealthyConnect(const FOwnUserDto& OwnUser)
{
    UE_LOG(LogChatSocket, Log, TEXT("Connection successful [ConnectionId=%s]"), *ConnectionId);

    if (PendingOnConnectCallback.IsBound())
    {
        PendingOnConnectCallback.Execute(OwnUser);
        PendingOnConnectCallback.Unbind();
    }

    StartMonitoring();
}

void FChatSocket::StartMonitoring()
{
    ReconnectAttempt = 0;

    // Reset any existing tickers
    StopMonitoring();
    KeepAliveTickerHandle = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FChatSocket::KeepAlive), GetDefault<UStreamChatWebSocketSettings>()->KeepAliveInterval);

    ReconnectTickerHandle = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FChatSocket::CheckNeedToReconnect), GetDefault<UStreamChatWebSocketSettings>()->CheckReconnectInterval);
}

void FChatSocket::StopMonitoring()
{
    if (KeepAliveTickerHandle.IsValid())
    {
        FTicker::GetCoreTicker().RemoveTicker(KeepAliveTickerHandle);
        KeepAliveTickerHandle.Reset();
    }

    if (ReconnectTickerHandle.IsValid())
    {
        FTicker::GetCoreTicker().RemoveTicker(ReconnectTickerHandle);
        ReconnectTickerHandle.Reset();
    }
}

bool FChatSocket::KeepAlive(float) const
{
    if (IsConnected())
    {
        UE_LOG(LogChatSocket, Verbose, TEXT("Sending keep alive signal"));
        WebSocket->Send(TEXT(""));
    }

    // Loop the ticker
    return true;
}

bool FChatSocket::CheckNeedToReconnect(float)
{
    const bool bShouldReconnect = [&]
    {
        if (!LastEventTime)
        {
            return false;
        }

        const float Delta = (FDateTime::UtcNow() - LastEventTime.GetValue()).GetTotalSeconds();
        return Delta > GetDefault<UStreamChatWebSocketSettings>()->ReconnectionTimeout;
    }();
    if (bShouldReconnect)
    {
        Reconnect(false);
    }
    // Loop the ticker
    return true;
}

void FChatSocket::Reconnect(const bool bRefreshToken)
{
    if (ConnectionState == EConnectionState::Reconnecting)
    {
        return;
    }

    SetConnectionState(EConnectionState::Reconnecting);

    CloseUnderlyingWebSocket();
    if (bRefreshToken)
    {
        WebSocket.Reset();
        CreateUnderlyingWebSocket(true);
    }

    StopMonitoring();

    ++ReconnectAttempt;

    // Delay the actual reconnection attempt
    const float Delay = GetReconnectDelay(ReconnectAttempt);

    UE_LOG(LogChatSocket, Log, TEXT("Enqueuing a reconnecting attempt [Attempt=%d, Delay=%g]"), ReconnectAttempt, Delay);
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            // Use a weak pointer to this FChatSocket in case it is destroyed by the time the ticker fires
            [WeakThis = TWeakPtr<IChatSocket>(AsShared())](float)
            {
                if (const TSharedPtr<FChatSocket> Pinned = StaticCastSharedPtr<FChatSocket>(WeakThis.Pin()))
                {
                    UE_LOG(LogChatSocket, Log, TEXT("Retrying connection [Attempt=%d]"), Pinned->ReconnectAttempt);
                    Pinned->ConnectUnderlyingWebSocket();
                }

                // Don't loop the ticker
                return false;
            }),
        Delay);
}

void FChatSocket::SetConnectionState(const EConnectionState NewState)
{
    const bool bWasOnline = ConnectionState == EConnectionState::Connected;
    const bool bOnline = NewState == EConnectionState::Connected;

    ConnectionState = NewState;

    if (bWasOnline != bOnline)
    {
        Events().Broadcast(FConnectionChangedEvent{{{FConnectionChangedEvent::StaticType, bOnline}}});

        if (bOnline && IsConnected())
        {
            UE_LOG(LogChatSocket, Log, TEXT("Connection recovered"));
            Events().Broadcast(FConnectionRecoveredEvent{{{FConnectionRecoveredEvent::StaticType}}});
        }
    }
}
