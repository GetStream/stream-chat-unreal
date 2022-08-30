// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
USTRUCT(BlueprintType, meta = (HasNativeMake))
struct STREAMCHAT_API FChannelProperties
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChannelProperties();

    /// Create channel properties from a DTO from the API
    explicit FChannelProperties(const FChannelResponseDto& Dto, UUserManager*);
    explicit FChannelProperties(const FChannelResponseDto& Dto, const TArray<FChannelMemberDto>& InMembers, UUserManager*);

    // For initial channel creation
    explicit FChannelProperties(const FString& Type, const FString& Id = TEXT(""), const FString& Team = TEXT(""), const FAdditionalFields& ExtraData = {});

    /// Convert to a channel request DTO to send to the API
    explicit operator FChannelRequestDto() const;

    /// Set the members using just their User IDs
    FChannelProperties& SetMembers(const TArray<FString>& UserIds);
    FChannelProperties& SetMembers(const TArray<FMember>&);
    /// Set the members using a list of Users
    FChannelProperties& SetMembers(const TArray<FUserRef>& Users);

    FChannelProperties& SetName(const FString&);
    FChannelProperties& SetImageUrl(const FString&);

    /// Merge new information from the API into this properties, retaining members
    void Merge(const FChannelResponseDto&, UUserManager*);
    /// Merge new information from the API into this properties, replacing members
    void Merge(const FChannelResponseDto& Dto, const TArray<FChannelMemberDto>& InMembers, UUserManager*);
    /// Append new members, skipping duplicates
    void AppendMembers(const TArray<FChannelMemberDto>& InMembers, UUserManager*);

    TOptional<FString> GetName() const;
    TOptional<FString> GetImageUrl() const;

    FMember* GetMember(const FUserRef&);
    /// Get user information of channel members other than the currently logged-in user
    TArray<FUserRef> GetOtherMemberUsers() const;

    /// Type of channel, either built-in or custom
    /// Default types are livestream, messaging, team, gaming and commerce. You can also create your own types.
    /// @see https://getstream.io/chat/docs/unreal/channel_features/
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
    int32 MemberCount = 0;

    /// The members of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel Properties")
    TArray<FMember> Members;

    /// Cooldown period after sending each message. No cooldown if zero.
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel Properties")
    FTimespan Cooldown = FTimespan::Zero();

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// The date of channel deletion
    UPROPERTY()
    FDateTime DeletedAt = FDateTime{0};

    /// The date of the last message
    UPROPERTY()
    FDateTime LastMessageAt = FDateTime{0};

    /// The user that created this channel
    UPROPERTY()
    FUserRef CreatedBy;

    UPROPERTY()
    bool bDisabled = false;

    /// Whether this channel is frozen or not
    UPROPERTY()
    bool bFrozen = false;

    /// Whether this channel is hidden by current user or not
    UPROPERTY()
    bool bHidden = false;

    /// Whether this channel is muted or not
    UPROPERTY()
    bool bMuted = false;

    /// Date of mute expiration
    UPROPERTY()
    FDateTime MuteExpiresAt = FDateTime{0};

    /// Date since when the message history is accessible
    UPROPERTY()
    FDateTime HideMessagesBefore = FDateTime{0};

    /// List of channel capabilities of the authenticated user
    UPROPERTY()
    TArray<FString> OwnCapabilities;

    /// Whether auto translation is enabled or not
    UPROPERTY()
    bool bAutoTranslationEnabled = false;

    /// Language to translate to when auto translation is active
    UPROPERTY()
    FString AutoTranslationLanguage;

    /// The team the channel belongs to (multi-tenant only)
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties", AdvancedDisplay)
    FString Team;

    /// Extra data for this channel
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|Channel Properties", AdvancedDisplay)
    FAdditionalFields ExtraData;
};

/**
 * @brief Blueprint functions for the Channel Properties struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMCHAT_API UChannelPropertiesBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /// Make a channel properties struct using a custom ID
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel Properties", meta = (NativeMakeFunc, DisplayName = "Make Channel Properties (ID)", AdvancedDisplay = "Team,ExtraData", AutoCreateRefTerm = ExtraData))
    static FChannelProperties MakeChannelPropertiesId(const FString& Type, const FString& Id, const FString& Team, const FAdditionalFields& ExtraData);
    /// Make a channel properties struct using a list of user IDs
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel Properties", meta = (NativeMakeFunc, DisplayName = "Make Channel Properties (User IDs)", AdvancedDisplay = "Team,ExtraData", AutoCreateRefTerm = ExtraData))
    static FChannelProperties MakeChannelPropertiesUserIds(
        const FString& Type,
        const TArray<FString>& UserIds,
        const FString& Team,
        const FAdditionalFields& ExtraData);
    /// Make a channel properties struct using a list of user refs
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Channel Properties", meta = (NativeMakeFunc, DisplayName = "Make Channel Properties (Users)", AdvancedDisplay = "Team,ExtraData", AutoCreateRefTerm = ExtraData))
    static FChannelProperties MakeChannelPropertiesUsers(
        const FString& Type,
        const TArray<FUserRef>& Users,
        const FString& Team,
        const FAdditionalFields& ExtraData);
};
