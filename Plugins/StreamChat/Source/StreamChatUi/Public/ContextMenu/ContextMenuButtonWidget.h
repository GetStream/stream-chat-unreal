// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"
#include "Components/TextBlock.h"

#include "ContextMenuButtonWidget.generated.h"

UENUM()
enum class EContextMenuButtonPosition : uint8
{
    Top,
    Mid,
    Bottom
};

UENUM()
enum class EContextMenuButtonStyle : uint8
{
    Standard,
    Negative,
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
    void Setup(EContextMenuButtonPosition InPosition, EContextMenuButtonStyle InStyle, UTexture2D* InIconTexture, const FText& InText);

    DECLARE_MULTICAST_DELEGATE(FContextMenuButtonClicked);
    FContextMenuButtonClicked OnContextMenuButtonClicked;

protected:
    virtual void OnSetup() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;
    UPROPERTY(meta = (BindWidget))
    UImage* IconImage;
    UPROPERTY(meta = (BindWidget))
    UImage* TopBorderImage;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(EditAnywhere, Category = Button)
    UTexture2D* TopButtonTexture;
    UPROPERTY(EditAnywhere, Category = Button)
    UTexture2D* MidButtonTexture;
    UPROPERTY(EditAnywhere, Category = Button)
    UTexture2D* BottomButtonTexture;
    UPROPERTY(EditAnywhere, Category = Button)
    FLinearColor DefaultButtonColor;
    UPROPERTY(EditAnywhere, Category = Button)
    FLinearColor SelectedButtonColor;
    UPROPERTY(EditAnywhere, Category = Style)
    FLinearColor DefaultTextColor;
    UPROPERTY(EditAnywhere, Category = Style)
    FLinearColor NegativeTextColor;
    UPROPERTY(EditAnywhere, Category = Style)
    FLinearColor DefaultIconColor;
    UPROPERTY(EditAnywhere, Category = Style)
    FLinearColor NegativeIconColor;

private:
    UFUNCTION()
    void OnButtonClicked();

    UTexture2D* GetButtonTexture() const;
    FMargin GetButtonMargin() const;
    FLinearColor GetIconColor() const;
    FLinearColor GetTextColor() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    EContextMenuButtonPosition Position;
    UPROPERTY(EditAnywhere, Category = Setup)
    EContextMenuButtonStyle Style;
    UPROPERTY(EditAnywhere, Category = Setup)
    UTexture2D* IconTexture;
    UPROPERTY(EditAnywhere, Category = Setup)
    FText Text;
};
