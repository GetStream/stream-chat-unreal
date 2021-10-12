#include "ChatSocket.h"

#include "Api/Dto/Response/ErrorResponseDto.h"
#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Dto/Event/HealthCheckEvent.h"
#include "Dto/Request/ConnectRequest.h"
#include "IWebSocket.h"
#include "StreamChatSettings.h"
#include "StreamJson.h"
#include "WebSocketCloseCodes.h"
#include "WebSocketsModule.h"

DEFINE_LOG_CATEGORY(LogChatSocket);

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

FChatSocket::FChatSocket(const FString& ApiKey, const FUser& User, const FTokenManager& TokenManager)
{
    const FString Url = BuildUrl(ApiKey, User, TokenManager);
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
    ConnectionState = EConnectionState::Connecting;

    PendingOnConnectCallback.BindLambda(Callback);

    InitWebSocket();
}

void FChatSocket::Disconnect()
{
    UE_LOG(LogChatSocket, Log, TEXT("Closing WebSocket connection"));
    ConnectionState = EConnectionState::Disconnecting;

    CloseWebSocket();
    StopMonitoring();
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
    return WebSocket && WebSocket->IsConnected();
}

bool FChatSocket::IsHealthy() const
{
    return IsConnected() && ConnectionState == EConnectionState::Healthy;
}

const FString& FChatSocket::GetConnectionId() const
{
    return ConnectionId;
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
    if (!IsConnected())
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

    HealthCheckEventDelegateHandle = SubscribeToEvent<FHealthCheckEvent>(
        TEventReceivedDelegate<FHealthCheckEvent>::CreateSP(this, &FChatSocket::OnHealthCheckEvent));
}

void FChatSocket::UnbindEventHandlers()
{
    WebSocket->OnConnected().RemoveAll(this);
    WebSocket->OnConnectionError().RemoveAll(this);
    WebSocket->OnClosed().RemoveAll(this);
    WebSocket->OnMessage().RemoveAll(this);

    UnsubscribeFromEvent<FHealthCheckEvent>(HealthCheckEventDelegateHandle);
}

void FChatSocket::HandleWebSocketConnected()
{
    ConnectionState = EConnectionState::Connected;
    UE_LOG(LogChatSocket, Log, TEXT("WebSocket connected"));
}

void FChatSocket::HandleWebSocketConnectionError(const FString& Error)
{
    ConnectionState = EConnectionState::NotConnected;
    UE_LOG(LogChatSocket, Error, TEXT("Failed to connect to WebSocket [Error=%s]"), *Error);

    Reconnect();
}

void FChatSocket::HandleWebSocketConnectionClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
    ConnectionState = EConnectionState::NotConnected;

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

    TSharedPtr<FJsonObject> JsonObject;
    if (!JsonObjectDeserialization::JsonObjectStringToJsonObject(Message, JsonObject))
    {
        UE_LOG(LogChatSocket, Warning, TEXT("Unable to parse JSON [Json=%s]"), *Message);
        return;
    }

    FString Type;
    if (!JsonObject->TryGetStringField(TEXT("type"), Type))
    {
        if (const TSharedPtr<FJsonObject>* ErrorJsonObject;
            JsonObject->TryGetObjectField(TEXT("error"), ErrorJsonObject))
        {
            FErrorResponseDto ErrorResponse;
            if (JsonObjectDeserialization::JsonObjectToUStruct(ErrorJsonObject->ToSharedRef(), &ErrorResponse))
            {
                UE_LOG(
                    LogChatSocket,
                    Error,
                    TEXT("WebSocket responded with error [Code=%d, Message=%s]"),
                    ErrorResponse.Code,
                    *ErrorResponse.Message);
            }
            else
            {
                UE_LOG(
                    LogChatSocket, Error, TEXT("Unable to deserialize WebSocket error event [Message=%s]"), *Message);
            }
        }
        else
        {
            UE_LOG(
                LogChatSocket, Error, TEXT("Trying to deserialize a WebSocket event with no type [JSON=%s]"), *Message);
        }
        return;
    }

    const TUniquePtr<IEventSubscription>* Subscription = Subscriptions.Find(FName(Type));
    if (!Subscription)
    {
        UE_LOG(LogChatSocket, Warning, TEXT("No subscriptions to WebSocket event. Discarding. [type=%s]"), *Type);
        return;
    }

    if (!Subscription->Get()->OnMessage(JsonObject.ToSharedRef()))
    {
        UE_LOG(
            LogChatSocket,
            Warning,
            TEXT("Unable to deserialize WebSocket event [type=%s]"),
            *JsonObject->GetStringField(TEXT("type")));
    }
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
    ConnectionState = EConnectionState::Healthy;
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
        GetDefault<UStreamChatSettings>()->WebSocketKeepAliveInterval);

    ReconnectTickerHandle = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FChatSocket::CheckNeedToReconnect),
        GetDefault<UStreamChatSettings>()->WebSocketCheckReconnectInterval);
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
    if (IsHealthy())
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
        return Delta > GetDefault<UStreamChatSettings>()->WebSocketReconnectionTimeout;
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
    ConnectionState = EConnectionState::Reconnecting;

    CloseWebSocket();
    StopMonitoring();

    ++ReconnectAttempt;

    // Delay the actual reconnection attempt
    const float Delay = GetReconnectDelay(ReconnectAttempt);

    UE_LOG(
        LogChatSocket, Log, TEXT("Enqueuing a reconnecting attempt [Attempt=%d, Delay=%g]"), ReconnectAttempt, Delay);
    FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            [WeakThis = TWeakPtr<FChatSocket>(AsShared())](float)
            {
                if (const TSharedPtr<FChatSocket> Pinned = WeakThis.Pin())
                {
                    UE_LOG(LogChatSocket, Log, TEXT("Retrying connection [Attempt=%d]"), Pinned->ReconnectAttempt);
                    Pinned->InitWebSocket();
                }

                // Don't loop the ticker
                return false;
            }),
        Delay);
}
