// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "User/User.h"
#include "User/UserRef.h"

#include "UserManager.generated.h"

struct FUserObjectDto;
struct FOwnUserDto;

UCLASS()
class UUserManager final : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    static UUserManager* Get();

    void SetCurrentUser(const FUser& InCurrentUser);
    const FUser& GetUser(const FUserRef&);
    bool HasUser(const FUserRef&) const;
    const FUserRef& GetCurrentUser() const;
    FUserRef UpsertUser(const FUser&);
    FUserRef UpsertUser(const FOwnUserDto&);
    FUserRef UpsertUser(const FUserObjectDto&);
    TArray<FUserRef> UpsertUsers(const TArray<FUser>&);
    TArray<FUserRef> UpsertUsers(const TArray<FUserObjectDto>&);
    FUserUpdatedMultiDelegate& OnUserUpdated(const FUserRef&);

private:
    FUserRef CurrentUser;
    TMap<FString, FUser> Users;
    TMap<FString, FUserUpdatedMultiDelegate> UserUpdatedDelegates;
};
