#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

स्थिर पूर्णाङ्क init_max_threads()
{
	ध्रुव वर्ण *v = दो_पर्या("YOSYS_MAX_THREADS");
	यदि (v == nullptr)
		प्रत्यागम INT32_MAX;
	प्रत्यागम म_से_प(v);
}

स्थिर पूर्णाङ्क get_max_threads()
{
	स्थिर पूर्णाङ्क max_threads = init_max_threads();
	प्रत्यागम max_threads;
}

स्थिर पूर्णाङ्क init_work_units_per_thread_override()
{
	ध्रुव वर्ण *v = दो_पर्या("YOSYS_WORK_UNITS_PER_THREAD");
	यदि (v == nullptr)
		प्रत्यागम 0;
	प्रत्यागम म_से_प(v);
}

स्थिर पूर्णाङ्क get_work_units_per_thread_override()
{
	स्थिर पूर्णाङ्क work_units_per_thread = init_work_units_per_thread_override();
	प्रत्यागम work_units_per_thread;
}

शून्य DeferredLogs::flush()
{
	प्रति (स्वतः &m : logs)
		यदि (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		अन्यथा
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

पूर्णाङ्क ThreadPool::pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	पूर्णाङ्क available_threads = std::min<पूर्णाङ्क>(std::thread::hardware_concurrency(), get_max_threads());
	पूर्णाङ्क अंक_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	प्रत्यागम std::max(0, अंक_threads);
#अन्यथा
	(शून्य)reserved_cores;
	(शून्य)max_worker_threads;
	(शून्य)get_max_threads();
	प्रत्यागम 0;
#endif
}

पूर्णाङ्क ThreadPool::work_pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क work_units, पूर्णाङ्क work_units_per_thread)
{
	पूर्णाङ्क work_units_per_thread_override = get_work_units_per_thread_override();
	यदि (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	प्रत्यागम pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(पूर्णाङ्क pool_size, std::function<शून्य(पूर्णाङ्क)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	प्रति (पूर्णाङ्क i = 0; i < pool_size; i++)
		threads.emplace_back([i, एतत्]{ body(i); });
#अन्यथा
	(शून्य)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	प्रति (स्वतः &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(पूर्णाङ्क अंक_वस्तुएँ, पूर्णाङ्क thread_अंक, पूर्णाङ्क अंक_threads)
{
	यदि (अंक_threads <= 1) {
		प्रत्यागम {0, अंक_वस्तुएँ};
	}
	पूर्णाङ्क वस्तुएँ_per_thread = अंक_वस्तुएँ / अंक_threads;
	पूर्णाङ्क extra_वस्तुएँ = अंक_वस्तुएँ % अंक_threads;
	// The first `extra_items` threads get one extra item.
	पूर्णाङ्क start = thread_अंक * वस्तुएँ_per_thread + std::min(thread_अंक, extra_वस्तुएँ);
	पूर्णाङ्क end = (thread_अंक + 1) * वस्तुएँ_per_thread + std::min(thread_अंक + 1, extra_वस्तुएँ);
	प्रत्यागम {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(पूर्णाङ्क pool_size)
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
	thread_pool = std::make_unique<ThreadPool>(अंक_worker_threads, [एतत्](पूर्णाङ्क thread_अंक){
		run_worker(thread_अंक);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	यदि (अंक_worker_threads == 0)
		प्रत्यागम;
	current_work = nullptr;
	अंक_active_worker_threads.store(अंक_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run(std::function<शून्य(ध्रुव RunCtx &)> work, पूर्णाङ्क max_threads)
{
	Multithreading multithreading;
	पूर्णाङ्क अंक_active_worker_threads_2 = अंक_threads(max_threads) - 1;
	यदि (अंक_active_worker_threads_2 == 0) {
		work({{0}, 1});
		प्रत्यागम;
	}
	अंक_active_worker_threads.store(अंक_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, अंक_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

शून्य ParallelDispatchThreadPool::run_worker(पूर्णाङ्क thread_अंक)
{
#ifdef YOSYS_ENABLE_THREADS
	यावत् (सत्य)
	{
		worker_wait_for_start(thread_अंक);
		यदि (current_work == nullptr)
			भङ्ग;
		पूर्णाङ्क अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
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
