#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"
#include "User/UserManager.h"

FMember::FMember(FUserManager& UserManager, const FChannelMemberDto& Dto) : User{UserManager.UpsertUser(Dto.User)}
{
}
