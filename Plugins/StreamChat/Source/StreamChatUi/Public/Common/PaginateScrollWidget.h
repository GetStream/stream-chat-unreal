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

public:
    /// Number of additional objects to be queried when paginating
    UPROPERTY(EditAnywhere, Category = Pagination)
    int32 Limit = 20;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPaginatingMessagesDelegate, EPaginationDirection, Direction, EHttpRequestState, RequestState);
    /// Fired during various stages of message pagination
    UPROPERTY(BlueprintAssignable)
    FPaginatingMessagesDelegate OnPaginatingMessages;

protected:
    virtual void NativeOnInitialized() override;
    virtual void OnPreConstruct() override;

    /// Attempts to maintain scroll location when replacing the children of the scroll box
    virtual void SetChildren(const TArray<UWidget*>&);

    /// Override with a function that performs the pagination API call
    virtual void Paginate(EPaginationDirection Direction, TFunction<void()> Callback)
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
    void SetPaginationRequestState(EHttpRequestState, EPaginationDirection);
    EPaginationDirection GetDirection() const;

    EPaginationDirection EndedPaginationDirections = EPaginationDirection::None;
    EHttpRequestState PaginationRequestState = EHttpRequestState::Ended;
};
