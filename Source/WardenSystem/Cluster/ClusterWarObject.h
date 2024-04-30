// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WardenSystem/WardenSystemDataTypes.h"
#include "ClusterWarObject.generated.h"

/**
 * @class Working cluster
 */
UCLASS()
class WARDENSYSTEM_API UClusterWarObject : public UObject
{
    GENERATED_BODY()

#pragma region API

public:

    /** @public **/
    virtual void Tick(float Time);

    /** @public **/
    virtual void RunCluster();

    /** @public **/
    void RegisterClusterData(const FString& TagCluster, FClusterData_WS& Data);

    /** @public **/
    void UnRegisterClusterData(const FString& TagCluster, UObject* Object);

    /** @public **/
    void UnRegisterClusterTag(const FString& TagCluster);

    /** @public **/
    bool IsFreeSlotsByTagCluster(const FString& TagCluster) const;

    /** @public **/
    bool IsHaveFreeSlots() const;

    /** @public **/
    bool IsContainsClusterTag(const FString& TagCluster) const;
    
    /** @public **/
    FString GetDrawDebugInfo();

    /** @public **/
    FString GetDrawDebugVerify();
    
private:

    /** @private  **/
    void VerifyCluster();
    
#pragma endregion
    
#pragma region Data

private:

    /** @private Current accumulated call time **/
    float CallTime{0.0f};
    
    /** @private Index **/
    int32 TargetIndex{0};

    /** @private Cluster **/
    TMap<FString, TArray<FClusterData_WS>> ClusterContainer;

    /** @private Timer handle **/
    FTimerHandle TimerHandle_VerifyCluster;
    
#pragma endregion
};
