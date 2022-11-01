// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Common/PaginateScrollWidget.h"
#include "CoreMinimal.h"
#include "SectionHeadingWidget.h"
#include "UserStatusWidget.h"

#include "UserListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UUserListWidget final : public UPaginateScrollWidget
{
    GENERATED_BODY()

public:
    UUserListWidget();
    void SetQuery(const FFilter& UsersQueryFilter = {});

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUserClicked, const FUserRef&, User, bool, bSelected);
    UPROPERTY(BlueprintAssignable)
    FUserClicked OnUserClicked;

protected:
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UUserStatusWidget> UserStatusWidgetClass = UUserStatusWidget::StaticClass();
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USectionHeadingWidget> SectionHeadingWidgetClass = USectionHeadingWidget::StaticClass();

private:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void Paginate(EPaginationDirection Directions, TFunction<void()> Callback) override;
    void Refetch();
    void PopulateScrollBox();

    UFUNCTION()
    void UserStatusClicked(const FUserRef& User, bool bSelected);

    FFilter Filter;
    TArray<FUserRef> Users;
};
