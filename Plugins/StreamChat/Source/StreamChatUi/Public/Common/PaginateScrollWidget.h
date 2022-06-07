// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    virtual void NativePreConstruct() override;
    virtual void NativeDestruct() override;

    /// Attempts to maintain scroll location when replacing the children of the scroll box
    virtual void SetChildren(const TArray<UWidget*>&);

    /// Override with a function that performs the pagination API call
    virtual void Paginate(EPaginationDirection Directions, TFunction<void()> Callback)
    {
    }

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    // If the scroll offset is below this value, then new messages will be fetched
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    float PaginateScrollThreshold = 100.f;

    // The end of the scroll box which causes pagination to be triggered
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    EPaginationDirection PaginationDirection = EPaginationDirection::Bottom;

private:
    UFUNCTION()
    void OnUserScroll(float CurrentOffset);
    void SetPaginationRequestState(EHttpRequestState, EPaginationDirection);
    EPaginationDirection GetDirections() const;

    EPaginationDirection EndedPaginationDirections = EPaginationDirection::None;
    EHttpRequestState PaginationRequestState = EHttpRequestState::Ended;
};
