// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/NamedAvatarWidget.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "Reaction/ReactionGroup.h"
#include "ReactionIconWidget.h"
#include "StreamWidget.h"

#include "ReactionsTooltipWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionsTooltipWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UReactionsTooltipWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FReactionGroup& InReactionGroup);

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* BackgroundBorder;

    UPROPERTY(meta = (BindWidget))
    UBorder* IconBorder;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* AvatarPanel;

    UPROPERTY(meta = (BindWidget))
    UReactionIconWidget* ReactionIcon;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UNamedAvatarWidget> NamedAvatarWidgetClass = UNamedAvatarWidget::StaticClass();

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

    FReactionGroup ReactionGroup;
};
