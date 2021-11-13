// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "StreamUserWidget.h"

#include "ReactionIconWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionIconWidget : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FName& InReactionType, EMessageSide InSide = EMessageSide::Me);

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

    UPROPERTY(EditAnywhere, Category = Setup)
    FName ReactionType;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
