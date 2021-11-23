// Copyright Stream.IO, Inc. All Rights Reserved.

#include "User/UserRef.h"

#include "User/User.h"
#include "UserManager.h"

FUserRef::FUserRef(const FString& UserId, const TSharedRef<FUserManager>& Manager) : UserId{UserId}, Manager{Manager}
{
}

const FUser& FUserRef::GetUser() const
{
    // Only really for editor designer support
    if (!Manager)
    {
        Manager = MakeShared<FUserManager>();
    }
    return Manager->GetUser(*this);
}

const FUser& FUserRef::operator*() const
{
    return GetUser();
}

const FUser* FUserRef::operator->() const
{
    return &GetUser();
}

bool FUserRef::operator==(const FUserRef& Other) const
{
    return Other.UserId == UserId;
}

bool FUserRef::operator!=(const FUserRef& Other) const
{
    return Other.UserId != UserId;
}

bool FUserRef::IsValid() const
{
    if (UserId.IsEmpty())
    {
        return false;
    }
    if (!Manager.IsValid())
    {
        return false;
    }
    return Manager->HasUser(*this);
}

bool FUserRef::IsCurrent() const
{
    if (!Manager.IsValid())
    {
        return false;
    }
    return Manager->GetCurrentUser() == *this;
}

FUserUpdatedMultiDelegate& FUserRef::OnUpdate() const
{
    return Manager->OnUserUpdated(*this);
}
