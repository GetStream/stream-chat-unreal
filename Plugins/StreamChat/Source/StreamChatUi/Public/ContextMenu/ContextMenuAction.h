// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/NoExportTypes.h"

#include "ContextMenuAction.generated.h"

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

    void Perform(const FMessage&, UWidget* OwningWidget);

protected:
    virtual void OnPerform(const FMessage&, UWidget* OwningWidget);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Perform"))
    void OnPerformBlueprint(const FMessage& Message, UWidget* OwningWidget);
};
