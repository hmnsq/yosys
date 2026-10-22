#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

স্থির পূর্ণাঙ্ক init_max_threads()
{
	ধ্রুব বর্ণ *v = getenv("YOSYS_MAX_THREADS");
	যদি (v == nullptr)
		প্রত্যাগম INT32_MAX;
	প্রত্যাগম atoi(v);
}

স্থির পূর্ণাঙ্ক get_max_threads()
{
	স্থির পূর্ণাঙ্ক max_threads = init_max_threads();
	প্রত্যাগম max_threads;
}

স্থির পূর্ণাঙ্ক init_work_units_per_thread_override()
{
	ধ্রুব বর্ণ *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	যদি (v == nullptr)
		প্রত্যাগম 0;
	প্রত্যাগম atoi(v);
}

স্থির পূর্ণাঙ্ক get_work_units_per_thread_override()
{
	স্থির পূর্ণাঙ্ক work_units_per_thread = init_work_units_per_thread_override();
	প্রত্যাগম work_units_per_thread;
}

শূন্য DeferredLogs::flush()
{
	প্রতি (auto &m : logs)
		যদি (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		অন্যথা
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

পূর্ণাঙ্ক ThreadPool::pool_size(পূর্ণাঙ্ক reserved_cores, পূর্ণাঙ্ক max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	পূর্ণাঙ্ক available_threads = std::min<পূর্ণাঙ্ক>(std::thread::hardware_concurrency(), get_max_threads());
	পূর্ণাঙ্ক অঙ্ক_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	প্রত্যাগম std::max(0, অঙ্ক_threads);
#অন্যথা
	(শূন্য)reserved_cores;
	(শূন্য)max_worker_threads;
	(শূন্য)get_max_threads();
	প্রত্যাগম 0;
#endif
}

পূর্ণাঙ্ক ThreadPool::work_pool_size(পূর্ণাঙ্ক reserved_cores, পূর্ণাঙ্ক work_units, পূর্ণাঙ্ক work_units_per_thread)
{
	পূর্ণাঙ্ক work_units_per_thread_override = get_work_units_per_thread_override();
	যদি (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	প্রত্যাগম pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(পূর্ণাঙ্ক pool_size, std::function<শূন্য(পূর্ণাঙ্ক)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	প্রতি (পূর্ণাঙ্ক i = 0; i < pool_size; i++)
		threads.emplace_back([i, এতত্]{ body(i); });
#অন্যথা
	(শূন্য)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	প্রতি (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(পূর্ণাঙ্ক অঙ্ক_বস্তুসমূহ, পূর্ণাঙ্ক thread_অঙ্ক, পূর্ণাঙ্ক অঙ্ক_threads)
{
	যদি (অঙ্ক_threads <= 1) {
		প্রত্যাগম {0, অঙ্ক_বস্তুসমূহ};
	}
	পূর্ণাঙ্ক বস্তুসমূহ_per_thread = অঙ্ক_বস্তুসমূহ / অঙ্ক_threads;
	পূর্ণাঙ্ক extra_বস্তুসমূহ = অঙ্ক_বস্তুসমূহ % অঙ্ক_threads;
	// The first `extra_items` threads get one extra item.
	পূর্ণাঙ্ক start = thread_অঙ্ক * বস্তুসমূহ_per_thread + std::min(thread_অঙ্ক, extra_বস্তুসমূহ);
	পূর্ণাঙ্ক end = (thread_অঙ্ক + 1) * বস্তুসমূহ_per_thread + std::min(thread_অঙ্ক + 1, extra_বস্তুসমূহ);
	প্রত্যাগম {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(পূর্ণাঙ্ক pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: অঙ্ক_worker_threads(std::max(1, pool_size) - 1)
#অন্যথা
		: অঙ্ক_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(শূন্য)pool_size;
#endif
	main_to_workers_signal.resize(অঙ্ক_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(অঙ্ক_worker_threads, [এতত্](পূর্ণাঙ্ক thread_অঙ্ক){
		run_worker(thread_অঙ্ক);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	যদি (অঙ্ক_worker_threads == 0)
		প্রত্যাগম;
	current_work = nullptr;
	অঙ্ক_active_worker_threads.store(অঙ্ক_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

শূন্য ParallelDispatchThreadPool::run(std::function<শূন্য(ধ্রুব RunCtx &)> work, পূর্ণাঙ্ক max_threads)
{
	Multithreading multithreading;
	পূর্ণাঙ্ক অঙ্ক_active_worker_threads_2 = অঙ্ক_threads(max_threads) - 1;
	যদি (অঙ্ক_active_worker_threads_2 == 0) {
		work({{0}, 1});
		প্রত্যাগম;
	}
	অঙ্ক_active_worker_threads.store(অঙ্ক_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, অঙ্ক_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

শূন্য ParallelDispatchThreadPool::run_worker(পূর্ণাঙ্ক thread_অঙ্ক)
{
#ifdef YOSYS_ENABLE_THREADS
	যাবত্ (সত্য)
	{
		worker_wait_for_start(thread_অঙ্ক);
		যদি (current_work == nullptr)
			ভঙ্গ;
		পূর্ণাঙ্ক অঙ্ক_active_worker_threads_2 = অঙ্ক_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_অঙ্ক + 1}, অঙ্ক_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#অন্যথা
	(শূন্য)thread_অঙ্ক;
	(শূন্য)current_work;
#endif
}

YOSYS_NAMESPACE_END
