// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace
{
UStreamChatClientComponent* Client;
UChatChannel* Channel = nullptr;
FString UserId;

// https://getstream.io/chat/docs/unreal/muting_channels/?language=unreal#muting-channels
void MutingChannels()
{
    // mute channel for current user
    Channel->Mute();

    // mute a channel for 2 weeks
    Channel->Mute({FTimespan::FromDays(15.)});

    // mute a channel for 10 seconds
    Channel->Mute({FTimespan::FromSeconds(10.)});

    // check if channel is muted
    Channel->IsMuted();
}

// https://getstream.io/chat/docs/unreal/muting_channels/?language=unreal#query-muted-channels
void QueryMutedChannels()
{
    // retrieve all channels excluding muted ones
    const FFilter NotMuted = FFilter::And({
        FFilter::In(TEXT("members"), {UserId}),
        FFilter::Equal(TEXT("muted"), false),
    });
    Client->QueryChannels(NotMuted);

    // retrieve all muted channels
    const FFilter Muted = FFilter::Equal(TEXT("muted"), true);
    Client->QueryChannels(Muted);
}

// https://getstream.io/chat/docs/unreal/muting_channels/?language=unreal#remove-a-channel-mute
void RemoveAChannelMute()
{
    // unmute channel for current user
    Channel->Unmute();
}

// https://getstream.io/chat/docs/unreal/muting_channels/?language=unreal#hiding-a-channel
void HidingAChannel()
{
    // hides the channel until a new message is added there
    Channel->Hide();

    // shows a previously hidden channel
    Channel->Show();

    // hide the channel and clear the message history
    Channel->Hide(true);
}
}    // namespace
