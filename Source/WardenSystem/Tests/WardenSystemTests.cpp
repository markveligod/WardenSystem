
#include "WardenSystem/Tests/WardenSystemTests.h"
#include "WardenSystem/SubSystem/WardenSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWardenSystemRegisterObjectTest, "WardenSystem.RegisterObject",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority)

bool FWardenSystemRegisterObjectTest::RunTest(const FString& Parameters)
{
    const UWorld* World = GetTestGameWorld();
    if (!TestNotNull("World is nullptr", World)) return false;
    UWardenSubsystem* WS = World->GetSubsystem<UWardenSubsystem>();
    if (!TestNotNull("WardenSubsystem is nullptr", WS)) return false;

    UWSTestObject* TestObject = NewObject<UWSTestObject>();
    if (!TestNotNull("TestObject is nullptr", TestObject)) return false;

    const FString TestName1(TEXT("TestName1"));
    FWSCallbackSignature Callback;
    Callback.BindDynamic(TestObject, &UWSTestObject::TestExecute);
    WS->RegisterClusterDataCallback(TestName1, Callback);
    if (!TestTrue(TEXT("RegisterClusterDataCallback is failed | TestName1 is not exist in cluster"), WS->IsExistClusterByTag(TestName1))) return false;

    WS->UnRegisterClusterTag(TestName1);
    if (!TestFalse(TEXT("UnRegisterClusterTag is failed | TestName1 is exist in cluster"), WS->IsExistClusterByTag(TestName1))) return false;

    const FString TestName2(TEXT("TestName2"));
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(TestObject, &UWSTestObject::TestExecute);
    WS->RegisterClusterDataTimerDelegate(TestName2, TimerDelegate);
    if (!TestTrue(TEXT("RegisterClusterDataTimerDelegate is failed | TestName2 is not exist in cluster"), WS->IsExistClusterByTag(TestName2))) return false;

    WS->UnRegisterClusterTag(TestName2);
    if (!TestFalse(TEXT("RegisterClusterDataTimerDelegate is failed | TestName2 is exist in cluster"), WS->IsExistClusterByTag(TestName2))) return false;

    return true;
}
