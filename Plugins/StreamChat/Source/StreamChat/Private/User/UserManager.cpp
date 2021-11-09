#include "User/UserManager.h"

#include "User/UserRef.h"
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
    Users.Add(User.Id, User);
    const FUserRef Ref = FUserRef{User.Id, AsShared()};
    OnUserUpdated(Ref).Broadcast();
    return Ref;
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
