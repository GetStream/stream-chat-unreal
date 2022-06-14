// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace UpdatingAChannel
{
UStreamChatClientComponent* Client = nullptr;
UChatChannel* Channel = nullptr;
FString Type;
FString Id;

// https://getstream.io/chat/docs/unreal/channel_update/?language=unreal#partial-update
void PartialUpdate()
{
    // Create a channel with some custom field data that might be useful
    FChannelProperties Props = FChannelProperties{Type, Id};

    // Set extra data using dynamic JSON. You'd more likely want to use statically typed structs.
    Props.ExtraData.SetString(TEXT("source"), TEXT("user"));
    const TSharedRef<FJsonObject> SourceDetail = MakeShared<FJsonObject>();
    SourceDetail->SetNumberField(TEXT("user_id"), 123);
    Props.ExtraData.SetJsonValue(TEXT("source_detail"), MakeShared<FJsonValueObject>(SourceDetail));
    const TSharedRef<FJsonObject> ChannelDetail = MakeShared<FJsonObject>();
    ChannelDetail->SetStringField(TEXT("topic"), TEXT("Plants and Animals"));
    ChannelDetail->SetStringField(TEXT("rating"), TEXT("pg"));
    Props.ExtraData.SetJsonValue(TEXT("source_detail"), MakeShared<FJsonValueObject>(ChannelDetail));
    Client->CreateChannel(
        Props,
        [](UChatChannel* Channel)
        {
            // let's change the source of this channel
            const TSharedRef<FJsonObject> NewSource = MakeShared<FJsonObject>();
            NewSource->SetStringField(TEXT("source"), TEXT("system"));
            Channel->PartialUpdate(NewSource);

            // since it's system generated we no longer need source_detail
            Channel->PartialUpdate({}, {TEXT("source_detail")});

            // and finally update one of the nested fields in the channel_detail
            const TSharedRef<FJsonObject> NewTopic = MakeShared<FJsonObject>();
            NewSource->SetStringField(TEXT("channel_detail.topic"), TEXT("Nature"));
            Channel->PartialUpdate(NewSource);

            // and maybe we decide we no longer need a rating
            Channel->PartialUpdate({}, {TEXT("channel_detail.rating")});
        });
}

// https://getstream.io/chat/docs/unreal/channel_update/?language=unreal#full-update-(overwrite)
void FullUpdate()
{
    FAdditionalFields Data;
    Data.SetString(TEXT("name"), TEXT("myspecialchannel"));
    Data.SetString(TEXT("color"), TEXT("green"));
    const FMessage Message{TEXT("Thierry changed the channel color to green")};
    Channel->Update(Data, Message);
}
}    // namespace UpdatingAChannel
