// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Components/Button.h"
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
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetTitle(const FText& InTitle);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetSubtitleContent(UWidget* Content) const;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetRightContent(UWidget* Content) const;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHeaderButtonClicked);
    // Button needs to be bound
    UPROPERTY(BlueprintAssignable)
    FHeaderButtonClicked OnHeaderButtonClicked;

protected:
    UPROPERTY(meta = (BindWidgetOptional))
    UBorder* Border;
    UPROPERTY(meta = (BindWidgetOptional))
    UButton* Button;

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
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnButtonClick();

    FButtonStyle ButtonStyle;
};
