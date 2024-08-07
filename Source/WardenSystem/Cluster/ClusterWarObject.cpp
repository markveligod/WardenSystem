﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ClusterWarObject.h"
#include "WardenSystem/Settings/WardenSettings.h"

void UClusterWarObject::Tick(float Time)
{
    CallTime += Time;
    if (CallTime >= UWardenSettings::GetFreqCheckClusterStatic())
    {
        VerifyCluster();
    }
}

void UClusterWarObject::RunCluster()
{
    LOG_WARDEN_SYSTEM(Display, "Run cluster: [%s]", *GetName());
}

void UClusterWarObject::RegisterClusterData(const FString& TagCluster, FClusterData_WS& Data)
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

void UClusterWarObject::UnRegisterClusterData(const FString& TagCluster, UObject* Object)
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

void UClusterWarObject::UnRegisterClusterTag(const FString& TagCluster)
{
    LOG_WARDEN_SYSTEM(Display, "Tag cluster: [%s]", *TagCluster);
    ClusterContainer.Remove(TagCluster);
}

bool UClusterWarObject::IsFreeSlotsByTagCluster(const FString& TagCluster) const
{
    if (TagCluster.IsEmpty()) return false;
    if (ClusterContainer.Contains(TagCluster))
    {
        return true;
    }
    if (IsHaveFreeSlots())
    {
        return true;
    }
    return false;
}

bool UClusterWarObject::IsHaveFreeSlots() const
{
    const int32 MaxClusterNumber = UWardenSettings::GetMaxClusterHaveStatic();
    if (MaxClusterNumber != INDEX_NONE && ClusterContainer.Num() < MaxClusterNumber)
    {
        return true;
    }
    return false;
}

bool UClusterWarObject::IsContainsClusterTag(const FString& TagCluster) const
{
    return ClusterContainer.Contains(TagCluster);
}

FString UClusterWarObject::GetDrawDebugInfo()
{
    FString Result;
    for (auto& Pair : ClusterContainer)
    {
        Result.Append(FString::Printf(TEXT("  TagCluster: [%s]\n"), *Pair.Key));
        for (auto& Data : Pair.Value)
        {
            Result.Append(FString::Printf(TEXT("    %s\n"), *Data.ToString()));
        }
    }
    return Result;
}

FString UClusterWarObject::GetDrawDebugVerify()
{
    FString Key = TEXT("None");
    TArray<FString> ArrayKeys;
    ClusterContainer.GetKeys(ArrayKeys);
    if (ArrayKeys.IsValidIndex(TargetIndex))
    {
        Key = ArrayKeys[TargetIndex];
    }

    return FString::Printf(TEXT("    TargetIndex: [%i] | Key: [%s] | CallTime: [%f]\n"), TargetIndex, *Key, CallTime);
}

void UClusterWarObject::VerifyCluster()
{
    SCOPED_NAMED_EVENT(UClusterWarObject_VerifyCluster, FColor::Orange);

    CallTime = 0.0f;
    TArray<FString> ArrayKeys;
    ClusterContainer.GetKeys(ArrayKeys);

    if (!ArrayKeys.IsValidIndex(TargetIndex))
    {
        TargetIndex = 0;
    }
    if (!ArrayKeys.IsValidIndex(TargetIndex)) return;

    const FString Key = ArrayKeys[TargetIndex];
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

    if (ArrayKeys.IsValidIndex(TargetIndex + 1))
    {
        ++TargetIndex;
    }
    else
    {
        TargetIndex = 0;
    }
}
