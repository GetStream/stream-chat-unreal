#include "Response/Message/MessageDto.h"

#include "StreamJson.h"

void FMessageDto::DeserializeExtra(const FJsonObject& JsonObject, FMessageDto& Self)
{
    Json::DeserializeField(JsonObject, TEXT("deleted_at"), Self.DeletedAt);
}
