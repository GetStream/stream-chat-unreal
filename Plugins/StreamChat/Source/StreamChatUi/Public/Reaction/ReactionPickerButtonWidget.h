// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "CoreMinimal.h"
#include "ReactionIconWidget.h"
#include "StreamUserWidget.h"

#include "ReactionPickerButtonWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionPickerButtonWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
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

private:
    virtual void OnSetup() override;
    virtual bool WantsTheme() override
    {
        return true;
    }
    virtual void OnTheme(const UThemeDataAsset*) override;

    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY(EditAnywhere, Category = Setup)
    FName ReactionType;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
