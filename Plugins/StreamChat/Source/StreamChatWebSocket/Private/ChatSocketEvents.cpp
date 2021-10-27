#include "ChatSocketEvents.h"

#include "LogChatSocket.h"
#include "Response/ErrorResponseDto.h"

void FChatSocketEvents::ProcessEvent(const FName& Type, const TSharedRef<FJsonObject> JsonObject)
{
    const FEventSubscriptionPtr* Subscription = Subscriptions.Find(Type);
    if (!Subscription)
    {
        UE_LOG(
            LogChatSocket,
            Warning,
            TEXT("No subscriptions to WebSocket event. Discarding. [type=%s]"),
            *Type.ToString());
        return;
    }

    if (!Subscription->Get()->OnMessage(JsonObject))
    {
        UE_LOG(
            LogChatSocket,
            Warning,
            TEXT("Unable to deserialize WebSocket event [type=%s]"),
            *JsonObject->GetStringField(TEXT("type")));
    }
}
