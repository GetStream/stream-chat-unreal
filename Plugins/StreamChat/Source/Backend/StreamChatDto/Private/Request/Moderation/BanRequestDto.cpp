#include "Request/Moderation/BanRequestDto.h"

void FBanRequestDto::SetTimeout(const float Timeout)
{
    AdditionalFields.SetNumber("timeout", Timeout);
}
