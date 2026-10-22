#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

স্থির পূর্ণসংখ্যা init_max_threads()
{
	ধ্রুবক অক্ষর *v = getenv("YOSYS_MAX_THREADS");
	যদি (v == nullptr)
		ফেরত INT32_MAX;
	ফেরত atoi(v);
}

স্থির পূর্ণসংখ্যা get_max_threads()
{
	স্থির পূর্ণসংখ্যা max_threads = init_max_threads();
	ফেরত max_threads;
}

স্থির পূর্ণসংখ্যা init_work_units_per_thread_override()
{
	ধ্রুবক অক্ষর *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	যদি (v == nullptr)
		ফেরত 0;
	ফেরত atoi(v);
}

স্থির পূর্ণসংখ্যা get_work_units_per_thread_override()
{
	স্থির পূর্ণসংখ্যা work_units_per_thread = init_work_units_per_thread_override();
	ফেরত work_units_per_thread;
}

শূন্য DeferredLogs::flush()
{
	জন্য (auto &m : logs)
		যদি (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		অন্যথা
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

পূর্ণসংখ্যা ThreadPool::pool_আকার(পূর্ণসংখ্যা reserved_cores, পূর্ণসংখ্যা max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	পূর্ণসংখ্যা available_threads = std::min<পূর্ণসংখ্যা>(std::thread::hardware_concurrency(), get_max_threads());
	পূর্ণসংখ্যা অঙ্ক_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ফেরত std::max(0, অঙ্ক_threads);
#অন্যথা
	(শূন্য)reserved_cores;
	(শূন্য)max_worker_threads;
	(শূন্য)get_max_threads();
	ফেরত 0;
#endif
}

পূর্ণসংখ্যা ThreadPool::work_pool_আকার(পূর্ণসংখ্যা reserved_cores, পূর্ণসংখ্যা work_units, পূর্ণসংখ্যা work_units_per_thread)
{
	পূর্ণসংখ্যা work_units_per_thread_override = get_work_units_per_thread_override();
	যদি (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ফেরত pool_আকার(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(পূর্ণসংখ্যা pool_আকার, std::function<শূন্য(পূর্ণসংখ্যা)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_আকার);
	জন্য (পূর্ণসংখ্যা i = 0; i < pool_আকার; i++)
		threads.emplace_back([i, এই]{ body(i); });
#অন্যথা
	(শূন্য)pool_আকার;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	জন্য (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(পূর্ণসংখ্যা অঙ্ক_বস্তুসমূহ, পূর্ণসংখ্যা thread_অঙ্ক, পূর্ণসংখ্যা অঙ্ক_threads)
{
	যদি (অঙ্ক_threads <= 1) {
		ফেরত {0, অঙ্ক_বস্তুসমূহ};
	}
	পূর্ণসংখ্যা বস্তুসমূহ_per_thread = অঙ্ক_বস্তুসমূহ / অঙ্ক_threads;
	পূর্ণসংখ্যা extra_বস্তুসমূহ = অঙ্ক_বস্তুসমূহ % অঙ্ক_threads;
	// The first `extra_items` threads get one extra item.
	পূর্ণসংখ্যা start = thread_অঙ্ক * বস্তুসমূহ_per_thread + std::min(thread_অঙ্ক, extra_বস্তুসমূহ);
	পূর্ণসংখ্যা end = (thread_অঙ্ক + 1) * বস্তুসমূহ_per_thread + std::min(thread_অঙ্ক + 1, extra_বস্তুসমূহ);
	ফেরত {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(পূর্ণসংখ্যা pool_আকার)
#ifdef YOSYS_ENABLE_THREADS
		: অঙ্ক_worker_threads(std::max(1, pool_আকার) - 1)
#অন্যথা
		: অঙ্ক_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(শূন্য)pool_আকার;
#endif
	main_to_workers_signal.resize(অঙ্ক_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(অঙ্ক_worker_threads, [এই](পূর্ণসংখ্যা thread_অঙ্ক){
		run_worker(thread_অঙ্ক);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	যদি (অঙ্ক_worker_threads == 0)
		ফেরত;
	current_work = nullptr;
	অঙ্ক_active_worker_threads.store(অঙ্ক_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

শূন্য ParallelDispatchThreadPool::run(std::function<শূন্য(ধ্রুবক RunCtx &)> work, পূর্ণসংখ্যা max_threads)
{
	Multithreading multithreading;
	পূর্ণসংখ্যা অঙ্ক_active_worker_threads_2 = অঙ্ক_threads(max_threads) - 1;
	যদি (অঙ্ক_active_worker_threads_2 == 0) {
		work({{0}, 1});
		ফেরত;
	}
	অঙ্ক_active_worker_threads.store(অঙ্ক_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, অঙ্ক_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

শূন্য ParallelDispatchThreadPool::run_worker(পূর্ণসংখ্যা thread_অঙ্ক)
{
#ifdef YOSYS_ENABLE_THREADS
	যখন (সত্য)
	{
		worker_wait_for_start(thread_অঙ্ক);
		যদি (current_work == nullptr)
			বিরতি;
		পূর্ণসংখ্যা অঙ্ক_active_worker_threads_2 = অঙ্ক_active_worker_threads.load(std::memory_order_relaxed);
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
