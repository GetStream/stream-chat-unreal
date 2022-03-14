// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "BackButton.generated.h"

/**
 * @brief A button with an icon which can be disabled and themed.
 *
 * UMG disabling affects the style in a way which we can't control.
 */
UCLASS()
class STREAMCHATUI_API UBackButton : public UStreamWidget
{
    GENERATED_BODY()

public:
    UBackButton();

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconTexture;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMargin IconPadding;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Button;

    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

private:
    virtual void NativeOnInitialized() override;
    virtual void OnTheme() override;

    UFUNCTION()
    void OnButtonClicked();
};
