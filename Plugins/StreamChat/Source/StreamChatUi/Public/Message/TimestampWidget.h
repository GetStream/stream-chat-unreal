// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"
#include "TextBubbleWidget.h"

#include "TimestampWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTimestampWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, bool bInShowUserName, bool bInShowMessageState);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* UserTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* MessageStateIconImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DateTimeTextBlock;

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconClock;

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconCheck;

    UPROPERTY(EditAnywhere, Category = Defaults)
    UTexture2D* IconCheckAll;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    bool bShowUserName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    bool bShowMessageState;

private:
    virtual void OnSetup() override;
    virtual bool WantsTheme() override
    {
        return true;
    }
    virtual void OnTheme(const UThemeDataAsset*) override;

    UTexture2D* GetStatusIcon() const;
    FText GetTimestampText() const;
    static FText GetDayOfWeek(const FDateTime&);
};
