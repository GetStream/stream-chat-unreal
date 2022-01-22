// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Context/ClientContextWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "TeamChatWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTeamChatWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UTeamChatWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UStreamChatClientComponent* InClient);

protected:
    UPROPERTY(meta = (BindWidget))
    UClientContextWidget* ClientContextWidget;

    UPROPERTY(meta = (BindWidget))
    UBorder* MessageListContainer;

private:
    virtual void OnSetup() override;
    virtual void OnTheme(const UThemeDataAsset*) override;
};
