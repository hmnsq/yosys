#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static 整数 init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	如果 (v == nullptr)
		返回 INT32_MAX;
	返回 atoi(v);
}

static 整数 get_max_threads()
{
	static 整数 max_threads = init_max_threads();
	返回 max_threads;
}

static 整数 init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	如果 (v == nullptr)
		返回 0;
	返回 atoi(v);
}

static 整数 get_work_units_per_thread_override()
{
	static 整数 work_units_per_thread = init_work_units_per_thread_override();
	返回 work_units_per_thread;
}

void DeferredLogs::flush()
{
	为 (auto &m : logs)
		如果 (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		否则
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

整数 ThreadPool::pool_size(整数 reserved_cores, 整数 max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	整数 available_threads = std::min<整数>(std::thread::hardware_concurrency(), get_max_threads());
	整数 num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	返回 std::max(0, num_threads);
#否则
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	返回 0;
#endif
}

整数 ThreadPool::work_pool_size(整数 reserved_cores, 整数 work_units, 整数 work_units_per_thread)
{
	整数 work_units_per_thread_override = get_work_units_per_thread_override();
	如果 (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	返回 pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(整数 pool_size, std::function<void(整数)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	为 (整数 i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#否则
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	为 (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(整数 num_items, 整数 thread_num, 整数 num_threads)
{
	如果 (num_threads <= 1) {
		返回 {0, num_items};
	}
	整数 items_per_thread = num_items / num_threads;
	整数 extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	整数 start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	整数 end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	返回 {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(整数 pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#否则
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](整数 thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	如果 (num_worker_threads_ == 0)
		返回;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, 整数 max_threads)
{
	Multithreading multithreading;
	整数 num_active_worker_threads = num_threads(max_threads) - 1;
	如果 (num_active_worker_threads == 0) {
		work({{0}, 1});
		返回;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(整数 thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	当 (true)
	{
		worker_wait_for_start(thread_num);
		如果 (current_work == nullptr)
			break;
		整数 num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#否则
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
