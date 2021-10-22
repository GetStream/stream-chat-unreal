#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"

FMember::FMember(const FChannelMemberDto& Dto) : User(Dto.User)
{
}
