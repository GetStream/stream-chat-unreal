// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ContextMenuAction.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

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
class STREAMCHATUI_API UContextMenuButtonWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UContextMenuButtonWidget();
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EContextMenuButtonPosition Position;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;
    virtual void OnClient() override;
    virtual void OnChannel() override;

    UFUNCTION()
    void OnButtonClicked();

    UTexture2D* GetButtonTexture() const;
    FMargin GetButtonMargin() const;
    const FLinearColor& GetIconColor() const;
    const FLinearColor& GetTextColor() const;

    UPROPERTY(EditAnywhere, Instanced, Category = Setup)
    UContextMenuAction* Action;
};
