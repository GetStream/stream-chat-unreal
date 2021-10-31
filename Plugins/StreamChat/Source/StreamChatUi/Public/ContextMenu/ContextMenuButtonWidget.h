// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"

#include "ContextMenuButtonWidget.generated.h"

UENUM()
enum class EContextMenuButtonPosition : uint8
{
    Top,
    Mid,
    Bottom
};

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UContextMenuButtonWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(EContextMenuButtonPosition InPosition);

    DECLARE_MULTICAST_DELEGATE(FContextMenuButtonClicked);
    FContextMenuButtonClicked OnContextMenuButtonClicked;

protected:
    virtual void OnSetup() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;
    UPROPERTY(meta = (BindWidget))
    UImage* TopBorderImage;

    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* TopButtonTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* MidButtonTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* BottomButtonTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    FLinearColor DefaultButtonColor;
    UPROPERTY(EditAnywhere, Category = Bubble)
    FLinearColor SelectedButtonColor;

private:
    UFUNCTION()
    void OnButtonClicked();

    UTexture2D* GetButtonTexture() const;
    FMargin GetButtonMargin() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    EContextMenuButtonPosition Position;
};
