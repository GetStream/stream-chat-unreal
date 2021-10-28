// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Channel/ChatChannel.h"
#include "ContextWidget.h"
#include "CoreMinimal.h"

#include "ChannelContextWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelContextWidget final : public UContextWidget
{
    GENERATED_BODY()

public:
    static UChatChannel* GetChannel(UWidget* Widget);
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UChatChannel* Channel;
};
