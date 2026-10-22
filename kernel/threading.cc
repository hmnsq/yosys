#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static ຈຳນວນເຕັມ init_max_threads()
{
	const var_na *v = getenv("YOSYS_MAX_THREADS");
	ຖ້າ (v == nullptr)
		ສົ່ງຄືນ INT32_MAX;
	ສົ່ງຄືນ atoi(v);
}

static ຈຳນວນເຕັມ get_max_threads()
{
	static ຈຳນວນເຕັມ max_threads = init_max_threads();
	ສົ່ງຄືນ max_threads;
}

static ຈຳນວນເຕັມ init_work_units_per_thread_override()
{
	const var_na *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ຖ້າ (v == nullptr)
		ສົ່ງຄືນ 0;
	ສົ່ງຄືນ atoi(v);
}

static ຈຳນວນເຕັມ get_work_units_per_thread_override()
{
	static ຈຳນວນເຕັມ work_units_per_thread = init_work_units_per_thread_override();
	ສົ່ງຄືນ work_units_per_thread;
}

void DeferredLogs::flush()
{
	ສຳລັບ (auto &m : logs)
		ຖ້າ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		ບໍ່ດັ່ງນັ້ນ
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

ຈຳນວນເຕັມ ThreadPool::pool_size(ຈຳນວນເຕັມ reserved_cores, ຈຳນວນເຕັມ max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	ຈຳນວນເຕັມ available_threads = std::min<ຈຳນວນເຕັມ>(std::thread::hardware_concurrency(), get_max_threads());
	ຈຳນວນເຕັມ num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ສົ່ງຄືນ std::max(0, num_threads);
#ບໍ່ດັ່ງນັ້ນ
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	ສົ່ງຄືນ 0;
#endif
}

ຈຳນວນເຕັມ ThreadPool::work_pool_size(ຈຳນວນເຕັມ reserved_cores, ຈຳນວນເຕັມ work_units, ຈຳນວນເຕັມ work_units_per_thread)
{
	ຈຳນວນເຕັມ work_units_per_thread_override = get_work_units_per_thread_override();
	ຖ້າ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ສົ່ງຄືນ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(ຈຳນວນເຕັມ pool_size, std::function<void(ຈຳນວນເຕັມ)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#ບໍ່ດັ່ງນັ້ນ
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	ສຳລັບ (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(ຈຳນວນເຕັມ num_items, ຈຳນວນເຕັມ thread_num, ຈຳນວນເຕັມ num_threads)
{
	ຖ້າ (num_threads <= 1) {
		ສົ່ງຄືນ {0, num_items};
	}
	ຈຳນວນເຕັມ items_per_thread = num_items / num_threads;
	ຈຳນວນເຕັມ extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	ຈຳນວນເຕັມ start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	ຈຳນວນເຕັມ end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ສົ່ງຄືນ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ຈຳນວນເຕັມ pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#ບໍ່ດັ່ງນັ້ນ
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](ຈຳນວນເຕັມ thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ຖ້າ (num_worker_threads_ == 0)
		ສົ່ງຄືນ;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, ຈຳນວນເຕັມ max_threads)
{
	Multithreading multithreading;
	ຈຳນວນເຕັມ num_active_worker_threads = num_threads(max_threads) - 1;
	ຖ້າ (num_active_worker_threads == 0) {
		work({{0}, 1});
		ສົ່ງຄືນ;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(ຈຳນວນເຕັມ thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	ໃນຂະນະທີ່ (true)
	{
		worker_wait_for_start(thread_num);
		ຖ້າ (current_work == nullptr)
			bha_nga;
		ຈຳນວນເຕັມ num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#ບໍ່ດັ່ງນັ້ນ
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
