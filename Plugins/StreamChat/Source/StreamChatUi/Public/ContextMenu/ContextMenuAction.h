// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Message/MessageSide.h"
#include "UObject/NoExportTypes.h"

#include "ContextMenuAction.generated.h"

class UChatChannel;
class UStreamChatClientComponent;

UENUM()
enum class EContextMenuButtonStyle : uint8
{
    Standard,
    Negative,
};

/**
 *
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class STREAMCHATUI_API UContextMenuAction : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = Visuals)
    FText Label;
    UPROPERTY(EditAnywhere, Category = Visuals)
    UTexture2D* IconTexture;
    UPROPERTY(EditAnywhere, Category = Visuals)
    EContextMenuButtonStyle Style;

    void SetContext(UStreamChatClientComponent*, UChatChannel*);
    void Perform(const FMessage&, UWidget* OwningWidget);
    bool ShouldDisplay(EMessageSide, const FMessage&) const;

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget);
    virtual bool OnShouldDisplay(EMessageSide, const FMessage&) const;

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Perform"))
    void OnPerformBlueprint(const FMessage& Message, UWidget* OwningWidget);
    UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Should Display"))
    bool OnShouldDisplayBlueprint(EMessageSide Side, const FMessage& Message) const;

    UPROPERTY(BlueprintReadOnly, Transient)
    UStreamChatClientComponent* Client;
    UPROPERTY(BlueprintReadOnly, Transient)
    UChatChannel* Channel;
};
