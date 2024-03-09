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

private:

#pragma endregion

#pragma region Data

private:
    
    
#pragma endregion
};
