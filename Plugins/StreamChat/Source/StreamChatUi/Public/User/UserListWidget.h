// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "SectionHeadingWidget.h"
#include "StreamWidget.h"
#include "UserStatusWidget.h"

#include "UserListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UUserListWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UUserListWidget();
    void SetQuery(const FFilter& UsersQueryFilter = {});

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUserClicked, const FUserRef&, User, bool, bSelected);
    UPROPERTY(BlueprintAssignable)
    FUserClicked OnUserClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UUserStatusWidget> UserStatusWidgetClass = UUserStatusWidget::StaticClass();
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USectionHeadingWidget> SectionHeadingWidgetClass = USectionHeadingWidget::StaticClass();

private:
    virtual void OnSetup() override;
    virtual void OnClient() override;
    void QueryUsers();
    void PopulateScrollBox(const TArray<FUserRef>&);

    UFUNCTION()
    void UserStatusClicked(const FUserRef& User, bool bSelected);

    FFilter Filter;
};
