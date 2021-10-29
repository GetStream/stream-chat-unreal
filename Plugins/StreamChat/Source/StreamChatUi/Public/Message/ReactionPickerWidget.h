// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "TextBubbleWidget.h"

#include "ReactionPickerWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionPickerWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    FMessage Message;
};
