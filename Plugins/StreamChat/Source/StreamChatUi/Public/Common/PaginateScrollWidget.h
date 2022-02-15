// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "PaginateScrollWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UPaginateScrollWidget : public UStreamWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;
    virtual void OnPreConstruct() override;

    // Attempts to maintain scroll location when replacing the children of the scroll box
    virtual void SetChildren(const TArray<UWidget*>&);
    virtual void Paginate()
    {
    }

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    // If the scroll offset is below this value, then new messages will be fetched
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    float PaginateScrollThreshold = 100.f;

private:
    UFUNCTION()
    void OnUserScroll(float CurrentOffset);
};
