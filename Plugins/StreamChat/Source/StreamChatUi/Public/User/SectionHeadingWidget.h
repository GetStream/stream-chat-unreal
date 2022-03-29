// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

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

private:
    virtual void OnSetup() override;

    FText Label;
};
