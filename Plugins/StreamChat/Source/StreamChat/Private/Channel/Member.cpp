#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"
#include "User/UserManager.h"

FMember::FMember(const FUserRef& User) : User{User}
{
}

FMember::FMember(FUserManager& UserManager, const FChannelMemberDto& Dto) : User{UserManager.UpsertUser(Dto.User)}
{
}
