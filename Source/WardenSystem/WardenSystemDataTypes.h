#pragma once

#include "CoreMinimal.h"
#include "WardenSystemDataTypes.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWardenSystem, All, All);

namespace WardenSystemSpace
{
inline bool IsLogPrint()
{
    const IConsoleVariable* WardenSystemSpaceShowLog = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowLog"));
    if (WardenSystemSpaceShowLog)
    {
#if WITH_EDITOR
        return true;
#else
        return WardenSystemSpaceShowLog->GetBool();
#endif
    }

    return false;
}

inline bool ClogPrint(bool Cond, TCHAR* NameFunction, const FString& Text)
{
    if (Cond)
    {
        UE_LOG(LogWardenSystem, Error, TEXT("[%s] | TEXT:[%s]"), NameFunction, *Text);
    }
    return Cond;
}
}  // namespace WardenSystemSpace

#define LOG_WARDEN_SYSTEM(Verbosity, Format, ...)                                                                 \
    {                                                                                                             \
        if (WardenSystemSpace::IsLogPrint())                                                                  \
        {                                                                                                         \
            const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__);                                     \
            UE_LOG(LogWardenSystem, Verbosity, TEXT("[%s] | TEXT:[%s]"), ANSI_TO_TCHAR(__FUNCTION__), *Msg); \
        }                                                                                                         \
    }

#define CLOG_WARDEN_SYSTEM(Cond, Format, ...) \
    WardenSystemSpace::ClogPrint(Cond, ANSI_TO_TCHAR(__FUNCTION__), FString::Printf(TEXT(Format), ##__VA_ARGS__))

DECLARE_DYNAMIC_DELEGATE(FWSCallbackSignature);

UENUM()
enum class ETypeExecuteFunc_WS: uint8
{
    TimerDelegate,
    Callback,
};

UENUM()
enum class EModeRunThreadCluster: uint8
{
    GameThread,
    OtherThread,
};

struct FClusterData_WS
{
    ETypeExecuteFunc_WS Type{ETypeExecuteFunc_WS::TimerDelegate};
    FTimerDelegate TimerDelegate;
    FWSCallbackSignature CallBack;

    void Execute() const
    {
        if (Type == ETypeExecuteFunc_WS::TimerDelegate)
        {
            TimerDelegate.Execute();
        }
        if (Type == ETypeExecuteFunc_WS::Callback)
        {
            CallBack.Execute();
        }
    }
    
    bool operator==(const FClusterData_WS& OtherData) const
    {
        if (Type == ETypeExecuteFunc_WS::TimerDelegate)
        {
            return this->TimerDelegate.GetUObject() == OtherData.TimerDelegate.GetUObject();
        }
        if (Type == ETypeExecuteFunc_WS::Callback)
        {
            return this->CallBack.GetUObject() == OtherData.CallBack.GetUObject();
        }
        return false;
    }

    bool operator==(const UObject* Object) const
    {
        if (!Object) return false;
        if (Type == ETypeExecuteFunc_WS::TimerDelegate)
        {
            return this->TimerDelegate.GetUObject() == Object;
        }
        if (Type == ETypeExecuteFunc_WS::Callback)
        {
            return this->CallBack.GetUObject() == Object;
        }
        return false;
    }

    bool IsValidData() const
    {
        if (Type == ETypeExecuteFunc_WS::TimerDelegate)
        {
            return IsValid(TimerDelegate.GetUObject()) && TimerDelegate.IsBound();
        }
        if (Type == ETypeExecuteFunc_WS::Callback)
        {
            return IsValid(CallBack.GetUObject()) && CallBack.IsBound();
        }
        return false;
    }
    
    FString ToString() const
    {
        return FString::Printf(TEXT("Type: [%s] | TimerDelegate: [%s] | CallBack: [%s]"),
            *UEnum::GetValueAsString(Type), *GetNameSafe(TimerDelegate.GetUObject()), *GetNameSafe(CallBack.GetUObject()));
    }
};

struct FInitClusterThreadParams
{
    TWeakObjectPtr<UObject> Owner{nullptr};
    float TimeSleep{0.5f};
    int32 TargetIndex{0};
    TMap<FString, TArray<FClusterData_WS>> ClusterContainer;
};
