// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Channel/ChatChannel.h"
#include "ContextWidget.h"
#include "CoreMinimal.h"

#include "ChannelContextWidget.generated.h"

/**
 * @brief Provide a reference to a \ref UChatChannel to child widgets
 *
 * A widget that can be initialized with a ChatChannel in order to conveniently provide children with access to said channel as well as channel-related UI
 * delegates
 */
UCLASS()
class STREAMCHATUI_API UChannelContextWidget final : public UContextWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel);

    UFUNCTION(BlueprintPure, Category = "Stream Chat", meta = (DisplayName = "Get Channel Context"))
    static UChannelContextWidget* Get(const UWidget* Widget);
    UFUNCTION(BlueprintPure, Category = "Stream Chat", meta = (DisplayName = "Get Channel Context"))
    static UChannelContextWidget* TryGet(const UWidget* Widget);
    UFUNCTION(BlueprintPure, Category = "Stream Chat")
    static UChatChannel* GetChannel(const UWidget* Widget);
    UFUNCTION(BlueprintPure, Category = "Stream Chat")
    static UChatChannel* TryGetChannel(const UWidget* Widget);
    UChatChannel* GetChannel() const;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartEditMessageDelegate, const FMessage&, Message);
    UPROPERTY(BlueprintAssignable)
    FStartEditMessageDelegate OnStartEditMessage;

private:
    UPROPERTY(Transient)
    UChatChannel* Channel;
};
