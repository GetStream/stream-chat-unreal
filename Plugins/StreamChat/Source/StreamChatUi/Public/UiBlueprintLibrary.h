#pragma once

#include "CoreMinimal.h"
#include "Input/Events.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "UiBlueprintLibrary.generated.h"

class UWidget;

UCLASS()
class STREAMCHATUI_API UUiBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat UI")
    static void AddContextMenu(UWidget* Widget, const FPointerEvent& InMouseEvent);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat UI")
    static void DismissContextMenu(UWidget* Widget);
};
