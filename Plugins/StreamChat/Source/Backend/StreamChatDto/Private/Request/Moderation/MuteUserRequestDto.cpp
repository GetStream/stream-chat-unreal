#include "Request/Moderation/MuteUserRequestDto.h"

void FMuteUserRequestDto::SetTimeout(const float Timeout)
{
    AdditionalFields.SetNumber("timeout", Timeout);
}
