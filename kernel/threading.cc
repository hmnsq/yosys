#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static целое init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	если (v == nullptr)
		вернуть INT32_MAX;
	вернуть atoi(v);
}

static целое get_max_threads()
{
	static целое max_threads = init_max_threads();
	вернуть max_threads;
}

static целое init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	если (v == nullptr)
		вернуть 0;
	вернуть atoi(v);
}

static целое get_work_units_per_thread_override()
{
	static целое work_units_per_thread = init_work_units_per_thread_override();
	вернуть work_units_per_thread;
}

void DeferredLogs::flush()
{
	для (auto &m : logs)
		если (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		иначе
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

целое ThreadPool::pool_size(целое reserved_cores, целое max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	целое available_threads = std::min<целое>(std::thread::hardware_concurrency(), get_max_threads());
	целое num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	вернуть std::max(0, num_threads);
#иначе
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	вернуть 0;
#endif
}

целое ThreadPool::work_pool_size(целое reserved_cores, целое work_units, целое work_units_per_thread)
{
	целое work_units_per_thread_override = get_work_units_per_thread_override();
	если (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	вернуть pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(целое pool_size, std::function<void(целое)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	для (целое i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#иначе
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	для (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(целое num_items, целое thread_num, целое num_threads)
{
	если (num_threads <= 1) {
		вернуть {0, num_items};
	}
	целое items_per_thread = num_items / num_threads;
	целое extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	целое start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	целое end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	вернуть {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(целое pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#иначе
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](целое thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	если (num_worker_threads_ == 0)
		вернуть;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, целое max_threads)
{
	Multithreading multithreading;
	целое num_active_worker_threads = num_threads(max_threads) - 1;
	если (num_active_worker_threads == 0) {
		work({{0}, 1});
		вернуть;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(целое thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	пока (true)
	{
		worker_wait_for_start(thread_num);
		если (current_work == nullptr)
			break;
		целое num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#иначе
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
