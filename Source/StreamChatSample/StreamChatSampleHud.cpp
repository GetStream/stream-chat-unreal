// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSampleHud.h"

#include "Channel/ChatChannel.h"
#include "User/User.h"

AStreamChatSampleHud::AStreamChatSampleHud()
{
    Client = CreateDefaultSubobject<UStreamChatClientComponent>(TEXT("Client"));
    Client->ApiKey = TEXT("kmajgxb2rk4p");
}

void AStreamChatSampleHud::BeginPlay()
{
    Super::BeginPlay();

    const FUser User{TEXT("sam")};
    const FString Token = TEXT("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoic2FtIn0.fn3ofYr6CHh34wI-fhY0ZnQKAr5scPbObrjysvSXdeQ");
    Client->ConnectUser(
        User,
        Token,
        [WeakThis = TWeakObjectPtr<AStreamChatSampleHud>(this)](const FUserRef& UserRef)
        {
            const FFilter Filter = FFilter::In(TEXT("members"), {UserRef->Id});
            WeakThis->Client->QueryChannels(
                Filter,
                {{EChannelSortField::LastMessageAt, ESortDirection::Descending}},
                EChannelFlags::State | EChannelFlags::Watch,
                {},
                [WeakThis](const TArray<UChatChannel*> ReceivedChannels)
                {
                    WeakThis->Channels = ReceivedChannels;
                    WeakThis->OnConnect();
                });
        });
}

void AStreamChatSampleHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Client->DisconnectUser();
}
