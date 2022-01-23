// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChannelWidget.h"
#include "ChannelList/ChannelListWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
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
    UPROPERTY(meta = (BindWidget))
    UBorder* ChannelListContainer;
    UPROPERTY(meta = (BindWidget))
    UChannelListWidget* ChannelList;
    UPROPERTY(meta = (BindWidget))
    UImage* HorizontalDivider;
    UPROPERTY(meta = (BindWidget))
    UImage* VerticalDivider;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UChannelWidget> ChannelWidgetClass = UChannelWidget::StaticClass();

private:
    virtual void OnSetup() override;
    virtual void OnTheme(const UThemeDataAsset*) override;

    UFUNCTION()
    void ChannelSelected(UChatChannel* SelectedChannel);
};
