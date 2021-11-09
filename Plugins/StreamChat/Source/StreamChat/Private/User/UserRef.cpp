#include "User/UserRef.h"

#include "UserManager.h"

FUserRef::FUserRef(const FString& UserId, const TSharedRef<FUserManager>& Manager) : UserId{UserId}, Manager{Manager}
{
}

const FUser* FUserRef::GetUser() const
{
    if (Manager)
    {
        return Manager->GetUser(*this);
    }
    return nullptr;
}

const FUser* FUserRef::operator*() const
{
    return GetUser();
}

const FUser* FUserRef::operator->() const
{
    return GetUser();
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
    return Manager->GetCurrentUser() == *this;
}

FUserUpdatedMultiDelegate& FUserRef::OnUpdate() const
{
    return Manager->OnUserUpdated(*this);
}
