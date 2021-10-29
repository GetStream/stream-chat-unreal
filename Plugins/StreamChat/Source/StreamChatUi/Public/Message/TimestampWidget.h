// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ContextMenuWidget.h"
#include "CoreMinimal.h"
#include "ReactionPickerWidget.h"
#include "TextBubbleWidget.h"

#include "TimestampWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTimestampWidget final : public UUserWidget
{
public:
    explicit UTimestampWidget(const FObjectInitializer& ObjectInitializer);

private:
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    FMessage Message;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    EBubbleStackSide Side;

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* UserTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* MessageStateIconImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DateTimeTextBlock;

    UPROPERTY(meta = (BindWidget))
    UButton* ReactionButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OptionsButton;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* HoverGroup;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* InfoGroup;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconClock;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconCheck;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconCheckAll;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = "Timestamp")
    TSubclassOf<UReactionPickerWidget> ReactionPickerWidgetClass = UReactionPickerWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = "Timestamp")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass = UContextMenuWidget::StaticClass();

private:
    UFUNCTION()
    void OnOptionsButtonClicked();
    UFUNCTION()
    void OnReactionButtonClicked();

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    UTexture2D* GetStatusIcon() const;
    FText GetTimestampText() const;
    static FText GetDayOfWeek(const FDateTime&);
};
