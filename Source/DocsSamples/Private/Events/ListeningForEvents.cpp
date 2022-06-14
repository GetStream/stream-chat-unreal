// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Event/Channel/MessageDeletedEvent.h"
#include "Event/Client/ConnectionChangedEvent.h"
#include "Event/Client/HealthCheckEvent.h"
#include "Event/User/UserPresenceChangedEvent.h"
#include "StreamChatClientComponent.h"

class UMyObject final : public UObject
{
    // https://getstream.io/chat/docs/unreal/event_listening/?language=unreal
    void ListeningForEvents()
    {
        // Subscribing via a UObject member function
        Channel->On<FMessageDeletedEvent>(this, &UMyObject::OnMessageDeleted);

        // Subscribing via a lambda function
        Channel->On<FMessageDeletedEvent>(
            [](const FMessageDeletedEvent& Event)
            {
                // Message was deleted
            });
    }

    void AllAtOnce()
    {
        // Not supported in the Unreal SDK
    }

    // https://getstream.io/chat/docs/unreal/event_listening/?language=unreal#client-events
    void ClientEvents()
    {
        Client->On<FHealthCheckEvent>(this, &UMyObject::OnHealthCheck);
        Client->On<FUserPresenceChangedEvent>(this, &UMyObject::OnUserPresenceChanged);
    }

    // https://getstream.io/chat/docs/unreal/event_listening/?language=unreal#connection-events
    void ConnectionEvents()
    {
        Client->On<FConnectionChangedEvent>(
            [](const FConnectionChangedEvent& Event)
            {
                if (Event.bOnline)
                {
                    // Came online
                }
                else
                {
                    // Went offline
                }
            });
    }

    // https://getstream.io/chat/docs/unreal/event_listening/?language=unreal#stop-listening-for-events
    void StopListeningForEvents()
    {
        // Subscribe
        const FDelegateHandle Handle = Client->On<FHealthCheckEvent>(this, &UMyObject::OnHealthCheck);

        // Unsubscribe
        const bool bSuccess = Client->Unsubscribe<FHealthCheckEvent>(Handle);
    }

    void OnMessageDeleted(const FMessageDeletedEvent&)
    {
    }
    void OnHealthCheck(const FHealthCheckEvent&)
    {
    }
    void OnUserPresenceChanged(const FUserPresenceChangedEvent&)
    {
    }

    UChatChannel* Channel = nullptr;
    UStreamChatClientComponent* Client = nullptr;
};
