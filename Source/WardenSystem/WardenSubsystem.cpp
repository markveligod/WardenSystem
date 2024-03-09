// Fill out your copyright notice in the Description page of Project Settings.


#include "WardenSubsystem.h"

#pragma region Default

UWardenSubsystem* UWardenSubsystem::Get(UWorld* World)
{
	if (!World) return nullptr;
	if (CLOG_WARDEN_SYSTEM(!World, "World is nullptr")) return nullptr;
	return World->GetSubsystem<UWardenSubsystem>();
}

UWardenSubsystem* UWardenSubsystem::GetWardenSubsystemSingleton(UObject* Context)
{
    if (CLOG_WARDEN_SYSTEM(Context == nullptr, "Context is nullptr")) return nullptr;
    UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::LogAndReturnNull);
    if (CLOG_WARDEN_SYSTEM(World == nullptr, "World is nullptr")) return nullptr;
    return World->GetSubsystem<UWardenSubsystem>();
}

void UWardenSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING

    const auto WardenSystemShowData = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowData"));
    if (WardenSystemShowData && WardenSystemShowData->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | END WardenSystem.ShowData | ---"));
        for (auto& Pair : ClusterContainer)
        {
            for (auto& Data : Pair.Value)
            {
                FString StrData = FString::Printf(TEXT("    %s"), *Data.ToString());
                GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, StrData);
            }
            FString StrType = FString::Printf(TEXT("TagCluster: [%s]"), *Pair.Key);
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, StrType);
        }
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | START WardenSystem.ShowData | ---"));
    }

    const auto WardenSystemShowVerify = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowVerify"));
    if (WardenSystemShowVerify && WardenSystemShowVerify->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | END WardenSystem.ShowVerify | ---"));
        FString Key = TEXT("None");
        if (ArrayIndexCluster.IsValidIndex(TargetIndex))
        {
            Key = ArrayIndexCluster[TargetIndex];
        }

        const FString Result = FString::Printf(TEXT("TargetIndex: [%i] | Key: [%s]"), TargetIndex, *Key);
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result);
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | START WardenSystem.ShowVerify | ---"));
    }
#endif
}

TStatId UWardenSubsystem::GetStatId() const
{
    return TStatId();
}

void UWardenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_VerifyCluster, this, &ThisClass::VerifyCluster, 1.0f, true);
    }
}

void UWardenSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

#pragma endregion

#pragma region API

void UWardenSubsystem::RegisterClusterDataTimerDelegate(const FString& TagCluster, FTimerDelegate TimerDelegate)
{
    if (CLOG_WARDEN_SYSTEM(!TimerDelegate.GetUObject(), "Timer delegate is not UObject")) return;
    if (CLOG_WARDEN_SYSTEM(TagCluster.IsEmpty(), "Tag cluster is empty")) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | TimerDelegate: [%s]", *TagCluster, *GetNameSafe(TimerDelegate.GetUObject()));
    ArrayIndexCluster.AddUnique(TagCluster);
    FClusterData_WS Data;
    Data.Type = ETypeExecuteFunc_WS::TimerDelegate;
    Data.TimerDelegate = TimerDelegate;
    RegisterCluster_Internal(TagCluster, Data);
}

void UWardenSubsystem::RegisterClusterDataTimerDelegateStatic(UObject* WorldContextObject, const FString& TagCluster,
    FTimerDelegate TimerDelegate)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        WardenSubsystem->RegisterClusterDataTimerDelegate(TagCluster, TimerDelegate);
    }
}

void UWardenSubsystem::RegisterClusterDataCallback(FString TagCluster, FWSCallbackSignature Callback)
{
    if (CLOG_WARDEN_SYSTEM(!Callback.GetUObject(), "Callback delegate is not UObject")) return;
    if (CLOG_WARDEN_SYSTEM(TagCluster.IsEmpty(), "Tag cluster is empty")) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | Callback: [%s]", *TagCluster, *GetNameSafe(Callback.GetUObject()));
    ArrayIndexCluster.AddUnique(TagCluster);
    FClusterData_WS Data;
    Data.Type = ETypeExecuteFunc_WS::Callback;
    Data.CallBack = Callback;
    RegisterCluster_Internal(TagCluster, Data);
}

void UWardenSubsystem::RegisterClusterDataCallbackStatic(UObject* WorldContextObject, FString TagCluster, FWSCallbackSignature Callback)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        WardenSubsystem->RegisterClusterDataCallback(TagCluster, Callback);
    }
}

void UWardenSubsystem::UnRegisterClusterTag(FString TagCluster)
{
    LOG_WARDEN_SYSTEM(Display, "Tag cluster: [%s]", *TagCluster);
    ArrayIndexCluster.Remove(TagCluster);
    ClusterContainer.Remove(TagCluster);
}

void UWardenSubsystem::UnRegisterClusterTagStatic(UObject* WorldContextObject, FString TagCluster)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        WardenSubsystem->UnRegisterClusterTag(TagCluster);
    }
}

void UWardenSubsystem::UnRegisterClusterTagByObject(FString TagCluster, UObject* Object)
{
    if (CLOG_WARDEN_SYSTEM(!Object, "Object is nullptr")) return;
    if (CLOG_WARDEN_SYSTEM(TagCluster.IsEmpty(), "Tag cluster is empty")) return;
    if (CLOG_WARDEN_SYSTEM(!ClusterContainer.Contains(TagCluster), "Tag cluster is not exist")) return;

    LOG_WARDEN_SYSTEM(Display, "Tag cluster: [%s] | Object: [%s]", *TagCluster, *GetNameSafe(Object));
    ClusterContainer[TagCluster].RemoveAll([Object](const FClusterData_WS& Data)
    {
        return Data == Object;
    });
}

void UWardenSubsystem::UnRegisterClusterTagByObjectStatic(UObject* WorldContextObject, FString TagCluster, UObject* Object)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        WardenSubsystem->UnRegisterClusterTagByObject(TagCluster, Object);
    }
}

void UWardenSubsystem::RegisterCluster_Internal(const FString& TagCluster, FClusterData_WS& Data)
{
    if (CLOG_WARDEN_SYSTEM(!Data.IsValidData(), "Data: [%s] is not valid", *Data.ToString())) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | Data: [%s]", *TagCluster, *Data.ToString());
    if (ClusterContainer.Contains(TagCluster))
    {
        ClusterContainer[TagCluster].AddUnique(Data);
    }
    else
    {
        TArray<FClusterData_WS> Array{MoveTemp(Data)};
        ClusterContainer.Add(TagCluster, Array);
    }
}

void UWardenSubsystem::VerifyCluster()
{
    if (!ArrayIndexCluster.IsValidIndex(TargetIndex))
    {
        TargetIndex = 0;
    }
    if (!ArrayIndexCluster.IsValidIndex(TargetIndex)) return;

    const FString Key = ArrayIndexCluster[TargetIndex];
    if (ClusterContainer.Contains(Key))
    {
        ClusterContainer[Key].RemoveAll([](const FClusterData_WS& Data)
        {
            return !Data.IsValidData();
        });
        for (auto& Data : ClusterContainer[Key])
        {
            Data.Execute();
        }
    }

    if (ArrayIndexCluster.IsValidIndex(TargetIndex + 1))
    {
        ++TargetIndex;
    }
    else
    {
        TargetIndex = 0;
    }
}

#pragma endregion
