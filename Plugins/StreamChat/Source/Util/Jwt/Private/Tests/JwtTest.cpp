// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Jwt.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJwtTest, "StreamChat.Jwt", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FJwtTest::RunTest(const FString& Parameters)
{
    {
        const FString UserId = TEXT("abc");
        const FString ApiSecret = TEXT("def");
        const FString Jwt = Jwt::GenerateJwt(ApiSecret, UserId);

        TestEqual("jwt", Jwt, TEXT("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoiYWJjIn0.2OmipI31zHUwwTWBygve_VR5JeO9jHpG7RSn9NX0zBo"));
    }

    return true;
}

#endif
