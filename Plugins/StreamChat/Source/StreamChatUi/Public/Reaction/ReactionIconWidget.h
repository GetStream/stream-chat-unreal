// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "StreamWidget.h"

#include "ReactionIconWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STREAMCHATUI_API UReactionIconWidget : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FName& InReactionType, EMessageSide InSide = EMessageSide::Me);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FName ReactionType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;
};
