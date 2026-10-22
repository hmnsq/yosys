#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static පූර්ණ_සංඛ්‍යා init_max_threads()
{
	const var_na *v = getenv("YOSYS_MAX_THREADS");
	නම් (v == nullptr)
		ආපසු INT32_MAX;
	ආපසු atoi(v);
}

static පූර්ණ_සංඛ්‍යා get_max_threads()
{
	static පූර්ණ_සංඛ්‍යා max_threads = init_max_threads();
	ආපසු max_threads;
}

static පූර්ණ_සංඛ්‍යා init_work_units_per_thread_override()
{
	const var_na *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	නම් (v == nullptr)
		ආපසු 0;
	ආපසු atoi(v);
}

static පූර්ණ_සංඛ්‍යා get_work_units_per_thread_override()
{
	static පූර්ණ_සංඛ්‍යා work_units_per_thread = init_work_units_per_thread_override();
	ආපසු work_units_per_thread;
}

void DeferredLogs::flush()
{
	සඳහා (auto &m : logs)
		නම් (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		නැතහොත්
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

පූර්ණ_සංඛ්‍යා ThreadPool::pool_size(පූර්ණ_සංඛ්‍යා reserved_cores, පූර්ණ_සංඛ්‍යා max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	පූර්ණ_සංඛ්‍යා available_threads = std::min<පූර්ණ_සංඛ්‍යා>(std::thread::hardware_concurrency(), get_max_threads());
	පූර්ණ_සංඛ්‍යා num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ආපසු std::max(0, num_threads);
#නැතහොත්
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	ආපසු 0;
#endif
}

පූර්ණ_සංඛ්‍යා ThreadPool::work_pool_size(පූර්ණ_සංඛ්‍යා reserved_cores, පූර්ණ_සංඛ්‍යා work_units, පූර්ණ_සංඛ්‍යා work_units_per_thread)
{
	පූර්ණ_සංඛ්‍යා work_units_per_thread_override = get_work_units_per_thread_override();
	නම් (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ආපසු pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(පූර්ණ_සංඛ්‍යා pool_size, std::function<void(පූර්ණ_සංඛ්‍යා)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#නැතහොත්
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	සඳහා (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(පූර්ණ_සංඛ්‍යා num_items, පූර්ණ_සංඛ්‍යා thread_num, පූර්ණ_සංඛ්‍යා num_threads)
{
	නම් (num_threads <= 1) {
		ආපසු {0, num_items};
	}
	පූර්ණ_සංඛ්‍යා items_per_thread = num_items / num_threads;
	පූර්ණ_සංඛ්‍යා extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	පූර්ණ_සංඛ්‍යා start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	පූර්ණ_සංඛ්‍යා end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ආපසු {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(පූර්ණ_සංඛ්‍යා pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#නැතහොත්
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](පූර්ණ_සංඛ්‍යා thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	නම් (num_worker_threads_ == 0)
		ආපසු;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, පූර්ණ_සංඛ්‍යා max_threads)
{
	Multithreading multithreading;
	පූර්ණ_සංඛ්‍යා num_active_worker_threads = num_threads(max_threads) - 1;
	නම් (num_active_worker_threads == 0) {
		work({{0}, 1});
		ආපසු;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(පූර්ණ_සංඛ්‍යා thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	අතරතුර (true)
	{
		worker_wait_for_start(thread_num);
		නම් (current_work == nullptr)
			bha_nga;
		පූර්ණ_සංඛ්‍යා num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#නැතහොත්
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
