// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/threads/?language=unreal
void Search()
{
    Client->SearchMessages(
        FFilter::In(TEXT("members"), {TEXT("john")}),    // Member filter
        {TEXT("supercalifragilisticexpialidocious")},    // Query
        {},                                              // Message filter
        {},                                              // Sort options
        {},                                              // Message limit
        [](const TArray<FMessage>& Messages)
        {
            // Do something with the results
        });
}

}    // namespace
