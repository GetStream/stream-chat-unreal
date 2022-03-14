#include "Request/Channel/QueryChannelsRequestDto.h"

FQueryChannelsRequestDto::FQueryChannelsRequestDto(
    const uint32 MessageLimit,
    const uint32 MemberLimit,
    const FString& ConnectionId,
    const bool bPresence,
    const TArray<FSortParamRequestDto>& Sort,
    const bool bState,
    const bool bWatch,
    const TOptional<uint32>& Limit,
    const TOptional<uint32>& Offset,
    const FJsonObjectWrapper& Filter)
    : FPaginatedRequest{Limit, Offset, Filter}
    , MessageLimit{MessageLimit}
    , MemberLimit{MemberLimit}
    , ConnectionId{ConnectionId}
    , bPresence{bPresence}
    , Sort{Sort}
    , bState{bState}
    , bWatch{bWatch}
{
}
