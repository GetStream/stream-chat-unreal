// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSampleHud.h"

#include "Channel/ChatChannel.h"
#include "User.h"

AStreamChatSampleHud::AStreamChatSampleHud()
{
    Client = CreateDefaultSubobject<UStreamChatClientComponent>(TEXT("Client"));
    Client->ApiKey = TEXT("kmajgxb2rk4p");
}

void AStreamChatSampleHud::BeginPlay()
{
    Super::BeginPlay();

    const FUser User{TEXT("tutorial-unreal")};
    const FString Token{TEXT(
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoidHV0b3JpYWwtdW5yZWFsIn0.EBRZ81XroGtR4boK7BF1UzHxhuuHPqvy0e7noNx5fj4")};
    Client->ConnectUser(
        User,
        Token,
        [this]
        {
            Client->QueryChannels(
                {},
                {},
                [this](const TArray<UChatChannel*> ReceivedChannels)
                {
                    Channels = ReceivedChannels;
                    OnConnect();
                });
        });
}

void AStreamChatSampleHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Client->DisconnectUser();
}
