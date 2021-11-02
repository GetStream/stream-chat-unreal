#include "Response/Reaction/ReactionResponseDto.h"

#include "Detail/ExtraFields.h"
#include "Dom/JsonObject.h"

void FReactionResponseDto::DeserializeExtra(const FJsonObject& JsonObject, FReactionResponseDto& Dto)
{
    ExtraFields::InvokeDeserializeExtra(*JsonObject.GetObjectField(TEXT("message")), Dto.Message);
}
