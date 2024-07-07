#pragma once

#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/ThreadSafeBool.h"
#include "WardenSystem/WardenSystemDataTypes.h"

class FClusterWarThread final : public FRunnable
{

#pragma region RunnableInterface
    
public:

    FClusterWarThread(FInitClusterThreadParams InitParams);
    virtual ~FClusterWarThread() override;
    
    /**
     * Initializes the runnable object.
     *
     * This method is called in the context of the thread object that aggregates this, not the
     * thread that passes this runnable to a new thread.
     *
     * @return True if initialization was successful, false otherwise
     * @see Run, Stop, Exit
     */
    virtual bool Init() override;

    /**
     * Runs the runnable object.
     *
     * This is where all per object thread work is done. This is only called if the initialization was successful.
     *
     * @return The exit code of the runnable object
     * @see Init, Stop, Exit
     */
    virtual uint32 Run() override;

    /**
     * Stops the runnable object.
     *
     * This is called if a thread is requested to terminate early.
     * @see Init, Run, Exit
     */
    virtual void Stop() override;

    /**
     * Exits the runnable object.
     *
     * Called in the context of the aggregating thread to perform any cleanup.
     * @see Init, Run, Stop
     */
    virtual void Exit() override;

#pragma endregion

#pragma region API

public:

    /** @public **/
    FString GetName() const;

    /** @public **/
    void LockMutexData();

    /** @public **/
    void UnLockMutexData();

    /** @public Use this method to kill the thread! **/
    void EnsureCompletion();

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

    /** @private Thread to run the worker FRunnable on **/
    FRunnableThread* Thread{nullptr};

    /** @private **/
    FCriticalSection Mutex;

    /** @private **/
    FEvent* Semaphore{nullptr};

    /** @private **/
    FInitClusterThreadParams Params;

    /** @private **/
    FThreadSafeBool Kill{false};

#pragma endregion
};
