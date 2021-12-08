// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"
#include "User/UserRef.h"

#include "ProfilePicWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UProfilePicWidget : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FUserRef& InUser);

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Image;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* InitialsTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FUserRef User;

private:
    virtual void OnSetup() override;

    void FetchRemoteImage();
};
