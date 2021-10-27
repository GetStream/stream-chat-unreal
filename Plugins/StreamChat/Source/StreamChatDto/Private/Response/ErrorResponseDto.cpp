#include "Response/ErrorResponseDto.h"

bool FErrorResponseDto::IsTokenExpired() const
{
    return Code == 40;
}
