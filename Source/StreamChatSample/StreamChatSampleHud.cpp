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

    const FUser User{TEXT("jc")};
    const FString Token{TEXT(
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoiamMifQ.2_5Hae3LKjVSfA0gQxXlZn54Bq6xDlhjPx2J7azUNB4")};
    Client->ConnectUser(
        User,
        Token,
        [WeakThis = TWeakObjectPtr<AStreamChatSampleHud>(this)](const FUserRef& UserRef)
        {
            const FFilter Filter = FFilter::In(TEXT("members"), {UserRef.UserId});
            WeakThis->Client->QueryChannels(
                [WeakThis](const TArray<UChatChannel*> ReceivedChannels)
                {
                    WeakThis->Channels = ReceivedChannels;
                    WeakThis->OnConnect();
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
