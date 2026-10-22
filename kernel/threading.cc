#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static puur_naa_nka init_max_threads()
{
	const var_na *v = getenv("YOSYS_MAX_THREADS");
	ᮚᮦᮔ᮪ (v == nullptr)
		ᮝᮜᮤ INT32_MAX;
	ᮝᮜᮤ atoi(v);
}

static puur_naa_nka get_max_threads()
{
	static puur_naa_nka max_threads = init_max_threads();
	ᮝᮜᮤ max_threads;
}

static puur_naa_nka init_work_units_per_thread_override()
{
	const var_na *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ᮚᮦᮔ᮪ (v == nullptr)
		ᮝᮜᮤ 0;
	ᮝᮜᮤ atoi(v);
}

static puur_naa_nka get_work_units_per_thread_override()
{
	static puur_naa_nka work_units_per_thread = init_work_units_per_thread_override();
	ᮝᮜᮤ work_units_per_thread;
}

void DeferredLogs::flush()
{
	for (auto &m : logs)
		ᮚᮦᮔ᮪ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		ᮜᮤᮚ
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

puur_naa_nka ThreadPool::pool_size(puur_naa_nka reserved_cores, puur_naa_nka max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	puur_naa_nka available_threads = std::min<puur_naa_nka>(std::thread::hardware_concurrency(), get_max_threads());
	puur_naa_nka num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ᮝᮜᮤ std::max(0, num_threads);
#ᮜᮤᮚ
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	ᮝᮜᮤ 0;
#endif
}

puur_naa_nka ThreadPool::work_pool_size(puur_naa_nka reserved_cores, puur_naa_nka work_units, puur_naa_nka work_units_per_thread)
{
	puur_naa_nka work_units_per_thread_override = get_work_units_per_thread_override();
	ᮚᮦᮔ᮪ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ᮝᮜᮤ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(puur_naa_nka pool_size, std::function<void(puur_naa_nka)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	for (puur_naa_nka i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#ᮜᮤᮚ
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

IntRange item_range_for_worker(puur_naa_nka num_items, puur_naa_nka thread_num, puur_naa_nka num_threads)
{
	ᮚᮦᮔ᮪ (num_threads <= 1) {
		ᮝᮜᮤ {0, num_items};
	}
	puur_naa_nka items_per_thread = num_items / num_threads;
	puur_naa_nka extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	puur_naa_nka start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	puur_naa_nka end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ᮝᮜᮤ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(puur_naa_nka pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#ᮜᮤᮚ
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](puur_naa_nka thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ᮚᮦᮔ᮪ (num_worker_threads_ == 0)
		ᮝᮜᮤ;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, puur_naa_nka max_threads)
{
	Multithreading multithreading;
	puur_naa_nka num_active_worker_threads = num_threads(max_threads) - 1;
	ᮚᮦᮔ᮪ (num_active_worker_threads == 0) {
		work({{0}, 1});
		ᮝᮜᮤ;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(puur_naa_nka thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	ᮞᮨᮓᮨᮜ (true)
	{
		worker_wait_for_start(thread_num);
		ᮚᮦᮔ᮪ (current_work == nullptr)
			bha_nga;
		puur_naa_nka num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#ᮜᮤᮚ
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
