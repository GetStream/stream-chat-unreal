// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "ReactionIconWidget.h"

#include "BitmapReactionIconWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UBitmapReactionIconWidget final : public UReactionIconWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Image;

    UPROPERTY(EditAnywhere, Category = Defaults)
    TMap<FName, UTexture2D*> Textures;

    UPROPERTY(EditAnywhere, Category = Defaults)
    bool bShouldTint = false;

    UPROPERTY(EditAnywhere, Category = Defaults)
    TMap<EMessageSide, FLinearColor> Colors;

private:
    virtual void OnSetup() override;
};
