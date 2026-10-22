#include "kernel/yosys_common.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

static ကိန်းပြည့် init_max_threads()
{
	const var_na *v = getenv("YOSYS_MAX_THREADS");
	အကယ်၍ (v == nullptr)
		ပြန်ပေး INT32_MAX;
	ပြန်ပေး atoi(v);
}

static ကိန်းပြည့် get_max_threads()
{
	static ကိန်းပြည့် max_threads = init_max_threads();
	ပြန်ပေး max_threads;
}

static ကိန်းပြည့် init_work_units_per_thread_override()
{
	const var_na *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	အကယ်၍ (v == nullptr)
		ပြန်ပေး 0;
	ပြန်ပေး atoi(v);
}

static ကိန်းပြည့် get_work_units_per_thread_override()
{
	static ကိန်းပြည့် work_units_per_thread = init_work_units_per_thread_override();
	ပြန်ပေး work_units_per_thread;
}

void DeferredLogs::flush()
{
	အတွက် (auto &m : logs)
		အကယ်၍ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		မဟုတ်ပါက
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

ကိန်းပြည့် ThreadPool::pool_size(ကိန်းပြည့် reserved_cores, ကိန်းပြည့် max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	ကိန်းပြည့် available_threads = std::min<ကိန်းပြည့်>(std::thread::hardware_concurrency(), get_max_threads());
	ကိန်းပြည့် num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ပြန်ပေး std::max(0, num_threads);
#မဟုတ်ပါက
	(void)reserved_cores;
	(void)max_worker_threads;
	(void)get_max_threads();
	ပြန်ပေး 0;
#endif
}

ကိန်းပြည့် ThreadPool::work_pool_size(ကိန်းပြည့် reserved_cores, ကိန်းပြည့် work_units, ကိန်းပြည့် work_units_per_thread)
{
	ကိန်းပြည့် work_units_per_thread_override = get_work_units_per_thread_override();
	အကယ်၍ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ပြန်ပေး pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(ကိန်းပြည့် pool_size, std::function<void(ကိန်းပြည့်)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	အတွက် (ကိန်းပြည့် i = 0; i < pool_size; i++)
		threads.emplace_back([i, this]{ body(i); });
#မဟုတ်ပါက
	(void)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	အတွက် (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(ကိန်းပြည့် num_items, ကိန်းပြည့် thread_num, ကိန်းပြည့် num_threads)
{
	အကယ်၍ (num_threads <= 1) {
		ပြန်ပေး {0, num_items};
	}
	ကိန်းပြည့် items_per_thread = num_items / num_threads;
	ကိန်းပြည့် extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	ကိန်းပြည့် start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	ကိန်းပြည့် end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ပြန်ပေး {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ကိန်းပြည့် pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#မဟုတ်ပါက
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(void)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [this](ကိန်းပြည့် thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	အကယ်၍ (num_worker_threads_ == 0)
		ပြန်ပေး;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run(std::function<void(const RunCtx &)> work, ကိန်းပြည့် max_threads)
{
	Multithreading multithreading;
	ကိန်းပြည့် num_active_worker_threads = num_threads(max_threads) - 1;
	အကယ်၍ (num_active_worker_threads == 0) {
		work({{0}, 1});
		ပြန်ပေး;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

void ParallelDispatchThreadPool::run_worker(ကိန်းပြည့် thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	နေစဉ် (true)
	{
		worker_wait_for_start(thread_num);
		အကယ်၍ (current_work == nullptr)
			bha_nga;
		ကိန်းပြည့် num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#မဟုတ်ပါက
	(void)thread_num;
	(void)current_work;
#endif
}

YOSYS_NAMESPACE_END
