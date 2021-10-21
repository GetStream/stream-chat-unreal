#include "ChatSocket.h"

#include "Containers/Ticker.h"
#include "Event/Client/ConnectionChangedEvent.h"
#include "Event/Client/ConnectionRecoveredEvent.h"
#include "Event/Client/HealthCheckEvent.h"
#include "IWebSocket.h"
#include "LogChatSocket.h"
#include "Request/ConnectRequest.h"
#include "StreamChatWebSocketSettings.h"
#include "StreamJson.h"
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

FChatSocket::FChatSocket(const FString& ApiKey, const FString& Token, const FString& Host, const FUserObjectDto& User)
    : ChatSocketEvents(MakeUnique<FChatSocketEvents>())
{
    const FString Url = BuildUrl(ApiKey, Token, Host, User);
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT("wss"));
    UE_LOG(LogChatSocket, Log, TEXT("WebSocket configured with URL: %s"), *Url);
}

FChatSocket::~FChatSocket()
{
    Disconnect();
    UnbindEventHandlers();
}

void FChatSocket::Connect(const TFunction<void()> Callback)
{
    if (ConnectionState == EConnectionState::Connecting)
    {
        UE_LOG(LogChatSocket, Error, TEXT("Attempted to connect while already connecting"));
        return;
    }

    UE_LOG(LogChatSocket, Log, TEXT("Initiating WebSocket connection"));
    SetConnectionState(EConnectionState::Connecting);

    PendingOnConnectCallback.BindLambda(Callback);

    InitWebSocket();
}

void FChatSocket::Disconnect()
{
    UE_LOG(LogChatSocket, Log, TEXT("Closing WebSocket connection"));
    SetConnectionState(EConnectionState::Disconnecting);

    CloseWebSocket();
    StopMonitoring();
}

FString
FChatSocket::BuildUrl(const FString& ApiKey, const FString& Token, const FString& Host, const FUserObjectDto& User)
{
    const FConnectRequestDto Request = {
        true,
        User.Id,
        User,
    };
    const FString Json = Json::Serialize(Request);
    return FString::Printf(
        TEXT("wss://%s/connect?json=%s&api_key=%s&authorization=%s&stream-auth-type=jwt"),
        *Host,
        *Json,
        *ApiKey,
        *Token);
}

bool FChatSocket::IsConnected() const
{
    return WebSocket->IsConnected() && ConnectionState == EConnectionState::Connected;
}

const FString& FChatSocket::GetConnectionId() const
{
    return ConnectionId;
}

FChatSocketEvents& FChatSocket::Events()
{
    return *ChatSocketEvents;
}

void FChatSocket::InitWebSocket()
{
    if (IsConnected())
    {
        UE_LOG(LogChatSocket, Error, TEXT("Trying to initialize a connected WebSocket"));
        return;
    }

    BindEventHandlers();
    WebSocket->Connect();
}

void FChatSocket::CloseWebSocket()
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

    Reconnect();
}

void FChatSocket::HandleWebSocketConnectionClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
    SetConnectionState(EConnectionState::NotConnected);

    const TCHAR* Code = WebSocketCloseCode::ToString(Status);
    if (bWasClean)
    {
        UE_LOG(
            LogChatSocket, Log, TEXT("WebSocket connection closed [Status=%d (%s), Reason=%s]"), Status, Code, *Reason);
    }
    else
    {
        UE_LOG(
            LogChatSocket,
            Warning,
            TEXT("WebSocket connection closed unexpectedly [Status=%d (%s), Reason=%s]"),
            Status,
            Code,
            *Reason);
        Reconnect();
    }
}

void FChatSocket::HandleWebSocketMessage(const FString& Message)
{
    LastEventTime = FDateTime::Now();
    UE_LOG(LogChatSocket, Verbose, TEXT("Websocket received message: %s"), *Message);

    Events().ProcessEvent(Message);
}

void FChatSocket::OnHealthCheckEvent(const FHealthCheckEvent& HealthCheckEvent)
{
    UE_LOG(LogChatSocket, Verbose, TEXT("Health check received [ConnectionId=%s]"), *ConnectionId);

    ConnectionId = HealthCheckEvent.ConnectionId;

    const bool bIsConnectionEvent = !HealthCheckEvent.Me.Id.IsEmpty();
    if (bIsConnectionEvent)
    {
        OnHealthyConnect();
    }
}

void FChatSocket::OnHealthyConnect()
{
    UE_LOG(LogChatSocket, Log, TEXT("Connection successful [ConnectionId=%s]"), *ConnectionId);

    if (PendingOnConnectCallback.IsBound())
    {
        PendingOnConnectCallback.Execute();
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
        FTickerDelegate::CreateSP(this, &FChatSocket::KeepAlive),
        GetDefault<UStreamChatWebSocketSettings>()->KeepAliveInterval);

    ReconnectTickerHandle = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FChatSocket::CheckNeedToReconnect),
        GetDefault<UStreamChatWebSocketSettings>()->CheckReconnectInterval);
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

        const float Delta = (FDateTime::Now() - LastEventTime.GetValue()).GetTotalSeconds();
        return Delta > GetDefault<UStreamChatWebSocketSettings>()->ReconnectionTimeout;
    }();
    if (bShouldReconnect)
    {
        Reconnect();
    }
    // Loop the ticker
    return true;
}

void FChatSocket::Reconnect()
{
    if (ConnectionState == EConnectionState::Reconnecting)
    {
        return;
    }

    SetConnectionState(EConnectionState::Reconnecting);

    CloseWebSocket();
    StopMonitoring();

    ++ReconnectAttempt;

    // Delay the actual reconnection attempt
    const float Delay = GetReconnectDelay(ReconnectAttempt);

    UE_LOG(
        LogChatSocket, Log, TEXT("Enqueuing a reconnecting attempt [Attempt=%d, Delay=%g]"), ReconnectAttempt, Delay);
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            // Use a weak pointer to this FChatSocket in case it is destroyed by the time the ticker fires
            [WeakThis = TWeakPtr<IChatSocket>(AsShared())](float)
            {
                if (const TSharedPtr<FChatSocket> Pinned = StaticCastSharedPtr<FChatSocket>(WeakThis.Pin()))
                {
                    UE_LOG(LogChatSocket, Log, TEXT("Retrying connection [Attempt=%d]"), Pinned->ReconnectAttempt);
                    Pinned->InitWebSocket();
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
    if (bWasOnline != bOnline)
    {
        Events().Broadcast(FConnectionChangedEvent{{{FConnectionChangedEvent::StaticType, bOnline}}});

        if (bOnline)
        {
            // TODO offline support: recovered connection
            Events().Broadcast(FConnectionRecoveredEvent{{{FConnectionRecoveredEvent::StaticType}}});
        }
    }

    ConnectionState = NewState;
}
