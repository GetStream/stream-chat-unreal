// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "UObject/WeakObjectPtr.h"

template <class T, class Arg = const T&>
class TCallbackAction final : public FPendingLatentAction
{
public:
    TCallbackAction(T& Result, const FLatentActionInfo& LatentInfo)
        : ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , Result(Result)
    {
    }
    TCallbackAction(const TCallbackAction&) = delete;
    TCallbackAction& operator=(const TCallbackAction&) = delete;

    static void CreateLatentAction(
        const UObject* WorldContextObject,
        const FLatentActionInfo LatentInfo,
        T& Output,
        TFunctionRef<void(TFunction<void(Arg)> Callback)> Action)
    {
        if (UWorld* World =
                GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
            if (LatentActionManager.FindExistingAction<TCallbackAction<T, Arg>>(
                    LatentInfo.CallbackTarget, LatentInfo.UUID))
            {
                return;
            }

            TCallbackAction<T, Arg>* DelegateAction = new TCallbackAction(Output, LatentInfo);
            Action(
                [DelegateAction](Arg InResult)
                {
                    DelegateAction->Result = InResult;
                    DelegateAction->bDone = true;
                });
            LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, DelegateAction);
        }
    }

private:
    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        Response.FinishAndTriggerIf(bDone, ExecutionFunction, OutputLink, CallbackTarget);
    }

    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;
    T& Result;
    bool bDone = false;
};
