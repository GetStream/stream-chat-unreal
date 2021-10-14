#include "StreamJson.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

FString Json::Serialize(const TSharedRef<FJsonObject>& JsonObject)
{
    FString OutJsonString;
    const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString);
    const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    ensure(bSuccess);
    JsonWriter->Close();
    return OutJsonString;
}
