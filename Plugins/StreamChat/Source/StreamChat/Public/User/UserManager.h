// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OwnUser.h"
#include "OwnUserDto.h"
#include "Subsystems/EngineSubsystem.h"
#include "User/User.h"
#include "User/UserRef.h"

#include "UserManager.generated.h"

struct FUserObjectDto;
struct FOwnUserDto;

UCLASS()
class STREAMCHAT_API UUserManager final : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    static UUserManager* Get();

    const FUser& GetUser(const FUserRef&);
    bool HasUser(const FUserRef&) const;
    FUserRef UpsertUser(const FUser&);
    FUserRef UpsertUser(const FUserDto&);
    FUserRef UpsertUser(const FUserObjectDto&);
    FUserRef UpsertUser(const FString& Id);
    TArray<FUserRef> UpsertUsers(const TArray<FUser>&);
    TArray<FUserRef> UpsertUsers(const TArray<FUserObjectDto>&);
    FUserUpdatedMultiDelegate& OnUserUpdated(const FUserRef&);

    const FOwnUser& SetCurrentUser(const FOwnUserDto& Dto);

    /**
     * @brief Record the full set of users the current user has blocked
     *
     * The API reports blocks only in the response to the block endpoints, never in the own user
     * payload or in an event, so the current user's list is kept up to date from here.
     */
    void SetBlockedUserIds(const TArray<FString>& BlockedUserIds);

    /// Record that the current user has blocked or unblocked one user
    void SetUserBlocked(const FString& UserId, bool bBlocked);

    void ResetCurrentUser();
    bool HasCurrentUser() const;
    UFUNCTION(BlueprintPure, Category = "Stream|Users")
    const FOwnUser& GetCurrentUser() const;

private:
    TOptional<FOwnUser> CurrentUser;
    TMap<FString, FUser> Users;
    TMap<FString, FUserUpdatedMultiDelegate> UserUpdatedDelegates;
};
