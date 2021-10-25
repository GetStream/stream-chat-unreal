#include "Request/Channel/MessagePaginationParamsRequestDto.h"

#include "StreamJson.h"

void FMessagePaginationParamsRequestDto::SerializeExtra(
    const FMessagePaginationParamsRequestDto& Self,
    FJsonObject& JsonObject)
{
    Json::SerializeField(Self.IdLt, TEXT("id_lt"), JsonObject);
    Json::SerializeField(Self.IdLte, TEXT("id_lte"), JsonObject);
    Json::SerializeField(Self.IdGt, TEXT("id_gt"), JsonObject);
    Json::SerializeField(Self.IdGte, TEXT("id_gte"), JsonObject);
    Json::SerializeField(Self.CreatedAtAfter, TEXT("created_at_after"), JsonObject);
    Json::SerializeField(Self.CreatedAtAfterOrEqual, TEXT("created_at_after_or_equal"), JsonObject);
    Json::SerializeField(Self.CreatedAtBefore, TEXT("created_at_before"), JsonObject);
    Json::SerializeField(Self.CreatedAtBeforeOrEqual, TEXT("created_at_before_or_equal"), JsonObject);
}
