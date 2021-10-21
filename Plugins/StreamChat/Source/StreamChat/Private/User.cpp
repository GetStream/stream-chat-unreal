#include "User.h"

#include "UserObjectDto.h"

FUser::FUser(const FUserObjectDto& Dto) : Id(Dto.Id)
{
}

FUser::operator FUserObjectDto() const
{
    return {Id};
}

FUser::FUser(const FString& InId) : Id(InId)
{
}
