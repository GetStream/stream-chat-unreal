#include "ChatSocket.h"

#include "Dto/Event/HealthCheckEvent.h"
#include "Dto/Event/NewMessageEvent.h"
#include "Dto/Request/ConnectRequest.h"
#include "IWebSocket.h"
#include "StreamChatSettings.h"
#include "StreamJson/Public/StreamJson.h"
#include "WebSocketCloseCodes.h"
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

void FChatSocket::Connect(const TFunction<void()> Callback)
{
    WebSocket->OnConnected().AddSP(this, &FChatSocket::HandleWebSocketConnected);
    WebSocket->OnConnectionError().AddSP(this, &FChatSocket::HandleWebSocketConnectionError);
    WebSocket->OnClosed().AddSP(this, &FChatSocket::HandleWebSocketConnectionClosed);
    WebSocket->OnMessage().AddSP(this, &FChatSocket::HandleWebSocketMessage);

    UE_LOG(LogTemp, Log, TEXT("Initiating websocket connection"));
    WebSocket->Connect();

    SubscribeToEvent<FHealthCheckEvent>(
        TEventReceivedDelegate<FHealthCheckEvent>::CreateSP(this, &FChatSocket::OnHealthCheckEvent, Callback));
}

void FChatSocket::Disconnect()
{
    if (IsConnected())
    {
        UE_LOG(LogTemp, Log, TEXT("Closing websocket connection"));
        bClosePending = true;
        WebSocket->Close(WebSocketCloseCode::GoingAway);
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

void FChatSocket::HandleWebSocketConnectionClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
    bClosePending = false;

    const TCHAR* Code = WebSocketCloseCode::ToString(Status);
    if (bWasClean)
    {
        UE_LOG(LogTemp, Log, TEXT("Websocket connection closed [Status=%s, Reason=%s]"), Code, *Reason);
    }
    else
    {
        UE_LOG(
            LogTemp, Warning, TEXT("Websocket connection closed unexpectedly [Status=%s, Reason=%s]"), Code, *Reason);
    }
}

void FChatSocket::HandleWebSocketMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Websocket received message: %s"), *Message);

    // TODO we end up deserializing twice to determine the event type: here and in TEventSubscription<::OnMessage
    const auto [Type, Cid, CreatedAt] = Json::Deserialize<FChatEvent>(Message);
    if (Type.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("Trying to deserialize a WebSocket event with no type"));
        return;
    }
    if (const TUniquePtr<IEventSubscription>* Subscription = Subscriptions.Find(Type))
    {
        Subscription->Get()->OnMessage(Message);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No subscriptions to WebSocket event. Discarding. [Type=%s]"), *Type.ToString());
    }
}

void FChatSocket::OnHealthCheckEvent(const FHealthCheckEvent& HealthCheckEvent, const TFunction<void()> Callback)
{
    ConnectionId = HealthCheckEvent.ConnectionId;
    if (Callback)
    {
        Callback();
    }
}
