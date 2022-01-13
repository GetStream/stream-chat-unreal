// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "User/UserManager.h"

#include "Engine/Engine.h"
#include "OwnUserDto.h"
#include "User/UserRef.h"
#include "UserObjectDto.h"
#include "Util.h"

UUserManager* UUserManager::Get()
{
    return GEngine->GetEngineSubsystem<UUserManager>();
}

void UUserManager::SetCurrentUser(const FUser& InCurrentUser)
{
    CurrentUser = UpsertUser(InCurrentUser);
}

const FUser& UUserManager::GetUser(const FUserRef& Ref)
{
    return Users.FindOrAdd(Ref.UserId);
}

bool UUserManager::HasUser(const FUserRef& Ref) const
{
    return Users.Contains(Ref.UserId);
}

const FUserRef& UUserManager::GetCurrentUser() const
{
    return CurrentUser;
}

FUserRef UUserManager::UpsertUser(const FUser& User)
{
    ensure(!User.Id.IsEmpty());

    const FUserRef Ref = FUserRef{User.Id, this};
    if (FUser* FoundUser = Users.Find(User.Id); !FoundUser || FoundUser->UpdatedAt < User.UpdatedAt)
    {
        Users.Add(User.Id, User);
        OnUserUpdated(Ref).Broadcast();
    }
    return Ref;
}

FUserRef UUserManager::UpsertUser(const FOwnUserDto& Dto)
{
    return UpsertUser(Util::Convert<FUser>(Dto));
}

FUserRef UUserManager::UpsertUser(const FUserObjectDto& Dto)
{
    return UpsertUser(Util::Convert<FUser>(Dto));
}

FUserRef UUserManager::UpsertUser(const FString& Id)
{
    return UpsertUser(FUser{Id});
}

TArray<FUserRef> UUserManager::UpsertUsers(const TArray<FUser>& NewUsers)
{
    TArray<FUserRef> Refs;
    for (const FUser& User : NewUsers)
    {
        Refs.Add(UpsertUser(User));
    }
    return Refs;
}

TArray<FUserRef> UUserManager::UpsertUsers(const TArray<FUserObjectDto>& Dtos)
{
    return UpsertUsers(Util::Convert<FUser>(Dtos));
}

FUserUpdatedMultiDelegate& UUserManager::OnUserUpdated(const FUserRef& Ref)
{
    return UserUpdatedDelegates.FindOrAdd(Ref.UserId);
}
