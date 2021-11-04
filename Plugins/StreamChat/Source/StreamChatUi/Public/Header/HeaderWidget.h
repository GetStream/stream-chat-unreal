// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
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

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetTitleDelegate, FText&, Title);
    UPROPERTY(BlueprintAssignable)
    FGetTitleDelegate OnGetTitle;
    DECLARE_DELEGATE_RetVal(FText, FNativeGetTitleDelegate);
    FNativeGetTitleDelegate OnNativeGetTitle;

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
    FText DefaultTitle;

private:
    FText GetTitleText() const;
};
