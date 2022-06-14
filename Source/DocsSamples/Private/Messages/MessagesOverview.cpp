// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/send_message/?language=unreal
void MessagesOverview()
{
    const FMessage Message{TEXT("@Josh I told them I was pesca-pescatarian. Which is one who eats solely fish who eat other fish.")};
    Channel->SendMessage(Message);
}

// https://getstream.io/chat/docs/unreal/send_message/?language=unreal#complex-example
void ComplexExample()
{
    FMessage Message{TEXT("@Josh I told them I was pesca-pescatarian. Which is one who eats solely fish who eat other fish.")};
    Message.ExtraData.SetNumber(TEXT("anotherCustomField"), 234);
    Channel->SendMessage(Message);
    // NOTE: the Unreal SDK does not currently support attachments or mentioned users
}

// https://getstream.io/chat/docs/unreal/send_message/?language=unreal#get-a-message
void GetAMessage()
{
    // Not yet supported in the Unreal SDK
}

}    // namespace

namespace ExistingMessage
{
UChatChannel* Channel = nullptr;
FMessage Message;

// https://getstream.io/chat/docs/unreal/send_message/?language=unreal#update-a-message
void UpdateAMessage()
{
    // Update some field of the message
    Message.Text = TEXT("my updated text");

    // Send the message to the channel
    Channel->UpdateMessage(Message);
}

// https://getstream.io/chat/docs/unreal/send_message/?language=unreal#delete-a-message
void DeleteAMessage()
{
    Channel->DeleteMessage(Message);
}
}    // namespace ExistingMessage
