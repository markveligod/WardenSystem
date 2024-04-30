// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WardenSystem/WardenSystemDataTypes.h"
#include "WardenSubsystem.generated.h"

class UClusterWarObject;
/**
 * @class The system implies control over clustering with callbacks
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
    virtual bool IsTickableInEditor() const override;
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
    void RegisterClusterDataTimerDelegate(const FString& TagCluster, const FTimerDelegate& TimerDelegate);

    /** @public Registering a cluster with a delegate timer STATIC. ONLY with UObject **/
    static void RegisterClusterDataTimerDelegateStatic(UObject* WorldContextObject, const FString& TagCluster, const FTimerDelegate& TimerDelegate);

    /** @public Registering a cluster with a delegate callback. ONLY with UObject **/
    UFUNCTION(BlueprintCallable, Category = "API")
    void RegisterClusterDataCallback(FString TagCluster, FWSCallbackSignature Callback);

    /** @public Registering a cluster with a delegate callback STATIC. ONLY with UObject **/
    UFUNCTION(BlueprintCallable, Category = "API", meta = (WorldContext = "WorldContextObject"))
    static void RegisterClusterDataCallbackStatic(UObject* WorldContextObject, FString TagCluster, FWSCallbackSignature Callback);

    /** @public Removing a cluster from verification **/
    UFUNCTION(BlueprintCallable, Category = "API")
    void UnRegisterClusterTag(FString TagCluster);

    /** @public Removing a cluster from verification STATIC **/
    UFUNCTION(BlueprintCallable, Category = "API", meta = (WorldContext = "WorldContextObject"))
    static void UnRegisterClusterTagStatic(UObject* WorldContextObject, FString TagCluster);

    /** @public Removing a cluster from verification **/
    UFUNCTION(BlueprintCallable, Category = "API")
    void UnRegisterClusterTagByObject(FString TagCluster, UObject* Object);

    /** @public Removing a cluster from verification STATIC **/
    UFUNCTION(BlueprintCallable, Category = "API", meta = (WorldContext = "WorldContextObject"))
    static void UnRegisterClusterTagByObjectStatic(UObject* WorldContextObject, FString TagCluster, UObject* Object);

    /** @public Finding an existing cluster by tag **/
    UFUNCTION(BlueprintCallable, Category = "API")
    bool IsExistClusterByTag(FString TagCluster);

    /** @public Finding an existing cluster by tag STATIC **/
    UFUNCTION(BlueprintCallable, Category = "API", meta = (WorldContext = "WorldContextObject"))
    static bool IsExistClusterByTagStatic(UObject* WorldContextObject, FString TagCluster);

private:

    /** @private  **/
    UClusterWarObject* FindClusterWarObject(const FString& TagCluster);

    /** @private  **/
    UClusterWarObject* FindExistClusterWarObject(const FString& TagCluster = TEXT(""));
    
    /** @private  **/
    void RegisterCluster_Internal(const FString& TagCluster, FClusterData_WS& Data);

    /** @private  **/
    UClusterWarObject* CreateNewCluster();

#pragma endregion

#pragma region Data

private:

    /** @private  **/
    UPROPERTY()
    TArray<UClusterWarObject*> ClusterWarObjects;
    
#pragma endregion
};
