#pragma once

#include "CoreMinimal.h"
#include "WardenSystemDataTypes.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWardenSystem, All, All);

namespace WardenSystemSpace
{
inline bool IsLogPrint()
{
#if WITH_EDITOR
    return true;
#endif
#if !UE_BUILD_SHIPPING
    const auto WardenSystemSpaceShowLog = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowLog"));
    return WardenSystemSpaceShowLog ? WardenSystemSpaceShowLog->GetBool() : false;
#endif
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

