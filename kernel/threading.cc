#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static số_nguyên init_max_threads()
{
	const char *v = getenv("YOSYS_MAX_THREADS");
	nếu (v == nullptr)
		trả_về INT32_MAX;
	trả_về atoi(v);
}

static số_nguyên get_max_threads()
{
	static số_nguyên max_threads = init_max_threads();
	trả_về max_threads;
}

static số_nguyên init_work_units_per_thread_override()
{
	const char *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	nếu (v == nullptr)
		trả_về 0;
	trả_về atoi(v);
}

static số_nguyên get_work_units_per_thread_override()
{
	static số_nguyên work_units_per_thread = init_work_units_per_thread_override();
	trả_về work_units_per_thread;
}

void DeferredLogs::flush()
{
	cho (auto &m : logs)
		nếu (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		không_thì
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

số_nguyên ThreadPool::pool_size(số_nguyên reserved_cores, số_nguyên max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	số_nguyên available_threads = std::min<số_nguyên>(std::thread::hardware_concurrency(), get_max_threads());
	số_nguyên num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	trả_về std::max(0, num_threads);
#không_thì
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	trả_về 0;
#endif
}

số_nguyên ThreadPool::work_pool_size(số_nguyên reserved_cores, số_nguyên work_units, số_nguyên work_units_per_thread)
{
	số_nguyên work_units_per_thread_override = get_work_units_per_thread_override();
	nếu (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	trả_về pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(số_nguyên pool_size, std::function<void(số_nguyên)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	cho (số_nguyên i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#không_thì
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	cho (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(số_nguyên num_items, số_nguyên thread_num, số_nguyên num_threads)
{
	nếu (num_threads <= 1) {
		trả_về {0, num_items};
	}
	số_nguyên items_per_thread = num_items / num_threads;
	số_nguyên extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	số_nguyên start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	số_nguyên end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	trả_về {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(số_nguyên pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#không_thì
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](số_nguyên thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	nếu (num_worker_threads_ == 0)
		trả_về;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, số_nguyên max_threads)
{
	Multithreading multithreading;
	số_nguyên num_active_worker_threads = num_threads(max_threads) - 1;
	nếu (num_active_worker_threads == 0) {
		work({{0}, 1});
		trả_về;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(số_nguyên thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	trong_khi (true)
	{
		worker_wait_for_start(thread_num);
		nếu (current_work == nullptr)
			break;
		số_nguyên num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#không_thì
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
