// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/NamedAvatarWidget.h"
#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "Reaction/ReactionGroup.h"
#include "ReactionIconWidget.h"
#include "StreamUserWidget.h"

#include "ReactionsTooltipWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionsTooltipWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FReactionGroup& InReactionGroup);

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* AvatarPanel;

    UPROPERTY(meta = (BindWidget))
    UReactionIconWidget* ReactionIcon;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UNamedAvatarWidget> NamedAvatarWidgetClass = UNamedAvatarWidget::StaticClass();

private:
    virtual void OnSetup() override;

    FReactionGroup ReactionGroup;
};
