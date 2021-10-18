#pragma once

#include "CoreMinimal.h"
#include "Misc/TVariant.h"

struct HTTPREQUESTS_API FQueryParameter
{
    // ReSharper disable CppNonExplicitConvertingConstructor
    FQueryParameter(bool bValue);
    FQueryParameter(float Value);
    FQueryParameter(int32 Value);
    FQueryParameter(const FString& Value);
    FQueryParameter(const TCHAR* Value);

    FString ToString() const;

private:
    TVariant<bool, float, int32, FString> Data;
};

using FQueryParameters = TMap<FString, FQueryParameter>;
