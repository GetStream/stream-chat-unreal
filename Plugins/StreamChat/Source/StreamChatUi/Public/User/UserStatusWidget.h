// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "CoreMinimal.h"
#include "Header/HeaderWidget.h"
#include "StreamWidget.h"

#include "UserStatusWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UUserStatusWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UUserStatusWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FUserRef& InUser);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserStatusClicked, const FUserRef&, User);
    UPROPERTY(BlueprintAssignable)
    FUserStatusClicked OnUserStatusClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(meta = (BindWidget))
    UHeaderWidget* Header;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SubtitleTextBlock;

private:
    virtual void OnSetup() override;

    UFUNCTION()
    void ButtonClicked();

    FUserRef User;
};
