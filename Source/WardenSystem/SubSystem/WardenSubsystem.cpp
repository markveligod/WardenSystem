// Fill out your copyright notice in the Description page of Project Settings.


#include "WardenSubsystem.h"

#include "Algo/ForEach.h"
#include "WardenSystem/Cluster/ClusterWarObject.h"
#include "WardenSystem/Settings/WardenSettings.h"

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

    const int32 StableFPS = UWardenSettings::GetStableFPSStatic();
    const float Approx = (1.0f / static_cast<float>(StableFPS)) * 100.0f;
    const float Time = DeltaTime * Approx;

    for (auto* Cluster : ClusterWarObjects)
    {
        if (!Cluster) continue;
        Cluster->Tick(Time);
    }

#if !UE_BUILD_SHIPPING

    const auto WardenSystemShowData = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowData"));
    if (WardenSystemShowData && WardenSystemShowData->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | END WardenSystem.ShowData | ---"));
        for (int32 i = 0; i < ClusterWarObjects.Num(); ++i)
        {
            UClusterWarObject* Cluster = ClusterWarObjects[i];
            if (!Cluster) continue;

            FString Info = FString::Printf(TEXT("Index cluster: [%i]\n"), i);
            Info.Append(Cluster->GetDrawDebugInfo());
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Info);
        }
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | START WardenSystem.ShowData | ---"));
    }

    const auto WardenSystemShowVerify = IConsoleManager::Get().FindConsoleVariable(TEXT("WardenSystem.ShowVerify"));
    if (WardenSystemShowVerify && WardenSystemShowVerify->GetBool() && GEngine && GetWorld() && !GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | END WardenSystem.ShowVerify | ---"));
        for (int32 i = 0; i < ClusterWarObjects.Num(); ++i)
        {
            UClusterWarObject* Cluster = ClusterWarObjects[i];
            if (!Cluster) continue;

            FString Info = FString::Printf(TEXT("Index cluster: [%i]\n"), i);
            Info.Append(Cluster->GetDrawDebugVerify());
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Info);
        }
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("--- | START WardenSystem.ShowVerify | ---"));
    }
#endif
}

bool UWardenSubsystem::IsTickableInEditor() const
{
    return true;
}

TStatId UWardenSubsystem::GetStatId() const
{
    return TStatId();
}

void UWardenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

}

void UWardenSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

#pragma endregion

#pragma region API

void UWardenSubsystem::RegisterClusterDataTimerDelegate(const FString& TagCluster, const FTimerDelegate& TimerDelegate)
{
    if (CLOG_WARDEN_SYSTEM(!TimerDelegate.GetUObject(), "Timer delegate is not UObject")) return;
    if (CLOG_WARDEN_SYSTEM(TagCluster.IsEmpty(), "Tag cluster is empty")) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | TimerDelegate: [%s]", *TagCluster, *GetNameSafe(TimerDelegate.GetUObject()));
    FClusterData_WS Data;
    Data.Type = ETypeExecuteFunc_WS::TimerDelegate;
    Data.TimerDelegate = TimerDelegate;
    RegisterCluster_Internal(TagCluster, Data);
}

void UWardenSubsystem::RegisterClusterDataTimerDelegateStatic(UObject* WorldContextObject, const FString& TagCluster,
    const FTimerDelegate& TimerDelegate)
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
    if (UClusterWarObject* ClusterWarObject = FindClusterWarObject(TagCluster))
    {
        ClusterWarObject->UnRegisterClusterTag(TagCluster);
    }
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

    if (UClusterWarObject* ClusterWarObject = FindClusterWarObject(TagCluster))
    {
        ClusterWarObject->UnRegisterClusterData(TagCluster, Object);
    }
}

void UWardenSubsystem::UnRegisterClusterTagByObjectStatic(UObject* WorldContextObject, FString TagCluster, UObject* Object)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        WardenSubsystem->UnRegisterClusterTagByObject(TagCluster, Object);
    }
}

bool UWardenSubsystem::IsExistClusterByTag(FString TagCluster)
{
    return FindClusterWarObject(TagCluster) != nullptr;
}

bool UWardenSubsystem::IsExistClusterByTagStatic(UObject* WorldContextObject, FString TagCluster)
{
    if (UWardenSubsystem* WardenSubsystem = UWardenSubsystem::GetWardenSubsystemSingleton(WorldContextObject))
    {
        return WardenSubsystem->FindClusterWarObject(TagCluster) != nullptr;
    }
    return false;
}

UClusterWarObject* UWardenSubsystem::FindClusterWarObject(const FString& TagCluster)
{
    const auto* FindElem = ClusterWarObjects.FindByPredicate([TagCluster](UClusterWarObject* Cluster)
    {
        if (Cluster)
        {
            return Cluster->IsContainsClusterTag(TagCluster);
        }
        return false;
    });
    return FindElem ? *FindElem : nullptr;
}

UClusterWarObject* UWardenSubsystem::FindExistClusterWarObject(const FString& TagCluster)
{
    if (!TagCluster.IsEmpty())
    {
        if (UClusterWarObject* ClusterWarObject = FindClusterWarObject(TagCluster))
        {
            return ClusterWarObject;
        }
    }

    for (UClusterWarObject* Cluster : ClusterWarObjects)
    {
        if (!Cluster) continue;
        if (Cluster->IsHaveFreeSlots())
        {
            return Cluster;
        }
    }
    return CreateNewCluster();
}

void UWardenSubsystem::RegisterCluster_Internal(const FString& TagCluster, FClusterData_WS& Data)
{
    if (CLOG_WARDEN_SYSTEM(!Data.IsValidData(), "Data: [%s] is not valid", *Data.ToString())) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | Data: [%s]", *TagCluster, *Data.ToString());
    UClusterWarObject* ClusterWarObject = FindExistClusterWarObject(TagCluster);
    if (CLOG_WARDEN_SYSTEM(!ClusterWarObject, "FIND/CREATED CLUSTER IS FAILED")) return;
    ClusterWarObject->RegisterClusterData(TagCluster, Data);
}

UClusterWarObject* UWardenSubsystem::CreateNewCluster()
{
    UClusterWarObject* Cluster = NewObject<UClusterWarObject>(this, UClusterWarObject::StaticClass());
    if (Cluster)
    {
        LOG_WARDEN_SYSTEM(Display, "Create new cluster: [%s]", *Cluster->GetName());
        Cluster->RunCluster();
        ClusterWarObjects.Add(Cluster);
    }
    return Cluster;
}

#pragma endregion
