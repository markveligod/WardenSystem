// Fill out your copyright notice in the Description page of Project Settings.


#include "WardenSettings.h"

int32 UWardenSettings::GetMaxClusterHave() const
{
    return MaxClusterHave;
}

float UWardenSettings::GetFreqCheckCluster() const
{
    return FreqCheckCluster;
}

int32 UWardenSettings::GetMaxClusterHaveStatic()
{
    if (const UWardenSettings* WardenSettings = GetDefault<UWardenSettings>())
    {
        return WardenSettings->GetMaxClusterHave();
    }
    return INDEX_NONE;
}

float UWardenSettings::GetFreqCheckClusterStatic()
{
    if (const UWardenSettings* WardenSettings = GetDefault<UWardenSettings>())
    {
        return WardenSettings->GetFreqCheckCluster();
    }
    return 0.0f;
}
