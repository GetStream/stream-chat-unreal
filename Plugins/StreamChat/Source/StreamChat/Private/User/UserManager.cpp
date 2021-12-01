// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "User/UserManager.h"

#include "OwnUserDto.h"
#include "User/UserRef.h"
#include "UserObjectDto.h"
#include "Util.h"

TSharedRef<FUserManager> FUserManager::Create(const FUser& InCurrentUser)
{
    TSharedRef<FUserManager> Manager = MakeShared<FUserManager>();
    Manager->CurrentUser = Manager->UpsertUser(InCurrentUser);
    return Manager;
}

const FUser& FUserManager::GetUser(const FUserRef& Ref)
{
    return Users.FindOrAdd(Ref.UserId);
}

bool FUserManager::HasUser(const FUserRef& Ref) const
{
    return Users.Contains(Ref.UserId);
}

const FUserRef& FUserManager::GetCurrentUser() const
{
    return CurrentUser;
}

FUserRef FUserManager::UpsertUser(const FUser& User)
{
    ensure(!User.Id.IsEmpty());

    const FUserRef Ref = FUserRef{User.Id, AsShared()};
    if (FUser* FoundUser = Users.Find(User.Id); !FoundUser || FoundUser->UpdatedAt < User.UpdatedAt)
    {
        Users.Add(User.Id, User);
        OnUserUpdated(Ref).Broadcast();
    }
    return Ref;
}

FUserRef FUserManager::UpsertUser(const FOwnUserDto& Dto)
{
    return UpsertUser(Util::Convert<FUser>(Dto));
}

FUserRef FUserManager::UpsertUser(const FUserObjectDto& Dto)
{
    return UpsertUser(Util::Convert<FUser>(Dto));
}

TArray<FUserRef> FUserManager::UpsertUsers(const TArray<FUser>& NewUsers)
{
    TArray<FUserRef> Refs;
    for (const FUser& User : NewUsers)
    {
        Refs.Add(UpsertUser(User));
    }
    return Refs;
}

TArray<FUserRef> FUserManager::UpsertUsers(const TArray<FUserObjectDto>& Dtos)
{
    return UpsertUsers(Util::Convert<FUser>(Dtos));
}

FUserUpdatedMultiDelegate& FUserManager::OnUserUpdated(const FUserRef& Ref)
{
    return UserUpdatedDelegates.FindOrAdd(Ref.UserId);
}
