// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Moderation/ChannelMuteDto.h"
#include "User/UserRef.h"

#include "MutedChannel.generated.h"

class UUserManager;

USTRUCT(BlueprintType)
struct STREAMCHAT_API FMutedChannel
{
    GENERATED_BODY()

public:
    FMutedChannel();
    explicit FMutedChannel(const FChannelMuteDto&, UUserManager*);

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted Channel")
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted Channel")
    FDateTime UpdatedAt = FDateTime{0};

    /// Date/time of mute expiration
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted Channel")
    FDateTime Expires = FDateTime{0};

    /// CID of muted channel
    /// TODO: Think about making this a UChatChannel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted Channel")
    FString Cid;

    /// Owner of channel mute
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted Channel")
    FUserRef User;
};
