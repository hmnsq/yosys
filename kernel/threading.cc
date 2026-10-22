#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static цяло init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	ако (v == nullptr)
		върни INT32_MAX;
	върни atoi(v);
}

static цяло get_max_threads()
{
	static цяло max_threads = init_max_threads();
	върни max_threads;
}

static цяло init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ако (v == nullptr)
		върни 0;
	върни atoi(v);
}

static цяло get_work_units_per_thread_override()
{
	static цяло work_units_per_thread = init_work_units_per_thread_override();
	върни work_units_per_thread;
}

void DeferredLogs::flush()
{
	за (auto &m : logs)
		ако (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		иначе
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

цяло ThreadPool::pool_size(цяло reserved_cores, цяло max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	цяло available_threads = std::min<цяло>(std::thread::hardware_concurrency(), get_max_threads());
	цяло num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	върни std::max(0, num_threads);
#иначе
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	върни 0;
#endif
}

цяло ThreadPool::work_pool_size(цяло reserved_cores, цяло work_units, цяло work_units_per_thread)
{
	цяло work_units_per_thread_override = get_work_units_per_thread_override();
	ако (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	върни pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(цяло pool_size, std::function<void(цяло)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	за (цяло i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#иначе
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

IntRange item_range_for_worker(цяло num_items, цяло thread_num, цяло num_threads)
{
	ако (num_threads <= 1) {
		върни {0, num_items};
	}
	цяло items_per_thread = num_items / num_threads;
	цяло extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	цяло start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	цяло end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	върни {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(цяло pool_size)
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
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](цяло thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ако (num_worker_threads_ == 0)
		върни;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, цяло max_threads)
{
	Multithreading multithreading;
	цяло num_active_worker_threads = num_threads(max_threads) - 1;
	ако (num_active_worker_threads == 0) {
		work({{0}, 1});
		върни;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(цяло thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	докато (true)
	{
		worker_wait_for_start(thread_num);
		ако (current_work == nullptr)
			break;
		цяло num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
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
