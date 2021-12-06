// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "User/UserRef.h"

#include "User/User.h"
#include "UserManager.h"

FUserRef::FUserRef(const FString& UserId, UUserManager* Manager) : UserId{UserId}, Manager{Manager}
{
}

FUserRef::FUserRef() = default;

const FUser& FUserRef::GetUser() const
{
    if (!::IsValid(Manager))
    {
        Manager = UUserManager::Get();
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
    if (!::IsValid(Manager))
    {
        return false;
    }
    return Manager->HasUser(*this);
}

bool FUserRef::IsCurrent() const
{
    if (!::IsValid(Manager))
    {
        return false;
    }
    return Manager->GetCurrentUser() == *this;
}

FUserUpdatedMultiDelegate& FUserRef::OnUpdate() const
{
    return Manager->OnUserUpdated(*this);
}
