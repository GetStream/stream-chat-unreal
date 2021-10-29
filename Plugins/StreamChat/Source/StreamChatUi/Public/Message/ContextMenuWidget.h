// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "TextBubbleWidget.h"

#include "ContextMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UContextMenuWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    FMessage Message;
};
