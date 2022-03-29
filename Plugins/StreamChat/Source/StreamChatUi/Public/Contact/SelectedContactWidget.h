// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/NamedAvatarWidget.h"
#include "Components/Border.h"
#include "CoreMinimal.h"

#include "SelectedContactWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API USelectedContactWidget final : public UNamedAvatarWidget
{
    GENERATED_BODY()

protected:
    USelectedContactWidget();
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

private:
    virtual void OnTheme() override;
};
