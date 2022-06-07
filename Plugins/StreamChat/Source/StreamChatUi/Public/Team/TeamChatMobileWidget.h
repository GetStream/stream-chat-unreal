// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChannelWidget.h"
#include "ChannelList/ChannelListWidget.h"
#include "Components/Border.h"
#include "Context/ClientContextWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "TeamChatMobileWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTeamChatMobileWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UTeamChatMobileWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UStreamChatClientComponent* InClient);

protected:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SwitchToChannelList();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SwitchToMessageList(UChatChannel* SelectedChannel);

    UPROPERTY(meta = (BindWidget))
    UClientContextWidget* ClientContextWidget;
    UPROPERTY(meta = (BindWidget))
    UBorder* Container;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UChannelListWidget> ChannelListWidgetClass = UChannelListWidget::StaticClass();
    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UChannelWidget> ChannelWidgetClass = UChannelWidget::StaticClass();

private:
    virtual void OnSetup() override;
    virtual void NativePreConstruct() override;

    UFUNCTION()
    void ChannelSelected(UChatChannel* SelectedChannel);
};
