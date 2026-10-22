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
	صحيح اڠک_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	أرجع std::max(0, اڠک_threads);
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

IntRange item_range_for_worker(صحيح اڠک_بارڠن, صحيح thread_اڠک, صحيح اڠک_threads)
{
	إذا (اڠک_threads <= 1) {
		أرجع {0, اڠک_بارڠن};
	}
	صحيح بارڠن_per_thread = اڠک_بارڠن / اڠک_threads;
	صحيح extra_بارڠن = اڠک_بارڠن % اڠک_threads;
	// The first `extra_items` threads get one extra item.
	صحيح start = thread_اڠک * بارڠن_per_thread + std::min(thread_اڠک, extra_بارڠن);
	صحيح end = (thread_اڠک + 1) * بارڠن_per_thread + std::min(thread_اڠک + 1, extra_بارڠن);
	أرجع {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: اڠک_worker_threads(std::max(1, pool_size) - 1)
#وإلا
		: اڠک_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(فراغ)pool_size;
#endif
	main_to_workers_signal.resize(اڠک_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(اڠک_worker_threads, [هذا](صحيح thread_اڠک){
		run_worker(thread_اڠک);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (اڠک_worker_threads == 0)
		أرجع;
	current_work = nullptr;
	اڠک_active_worker_threads.store(اڠک_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run(std::function<فراغ(ثابت RunCtx &)> work, صحيح max_threads)
{
	Multithreading multithreading;
	صحيح اڠک_active_worker_threads_2 = اڠک_threads(max_threads) - 1;
	إذا (اڠک_active_worker_threads_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	اڠک_active_worker_threads.store(اڠک_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, اڠک_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

فراغ ParallelDispatchThreadPool::run_worker(صحيح thread_اڠک)
{
#ifdef YOSYS_ENABLE_THREADS
	طالما (true)
	{
		worker_wait_for_start(thread_اڠک);
		إذا (current_work == nullptr)
			اكسر;
		صحيح اڠک_active_worker_threads_2 = اڠک_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_اڠک + 1}, اڠک_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#وإلا
	(فراغ)thread_اڠک;
	(فراغ)current_work;
#endif
}

YOSYS_NAMESPACE_END
