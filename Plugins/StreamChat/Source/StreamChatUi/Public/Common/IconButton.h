// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "IconButton.generated.h"

/**
 * @brief A button with an icon which can be disabled and themed.
 *
 * UMG disabling affects the style in a way which we can't control.
 */
UCLASS()
class STREAMCHATUI_API UIconButton : public UStreamWidget
{
    GENERATED_BODY()

public:
    UIconButton();
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetEnabled(bool bInEnabled);
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetIconFromTexture(UTexture2D* Texture);
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetIconPadding(const FMargin& InIconPadding);

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconTexture;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMargin IconPadding;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defaults)
    bool bEnabled = true;

    /** Called when the button is clicked */
    UPROPERTY(BlueprintAssignable, Category = "Button|Event")
    FOnButtonClickedEvent OnClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FButtonStyle EnabledStyle;
    UPROPERTY(EditAnywhere, Category = Defaults)
    FButtonStyle DisabledStyle;

private:
    virtual void SynchronizeProperties() override;
    virtual void NativeOnInitialized() override;
    virtual void OnTheme() override;

    UFUNCTION()
    void OnButtonClicked();
};
