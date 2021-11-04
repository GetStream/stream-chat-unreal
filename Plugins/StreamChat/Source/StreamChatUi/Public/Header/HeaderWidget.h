// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"

#include "HeaderWidget.generated.h"

/**
 *
 * https://www.figma.com/file/ekifwChR9tR7zRJg1QEzSM/Chat-Design-Kit---Current-version?node-id=15271%3A237867
 */
UCLASS()
class STREAMCHATUI_API UHeaderWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetTitle(const FText& InTitle);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HeaderTitleTextBlock;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* SubtitleSlot;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* LeftSlot;

    UPROPERTY(meta = (BindWidget))
    UNamedSlot* RightSlot;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FText Title;
};
