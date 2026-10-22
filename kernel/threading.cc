#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

स्थिर पूर्णांक init_max_threads()
{
	स्थिरांक अक्षर *v = दो_पर्या("YOSYS_MAX_THREADS");
	जर (v == nullptr)
		परत INT32_MAX;
	परत म_से_प(v);
}

स्थिर पूर्णांक get_max_threads()
{
	स्थिर पूर्णांक max_threads = init_max_threads();
	परत max_threads;
}

स्थिर पूर्णांक init_work_units_per_thread_override()
{
	स्थिरांक अक्षर *v = दो_पर्या("YOSYS_WORK_UNITS_PER_THREAD");
	जर (v == nullptr)
		परत 0;
	परत म_से_प(v);
}

स्थिर पूर्णांक get_work_units_per_thread_override()
{
	स्थिर पूर्णांक work_units_per_thread = init_work_units_per_thread_override();
	परत work_units_per_thread;
}

शून्य DeferredLogs::flush()
{
	साठी (स्वतः &m : logs)
		जर (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		अन्यथा
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

पूर्णांक ThreadPool::pool_size(पूर्णांक reserved_cores, पूर्णांक max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	पूर्णांक available_threads = std::min<पूर्णांक>(std::thread::hardware_concurrency(), get_max_threads());
	पूर्णांक अंक_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	परत std::max(0, अंक_threads);
#अन्यथा
	(शून्य)reserved_cores;
	(शून्य)max_worker_threads;
	(शून्य)get_max_threads();
	परत 0;
#endif
}

पूर्णांक ThreadPool::work_pool_size(पूर्णांक reserved_cores, पूर्णांक work_units, पूर्णांक work_units_per_thread)
{
	पूर्णांक work_units_per_thread_override = get_work_units_per_thread_override();
	जर (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	परत pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(पूर्णांक pool_size, std::function<शून्य(पूर्णांक)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	साठी (पूर्णांक i = 0; i < pool_size; i++)
		threads.emplace_back([i, हे]{ body(i); });
#अन्यथा
	(शून्य)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	साठी (स्वतः &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(पूर्णांक अंक_वस्तुसंच, पूर्णांक thread_अंक, पूर्णांक अंक_threads)
{
	जर (अंक_threads <= 1) {
		परत {0, अंक_वस्तुसंच};
	}
	पूर्णांक वस्तुसंच_per_thread = अंक_वस्तुसंच / अंक_threads;
	पूर्णांक extra_वस्तुसंच = अंक_वस्तुसंच % अंक_threads;
	// The first `extra_items` threads get one extra item.
	पूर्णांक start = thread_अंक * वस्तुसंच_per_thread + std::min(thread_अंक, extra_वस्तुसंच);
	पूर्णांक end = (thread_अंक + 1) * वस्तुसंच_per_thread + std::min(thread_अंक + 1, extra_वस्तुसंच);
	परत {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(पूर्णांक pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: अंक_worker_threads(std::max(1, pool_size) - 1)
#अन्यथा
		: अंक_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(शून्य)pool_size;
#endif
	main_to_workers_signal.resize(अंक_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(अंक_worker_threads, [हे](पूर्णांक thread_अंक){
		run_worker(thread_अंक);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	जर (अंक_worker_threads == 0)
		परत;
	current_work = nullptr;
	अंक_active_worker_threads.store(अंक_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run(std::function<शून्य(स्थिरांक RunCtx &)> work, पूर्णांक max_threads)
{
	Multithreading multithreading;
	पूर्णांक अंक_active_worker_threads_2 = अंक_threads(max_threads) - 1;
	जर (अंक_active_worker_threads_2 == 0) {
		work({{0}, 1});
		परत;
	}
	अंक_active_worker_threads.store(अंक_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, अंक_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run_worker(पूर्णांक thread_अंक)
{
#ifdef YOSYS_ENABLE_THREADS
	पर्यंत (सत्य)
	{
		worker_wait_for_start(thread_अंक);
		जर (current_work == nullptr)
			थांबा;
		पूर्णांक अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_अंक + 1}, अंक_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#अन्यथा
	(शून्य)thread_अंक;
	(शून्य)current_work;
#endif
}

YOSYS_NAMESPACE_END
