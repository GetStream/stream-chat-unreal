// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelMemberRequestDto.h"
#include "ChannelRequestDto.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/Message/MessageRequestDto.h"

#include "UpdateChannelRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateChannelRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateChannelRequestDto
{
    GENERATED_BODY()

    /// Message to send to the chat when channel is successfully updated
    void SetMessage(const FMessageRequestDto& Message)
    {
        AdditionalFields.Set(TEXT("message"), Message);
    }

    /// Set to `true` to accept the invite
    UPROPERTY()
    bool bAcceptInvite = false;

    /// Set to `true` to reject the invite
    UPROPERTY()
    bool bRejectInvite = false;

    /// Set to `true` to hide channel's history when adding new members
    UPROPERTY()
    bool bHideHistory = false;

    /// List of user IDs to add to the channel
    UPROPERTY()
    TArray<FChannelMemberRequestDto> AddMembers;

    /// List of user IDs to remove from the channel
    UPROPERTY()
    TArray<FString> RemoveMembers;

    /// List of user IDs to make channel moderators
    UPROPERTY()
    TArray<FString> AddModerators;

    /// List of user IDs to take away moderators status from
    UPROPERTY()
    TArray<FString> DemoteModerators;

    /// List of channel member role assignments. If any specified user is not part of the channel, the request will fail
    UPROPERTY()
    TArray<FChannelMemberRequestDto> AssignRoles;

    /// List of user IDs to invite to the channel
    UPROPERTY()
    TArray<FChannelMemberRequestDto> Invites;

    /// Sets cool down period for the channel in seconds
    UPROPERTY()
    uint32 Cooldown = TNumericLimits<uint32>::Max();

    /// Channel data to update
    UPROPERTY()
    FChannelRequestDto Data;

    /// When `message` is set disables all push notifications for it
    UPROPERTY()
    bool bSkipPush = false;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
