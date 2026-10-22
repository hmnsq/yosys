#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static цео init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	ако (v == nullptr)
		врати INT32_MAX;
	врати atoi(v);
}

static цео get_max_threads()
{
	static цео max_threads = init_max_threads();
	врати max_threads;
}

static цео init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ако (v == nullptr)
		врати 0;
	врати atoi(v);
}

static цео get_work_units_per_thread_override()
{
	static цео work_units_per_thread = init_work_units_per_thread_override();
	врати work_units_per_thread;
}

void DeferredLogs::flush()
{
	за (auto &m : logs)
		ако (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		инако
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

цео ThreadPool::pool_size(цео reserved_cores, цео max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	цео available_threads = std::min<цео>(std::thread::hardware_concurrency(), get_max_threads());
	цео num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	врати std::max(0, num_threads);
#инако
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	врати 0;
#endif
}

цео ThreadPool::work_pool_size(цео reserved_cores, цео work_units, цео work_units_per_thread)
{
	цео work_units_per_thread_override = get_work_units_per_thread_override();
	ако (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	врати pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(цео pool_size, std::function<void(цео)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	за (цео i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#инако
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	за (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(цео num_items, цео thread_num, цео num_threads)
{
	ако (num_threads <= 1) {
		врати {0, num_items};
	}
	цео items_per_thread = num_items / num_threads;
	цео extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	цео start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	цео end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	врати {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(цео pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#инако
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](цео thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ако (num_worker_threads_ == 0)
		врати;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, цео max_threads)
{
	Multithreading multithreading;
	цео num_active_worker_threads = num_threads(max_threads) - 1;
	ако (num_active_worker_threads == 0) {
		work({{0}, 1});
		врати;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(цео thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	док (true)
	{
		worker_wait_for_start(thread_num);
		ако (current_work == nullptr)
			break;
		цео num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#инако
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
