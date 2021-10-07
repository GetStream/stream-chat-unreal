// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSampleHud.h"

#include "Channel/ChatChannel.h"
#include "User.h"

AStreamChatSampleHud::AStreamChatSampleHud()
{
    Client = CreateDefaultSubobject<UStreamChatClientComponent>(TEXT("Client"));
    Client->ApiKey = TEXT("b67pax5b2wdq");
}

void AStreamChatSampleHud::BeginPlay()
{
    Super::BeginPlay();

    const FUser User{TEXT("tutorial-unreal")};
    const FString Token{
        TEXT("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoidHV0b3JpYWwtZmx1dHRlciJ9.S-"
             "MJpoSwDiqyXpUURgO5wVqJ4vKlIVFLSEyrFYCOE1c")};
    Client->ConnectUser(
        [this]
        {
            Channel = Client->Channel(TEXT("messaging"), TEXT("flutterdevs"));
            Channel->Watch(
                [this]
                {
                    OnConnect();
                });
        },
        User,
        Token);
}

void AStreamChatSampleHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Client->DisconnectUser();
}
