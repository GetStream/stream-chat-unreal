// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Reaction/Reaction.h"

namespace
{
UChatChannel* Channel;
FMessage Message;

// https://getstream.io/chat/docs/unreal/send_reaction/?language=unreal
void Reactions()
{
    const FReaction Reaction = Channel->SendReaction(Message, TEXT("love"));
}

namespace
{
FReaction Reaction;
// https://getstream.io/chat/docs/unreal/send_reaction/?language=unreal#removing-a-reaction
void RemovingAReaction()
{
    Channel->DeleteReaction(Message, Reaction);
}
}    // namespace

// https://getstream.io/chat/docs/unreal/send_reaction/?language=unreal#paginating-reactions
void PaginatingReactions()
{
    // get the first 10 reactions
    Channel->GetReactions(
        Message,
        {
            10,    // Limit
        });

    // get 3 reactions past the first 10
    Channel->GetReactions(
        Message,
        {
            3,     // Limit
            10,    // Offset
        });
}

// https://getstream.io/chat/docs/unreal/send_reaction/?language=unreal#cumulative-(clap)-reactions
void CumulativeClapReactions()
{
    // user claps 5 times on a message
    Channel->SendReaction(Message, TEXT("like"), 5);

    // same user claps 20 times more
    Channel->SendReaction(Message, TEXT("like"), 20);
}

}    // namespace
