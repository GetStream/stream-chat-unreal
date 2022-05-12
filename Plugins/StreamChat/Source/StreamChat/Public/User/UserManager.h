// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

    void SetCurrentUser(const FUserRef& InCurrentUser);
    void ResetCurrentUser();
    const FUser& GetUser(const FUserRef&);
    bool HasUser(const FUserRef&) const;
    FUserRef UpsertUser(const FUser&);
    FUserRef UpsertUser(const FOwnUserDto&);
    FUserRef UpsertUser(const FUserObjectDto&);
    FUserRef UpsertUser(const FString& Id);
    TArray<FUserRef> UpsertUsers(const TArray<FUser>&);
    TArray<FUserRef> UpsertUsers(const TArray<FUserObjectDto>&);
    FUserUpdatedMultiDelegate& OnUserUpdated(const FUserRef&);

    UFUNCTION(BlueprintPure, Category = "Stream|Users")
    const FUserRef& GetCurrentUser() const;

private:
    FUserRef CurrentUser;
    TMap<FString, FUser> Users;
    TMap<FString, FUserUpdatedMultiDelegate> UserUpdatedDelegates;
};
