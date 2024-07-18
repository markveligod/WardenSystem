#include "ClusterWarThread.h"
#include "WardenSystem/WardenSystemDataTypes.h"
#include "WardenSystem/Settings/WardenSettings.h"

#pragma region RunnableInterface

FClusterWarThread::FClusterWarThread(FInitClusterThreadParams InitParams)
{
    Params = InitParams;
    Semaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
    Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("Thread_%s"), *FGuid::NewGuid().ToString()), 0, TPri_Lowest);
}

FClusterWarThread::~FClusterWarThread()
{
    if (Semaphore)
    {
        //Cleanup the FEvent
        FGenericPlatformProcess::ReturnSynchEventToPool(Semaphore);
        Semaphore = nullptr;
    }

    if (Thread)
    {
        //Cleanup the worker thread
        delete Thread;
        Thread = nullptr;
    }
}

bool FClusterWarThread::Init()
{
    LOG_WARDEN_SYSTEM(Display, "Thread: [%s] | Init cluster thread", *GetName());
    return true;
}

uint32 FClusterWarThread::Run()
{
    LOG_WARDEN_SYSTEM(Display, "Thread: [%s] | Run cluster thread", *GetName());

    //Initial wait before starting
    FPlatformProcess::Sleep(0.5f);

    while (!Kill)
    {
        LockMutexData();
        VerifyCluster();
        UnLockMutexData();

        //A little sleep between the chunks (So CPU will rest a bit -- (may be omitted))
        FPlatformProcess::Sleep(Params.TimeSleep);
    }
    return 0;
}

void FClusterWarThread::Stop()
{
    LOG_WARDEN_SYSTEM(Display, "Thread: [%s] | Stop cluster thread", *GetName());

    Kill = true; //Thread kill condition 
    if (Semaphore)
    {
        //We shall signal "Trigger" the FEvent (in case the Thread is sleeping it shall wake up!!)
        Semaphore->Trigger();
    }
}

void FClusterWarThread::Exit()
{
    LOG_WARDEN_SYSTEM(Display, "Thread: [%s] | Exit cluster thread", *GetName());
    FRunnable::Exit();
}

FString FClusterWarThread::GetName() const
{
    return Thread ? Thread->GetThreadName() : FString(TEXT("None"));
}

void FClusterWarThread::LockMutexData()
{
    Mutex.Lock();
}

void FClusterWarThread::UnLockMutexData()
{
    Mutex.Unlock();
}

#pragma endregion

#pragma region API

void FClusterWarThread::EnsureCompletion()
{
    Stop();

    if (Thread)
    {
        Thread->WaitForCompletion();
    }
}

void FClusterWarThread::RegisterClusterData(const FString& TagCluster, FClusterData_WS& Data)
{
    if (CLOG_WARDEN_SYSTEM(!Data.IsValidData(), "Data: [%s] is not valid", *Data.ToString())) return;

    LOG_WARDEN_SYSTEM(Display, "TagCluster: [%s] | Data: [%s]", *TagCluster, *Data.ToString());
    if (Params.ClusterContainer.Contains(TagCluster))
    {
        Params.ClusterContainer[TagCluster].AddUnique(Data);
    }
    else
    {
        TArray<FClusterData_WS> Array{MoveTemp(Data)};
        Params.ClusterContainer.Add(TagCluster, Array);
    }
}

void FClusterWarThread::UnRegisterClusterData(const FString& TagCluster, UObject* Object)
{
    if (CLOG_WARDEN_SYSTEM(!Object, "Object is nullptr")) return;
    if (CLOG_WARDEN_SYSTEM(TagCluster.IsEmpty(), "Tag cluster is empty")) return;
    if (CLOG_WARDEN_SYSTEM(!Params.ClusterContainer.Contains(TagCluster), "Tag cluster is not exist")) return;

    LOG_WARDEN_SYSTEM(Display, "Tag cluster: [%s] | Object: [%s]", *TagCluster, *GetNameSafe(Object));
    Params.ClusterContainer[TagCluster].RemoveAll([Object](const FClusterData_WS& Data)
    {
        return Data == Object;
    });
}

void FClusterWarThread::UnRegisterClusterTag(const FString& TagCluster)
{
    LOG_WARDEN_SYSTEM(Display, "Thread: [%s] | Tag cluster: [%s]", *GetName(), *TagCluster);
    Params.ClusterContainer.Remove(TagCluster);
}

bool FClusterWarThread::IsFreeSlotsByTagCluster(const FString& TagCluster) const
{
    if (TagCluster.IsEmpty()) return false;
    if (Params.ClusterContainer.Contains(TagCluster))
    {
        return true;
    }
    if (IsHaveFreeSlots())
    {
        return true;
    }
    return false;
}

bool FClusterWarThread::IsHaveFreeSlots() const
{
    const int32 MaxClusterNumber = UWardenSettings::GetMaxClusterHaveStatic();
    if (MaxClusterNumber != INDEX_NONE && Params.ClusterContainer.Num() < MaxClusterNumber)
    {
        return true;
    }
    return false;
}

bool FClusterWarThread::IsContainsClusterTag(const FString& TagCluster) const
{
    return Params.ClusterContainer.Contains(TagCluster);
}

FString FClusterWarThread::GetDrawDebugInfo()
{
    FString Result;
    for (auto& Pair : Params.ClusterContainer)
    {
        Result.Append(FString::Printf(TEXT("  TagCluster: [%s]\n"), *Pair.Key));
        for (auto& Data : Pair.Value)
        {
            Result.Append(FString::Printf(TEXT("    %s\n"), *Data.ToString()));
        }
    }
    return Result;
}

FString FClusterWarThread::GetDrawDebugVerify()
{
    FString Key = TEXT("None");
    TArray<FString> ArrayKeys;
    Params.ClusterContainer.GetKeys(ArrayKeys);
    if (ArrayKeys.IsValidIndex(Params.TargetIndex))
    {
        Key = ArrayKeys[Params.TargetIndex];
    }

    return FString::Printf(TEXT("    TargetIndex: [%i] | Key: [%s] \n"), Params.TargetIndex, *Key);
}

void FClusterWarThread::VerifyCluster()
{
    SCOPED_NAMED_EVENT(FClusterWarThread_VerifyCluster, FColor::Orange);

    TArray<FString> ArrayKeys;
    Params.ClusterContainer.GetKeys(ArrayKeys);

    if (!ArrayKeys.IsValidIndex(Params.TargetIndex))
    {
        Params.TargetIndex = 0;
    }
    if (!ArrayKeys.IsValidIndex(Params.TargetIndex)) return;

    const FString Key = ArrayKeys[Params.TargetIndex];
    if (Params.ClusterContainer.Contains(Key))
    {
        Params.ClusterContainer[Key].RemoveAll([](const FClusterData_WS& Data)
        {
            return !Data.IsValidData();
        });
        for (auto& Data : Params.ClusterContainer[Key])
        {
            Data.Execute();
        }
    }

    if (ArrayKeys.IsValidIndex(Params.TargetIndex + 1))
    {
        ++Params.TargetIndex;
    }
    else
    {
        Params.TargetIndex = 0;
    }
}

#pragma endregion
