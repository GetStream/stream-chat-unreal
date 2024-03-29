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
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetIconPaletteColor(const FName& InPaletteColor);
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetEnabledBackgroundColor(const FName& InPaletteColor);
    UFUNCTION(BlueprintCallable, Category = "Icon Button")
    void SetDisabledBackgroundColor(const FName& InPaletteColor);

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconTexture;
    // IconTexture use if null
    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* DisabledIconTexture;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMargin IconPadding;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FName IconPaletteColor = TEXT("text-high-emphasis");
    UPROPERTY(EditAnywhere, Category = Defaults)
    FName EnabledBackgroundColor = TEXT("accent-primary");
    UPROPERTY(EditAnywhere, Category = Defaults)
    FName DisabledBackgroundColor = TEXT("disabled");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defaults) bool bEnabled = true;

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
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnButtonClicked();
};
