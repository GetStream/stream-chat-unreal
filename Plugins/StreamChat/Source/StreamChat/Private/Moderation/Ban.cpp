#include "Moderation/Ban.h"

#include "StreamChatClientComponent.h"
#include "User/UserManager.h"

FBan::FBan(const FBanResponseDto& Dto, UUserManager* UserManager)
    : BannedBy{UserManager->UpsertUser(Dto.BannedBy)}
    , Channel{Dto.Channel, UserManager}
    , CreatedAt{Dto.CreatedAt}
    , Expires{Dto.Expires}
    , bShadow{Dto.bShadow}
    , User{UserManager->UpsertUser(Dto.User)}
{
}
