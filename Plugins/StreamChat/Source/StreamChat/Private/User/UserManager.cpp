// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserManager.h"

#include "Engine/Engine.h"
#include "User/UserRef.h"
#include "UserObjectDto.h"
#include "Util.h"

void UUserManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GIsEditor)
    {
        FUser TestUser1(TEXT("test-user-1"));
        TestUser1.Name = TEXT("Test user 1");
        TestUser1.Image = TEXT("https://avatars.dicebear.com/api/big-ears-neutral/test-user-1.png");
        FUser TestUser2(TEXT("test-user-2"));
        TestUser2.Name = TEXT("Test user 1 with especially long name");
        TestUser2.Image = TEXT("https://avatars.dicebear.com/api/big-ears-neutral/test-user-2.png");
        UpsertUser(TestUser2);
        CurrentUser = FOwnUser{UpsertUser(TestUser1)};
    }
}

UUserManager* UUserManager::Get()
{
    return GEngine->GetEngineSubsystem<UUserManager>();
}

void UUserManager::ResetCurrentUser()
{
    CurrentUser.Reset();
}

bool UUserManager::HasCurrentUser() const
{
    return CurrentUser.IsSet();
}

const FUser& UUserManager::GetUser(const FUserRef& Ref)
{
    return Users.FindOrAdd(Ref.UserId);
}

bool UUserManager::HasUser(const FUserRef& Ref) const
{
    return Users.Contains(Ref.UserId);
}

const FOwnUser& UUserManager::GetCurrentUser() const
{
    return CurrentUser.GetValue();
}

FUserRef UUserManager::UpsertUser(const FUser& User)
{
    if (User.Id.IsEmpty())
    {
        return {};
    }

    const FUserRef Ref = FUserRef{User.Id, this};
    FUser* FoundUser = Users.Find(User.Id);
    if (!FoundUser)
    {
        Users.Add(User.Id, User);
        OnUserUpdated(Ref).Broadcast();
    }
    else if (FoundUser->UpdatedAt <= User.UpdatedAt)
    {
        FoundUser->Update(User);
        OnUserUpdated(Ref).Broadcast();
    }
    return Ref;
}

FUserRef UUserManager::UpsertUser(const FUserDto& Dto)
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

const FOwnUser& UUserManager::SetCurrentUser(const FOwnUserDto& Dto)
{
    CurrentUser.Emplace(Dto, this);
    return CurrentUser.GetValue();
}
