// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AvatarWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamUserWidget.h"
#include "User/UserRef.h"

#include "NamedAvatarWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UNamedAvatarWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FUserRef& InUser);

protected:
    UPROPERTY(meta = (BindWidget))
    UAvatarWidget* Avatar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

private:
    virtual void OnSetup() override;

    FUserRef User;
};
