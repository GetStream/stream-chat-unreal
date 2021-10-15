#include "ChatSocketEvents.h"

#include "LogChatSocket.h"
#include "Response/ErrorResponseDto.h"

void FChatSocketEvents::ProcessEvent(const FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    if (!JsonObjectDeserialization::JsonObjectStringToJsonObject(JsonString, JsonObject))
    {
        UE_LOG(LogChatSocket, Warning, TEXT("Unable to parse JSON [Json=%s]"), *JsonString);
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
                    LogChatSocket,
                    Error,
                    TEXT("Unable to deserialize WebSocket error event [Message=%s]"),
                    *JsonString);
            }
        }
        else
        {
            UE_LOG(
                LogChatSocket,
                Error,
                TEXT("Trying to deserialize a WebSocket event with no type [JSON=%s]"),
                *JsonString);
        }
        return;
    }

    const FEventSubscriptionPtr* Subscription = Subscriptions.Find(FName(Type));
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
