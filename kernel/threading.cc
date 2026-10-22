#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static ཧྲིལ init_max_threads()
{
	const var_na *v = getenv("YOSYS_MAX_THREADS");
	གལ་སྲིད (v == nullptr)
		ལོག INT32_MAX;
	ལོག atoi(v);
}

static ཧྲིལ get_max_threads()
{
	static ཧྲིལ max_threads = init_max_threads();
	ལོག max_threads;
}

static ཧྲིལ init_work_units_per_thread_override()
{
	const var_na *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	གལ་སྲིད (v == nullptr)
		ལོག 0;
	ལོག atoi(v);
}

static ཧྲིལ get_work_units_per_thread_override()
{
	static ཧྲིལ work_units_per_thread = init_work_units_per_thread_override();
	ལོག work_units_per_thread;
}

void DeferredLogs::flush()
{
	དོན་ལུ (auto &m : logs)
		གལ་སྲིད (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		དེ་མིན
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

ཧྲིལ ThreadPool::pool_size(ཧྲིལ reserved_cores, ཧྲིལ max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	ཧྲིལ available_threads = std::min<ཧྲིལ>(std::thread::hardware_concurrency(), get_max_threads());
	ཧྲིལ num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ལོག std::max(0, num_threads);
#དེ་མིན
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	ལོག 0;
#endif
}

ཧྲིལ ThreadPool::work_pool_size(ཧྲིལ reserved_cores, ཧྲིལ work_units, ཧྲིལ work_units_per_thread)
{
	ཧྲིལ work_units_per_thread_override = get_work_units_per_thread_override();
	གལ་སྲིད (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ལོག pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(ཧྲིལ pool_size, std::function<void(ཧྲིལ)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	དོན་ལུ (ཧྲིལ i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#དེ་མིན
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	དོན་ལུ (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(ཧྲིལ num_items, ཧྲིལ thread_num, ཧྲིལ num_threads)
{
	གལ་སྲིད (num_threads <= 1) {
		ལོག {0, num_items};
	}
	ཧྲིལ items_per_thread = num_items / num_threads;
	ཧྲིལ extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	ཧྲིལ start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	ཧྲིལ end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ལོག {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ཧྲིལ pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#དེ་མིན
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](ཧྲིལ thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	གལ་སྲིད (num_worker_threads_ == 0)
		ལོག;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, ཧྲིལ max_threads)
{
	Multithreading multithreading;
	ཧྲིལ num_active_worker_threads = num_threads(max_threads) - 1;
	གལ་སྲིད (num_active_worker_threads == 0) {
		work({{0}, 1});
		ལོག;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(ཧྲིལ thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	སྐབས (true)
	{
		worker_wait_for_start(thread_num);
		གལ་སྲིད (current_work == nullptr)
			bha_nga;
		ཧྲིལ num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#དེ་མིན
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
