#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static 정수 init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	만약 (v == nullptr)
		반환 INT32_MAX;
	반환 atoi(v);
}

static 정수 get_max_threads()
{
	static 정수 max_threads = init_max_threads();
	반환 max_threads;
}

static 정수 init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	만약 (v == nullptr)
		반환 0;
	반환 atoi(v);
}

static 정수 get_work_units_per_thread_override()
{
	static 정수 work_units_per_thread = init_work_units_per_thread_override();
	반환 work_units_per_thread;
}

void DeferredLogs::flush()
{
	위해 (auto &m : logs)
		만약 (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		아니면
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

정수 ThreadPool::pool_size(정수 reserved_cores, 정수 max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	정수 available_threads = std::min<정수>(std::thread::hardware_concurrency(), get_max_threads());
	정수 num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	반환 std::max(0, num_threads);
#아니면
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	반환 0;
#endif
}

정수 ThreadPool::work_pool_size(정수 reserved_cores, 정수 work_units, 정수 work_units_per_thread)
{
	정수 work_units_per_thread_override = get_work_units_per_thread_override();
	만약 (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	반환 pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(정수 pool_size, std::function<void(정수)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	위해 (정수 i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#아니면
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	위해 (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(정수 num_items, 정수 thread_num, 정수 num_threads)
{
	만약 (num_threads <= 1) {
		반환 {0, num_items};
	}
	정수 items_per_thread = num_items / num_threads;
	정수 extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	정수 start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	정수 end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	반환 {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(정수 pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#아니면
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](정수 thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	만약 (num_worker_threads_ == 0)
		반환;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, 정수 max_threads)
{
	Multithreading multithreading;
	정수 num_active_worker_threads = num_threads(max_threads) - 1;
	만약 (num_active_worker_threads == 0) {
		work({{0}, 1});
		반환;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(정수 thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	동안 (true)
	{
		worker_wait_for_start(thread_num);
		만약 (current_work == nullptr)
			break;
		정수 num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#아니면
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
