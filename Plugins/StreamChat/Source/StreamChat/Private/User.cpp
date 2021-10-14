#include "User.h"

#include "UserDto.h"

FUser::FUser(const FUserDto& Dto) : Id(Dto.Id)
{
}

FUser::operator FUserDto() const
{
    return {Id};
}

FUser::FUser(const FString& InId) : Id(InId)
{
}
