#pragma once

#include "CoreMinimal.h"

#include "UserRef.generated.h"

struct FUser;
class FUserManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserUpdatedMultiDelegate);

USTRUCT(BlueprintType)
struct STREAMCHAT_API FUserRef
{
    GENERATED_BODY()

public:
    FUserRef() = default;

    const FUser& GetUser() const;
    const FUser& operator*() const;
    const FUser* operator->() const;

    bool operator==(const FUserRef&) const;
    bool operator!=(const FUserRef&) const;

    bool IsValid() const;

    // Is this user the currently logged in local user
    bool IsCurrent() const;

    FUserUpdatedMultiDelegate& OnUpdate() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defaults)
    FString UserId;

private:
    friend FUserManager;
    FUserRef(const FString& UserId, const TSharedRef<FUserManager>&);
    mutable TSharedPtr<FUserManager> Manager;
};
