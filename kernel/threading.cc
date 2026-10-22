#شامل "kernel/yosys_common.h"
#شامل "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ایستا صحیح init_max_threads()
{
	ثابت نویسه *v = getenv("YOSYS_MAX_THREADS");
	اگر (v == nullptr)
		بازگشت INT32_MAX;
	بازگشت atoi(v);
}

ایستا صحیح get_max_threads()
{
	ایستا صحیح max_threads = init_max_threads();
	بازگشت max_threads;
}

ایستا صحیح init_work_units_per_thread_override()
{
	ثابت نویسه *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	اگر (v == nullptr)
		بازگشت 0;
	بازگشت atoi(v);
}

ایستا صحیح get_work_units_per_thread_override()
{
	ایستا صحیح work_units_per_thread = init_work_units_per_thread_override();
	بازگشت work_units_per_thread;
}

تهی DeferredLogs::flush()
{
	برای (auto &m : logs)
		اگر (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		وگرنه
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

صحیح ThreadPool::pool_size(صحیح reserved_cores, صحیح max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	صحیح available_threads = std::min<صحیح>(std::thread::hardware_concurrency(), get_max_threads());
	صحیح رقم_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	بازگشت std::max(0, رقم_threads);
#وگرنه
	(تهی)reserved_cores;
	(تهی)max_worker_threads;
	(تهی)get_max_threads();
	بازگشت 0;
#endif
}

صحیح ThreadPool::work_pool_size(صحیح reserved_cores, صحیح work_units, صحیح work_units_per_thread)
{
	صحیح work_units_per_thread_override = get_work_units_per_thread_override();
	اگر (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	بازگشت pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(صحیح pool_size, std::function<تهی(صحیح)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	برای (صحیح i = 0; i < pool_size; i++)
		threads.emplace_back([i, این]{ body(i); });
#وگرنه
	(تهی)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	برای (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(صحیح رقم_اقلام, صحیح thread_رقم, صحیح رقم_threads)
{
	اگر (رقم_threads <= 1) {
		بازگشت {0, رقم_اقلام};
	}
	صحیح اقلام_per_thread = رقم_اقلام / رقم_threads;
	صحیح extra_اقلام = رقم_اقلام % رقم_threads;
	// The first `extra_items` threads get one extra item.
	صحیح start = thread_رقم * اقلام_per_thread + std::min(thread_رقم, extra_اقلام);
	صحیح end = (thread_رقم + 1) * اقلام_per_thread + std::min(thread_رقم + 1, extra_اقلام);
	بازگشت {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحیح pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: رقم_worker_threads(std::max(1, pool_size) - 1)
#وگرنه
		: رقم_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(تهی)pool_size;
#endif
	main_to_workers_signal.resize(رقم_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(رقم_worker_threads, [این](صحیح thread_رقم){
		run_worker(thread_رقم);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	اگر (رقم_worker_threads == 0)
		بازگشت;
	current_work = nullptr;
	رقم_active_worker_threads.store(رقم_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

تهی ParallelDispatchThreadPool::run(std::function<تهی(ثابت RunCtx &)> work, صحیح max_threads)
{
	Multithreading multithreading;
	صحیح رقم_active_worker_threads_2 = رقم_threads(max_threads) - 1;
	اگر (رقم_active_worker_threads_2 == 0) {
		work({{0}, 1});
		بازگشت;
	}
	رقم_active_worker_threads.store(رقم_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, رقم_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

تهی ParallelDispatchThreadPool::run_worker(صحیح thread_رقم)
{
#ifdef YOSYS_ENABLE_THREADS
	وقتی (درست)
	{
		worker_wait_for_start(thread_رقم);
		اگر (current_work == nullptr)
			بشکن;
		صحیح رقم_active_worker_threads_2 = رقم_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_رقم + 1}, رقم_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#وگرنه
	(تهی)thread_رقم;
	(تهی)current_work;
#endif
}

YOSYS_NAMESPACE_END
