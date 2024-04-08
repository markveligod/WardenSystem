// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WardenSettings.generated.h"

/**
 * @class Configurations of the cluster monitoring system
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Warden Settings"))
class WARDENSYSTEM_API UWardenSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    /** @public  **/
    UFUNCTION(BlueprintPure, Category = "UWardenSettings")
    int32 GetMaxClusterHave() const;

    /** @public  **/
    UFUNCTION(BlueprintPure, Category = "UWardenSettings")
    float GetFreqCheckCluster() const;

    /** @public  **/
    UFUNCTION(BlueprintPure, Category = "UWardenSettings")
    static int32 GetMaxClusterHaveStatic();

    /** @public  **/
    UFUNCTION(BlueprintPure, Category = "UWardenSettings")
    static float GetFreqCheckClusterStatic();
    
private:

    /** @private **/
    UPROPERTY(Config, EditAnywhere, Category = "General", meta = (ClampMin = "1"))
    int32 MaxClusterHave{20};

    /** @private **/
    UPROPERTY(Config, EditAnywhere, Category = "General", meta = (ClampMin = "0.1", ForceUnits = "s"))
    float FreqCheckCluster{0.1f};

};
