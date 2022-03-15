#include "Moderation/Ban.h"

#include "StreamChatClientComponent.h"
#include "User/UserManager.h"

FBan::FBan(const FBanResponseDto& Dto, UStreamChatClientComponent* Client, UUserManager* UserManager)
    : BannedBy{UserManager->UpsertUser(Dto.BannedBy)}
    , CreatedAt{Dto.CreatedAt}
    , Expires{Dto.Expires}
    , bShadow{Dto.bShadow}
    , User{UserManager->UpsertUser(Dto.User)}
{
}
