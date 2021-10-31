// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "CoreMinimal.h"

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
class STREAMCHATUI_API UContextMenuButtonWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(EContextMenuButtonPosition InPosition);

    DECLARE_MULTICAST_DELEGATE(FContextMenuButtonClicked);
    FContextMenuButtonClicked OnContextMenuButtonClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Button;

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

    EContextMenuButtonPosition Position;
};
