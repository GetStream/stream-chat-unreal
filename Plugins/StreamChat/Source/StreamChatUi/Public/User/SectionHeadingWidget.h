// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "SectionHeadingWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API USectionHeadingWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    USectionHeadingWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FText& InLabel);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    FText Label;

private:
    virtual void OnSetup() override;
    virtual void NativePreConstruct() override;
};
