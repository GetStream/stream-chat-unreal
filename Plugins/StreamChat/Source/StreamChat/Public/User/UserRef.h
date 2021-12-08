// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserRef.generated.h"

class UUserManager;
struct FUser;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserUpdatedMultiDelegate);

/**
 * @brief A reference to a unique User
 *
 * All users are stored once in a central location (UserManager).
 * This struct can be copied around freely and then dereferenced when the real user information is required.
 * This struct also provides access to a delegate which can be used to be informed of underlying user updates.
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUserRef
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FUserRef();

    /// Access the underlying user
    const FUser& GetUser() const;
    /// Access the underlying user
    const FUser& operator*() const;
    /// Access the underlying user
    const FUser* operator->() const;

    /// Do these reference the same user?
    bool operator==(const FUserRef&) const;
    /// Do these reference different users?
    bool operator!=(const FUserRef&) const;

    /// Will dereferencing this yield a valid user?
    bool IsValid() const;

    /// Is this user the currently logged-in local user?
    bool IsCurrent() const;

    /// Get a delegate which can be be used to be informed of the user's updates.
    /// The delegate is guaranteed to be unique to the underlying user.
    FUserUpdatedMultiDelegate& OnUpdate() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
    FString UserId;

private:
    friend UUserManager;

    FUserRef(const FString& UserId, UUserManager*);

    UPROPERTY(Transient)
    mutable UUserManager* Manager = nullptr;
};
