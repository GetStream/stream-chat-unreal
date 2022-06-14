// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Event/Channel/TypingStartEvent.h"
#include "Event/Channel/TypingStopEvent.h"

namespace TypingIndicators
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/typing_indicators/?language=unreal#sending-start-and-stop-typing-events
void SendingStartAndStopTypingEvents()
{
    // The Unreal SDK keeps track of the typing state for you.
    // Just call `Channel->Keystroke()` when the user types and
    // `Channel->StopTyping()` when the user sends the message (or aborts)

    // Sends a typing.start event at most once every two seconds,
    // and a typing.stop event two seconds after the last keystroke
    Channel->KeyStroke();

    // Manually sends the typing.stop event, and cancels any pending
    // typing.stop event queued by Keystroke()
    Channel->StopTyping();
}

class UMyWidget final : public UObject
{
    void ReceivingTypingIndicatorEvents()
    {
        // You can manually subscribe to the typing channel events
        Channel->On<FTypingStartEvent>(this, &UMyWidget::OnTypingStart);
        Channel->On<FTypingStopEvent>(this, &UMyWidget::OnTypingStop);

        // Alternatively you can bind to the OnTypingIndicator delegate (also from Blueprints)
        Channel->OnTypingIndicator.AddDynamic(this, &UMyWidget::OnTypingIndicator);
    }

    void OnTypingStart(const FTypingStartEvent& Event)
    {
    }
    void OnTypingStop(const FTypingStopEvent& Event)
    {
    }
    UFUNCTION()
    void OnTypingIndicator(ETypingIndicatorState TypingState, const FUserRef& User)
    {
    }
};

}    // namespace TypingIndicators
