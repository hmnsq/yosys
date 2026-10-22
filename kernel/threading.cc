#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

स्थिर पूर्णाङ्क init_max_threads()
{
	ध्रुव वर्ण *v = getenv("YOSYS_MAX_THREADS");
	सचे (v == nullptr)
		पटिनिवत्त INT32_MAX;
	पटिनिवत्त atoi(v);
}

स्थिर पूर्णाङ्क get_max_threads()
{
	स्थिर पूर्णाङ्क max_threads = init_max_threads();
	पटिनिवत्त max_threads;
}

स्थिर पूर्णाङ्क init_work_units_per_thread_override()
{
	ध्रुव वर्ण *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	सचे (v == nullptr)
		पटिनिवत्त 0;
	पटिनिवत्त atoi(v);
}

स्थिर पूर्णाङ्क get_work_units_per_thread_override()
{
	स्थिर पूर्णाङ्क work_units_per_thread = init_work_units_per_thread_override();
	पटिनिवत्त work_units_per_thread;
}

शून्य DeferredLogs::flush()
{
	अत्थाय (auto &m : logs)
		सचे (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		अञ्ञथा
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

पूर्णाङ्क ThreadPool::pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	पूर्णाङ्क available_threads = std::min<पूर्णाङ्क>(std::thread::hardware_concurrency(), get_max_threads());
	पूर्णाङ्क अङ्कः_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	पटिनिवत्त std::max(0, अङ्कः_threads);
#अञ्ञथा
	(शून्य)reserved_cores;
	(शून्य)max_worker_threads;
	(शून्य)get_max_threads();
	पटिनिवत्त 0;
#endif
}

पूर्णाङ्क ThreadPool::work_pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क work_units, पूर्णाङ्क work_units_per_thread)
{
	पूर्णाङ्क work_units_per_thread_override = get_work_units_per_thread_override();
	सचे (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	पटिनिवत्त pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(पूर्णाङ्क pool_size, std::function<शून्य(पूर्णाङ्क)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	अत्थाय (पूर्णाङ्क i = 0; i < pool_size; i++)
		threads.emplace_back([i, एतत्]{ body(i); });
#अञ्ञथा
	(शून्य)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	अत्थाय (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(पूर्णाङ्क अङ्कः_वस्तूनि, पूर्णाङ्क thread_अङ्कः, पूर्णाङ्क अङ्कः_threads)
{
	सचे (अङ्कः_threads <= 1) {
		पटिनिवत्त {0, अङ्कः_वस्तूनि};
	}
	पूर्णाङ्क वस्तूनि_per_thread = अङ्कः_वस्तूनि / अङ्कः_threads;
	पूर्णाङ्क extra_वस्तूनि = अङ्कः_वस्तूनि % अङ्कः_threads;
	// The first `extra_items` threads get one extra item.
	पूर्णाङ्क start = thread_अङ्कः * वस्तूनि_per_thread + std::min(thread_अङ्कः, extra_वस्तूनि);
	पूर्णाङ्क end = (thread_अङ्कः + 1) * वस्तूनि_per_thread + std::min(thread_अङ्कः + 1, extra_वस्तूनि);
	पटिनिवत्त {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(पूर्णाङ्क pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: अङ्कः_worker_threads(std::max(1, pool_size) - 1)
#अञ्ञथा
		: अङ्कः_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(शून्य)pool_size;
#endif
	main_to_workers_signal.resize(अङ्कः_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(अङ्कः_worker_threads, [एतत्](पूर्णाङ्क thread_अङ्कः){
		run_worker(thread_अङ्कः);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	सचे (अङ्कः_worker_threads == 0)
		पटिनिवत्त;
	current_work = nullptr;
	अङ्कः_active_worker_threads.store(अङ्कः_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run(std::function<शून्य(ध्रुव RunCtx &)> work, पूर्णाङ्क max_threads)
{
	Multithreading multithreading;
	पूर्णाङ्क अङ्कः_active_worker_threads_2 = अङ्कः_threads(max_threads) - 1;
	सचे (अङ्कः_active_worker_threads_2 == 0) {
		work({{0}, 1});
		पटिनिवत्त;
	}
	अङ्कः_active_worker_threads.store(अङ्कः_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, अङ्कः_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run_worker(पूर्णाङ्क thread_अङ्कः)
{
#ifdef YOSYS_ENABLE_THREADS
	याव (सच्च)
	{
		worker_wait_for_start(thread_अङ्कः);
		सचे (current_work == nullptr)
			भङ्ग;
		पूर्णाङ्क अङ्कः_active_worker_threads_2 = अङ्कः_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_अङ्कः + 1}, अङ्कः_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#अञ्ञथा
	(शून्य)thread_अङ्कः;
	(शून्य)current_work;
#endif
}

YOSYS_NAMESPACE_END
