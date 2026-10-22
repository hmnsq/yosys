#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_max_threads()
{
	ଅଚଳ ଅକ୍ଷର *v = getenv("YOSYS_MAX_THREADS");
	ଯଦି (v == nullptr)
		ଫେରନ୍ତୁ INT32_MAX;
	ଫେରନ୍ତୁ atoi(v);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_max_threads()
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads = init_max_threads();
	ଫେରନ୍ତୁ max_threads;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_work_units_per_thread_override()
{
	ଅଚଳ ଅକ୍ଷର *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ଯଦି (v == nullptr)
		ଫେରନ୍ତୁ 0;
	ଫେରନ୍ତୁ atoi(v);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_work_units_per_thread_override()
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units_per_thread = init_work_units_per_thread_override();
	ଫେରନ୍ତୁ work_units_per_thread;
}

ଶୂନ୍ୟ DeferredLogs::flush()
{
	ପାଇଁ (auto &m : logs)
		ଯଦି (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		ଅନ୍ୟଥା
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ThreadPool::pool_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserved_cores, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା available_threads = std::min<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(std::thread::hardware_concurrency(), get_max_threads());
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ଫେରନ୍ତୁ std::max(0, ଅଙ୍କ_threads);
#ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)reserved_cores;
	(ଶୂନ୍ୟ)max_worker_threads;
	(ଶୂନ୍ୟ)get_max_threads();
	ଫେରନ୍ତୁ 0;
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ThreadPool::work_pool_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserved_cores, ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units, ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units_per_thread)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units_per_thread_override = get_work_units_per_thread_override();
	ଯଦି (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ଫେରନ୍ତୁ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size, std::function<ଶୂନ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pool_size; i++)
		threads.emplace_back([i, ଏହି]{ body(i); });
#ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	ପାଇଁ (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads)
{
	ଯଦି (ଅଙ୍କ_threads <= 1) {
		ଫେରନ୍ତୁ {0, ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ};
	}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବସ୍ତୁଗୁଡ଼ିକ_per_thread = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ / ଅଙ୍କ_threads;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_ବସ୍ତୁଗୁଡ଼ିକ = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ % ଅଙ୍କ_threads;
	// The first `extra_items` threads get one extra item.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start = thread_ଅଙ୍କ * ବସ୍ତୁଗୁଡ଼ିକ_per_thread + std::min(thread_ଅଙ୍କ, extra_ବସ୍ତୁଗୁଡ଼ିକ);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end = (thread_ଅଙ୍କ + 1) * ବସ୍ତୁଗୁଡ଼ିକ_per_thread + std::min(thread_ଅଙ୍କ + 1, extra_ବସ୍ତୁଗୁଡ଼ିକ);
	ଫେରନ୍ତୁ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: ଅଙ୍କ_worker_threads(std::max(1, pool_size) - 1)
#ଅନ୍ୟଥା
		: ଅଙ୍କ_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(ଶୂନ୍ୟ)pool_size;
#endif
	main_to_workers_signal.resize(ଅଙ୍କ_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(ଅଙ୍କ_worker_threads, [ଏହି](ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ){
		run_worker(thread_ଅଙ୍କ);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ଯଦି (ଅଙ୍କ_worker_threads == 0)
		ଫେରନ୍ତୁ;
	current_work = nullptr;
	ଅଙ୍କ_active_worker_threads.store(ଅଙ୍କ_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

ଶୂନ୍ୟ ParallelDispatchThreadPool::run(std::function<ଶୂନ୍ୟ(ଅଚଳ RunCtx &)> work, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads)
{
	Multithreading multithreading;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_active_worker_threads_2 = ଅଙ୍କ_threads(max_threads) - 1;
	ଯଦି (ଅଙ୍କ_active_worker_threads_2 == 0) {
		work({{0}, 1});
		ଫେରନ୍ତୁ;
	}
	ଅଙ୍କ_active_worker_threads.store(ଅଙ୍କ_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, ଅଙ୍କ_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

ଶୂନ୍ୟ ParallelDispatchThreadPool::run_worker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ)
{
#ifdef YOSYS_ENABLE_THREADS
	ଯେତେବେଳେ (ସତ୍ୟ)
	{
		worker_wait_for_start(thread_ଅଙ୍କ);
		ଯଦି (current_work == nullptr)
			ଭାଙ୍ଗ;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_active_worker_threads_2 = ଅଙ୍କ_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_ଅଙ୍କ + 1}, ଅଙ୍କ_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)thread_ଅଙ୍କ;
	(ଶୂନ୍ୟ)current_work;
#endif
}

YOSYS_NAMESPACE_END
