#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

સ્થિર પૂર્ણાંક init_max_threads()
{
	અચલ અક્ષર *v = getenv("YOSYS_MAX_THREADS");
	જો (v == nullptr)
		પાછા INT32_MAX;
	પાછા atoi(v);
}

સ્થિર પૂર્ણાંક get_max_threads()
{
	સ્થિર પૂર્ણાંક max_threads = init_max_threads();
	પાછા max_threads;
}

સ્થિર પૂર્ણાંક init_work_units_per_thread_override()
{
	અચલ અક્ષર *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	જો (v == nullptr)
		પાછા 0;
	પાછા atoi(v);
}

સ્થિર પૂર્ણાંક get_work_units_per_thread_override()
{
	સ્થિર પૂર્ણાંક work_units_per_thread = init_work_units_per_thread_override();
	પાછા work_units_per_thread;
}

શૂન્ય DeferredLogs::flush()
{
	માટે (auto &m : logs)
		જો (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		નહીંતર
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

પૂર્ણાંક ThreadPool::pool_size(પૂર્ણાંક reserved_cores, પૂર્ણાંક max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	પૂર્ણાંક available_threads = std::min<પૂર્ણાંક>(std::thread::hardware_concurrency(), get_max_threads());
	પૂર્ણાંક અંક_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	પાછા std::max(0, અંક_threads);
#નહીંતર
	(શૂન્ય)reserved_cores;
	(શૂન્ય)max_worker_threads;
	(શૂન્ય)get_max_threads();
	પાછા 0;
#endif
}

પૂર્ણાંક ThreadPool::work_pool_size(પૂર્ણાંક reserved_cores, પૂર્ણાંક work_units, પૂર્ણાંક work_units_per_thread)
{
	પૂર્ણાંક work_units_per_thread_override = get_work_units_per_thread_override();
	જો (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	પાછા pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(પૂર્ણાંક pool_size, std::function<શૂન્ય(પૂર્ણાંક)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	માટે (પૂર્ણાંક i = 0; i < pool_size; i++)
		threads.emplace_back([i, આ]{ body(i); });
#નહીંતર
	(શૂન્ય)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	માટે (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(પૂર્ણાંક અંક_વસ્તુઓ, પૂર્ણાંક thread_અંક, પૂર્ણાંક અંક_threads)
{
	જો (અંક_threads <= 1) {
		પાછા {0, અંક_વસ્તુઓ};
	}
	પૂર્ણાંક વસ્તુઓ_per_thread = અંક_વસ્તુઓ / અંક_threads;
	પૂર્ણાંક extra_વસ્તુઓ = અંક_વસ્તુઓ % અંક_threads;
	// The first `extra_items` threads get one extra item.
	પૂર્ણાંક start = thread_અંક * વસ્તુઓ_per_thread + std::min(thread_અંક, extra_વસ્તુઓ);
	પૂર્ણાંક end = (thread_અંક + 1) * વસ્તુઓ_per_thread + std::min(thread_અંક + 1, extra_વસ્તુઓ);
	પાછા {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(પૂર્ણાંક pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: અંક_worker_threads(std::max(1, pool_size) - 1)
#નહીંતર
		: અંક_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(શૂન્ય)pool_size;
#endif
	main_to_workers_signal.resize(અંક_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(અંક_worker_threads, [આ](પૂર્ણાંક thread_અંક){
		run_worker(thread_અંક);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	જો (અંક_worker_threads == 0)
		પાછા;
	current_work = nullptr;
	અંક_active_worker_threads.store(અંક_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

શૂન્ય ParallelDispatchThreadPool::run(std::function<શૂન્ય(અચલ RunCtx &)> work, પૂર્ણાંક max_threads)
{
	Multithreading multithreading;
	પૂર્ણાંક અંક_active_worker_threads_2 = અંક_threads(max_threads) - 1;
	જો (અંક_active_worker_threads_2 == 0) {
		work({{0}, 1});
		પાછા;
	}
	અંક_active_worker_threads.store(અંક_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, અંક_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

શૂન્ય ParallelDispatchThreadPool::run_worker(પૂર્ણાંક thread_અંક)
{
#ifdef YOSYS_ENABLE_THREADS
	જ્યારે (સાચું)
	{
		worker_wait_for_start(thread_અંક);
		જો (current_work == nullptr)
			તોડો;
		પૂર્ણાંક અંક_active_worker_threads_2 = અંક_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_અંક + 1}, અંક_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#નહીંતર
	(શૂન્ય)thread_અંક;
	(શૂન્ય)current_work;
#endif
}

YOSYS_NAMESPACE_END
