#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static int init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	if (v == nullptr)
		return INT32_MAX;
	return atoi(v);
}

static int get_max_threads()
{
	static int max_threads = init_max_threads();
	return max_threads;
}

static int init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	if (v == nullptr)
		return 0;
	return atoi(v);
}

static int get_work_units_per_thread_override()
{
	static int work_units_per_thread = init_work_units_per_thread_override();
	return work_units_per_thread;
}

void DeferredLogs::flush()
{
	for (auto &m : logs)
		if (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		else
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

int ThreadPool::pool_size(int reserved_cores, int max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	int available_threads = std::min<int>(std::thread::hardware_concurrency(), get_max_threads());
	int नुमेरो_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	return std::max(0, नुमेरो_threads);
#else
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	return 0;
#endif
}

int ThreadPool::work_pool_size(int reserved_cores, int work_units, int work_units_per_thread)
{
	int work_units_per_thread_override = get_work_units_per_thread_override();
	if (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	return pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(int pool_size, std::function<void(int)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	for (int i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#else
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	for (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(int नुमेरो_मगा_बागाय, int thread_नुमेरो, int नुमेरो_threads)
{
	if (नुमेरो_threads <= 1) {
		return {0, नुमेरो_मगा_बागाय};
	}
	int मगा_बागाय_per_thread = नुमेरो_मगा_बागाय / नुमेरो_threads;
	int extra_मगा_बागाय = नुमेरो_मगा_बागाय % नुमेरो_threads;
	// The first `extra_items` threads get one extra item.
	int start = thread_नुमेरो * मगा_बागाय_per_thread + std::min(thread_नुमेरो, extra_मगा_बागाय);
	int end = (thread_नुमेरो + 1) * मगा_बागाय_per_thread + std::min(thread_नुमेरो + 1, extra_मगा_बागाय);
	return {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(int pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: नुमेरो_worker_threads(std::max(1, pool_size) - 1)
#else
		: नुमेरो_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(नुमेरो_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(नुमेरो_worker_threads, [this](int thread_नुमेरो){
		run_worker(thread_नुमेरो);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	if (नुमेरो_worker_threads == 0)
		return;
	current_work = nullptr;
	नुमेरो_active_worker_threads.store(नुमेरो_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, int max_threads)
{
	Multithreading multithreading;
	int नुमेरो_active_worker_threads_2 = नुमेरो_threads(max_threads) - 1;
	if (नुमेरो_active_worker_threads_2 == 0) {
		work({{0}, 1});
		return;
	}
	नुमेरो_active_worker_threads.store(नुमेरो_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, नुमेरो_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(int thread_नुमेरो)
{
#ifdef YOSYS_ENABLE_THREADS
	while (true)
	{
		worker_wait_for_start(thread_नुमेरो);
		if (current_work == nullptr)
			break;
		int नुमेरो_active_worker_threads_2 = नुमेरो_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_नुमेरो + 1}, नुमेरो_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#else
	(void)thread_नुमेरो;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
