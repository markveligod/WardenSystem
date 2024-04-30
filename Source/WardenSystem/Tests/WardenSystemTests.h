#pragma once

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

inline UWorld* GetTestGameWorld()
{
    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    auto FindElem = Algo::FindByPredicate(WorldContexts, [](const FWorldContext& Context)
    {
        return (Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game) && Context.World();
    });

    if (!FindElem)
    {
        FindElem = Algo::FindByPredicate(WorldContexts, [](const FWorldContext& Context)
        {
            return Context.WorldType == EWorldType::Editor && Context.World();
        });
    }

    return FindElem ? FindElem->World() : nullptr;
}

#endif

#include "WardenSystemTests.generated.h"

UCLASS()
class WARDENSYSTEM_API UWSTestObject : public UObject
{
    GENERATED_BODY()


public:

    UFUNCTION()
    void TestExecute() { bExecute = true; }

    bool bExecute{false};
};

