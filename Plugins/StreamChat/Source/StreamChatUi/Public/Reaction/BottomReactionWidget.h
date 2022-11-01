// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/MenuAnchor.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "Reaction/ReactionGroup.h"
#include "ReactionIconWidget.h"
#include "ReactionsTooltipWidget.h"
#include "StreamWidget.h"

#include "BottomReactionWidget.generated.h"

/**
 * @brief A numbered reaction shown below a message (similar to Slack)
 * @see https://www.figma.com/file/oeHiSMDoutuVbJ9WiVyhcG/Chat-Design-Kit-1.5-Android---Work-in-Progress?node-id=17089%3A113943
 */
UCLASS()
class STREAMCHATUI_API UBottomReactionWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UBottomReactionWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FReactionGroup& InReactionGroup);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBottomReactionClicked, const FReactionGroup&, Reaction);
    UPROPERTY(BlueprintAssignable)
    FBottomReactionClicked OnBottomReactionClicked;

    DECLARE_MULTICAST_DELEGATE_OneParam(FBottomReactionClickedNative, const FReactionGroup&);
    FBottomReactionClickedNative OnBottomReactionClickedNative;

protected:
    UPROPERTY(meta = (BindWidget))
    UReactionIconWidget* ReactionIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReactionCountTextBlock;

    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UMenuAnchor* Anchor;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UReactionsTooltipWidget> ReactionsTooltipWidgetClass = UReactionsTooltipWidget::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FReactionGroup ReactionGroup;

private:
    virtual void OnSetup() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    EMessageSide GetSide() const;

    void CancelTooltip();

    UFUNCTION()
    void OnButtonClicked();

    UFUNCTION()
    UUserWidget* CreateTooltip();

    FTimerHandle HoverTimerHandle;
};
