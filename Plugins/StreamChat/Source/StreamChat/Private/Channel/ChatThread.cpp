// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatThread.h"

#include "Algo/Transform.h"
#include "Response/Thread/ThreadStateResponseDto.h"
#include "User/UserManager.h"
#include "Util.h"

FChatThread::FChatThread() = default;

FChatThread::FChatThread(const FThreadStateResponseDto& Dto, UUserManager* UserManager)
    : ChannelCid{Dto.ChannelCid}
    , ParentMessageId{Dto.ParentMessageId}
    , ParentMessage{Dto.ParentMessage, UserManager}
    , CreatedBy{UserManager->UpsertUser(Dto.CreatedBy)}
    , ReplyCount{Dto.ReplyCount}
    , ParticipantCount{Dto.ParticipantCount}
    , ActiveParticipantCount{Dto.ActiveParticipantCount}
    , LatestReplies{Util::Convert<FMessage>(Dto.LatestReplies, UserManager)}
    , Read{Util::Convert<FRead>(Dto.Read, UserManager)}
    , LastMessageAt{Dto.LastMessageAt}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , Title{Dto.Title}
    , ExtraData{Dto.AdditionalFields}
{
    // A participant carries more than its user (when it last read the thread, when it left), but the
    // read states already cover what a thread list needs, so only the users are lifted out here.
    Algo::Transform(Dto.ThreadParticipants, ThreadParticipants, [UserManager](const FThreadParticipantDto& P) { return UserManager->UpsertUser(P.User); });
}

int32 FChatThread::UnreadCount() const
{
    if (const FRead* CurrentUserRead = Read.FindByPredicate([](const FRead& R) { return R.User.IsCurrent(); }))
    {
        return CurrentUserRead->UnreadMessages;
    }
    return 0;
}
