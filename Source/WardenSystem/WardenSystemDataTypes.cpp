
#include "WardenSystemDataTypes.h"

#if !UE_BUILD_SHIPPING

static TAutoConsoleVariable<bool> EnableD_WardenSystemShowLog(
	TEXT("WardenSystem.ShowLog"), false, TEXT("WardenSystem.ShowLog [true/false]"), ECVF_Cheat);

static TAutoConsoleVariable<bool> EnableD_WardenSystemShowData(
    TEXT("WardenSystem.ShowData"), false, TEXT("WardenSystem.ShowData [true/false]"), ECVF_Cheat);

static TAutoConsoleVariable<bool> EnableD_WardenSystemShowVerify(
    TEXT("WardenSystem.ShowVerify"), false, TEXT("WardenSystem.ShowVerify [true/false]"), ECVF_Cheat);

#endif