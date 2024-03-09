// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WardenSystemDataTypes.h"
#include "WardenSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WARDENSYSTEM_API UWardenSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Default

public:
	/** @public Get Singleton class **/
	static UWardenSubsystem* Get(UWorld* World);

	/** @public Get Singleton class **/
	UFUNCTION(BlueprintPure, Category = "UAssetWorldSubsystem")
	static UWardenSubsystem* GetWardenSubsystemSingleton(UObject* Context);

	// FTickableGameObject implementation Begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject implementation End

	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem implementation End

#pragma endregion

#pragma region API

public:

    /** @public Registering a cluster with a delegate timer. ONLY with UObject **/
    void RegisterClusterDataTimerDelegate(const FString& TagCluster, FTimerDelegate TimerDelegate);

    /** @public Registering a cluster with a delegate timer STATIC. ONLY with UObject **/
    static void RegisterClusterDataTimerDelegateStatic(UObject* WorldContextObject, const FString& TagCluster, FTimerDelegate TimerDelegate);

    /** @public Registering a cluster with a delegate callback. ONLY with UObject **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem")
    void RegisterClusterDataCallback(FString TagCluster, FWSCallbackSignature Callback);

    /** @public Registering a cluster with a delegate callback STATIC. ONLY with UObject **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem", meta = (WorldContext = "WorldContextObject"))
    static void RegisterClusterDataCallbackStatic(UObject* WorldContextObject, FString TagCluster, FWSCallbackSignature Callback);
    
    /** @public Removing a cluster from verification **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem")
    void UnRegisterClusterTag(FString TagCluster);

    /** @public Removing a cluster from verification STATIC **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem", meta = (WorldContext = "WorldContextObject"))
    static void UnRegisterClusterTagStatic(UObject* WorldContextObject, FString TagCluster);

    /** @public Removing a cluster from verification **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem")
    void UnRegisterClusterTagByObject(FString TagCluster, UObject* Object);

    /** @public Removing a cluster from verification STATIC **/
    UFUNCTION(BlueprintCallable, Category = "UWardenSubsystem", meta = (WorldContext = "WorldContextObject"))
    static void UnRegisterClusterTagByObjectStatic(UObject* WorldContextObject, FString TagCluster, UObject* Object);

private:

    /** @private  **/
    void RegisterCluster_Internal(const FString& TagCluster, FClusterData_WS& Data);

    /** @private  **/
    void VerifyCluster();
    
#pragma endregion

#pragma region Data

private:

    /** @private Index **/
    int32 TargetIndex{0};
    
    /** @private Indexing clusters **/
    TArray<FString> ArrayIndexCluster;

    /** @private Cluster **/
    TMap<FString, TArray<FClusterData_WS>> ClusterContainer;

    /** @private Timer handle **/
    FTimerHandle TimerHandle_VerifyCluster;

#pragma endregion
};
