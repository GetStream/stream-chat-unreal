// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfig.h"
#include "CoreMinimal.h"
#include "Member.h"
#include "Message.h"
#include "Request/Channel/ChannelRequestDto.h"

#include "ChannelProperties.generated.h"

struct FChannelResponseDto;

/**
 * @brief Encapsulates the static properties of a channel
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FChannelProperties
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChannelProperties();

    /// Create channel properties from a DTO from the API
    explicit FChannelProperties(const FChannelResponseDto&, UUserManager&);

    /// Convert to a channel request DTO to send to the API
    explicit operator FChannelRequestDto() const;

    static FChannelProperties WithType(const FString& InType);
    static FChannelProperties WithId(const FString& InType, const FString& InId);

    /// Set the members using just their User IDs
    FChannelProperties& SetMembers(const TArray<FString>& UserIds);
    /// Set the members using a list of Users
    FChannelProperties& SetMembers(const TArray<FUserRef>& Users);

    FChannelProperties& SetName(const FString&);
    FChannelProperties& SetImageUrl(const FString&);
    TOptional<FString> GetName() const;
    TOptional<FString> GetImageUrl() const;

    /// Get user information of channel members other than the currently logged-in user
    TArray<FUserRef> GetOtherMemberUsers() const;

    /// Type of channel, either built-in or custom
    /// Default types are livestream, messaging, team, gaming and commerce. You can also create your own types.
    /// @see https://getstream.io/chat/docs/other-rest/channel_features/
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties")
    FString Type;

    /// An identifier for this channel. Not globally unique. A subset of the Cid.
    /// Auto-generated if members are specified
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties")
    FString Id;

    /// The cid of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel Properties")
    FString Cid;

    /// Configuration of the channel (usually set via the dashboard)
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel Properties")
    FChannelConfig Config;

    /// The count of the members of this channel
    UPROPERTY()
    int32 MemberCount;

    /// The members of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel Properties")
    TArray<FMember> Members;

    /// Cooldown period after sending each message
    UPROPERTY()
    int32 Cooldown;

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt;

    /// The date of channel deletion
    UPROPERTY()
    FDateTime DeletedAt;

    /// The date of the last message
    UPROPERTY()
    FDateTime LastMessageAt;

    /// The user that created this channel
    UPROPERTY()
    FUserRef CreatedBy;

    UPROPERTY()
    bool bDisabled;

    /// Whether this channel is frozen or not
    UPROPERTY()
    bool bFrozen;

    /// Whether this channel is hidden by current user or not
    UPROPERTY()
    bool bHidden;

    /// Whether this channel is muted or not
    UPROPERTY()
    bool bMuted;

    /// Date of mute expiration
    UPROPERTY()
    FDateTime MuteExpiresAt;

    /// Date since when the message history is accessible
    UPROPERTY()
    FDateTime HideMessagesBefore;

    /// List of channel capabilities of the authenticated user
    UPROPERTY()
    TArray<FString> OwnCapabilities;

    /// Whether auto translation is enabled or not
    UPROPERTY()
    bool bAutoTranslationEnabled;

    /// Language to translate to when auto translation is active
    UPROPERTY()
    FString AutoTranslationLanguage;

    /// The team the channel belongs to (multi-tenant only)
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties", AdvancedDisplay)
    FString Team;

    /// Extra data for this channel
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties")
    FAdditionalFields ExtraData;

private:
    void SetMembers(UUserManager&, const TArray<FChannelMemberDto>&);
};

/**
 * @brief Blueprint functions for the Additional Fields struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMJSON_API UChannelPropertiesBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /// Set the members using just their User IDs
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel Properties", meta = (DisplayName = "Set Members (User ID)"))
    static void SetMembers_UserId(UPARAM(ref) FChannelProperties& ChannelProperties, const TArray<FString>& UserIds, FChannelProperties& Out);

    /// Set the members using just their User IDs
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel Properties", meta = (DisplayName = "Set Members (User)"))
    static void SetMembers_User(UPARAM(ref) FChannelProperties& ChannelProperties, const TArray<FUserRef>& Users, FChannelProperties& Out);
};
