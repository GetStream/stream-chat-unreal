// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace
{
UStreamChatClientComponent* Client;

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#inviting-users
void InvitingUsers()
{
    // Not yet supported in the Unreal SDK
}

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#accepting-an-invite
void AcceptingAnInvite()
{
    // Not yet supported in the Unreal SDK
}

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#rejecting-an-invite
void RejectingAnInvite()
{
    // Not yet supported in the Unreal SDK
}

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#query-for-accepted-invites
void QueryForAcceptedInvites()
{
    Client->QueryChannels(FFilter::Equal(TEXT("invite"), TEXT("accepted")));
}

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#query-for-rejected-invites
void QueryForRejectedInvites()
{
    Client->QueryChannels(FFilter::Equal(TEXT("invite"), TEXT("rejected")));
}

// https://getstream.io/chat/docs/unreal/channel_invites/?language=unreal#query-for-pending-invites
void QueryForPendingInvites()
{
    Client->QueryChannels(FFilter::Equal(TEXT("invite"), TEXT("pending")));
}
}    // namespace
