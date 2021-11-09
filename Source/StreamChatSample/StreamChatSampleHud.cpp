// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSampleHud.h"

#include "Channel/ChatChannel.h"
#include "User/User.h"

AStreamChatSampleHud::AStreamChatSampleHud()
{
    Client = CreateDefaultSubobject<UStreamChatClientComponent>(TEXT("Client"));
    Client->ApiKey = TEXT("qx5us2v6xvmh");
}

void AStreamChatSampleHud::BeginPlay()
{
    Super::BeginPlay();

    const FUser User{TEXT("tommaso")};
    const FString Token{TEXT(
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoidG9tbWFzbyJ9.lNaWC2Opyq6gmV50a2BGxK-5gm5mwCpefnUA30_k9YA")};
    Client->ConnectUser(
        User,
        Token,
        [this, UserId = User.Id]
        {
            const FFilter Filter = FFilter::In(TEXT("members"), {UserId});
            Client->QueryChannels(
                [this](const TArray<UChatChannel*> ReceivedChannels)
                {
                    Channels = ReceivedChannels;
                    OnConnect();
                },
                Filter,
                {{ESortField::LastMessageAt, ESortDirection::Descending}});
        });
}

void AStreamChatSampleHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Client->DisconnectUser();
}
