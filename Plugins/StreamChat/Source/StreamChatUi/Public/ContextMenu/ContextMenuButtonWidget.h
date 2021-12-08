// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ContextMenuAction.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"

#include "ContextMenuButtonWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FContextMenuButtonClicked);

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
    void Setup(const FMessage& InMessage, EContextMenuButtonPosition InPosition, UContextMenuAction* InAction);

protected:
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

private:
    virtual void OnSetup() override;
    virtual bool WantsTheme() override
    {
        return true;
    }
    virtual void OnTheme(const UThemeDataAsset*) override;

    UFUNCTION()
    void OnButtonClicked();

    UTexture2D* GetButtonTexture() const;
    FMargin GetButtonMargin() const;
    const FLinearColor& GetIconColor(const UThemeDataAsset* Theme) const;
    const FLinearColor& GetTextColor(const UThemeDataAsset* Theme) const;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, Category = Setup)
    EContextMenuButtonPosition Position;
    UPROPERTY(EditAnywhere, Instanced, Category = Setup)
    UContextMenuAction* Action;
};
