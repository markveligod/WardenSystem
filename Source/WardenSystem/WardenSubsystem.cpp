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
