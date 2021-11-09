#pragma once

#include "CoreMinimal.h"
#include "User/User.h"
#include "User/UserRef.h"
#include "UserObjectDto.h"

class FUserManager : public TSharedFromThis<FUserManager>
{
public:
    static TSharedRef<FUserManager> Create(const FUser& InCurrentUser);
    const FUser* GetUser(const FUserRef&) const;
    bool HasUser(const FUserRef&) const;
    const FUserRef& GetCurrentUser() const;
    FUserRef UpsertUser(const FUser&);
    FUserRef UpsertUser(const FUserObjectDto&);
    TArray<FUserRef> UpsertUsers(const TArray<FUser>&);
    TArray<FUserRef> UpsertUsers(const TArray<FUserObjectDto>&);
    FUserUpdatedMultiDelegate& OnUserUpdated(const FUserRef&);

private:
    FUserRef CurrentUser;
    TMap<FString, FUser> Users;
    TMap<FString, FUserUpdatedMultiDelegate> UserUpdatedDelegates;
};
