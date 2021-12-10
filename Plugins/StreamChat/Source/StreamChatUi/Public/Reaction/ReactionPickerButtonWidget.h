// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "CoreMinimal.h"
#include "ReactionIconWidget.h"
#include "StreamWidget.h"

#include "ReactionPickerButtonWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionPickerButtonWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UReactionPickerButtonWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FName& InReactionType, EMessageSide InSide);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReactionPickerButtonClicked, const FName&, ReactionType);
    UPROPERTY(BlueprintAssignable)
    FReactionPickerButtonClicked OnReactionPickerButtonClicked;

    DECLARE_MULTICAST_DELEGATE_OneParam(FReactionPickerButtonClickedNative, const FName& /* ReactionType */);
    FReactionPickerButtonClickedNative OnReactionPickerButtonClickedNative;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UReactionIconWidget* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FName ReactionType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void OnSetup() override;
    virtual void OnTheme(const UThemeDataAsset*) override;

    UFUNCTION()
    void OnButtonClicked();
};
