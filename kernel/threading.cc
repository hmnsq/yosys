#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_threads()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	إذا (v == nullptr)
		أرجع INT32_MAX;
	أرجع atoi(v);
}

ساكن صحيح get_max_threads()
{
	ساكن صحيح max_threads = init_max_threads();
	أرجع max_threads;
}

ساكن صحيح init_work_units_per_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	إذا (v == nullptr)
		أرجع 0;
	أرجع atoi(v);
}

ساكن صحيح get_work_units_per_thread_override()
{
	ساكن صحيح work_units_per_thread = init_work_units_per_thread_override();
	أرجع work_units_per_thread;
}

فراغ DeferredLogs::flush()
{
	لكل (auto &m : logs)
		إذا (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		وإلا
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

صحيح ThreadPool::pool_size(صحيح reserved_cores, صحيح max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	صحيح available_threads = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_threads());
	صحيح عدد_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	أرجع std::max(0, عدد_threads);
#وإلا
	(فراغ)reserved_cores;
	(فراغ)max_worker_threads;
	(فراغ)get_max_threads();
	أرجع 0;
#endif
}

صحيح ThreadPool::work_pool_size(صحيح reserved_cores, صحيح work_units, صحيح work_units_per_thread)
{
	صحيح work_units_per_thread_override = get_work_units_per_thread_override();
	إذا (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	أرجع pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(صحيح pool_size, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	لكل (صحيح i = 0; i < pool_size; i++)
		threads.emplace_back([i, هذا]{ body(i); });
#وإلا
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

IntRange item_range_for_worker(صحيح عدد_اشیاء, صحيح thread_عدد, صحيح عدد_threads)
{
	إذا (عدد_threads <= 1) {
		أرجع {0, عدد_اشیاء};
	}
	صحيح اشیاء_per_thread = عدد_اشیاء / عدد_threads;
	صحيح extra_اشیاء = عدد_اشیاء % عدد_threads;
	// The first `extra_items` threads get one extra item.
	صحيح start = thread_عدد * اشیاء_per_thread + std::min(thread_عدد, extra_اشیاء);
	صحيح end = (thread_عدد + 1) * اشیاء_per_thread + std::min(thread_عدد + 1, extra_اشیاء);
	أرجع {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: عدد_worker_threads(std::max(1, pool_size) - 1)
#وإلا
		: عدد_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(فراغ)pool_size;
#endif
	main_to_workers_signal.resize(عدد_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(عدد_worker_threads, [هذا](صحيح thread_عدد){
		run_worker(thread_عدد);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (عدد_worker_threads == 0)
		أرجع;
	current_work = nullptr;
	عدد_active_worker_threads.store(عدد_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run(std::function<فراغ(ثابت RunCtx &)> work, صحيح max_threads)
{
	Multithreading multithreading;
	صحيح عدد_active_worker_threads_2 = عدد_threads(max_threads) - 1;
	إذا (عدد_active_worker_threads_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	عدد_active_worker_threads.store(عدد_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, عدد_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run_worker(صحيح thread_عدد)
{
#ifdef YOSYS_ENABLE_THREADS
	طالما (true)
	{
		worker_wait_for_start(thread_عدد);
		إذا (current_work == nullptr)
			اكسر;
		صحيح عدد_active_worker_threads_2 = عدد_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_عدد + 1}, عدد_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#وإلا
	(فراغ)thread_عدد;
	(فراغ)current_work;
#endif
}

YOSYS_NAMESPACE_END
