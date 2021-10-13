#include "User.h"

#include "Dto/Response/UserDto.h"

FUser::FUser(const FUserDto& Dto) : Id(Dto.Id)
{
}

FUser::FUser(const FString& InId) : Id(InId)
{
}
