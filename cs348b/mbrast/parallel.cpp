
/*
    pbrt source code Copyright(c) 1998-2012 Matt Pharr and Greg Humphreys.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// core/parallel.cpp*
#include "parallel.h"
#include "memory.h"
#if !defined(PBRT_IS_WINDOWS)
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <errno.h>
#endif 
#include <assert.h>
#include <list>
#include <stdio.h>
#include <stdint.h>

// Parallel Local Declarations
#if defined(PBRT_IS_WINDOWS)
static HANDLE *threads;
#else
static pthread_t *threads;
#endif
static Mutex *taskQueueMutex = Mutex::Create();
static std::vector<Task *> taskQueue;
static Semaphore *workerSemaphore;
static uint32_t numUnfinishedTasks;
static ConditionVariable *tasksRunningCondition;
static
#if defined(PBRT_IS_WINDOWS)
DWORD WINAPI taskEntry(LPVOID arg);
#else
void *taskEntry(void *arg);
#endif

// Parallel Definitions
#if !defined(PBRT_IS_WINDOWS)

Mutex *Mutex::Create() {
    return new Mutex;
}



void Mutex::Destroy(Mutex *m) {
    delete m;
}



Mutex::Mutex() {
    int err;
    if ((err = pthread_mutex_init(&mutex, NULL)) != 0)
        fprintf(stderr, "Error from pthread_mutex_init: %s", strerror(err));
}



Mutex::~Mutex() {
    int err;
    if ((err = pthread_mutex_destroy(&mutex)) != 0)
        fprintf(stderr, "Error from pthread_mutex_destroy: %s", strerror(err));
}




MutexLock::MutexLock(Mutex &m) : mutex(m) {
    int err;
    if ((err = pthread_mutex_lock(&m.mutex)) != 0)
        fprintf(stderr, "Error from pthread_mutex_lock: %s", strerror(err));
}



MutexLock::~MutexLock() {
    int err;
    if ((err = pthread_mutex_unlock(&mutex.mutex)) != 0)
        fprintf(stderr, "Error from pthread_mutex_unlock: %s", strerror(err));
}




RWMutex *RWMutex::Create() {
    return new RWMutex;
}



void RWMutex::Destroy(RWMutex *m) {
    delete m;
}



RWMutex::RWMutex() {
    int err;
    if ((err = pthread_rwlock_init(&mutex, NULL)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_init: %s", strerror(err));
}



RWMutex::~RWMutex() {
    int err;
    if ((err = pthread_rwlock_destroy(&mutex)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_destroy: %s", strerror(err));
}




RWMutexLock::RWMutexLock(RWMutex &m, RWMutexLockType t) : type(t), mutex(m) {
    int err;
    if (t == READ) err = pthread_rwlock_rdlock(&m.mutex);
    else           err = pthread_rwlock_wrlock(&m.mutex);
}



RWMutexLock::~RWMutexLock() {
    int err;
    if ((err = pthread_rwlock_unlock(&mutex.mutex)) != 0)
       fprintf(stderr, "Error from pthread_rwlock_unlock: %s", strerror(err));
}



void RWMutexLock::UpgradeToWrite() {
    assert(type == READ);
    int err;
    if ((err = pthread_rwlock_unlock(&mutex.mutex)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_unlock: %s", strerror(err));
    if ((err = pthread_rwlock_wrlock(&mutex.mutex)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_wrlock: %s", strerror(err));
    type = WRITE;
}



void RWMutexLock::DowngradeToRead() {
    assert(type == WRITE);
    int err;
    if ((err = pthread_rwlock_unlock(&mutex.mutex)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_unlock: %s", strerror(err));
    if ((err = pthread_rwlock_rdlock(&mutex.mutex)) != 0)
        fprintf(stderr, "Error from pthread_rwlock_rdlock: %s", strerror(err));
    type = READ;
}


#endif // !PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)

Mutex *Mutex::Create() {
    return new Mutex;
}



void Mutex::Destroy(Mutex *m) {
    delete m;
}



Mutex::Mutex() {
    InitializeCriticalSection(&criticalSection);
}



Mutex::~Mutex() {
    DeleteCriticalSection(&criticalSection);
}



MutexLock::MutexLock(Mutex &m) : mutex(m) {
    EnterCriticalSection(&mutex.criticalSection);
}



MutexLock::~MutexLock() {
    LeaveCriticalSection(&mutex.criticalSection);
}



RWMutex *RWMutex::Create() {
    return new RWMutex;
}



void RWMutex::Destroy(RWMutex *m) {
    delete m;
}



// vista has 'slim reader/writer (SRW)' locks... sigh.

RWMutex::RWMutex() {
    numWritersWaiting = numReadersWaiting = activeWriterReaders = 0;
    InitializeCriticalSection(&cs);

    hReadyToRead = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hReadyToRead == NULL) {
        fprintf(stderr, "Error creating event for RWMutex: %d", GetLastError());
    }

    hReadyToWrite = CreateSemaphore(NULL, 0, 1, NULL);
    if (hReadyToWrite == NULL) {
        DWORD lastError = GetLastError();
        CloseHandle(hReadyToRead);
        fprintf(stderr, "Error creating semaphore for RWMutex: %d", lastError);
    }
}



RWMutex::~RWMutex() {
    if (hReadyToRead)
        CloseHandle(hReadyToRead);
    if (hReadyToWrite != NULL)
        CloseHandle(hReadyToWrite);
    DeleteCriticalSection(&cs);
}




RWMutexLock::RWMutexLock(RWMutex &m, RWMutexLockType t)
    : type(t), mutex(m) {
    if (type == READ) mutex.AcquireRead();
    else              mutex.AcquireWrite();
}



void
RWMutex::AcquireRead() {
    bool fNotifyReaders = false;

    EnterCriticalSection(&cs);

    if ((numWritersWaiting > 0) || (HIWORD(activeWriterReaders) > 0)) {
        ++numReadersWaiting;

        while (true) {
            ResetEvent(hReadyToRead);
            LeaveCriticalSection(&cs);
            WaitForSingleObject(hReadyToRead, INFINITE);
            EnterCriticalSection(&cs);

            // The reader is only allowed to read if there aren't
            // any writers waiting and if a writer doesn't own the
            // lock.
            if ((numWritersWaiting == 0) && (HIWORD(activeWriterReaders) == 0))
                break;
        }

        // Reader is done waiting.
        --numReadersWaiting;

        // Reader can read.
        ++activeWriterReaders;
    }
    else {
        // Reader can read.
        if ((++activeWriterReaders == 1) && (numReadersWaiting != 0)) {
            // Set flag to notify other waiting readers
            // outside of the critical section
            // so that they don't when the threads
            // are dispatched by the scheduler they
            // don't immediately block on the critical
            // section that this thread is holding.
            fNotifyReaders = true;
        }
    }

    assert(HIWORD(activeWriterReaders) == 0);
    LeaveCriticalSection(&cs);

    if (fNotifyReaders)
        SetEvent(hReadyToRead);
}




void
RWMutex::AcquireWrite() {
    EnterCriticalSection(&cs);

    // Are there active readers?
    if (activeWriterReaders != 0) {
        ++numWritersWaiting;

        LeaveCriticalSection(&cs);
        WaitForSingleObject(hReadyToWrite, INFINITE);

        // Upon wakeup theirs no need for the writer
        // to acquire the critical section.  It
        // already has been transfered ownership of the
        // lock by the signaler.
    }
    else {
        assert(activeWriterReaders == 0);

        // Set that the writer owns the lock.
        activeWriterReaders = MAKELONG(0, 1);

        LeaveCriticalSection(&cs);
    }
}



void
RWMutex::ReleaseRead() {
    EnterCriticalSection(&cs);

    // assert that the lock isn't held by a writer.
    assert(HIWORD(activeWriterReaders) == 0);

    // assert that the lock is held by readers.
    assert(LOWORD(activeWriterReaders > 0));

    // Decrement the number of active readers.
    if (--activeWriterReaders == 0)
        ResetEvent(hReadyToRead);

    // if writers are waiting and this is the last reader
    // hand owneership over to a writer.
    if ((numWritersWaiting != 0) && (activeWriterReaders == 0)) {
        // Decrement the number of waiting writers
        --numWritersWaiting;

        // Pass ownership to a writer thread.
        activeWriterReaders = MAKELONG(0, 1);
        ReleaseSemaphore(hReadyToWrite, 1, NULL);
    }

    LeaveCriticalSection(&cs);
}



void
RWMutex::ReleaseWrite() {
    bool fNotifyWriter = false;
    bool fNotifyReaders = false;

    EnterCriticalSection(&cs);

    // assert that the lock is owned by a writer.
    assert(HIWORD(activeWriterReaders) == 1);

    // assert that the lock isn't owned by one or more readers
    assert(LOWORD(activeWriterReaders) == 0);

    if (numWritersWaiting != 0) {
        // Writers waiting, decrement the number of
        // waiting writers and release the semaphore
        // which means ownership is passed to the thread
        // that has been released.
        --numWritersWaiting;
        fNotifyWriter = true;
    }
    else {
        // There aren't any writers waiting
        // Release the exclusive hold on the lock.
        activeWriterReaders = 0;

        // if readers are waiting set the flag
        // that will cause the readers to be notified
        // once the critical section is released.  This
        // is done so that an awakened reader won't immediately
        // block on the critical section which is still being
        // held by this thread.
        if (numReadersWaiting != 0)
            fNotifyReaders = true;
    }

    LeaveCriticalSection(&cs);

    if (fNotifyWriter)
        ReleaseSemaphore(hReadyToWrite, 1, NULL);
    else if (fNotifyReaders)
        SetEvent(hReadyToRead);
}



RWMutexLock::~RWMutexLock() {
    if (type == READ) mutex.ReleaseRead();
    else              mutex.ReleaseWrite();
}



void RWMutexLock::UpgradeToWrite() {
    assert(type == READ);
    mutex.ReleaseRead();
    mutex.AcquireWrite();
    type = WRITE;
}



void RWMutexLock::DowngradeToRead() {
    assert(type == WRITE);
    mutex.ReleaseWrite();
    mutex.AcquireRead();
    type = READ;
}


#endif 
#if !defined(PBRT_IS_WINDOWS)
Semaphore::Semaphore() {
#ifdef PBRT_IS_OPENBSD
    sem = (sem_t *)malloc(sizeof(sem_t));
    if (!sem)
        fprintf(stderr, "Error from sem_open");
    int err = sem_init(sem, 0, 0);
    if (err == -1)
        fprintf(stderr, "Error from sem_init: %s", strerror(err));
#else
    char name[32];
    sprintf(name, "pbrt.%d-%d", (int)getpid(), count++);
    sem = sem_open(name, O_CREAT, S_IRUSR|S_IWUSR, 0);
    if (!sem)
        fprintf(stderr, "Error from sem_open: %s", strerror(errno));
#endif // !PBRT_IS_OPENBSD
}


#endif // !PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
Semaphore::Semaphore() {
    handle = CreateSemaphore(NULL, 0, 65535,  NULL);
    if (!handle)
        fprintf(stderr, "Error from CreateSemaphore: %d", GetLastError());
}


#endif // PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
int Semaphore::count = 0;
#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
Semaphore::~Semaphore() {
#ifdef PBRT_IS_OPENBSD
    int err = sem_destroy(sem);
    free((void *)sem);
    sem = NULL;
    if (err != 0)
        fprintf(stderr, "Error from sem_destroy: %s", strerror(err));
#else
    int err;
    if ((err = sem_close(sem)) != 0)
        fprintf(stderr, "Error from sem_close: %s", strerror(err));
#endif // !PBRT_IS_OPENBSD
}


#endif // !PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
Semaphore::~Semaphore() {
    CloseHandle(handle);
}


#endif // PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
void Semaphore::Wait() {
    int err;
    if ((err = sem_wait(sem)) != 0)
        fprintf(stderr, "Error from sem_wait: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
bool Semaphore::TryWait() {
    return (sem_trywait(sem) == 0);
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
void Semaphore::Post(int count) {
    int err;
    while (count-- > 0)
        if ((err = sem_post(sem)) != 0)
            fprintf(stderr, "Error from sem_post: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void Semaphore::Wait() {
    if (WaitForSingleObject(handle, INFINITE) == WAIT_FAILED)
        fprintf(stderr, "Error from WaitForSingleObject: %d", GetLastError());

}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
bool Semaphore::TryWait() {
    return (WaitForSingleObject(handle, 0L) == WAIT_OBJECT_0);
}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void Semaphore::Post(int count) {
    if (!ReleaseSemaphore(handle, count, NULL))
        fprintf(stderr, "Error from ReleaseSemaphore: %d", GetLastError());
}


#endif // PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
ConditionVariable::ConditionVariable() {
   int err;
   if ((err = pthread_cond_init(&cond, NULL)) != 0)
        fprintf(stderr, "Error from pthread_cond_init: %s", strerror(err));
   if ((err = pthread_mutex_init(&mutex, NULL)) != 0)
        fprintf(stderr, "Error from pthread_mutex_init: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
ConditionVariable::~ConditionVariable() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
void ConditionVariable::Lock() {
    int err;
    if ((err = pthread_mutex_lock(&mutex)) != 0)
        fprintf(stderr, "Error from pthread_mutex_lock: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if !(defined(PBRT_IS_WINDOWS))
void ConditionVariable::Unlock() {
    int err;
    if ((err = pthread_mutex_unlock(&mutex)) != 0)
        fprintf(stderr, "Error from pthread_mutex_unlock: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
void ConditionVariable::Wait() {
    int err;
    if ((err = pthread_cond_wait(&cond, &mutex)) != 0)
        fprintf(stderr, "Error from pthread_cond_wait: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if !defined(PBRT_IS_WINDOWS)
void ConditionVariable::Signal() {
    int err;
    if ((err = pthread_cond_signal(&cond)) != 0)
        fprintf(stderr, "Error from pthread_cond_signal: %s", strerror(err));
}


#endif // !PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)

// http://www.cs.wustl.edu/\~schmidt/win32-cv-1.html

ConditionVariable::ConditionVariable() {
    waitersCount = 0;
    InitializeCriticalSection(&waitersCountMutex);
    InitializeCriticalSection(&conditionMutex);

    events[SIGNAL] = CreateEvent (NULL,  // no security
                               FALSE, // auto-reset event
                               FALSE, // non-signaled initially
                               NULL); // unnamed
    events[BROADCAST] = CreateEvent (NULL,  // no security
                                  TRUE,  // manual-reset
                                  FALSE, // non-signaled initially
                                  NULL); // unnamed

}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
ConditionVariable::~ConditionVariable() {
    CloseHandle(events[SIGNAL]);
    CloseHandle(events[BROADCAST]);
}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void ConditionVariable::Lock() {
    EnterCriticalSection(&conditionMutex);
}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void ConditionVariable::Unlock() {
    LeaveCriticalSection(&conditionMutex);
}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void ConditionVariable::Wait() {
    // Avoid race conditions.
    EnterCriticalSection(&waitersCountMutex);
    waitersCount++;
    LeaveCriticalSection(&waitersCountMutex);

    // It's ok to release the <external_mutex> here since Win32
    // manual-reset events maintain state when used with
    // <SetEvent>.  This avoids the "lost wakeup" bug...
    LeaveCriticalSection(&conditionMutex);

    // Wait for either event to become signaled due to <pthread_cond_signal>
    // being called or <pthread_cond_broadcast> being called.
    int result = WaitForMultipleObjects(2, events, FALSE, INFINITE);

    EnterCriticalSection(&waitersCountMutex);
    waitersCount--;
    int last_waiter = (result == WAIT_OBJECT_0 + BROADCAST) &&
        (waitersCount == 0);
    LeaveCriticalSection(&waitersCountMutex);

    // Some thread called <pthread_cond_broadcast>.
    if (last_waiter)
        // We're the last waiter to be notified or to stop waiting, so
        // reset the manual event.
        ResetEvent(events[BROADCAST]);

    EnterCriticalSection(&conditionMutex);
}


#endif // PBRT_IS_WINDOWS
#if defined(PBRT_IS_WINDOWS)
void ConditionVariable::Signal() {
    EnterCriticalSection(&waitersCountMutex);
    int haveWaiters = (waitersCount > 0);
    LeaveCriticalSection(&waitersCountMutex);

    if (haveWaiters)
        SetEvent(events[SIGNAL]);
}


#endif // PBRT_IS_WINDOWS
void TasksInit() {
    static const int nThreads = NumSystemCores();
    workerSemaphore = new Semaphore;
    tasksRunningCondition = new ConditionVariable;
#if !defined(PBRT_IS_WINDOWS)
    threads = new pthread_t[nThreads];
    for (int i = 0; i < nThreads; ++i) {
        int err = pthread_create(&threads[i], NULL, &taskEntry, reinterpret_cast<void *>(i));
        if (err != 0)
            fprintf(stderr, "Error from pthread_create: %s", strerror(err));
    }
#else
    threads = new HANDLE[nThreads];
    for (int i = 0; i < nThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, taskEntry, reinterpret_cast<void *>(i), 0, NULL);
        if (threads[i] == NULL)
            fprintf(stderr, "Error from CreateThread");
    }
#endif // PBRT_IS_WINDOWS
}


void TasksCleanup() {
    if (!taskQueueMutex || !workerSemaphore)
        return;
    { MutexLock lock(*taskQueueMutex);
    assert(taskQueue.size() == 0);
    }

    static const int nThreads = NumSystemCores();
    if (workerSemaphore != NULL)
        workerSemaphore->Post(nThreads);

    if (threads != NULL) {
#if !defined(PBRT_IS_WINDOWS)
        for (int i = 0; i < nThreads; ++i) {
            int err = pthread_join(threads[i], NULL);
            if (err != 0)
                fprintf(stderr, "Error from pthread_join: %s", strerror(err));
        }
#else
        WaitForMultipleObjects(nThreads, threads, TRUE, INFINITE);
        for (int i = 0; i < nThreads; ++i) {
            CloseHandle(threads[i]);
        }
#endif // PBRT_IS_WINDOWS
        delete[] threads;
        threads = NULL;
    }
}


Task::~Task() {
}


void EnqueueTasks(const std::vector<Task *> &tasks) {
    if (!threads)
        TasksInit();

    { MutexLock lock(*taskQueueMutex);
    for (unsigned int i = 0; i < tasks.size(); ++i)
        taskQueue.push_back(tasks[i]);
    }
    tasksRunningCondition->Lock();
    numUnfinishedTasks += tasks.size();
    tasksRunningCondition->Unlock();

    workerSemaphore->Post(tasks.size());
}


#if defined(PBRT_IS_WINDOWS)
static DWORD WINAPI taskEntry(LPVOID arg) {
#else
static void *taskEntry(void *arg) {
#endif
    int threadIndex = (uint64_t)arg;
    while (true) {
        workerSemaphore->Wait();
        // Try to get task from task queue
        Task *myTask = NULL;
        { MutexLock lock(*taskQueueMutex);
        if (taskQueue.size() == 0)
            break;
        myTask = taskQueue.back();
        taskQueue.pop_back();
        }

        // Do work for _myTask_
        myTask->Run(threadIndex);
        tasksRunningCondition->Lock();
        int unfinished = --numUnfinishedTasks;
        if (unfinished == 0)
            tasksRunningCondition->Signal();
        tasksRunningCondition->Unlock();
    }
    // Cleanup from task thread and exit
#if !defined(PBRT_IS_WINDOWS)
    pthread_exit(NULL);
#endif // !PBRT_IS_WINDOWS
    return 0;
}


void WaitForAllTasks() {
    if (!tasksRunningCondition)
        return;  // no tasks have been enqueued, so TasksInit() never called
    tasksRunningCondition->Lock();
    while (numUnfinishedTasks > 0)
        tasksRunningCondition->Wait();
    tasksRunningCondition->Unlock();
}


int NumSystemCores() {
#if defined(PBRT_IS_WINDOWS)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(PBRT_IS_LINUX)
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    // mac/bsds
#ifdef PBRT_IS_OPENBSD
    int mib[2] = { CTL_HW, HW_NCPU };
#else
    int mib[2];
    mib[0] = CTL_HW;
    size_t length = 2;
    if (sysctlnametomib("hw.logicalcpu", mib, &length) == -1) {
        fprintf(stderr, "sysctlnametomib() filed.  Guessing 2 CPU cores.");
        return 2;
    }
    assert(length == 2);
#endif
    int nCores = 0;
    size_t size = sizeof(nCores);

    /* get the number of CPUs from the system */
    if (sysctl(mib, 2, &nCores, &size, NULL, 0) == -1) {
        fprintf(stderr, "sysctl() to find number of cores present failed");
        return 2;
    }
    return nCores;
#endif
}


