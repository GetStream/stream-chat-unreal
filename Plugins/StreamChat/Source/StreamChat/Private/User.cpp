#include "User.h"

#include "UserObjectDto.h"

FUser::FUser(const FUserObjectDto& Dto)
    : Id{Dto.Id}
    , bOnline{Dto.bOnline}
    , CreatedAt(Dto.CreatedAt)
    , UpdatedAt{Dto.UpdatedAt}
    , LastActive{Dto.LastActive}
    , Name{Dto.Name}
    , Image{Dto.Image}
{
}

FUser::operator FUserObjectDto() const
{
    return {Id, bOnline, CreatedAt, UpdatedAt, LastActive, Name, Image};
}

FUser::FUser(const FString& InId) : Id(InId), bOnline(false)
{
}

FString FUser::GetInitials(const int32 Limit) const
{
    FString Initials;
    Initials.Reserve(Limit);
    int32 Pos = 0;
    while (Pos < Name.Len())
    {
        while (Pos < Name.Len() && FChar::IsWhitespace((*Name)[Pos]))
        {
            Pos++;
        }
        Initials.AppendChar(Name[Pos]);
        if (Initials.Len() >= Limit)
        {
            break;
        }
        while (Pos < Name.Len() && !FChar::IsWhitespace((*Name)[Pos]))
        {
            Pos++;
        }
    }
    Initials.ToUpperInline();
    return Initials;
}
