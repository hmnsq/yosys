#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_threads()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	ܐܢ (v == nullptr)
		ܦܢܐ INT32_MAX;
	ܦܢܐ atoi(v);
}

ساكن صحيح get_max_threads()
{
	ساكن صحيح max_threads = init_max_threads();
	ܦܢܐ max_threads;
}

ساكن صحيح init_work_units_per_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ܐܢ (v == nullptr)
		ܦܢܐ 0;
	ܦܢܐ atoi(v);
}

ساكن صحيح get_work_units_per_thread_override()
{
	ساكن صحيح work_units_per_thread = init_work_units_per_thread_override();
	ܦܢܐ work_units_per_thread;
}

فراغ DeferredLogs::flush()
{
	لكل (auto &m : logs)
		ܐܢ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		ܐܚܪܢܐ
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

صحيح ThreadPool::pool_size(صحيح reserved_cores, صحيح max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	صحيح available_threads = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_threads());
	صحيح num_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ܦܢܐ std::max(0, num_threads);
#ܐܚܪܢܐ
	(فراغ)reserved_cores;
	(فراغ)max_worker_threads;
	(فراغ)get_max_threads();
	ܦܢܐ 0;
#endif
}

صحيح ThreadPool::work_pool_size(صحيح reserved_cores, صحيح work_units, صحيح work_units_per_thread)
{
	صحيح work_units_per_thread_override = get_work_units_per_thread_override();
	ܐܢ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ܦܢܐ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(صحيح pool_size, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	لكل (صحيح i = 0; i < pool_size; i++)
		threads.emplace_back([i, هذا]{ body(i); });
#ܐܚܪܢܐ
	(فراغ)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	لكل (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(صحيح num_items, صحيح thread_num, صحيح num_threads)
{
	ܐܢ (num_threads <= 1) {
		ܦܢܐ {0, num_items};
	}
	صحيح items_per_thread = num_items / num_threads;
	صحيح extra_items = num_items % num_threads;
	// The first `extra_items` threads get one extra item.
	صحيح start = thread_num * items_per_thread + std::min(thread_num, extra_items);
	صحيح end = (thread_num + 1) * items_per_thread + std::min(thread_num + 1, extra_items);
	ܦܢܐ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: num_worker_threads_(std::max(1, pool_size) - 1)
#ܐܚܪܢܐ
		: num_worker_threads_(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(فراغ)pool_size;
#endif
	main_to_workers_signal.resize(num_worker_threads_, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(num_worker_threads_, [هذا](صحيح thread_num){
		run_worker(thread_num);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ܐܢ (num_worker_threads_ == 0)
		ܦܢܐ;
	current_work = nullptr;
	num_active_worker_threads_.store(num_worker_threads_, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run(std::function<فراغ(ثابت RunCtx &)> work, صحيح max_threads)
{
	Multithreading multithreading;
	صحيح num_active_worker_threads = num_threads(max_threads) - 1;
	ܐܢ (num_active_worker_threads == 0) {
		work({{0}, 1});
		ܦܢܐ;
	}
	num_active_worker_threads_.store(num_active_worker_threads, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, num_active_worker_threads + 1});
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run_worker(صحيح thread_num)
{
#ifdef YOSYS_ENABLE_THREADS
	ܟܡܐ (ܫܪܪܐ)
	{
		worker_wait_for_start(thread_num);
		ܐܢ (current_work == nullptr)
			اكسر;
		صحيح num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		(*current_work)({{thread_num + 1}, num_active_worker_threads + 1});
		signal_worker_done();
	}
	signal_worker_done();
#ܐܚܪܢܐ
	(فراغ)thread_num;
	(فراغ)current_work;
#endif
}

YOSYS_NAMESPACE_END
