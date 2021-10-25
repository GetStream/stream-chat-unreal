#pragma once

#include "ChannelConfig.h"
#include "CoreMinimal.h"
#include "Member.h"
#include "Message.h"

#include "ChannelState.generated.h"

struct FChannelStateResponseFieldsDto;

USTRUCT(BlueprintType)
struct FChannelState
{
    GENERATED_BODY()

    FChannelState() = default;
    explicit FChannelState(const FChannelStateResponseFieldsDto&);

    void Merge(const FChannelStateResponseFieldsDto&);
    void AddMessage(const FMessage&);

    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Type;

    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Id;

    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    TArray<FMember> Members;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Name;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString ImageUrl;

    /// The cid of this channel
    FString Cid;

    FChannelConfig Config;

    TArray<FMessage> Messages;
};
