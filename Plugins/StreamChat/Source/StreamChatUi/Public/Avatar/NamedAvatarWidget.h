// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AvatarWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"
#include "User/UserRef.h"

#include "NamedAvatarWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UNamedAvatarWidget : public UStreamWidget
{
    GENERATED_BODY()

public:
    UNamedAvatarWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FUserRef& InUser);

protected:
    virtual void NativePreConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(EditAnywhere, Category = "Stream Chat")
    FUserRef User;

private:
    virtual void OnSetup() override;
};
