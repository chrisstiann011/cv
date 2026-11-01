#pragma once
#include "../../Precompiled.h"
#include <functional>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <windows.h>

#include <windows.h>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <string>

class CThreader;

inline std::unique_ptr<CThreader> g_Threader;

class CThreader {
public:

    using CreateSimpleThread_t = void* (__cdecl*)(
        void* pFunc,
        void* pParam,
        DWORD* pThreadID,
        int unk1,
        int unk2,
        void* unk3,
        void* unk4
        );
    using ReleaseThreadHandle_t = void( __cdecl* )(void* hThread);

    static inline CreateSimpleThread_t CreateSimpleThreadFn = nullptr;
    static inline ReleaseThreadHandle_t ReleaseThreadHandleFn = nullptr;

private:
    std::vector<void*> m_workerThreadHandles;
    std::queue<std::function<void( )>> m_taskQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_workerCondition;
    std::condition_variable m_finishedCondition;
    std::atomic<bool> m_stop{ false };
    std::atomic<size_t> m_activeJobs{ 0 };

    static void WorkerThreadLoop( void* pParam ) {
        CThreader* threader = static_cast<CThreader*>(pParam);
        if ( !threader ) return;

        while ( true ) {
            std::function<void( )> task;
            {
                std::unique_lock<std::mutex> lock( threader->m_queueMutex );

                threader->m_workerCondition.wait( lock, [&] {
                    return threader->m_stop.load( std::memory_order_relaxed ) || !threader->m_taskQueue.empty( );
                    } );

                if ( threader->m_stop.load( std::memory_order_relaxed ) && threader->m_taskQueue.empty( ) ) {
                    return;
                }

                if ( !threader->m_taskQueue.empty( ) ) {
                    task = std::move( threader->m_taskQueue.front( ) );
                    threader->m_taskQueue.pop( );

                }
                else {

                    continue;
                }
            }

            if ( task ) {
                try {
                    task( );
                }
                catch ( const std::exception& e ) {

                }
                catch ( ... ) {
                }

                if ( threader->m_activeJobs.fetch_sub( 1, std::memory_order_acq_rel ) == 1 ) {

                    std::unique_lock<std::mutex> finishLock( threader->m_queueMutex );
                    threader->m_finishedCondition.notify_all( );
                }
            }
        }
    }

public:
    explicit CThreader( size_t numThreads = 0 ) {
        if ( numThreads == 0 ) {
            numThreads = std::thread::hardware_concurrency( );
        }
        numThreads = std::max<size_t>( 1, numThreads );

        if ( !CreateSimpleThreadFn || !ReleaseThreadHandleFn ) {
            HMODULE tier0 = GetModuleHandleA( "tier0.dll" );
            if ( !tier0 ) {
                throw std::runtime_error( "CThreader: Failed to get handle for tier0.dll. Ensure it's loaded." );
            }

            CreateSimpleThreadFn = reinterpret_cast<CreateSimpleThread_t>(
                GetProcAddress( tier0, "CreateSimpleThread" )
                );
            ReleaseThreadHandleFn = reinterpret_cast<ReleaseThreadHandle_t>(
                GetProcAddress( tier0, "ReleaseThreadHandle" )
                );

            if ( !CreateSimpleThreadFn ) {
                throw std::runtime_error( "CThreader: Failed to find 'CreateSimpleThread' in tier0.dll." );
            }
            if ( !ReleaseThreadHandleFn ) {
                throw std::runtime_error( "CThreader: Failed to find 'ReleaseThreadHandle' in tier0.dll." );
            }
        }

        m_workerThreadHandles.reserve( numThreads );

        for ( size_t i = 0; i < numThreads; ++i ) {
            DWORD threadId = 0;

            void* hThread = CreateSimpleThreadFn(
                &CThreader::WorkerThreadLoop,
                this,
                &threadId,
                0,
                0,
                nullptr,
                nullptr
            );

            if ( hThread ) {
                m_workerThreadHandles.push_back( hThread );
            }
            else {

            }
        }

        if ( m_workerThreadHandles.empty( ) && numThreads > 0 ) {
            throw std::runtime_error( "CThreader: Failed to create any worker threads." );
        }
    }

    ~CThreader( ) {
        InternalShutdown( );
    }

    void Shutdown( ) {
        InternalShutdown( );
    }

private:
    void InternalShutdown( ) {

        bool expected_stop = false;
        if ( !m_stop.compare_exchange_strong( expected_stop, true, std::memory_order_acq_rel ) ) {

            if ( !m_workerThreadHandles.empty( ) ) {
                for ( void* hThread : m_workerThreadHandles ) {
                    if ( hThread ) ReleaseThreadHandleFn( hThread );
                }
                m_workerThreadHandles.clear( );
            }
            return;
        }

        m_workerCondition.notify_all( );

        WaitUntilFinished( );

        for ( void* hThread : m_workerThreadHandles ) {
            if ( hThread ) {
                ReleaseThreadHandleFn( hThread );
            }
        }
        m_workerThreadHandles.clear( );

    }

public:

    template<typename F, typename... Args>
    void EnqueueJob( F&& func, Args&&... args ) {
        if ( m_stop.load( std::memory_order_relaxed ) ) {

            return;
        }

        auto task = std::bind( std::forward<F>( func ), std::forward<Args>( args )... );

        {
            std::unique_lock<std::mutex> lock( m_queueMutex );
            m_taskQueue.emplace( [task_capture = std::move( task )]( ) { task_capture( ); } );
            m_activeJobs.fetch_add( 1, std::memory_order_relaxed );
        }

        m_workerCondition.notify_one( );
    }

    void WaitUntilFinished( ) {
        std::unique_lock<std::mutex> lock( m_queueMutex );

        m_finishedCondition.wait( lock, [&] {
            return m_activeJobs.load( std::memory_order_relaxed ) == 0;
            } );
    }

    size_t GetQueueSize( ) {
        std::unique_lock<std::mutex> lock( m_queueMutex );
        return m_taskQueue.size( );
    }

    size_t GetWorkerThreadCount( ) const {
        return m_workerThreadHandles.size( );
    }
};

inline void InitializeThreader( size_t numThreads = 0 ) {
    if ( !g_Threader ) {
        try {
            g_Threader = std::make_unique<CThreader>( numThreads );
        }
        catch ( const std::runtime_error& e ) {

            g_Threader.reset( );
        }
    }
}

inline void ShutdownThreader( ) {
    if ( g_Threader ) {
        g_Threader->Shutdown( );
        g_Threader.reset( );
    }
}