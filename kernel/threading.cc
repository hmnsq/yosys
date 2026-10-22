#समावेश "kernel/yosys_common.h"
#समावेश "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

अटल पूर्णांक init_max_threads()
{
	स्थिर अक्षर *v = दो_पर्या("YOSYS_MAX_THREADS");
	अगर (v == nullptr)
		वापस INT32_MAX;
	वापस म_से_प(v);
}

अटल पूर्णांक get_max_threads()
{
	अटल पूर्णांक max_threads = init_max_threads();
	वापस max_threads;
}

अटल पूर्णांक init_work_units_per_thread_override()
{
	स्थिर अक्षर *v = दो_पर्या("YOSYS_WORK_UNITS_PER_THREAD");
	अगर (v == nullptr)
		वापस 0;
	वापस म_से_प(v);
}

अटल पूर्णांक get_work_units_per_thread_override()
{
	अटल पूर्णांक work_units_per_thread = init_work_units_per_thread_override();
	वापस work_units_per_thread;
}

व्योम DeferredLogs::सफाई()
{
	क्रम (स्वतः &m : logs)
		अगर (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		अन्यथा
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

पूर्णांक ThreadPool::pool_आकार(पूर्णांक reserved_cores, पूर्णांक max_worker_threads)
{
#अगर_घोषित YOSYS_ENABLE_THREADS
	पूर्णांक available_threads = std::min<पूर्णांक>(std::thread::hardware_concurrency(), get_max_threads());
	पूर्णांक अंक_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	वापस std::max(0, अंक_threads);
#अन्यथा
	(व्योम)reserved_cores;
	(व्योम)max_worker_threads;
	(व्योम)get_max_threads();
	वापस 0;
#पूर्ण_अगर
}

पूर्णांक ThreadPool::work_pool_आकार(पूर्णांक reserved_cores, पूर्णांक work_units, पूर्णांक work_units_per_thread)
{
	पूर्णांक work_units_per_thread_override = get_work_units_per_thread_override();
	अगर (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	वापस pool_आकार(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(पूर्णांक pool_आकार, std::function<व्योम(पूर्णांक)> b)
	: body(std::move(b))
{
#अगर_घोषित YOSYS_ENABLE_THREADS
	threads.reserve(pool_आकार);
	क्रम (पूर्णांक i = 0; i < pool_आकार; i++)
		threads.emplace_back([i, यह]{ body(i); });
#अन्यथा
	(व्योम)pool_आकार;
#पूर्ण_अगर
}

ThreadPool::~ThreadPool()
{
#अगर_घोषित YOSYS_ENABLE_THREADS
	क्रम (स्वतः &t : threads)
		t.join();
#पूर्ण_अगर
}

IntRange item_range_for_worker(पूर्णांक अंक_वस्तुएँ, पूर्णांक thread_अंक, पूर्णांक अंक_threads)
{
	अगर (अंक_threads <= 1) {
		वापस {0, अंक_वस्तुएँ};
	}
	पूर्णांक वस्तुएँ_per_thread = अंक_वस्तुएँ / अंक_threads;
	पूर्णांक extra_वस्तुएँ = अंक_वस्तुएँ % अंक_threads;
	// The first `extra_items` threads get one extra item.
	पूर्णांक start = thread_अंक * वस्तुएँ_per_thread + std::min(thread_अंक, extra_वस्तुएँ);
	पूर्णांक end = (thread_अंक + 1) * वस्तुएँ_per_thread + std::min(thread_अंक + 1, extra_वस्तुएँ);
	वापस {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(पूर्णांक pool_आकार)
#अगर_घोषित YOSYS_ENABLE_THREADS
		: अंक_worker_threads(std::max(1, pool_आकार) - 1)
#अन्यथा
		: अंक_worker_threads(0)
#पूर्ण_अगर
{
#अगर_अघोषित YOSYS_ENABLE_THREADS
	(व्योम)pool_आकार;
#पूर्ण_अगर
	main_to_workers_signal.resize(अंक_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(अंक_worker_threads, [यह](पूर्णांक thread_अंक){
		run_worker(thread_अंक);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	अगर (अंक_worker_threads == 0)
		वापस;
	current_work = nullptr;
	अंक_active_worker_threads.store(अंक_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

व्योम ParallelDispatchThreadPool::run(std::function<व्योम(स्थिर RunCtx &)> work, पूर्णांक max_threads)
{
	Multithreading multithreading;
	पूर्णांक अंक_active_worker_threads_2 = अंक_threads(max_threads) - 1;
	अगर (अंक_active_worker_threads_2 == 0) {
		work({{0}, 1});
		वापस;
	}
	अंक_active_worker_threads.store(अंक_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, अंक_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

व्योम ParallelDispatchThreadPool::run_worker(पूर्णांक thread_अंक)
{
#अगर_घोषित YOSYS_ENABLE_THREADS
	जबतक (true)
	{
		worker_wait_for_start(thread_अंक);
		अगर (current_work == nullptr)
			अवरोध;
		पूर्णांक अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_अंक + 1}, अंक_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#अन्यथा
	(व्योम)thread_अंक;
	(व्योम)current_work;
#पूर्ण_अगर
}

YOSYS_NAMESPACE_END
