// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "HeaderWidget.generated.h"

/**
 * @brief A top bar widget
 * @see https://www.figma.com/file/ekifwChR9tR7zRJg1QEzSM/Chat-Design-Kit---Current-version?node-id=15271%3A237867
 */
UCLASS()
class STREAMCHATUI_API UHeaderWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UHeaderWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetTitle(const FText& InTitle);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetSubtitleContent(UWidget* Content) const;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetRightContent(UWidget* Content) const;

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HeaderTitleTextBlock;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* SubtitleSlot;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* LeftSlot;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* RightSlot;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FText Title;

private:
    virtual void OnSetup() override;
    virtual void OnTheme(const UThemeDataAsset*) override;
};
