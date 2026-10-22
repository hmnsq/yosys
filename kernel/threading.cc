#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

സ്ഥിതം പൂർണ്ണസംഖ്യ init_max_threads()
{
	സ്ഥിരം അക്ഷരം *v = getenv("YOSYS_MAX_THREADS");
	എങ്കിൽ (v == nullptr)
		തിരികെ INT32_MAX;
	തിരികെ atoi(v);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_max_threads()
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ max_threads = init_max_threads();
	തിരികെ max_threads;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ init_work_units_per_thread_override()
{
	സ്ഥിരം അക്ഷരം *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	എങ്കിൽ (v == nullptr)
		തിരികെ 0;
	തിരികെ atoi(v);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_work_units_per_thread_override()
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ work_units_per_thread = init_work_units_per_thread_override();
	തിരികെ work_units_per_thread;
}

ശൂന്യം DeferredLogs::flush()
{
	വേണ്ടി (auto &m : logs)
		എങ്കിൽ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		അല്ലെങ്കിൽ
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

പൂർണ്ണസംഖ്യ ThreadPool::pool_size(പൂർണ്ണസംഖ്യ reserved_cores, പൂർണ്ണസംഖ്യ max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	പൂർണ്ണസംഖ്യ available_threads = std::min<പൂർണ്ണസംഖ്യ>(std::thread::hardware_concurrency(), get_max_threads());
	പൂർണ്ണസംഖ്യ അക്കം_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	തിരികെ std::max(0, അക്കം_threads);
#അല്ലെങ്കിൽ
	(ശൂന്യം)reserved_cores;
	(ശൂന്യം)max_worker_threads;
	(ശൂന്യം)get_max_threads();
	തിരികെ 0;
#endif
}

പൂർണ്ണസംഖ്യ ThreadPool::work_pool_size(പൂർണ്ണസംഖ്യ reserved_cores, പൂർണ്ണസംഖ്യ work_units, പൂർണ്ണസംഖ്യ work_units_per_thread)
{
	പൂർണ്ണസംഖ്യ work_units_per_thread_override = get_work_units_per_thread_override();
	എങ്കിൽ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	തിരികെ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(പൂർണ്ണസംഖ്യ pool_size, std::function<ശൂന്യം(പൂർണ്ണസംഖ്യ)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pool_size; i++)
		threads.emplace_back([i, ഇത്]{ body(i); });
#അല്ലെങ്കിൽ
	(ശൂന്യം)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	വേണ്ടി (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(പൂർണ്ണസംഖ്യ അക്കം_ഇനങ്ങൾ, പൂർണ്ണസംഖ്യ thread_അക്കം, പൂർണ്ണസംഖ്യ അക്കം_threads)
{
	എങ്കിൽ (അക്കം_threads <= 1) {
		തിരികെ {0, അക്കം_ഇനങ്ങൾ};
	}
	പൂർണ്ണസംഖ്യ ഇനങ്ങൾ_per_thread = അക്കം_ഇനങ്ങൾ / അക്കം_threads;
	പൂർണ്ണസംഖ്യ extra_ഇനങ്ങൾ = അക്കം_ഇനങ്ങൾ % അക്കം_threads;
	// The first `extra_items` threads get one extra item.
	പൂർണ്ണസംഖ്യ start = thread_അക്കം * ഇനങ്ങൾ_per_thread + std::min(thread_അക്കം, extra_ഇനങ്ങൾ);
	പൂർണ്ണസംഖ്യ end = (thread_അക്കം + 1) * ഇനങ്ങൾ_per_thread + std::min(thread_അക്കം + 1, extra_ഇനങ്ങൾ);
	തിരികെ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(പൂർണ്ണസംഖ്യ pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: അക്കം_worker_threads(std::max(1, pool_size) - 1)
#അല്ലെങ്കിൽ
		: അക്കം_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(ശൂന്യം)pool_size;
#endif
	main_to_workers_signal.resize(അക്കം_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(അക്കം_worker_threads, [ഇത്](പൂർണ്ണസംഖ്യ thread_അക്കം){
		run_worker(thread_അക്കം);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	എങ്കിൽ (അക്കം_worker_threads == 0)
		തിരികെ;
	current_work = nullptr;
	അക്കം_active_worker_threads.store(അക്കം_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

ശൂന്യം ParallelDispatchThreadPool::run(std::function<ശൂന്യം(സ്ഥിരം RunCtx &)> work, പൂർണ്ണസംഖ്യ max_threads)
{
	Multithreading multithreading;
	പൂർണ്ണസംഖ്യ അക്കം_active_worker_threads_2 = അക്കം_threads(max_threads) - 1;
	എങ്കിൽ (അക്കം_active_worker_threads_2 == 0) {
		work({{0}, 1});
		തിരികെ;
	}
	അക്കം_active_worker_threads.store(അക്കം_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, അക്കം_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

ശൂന്യം ParallelDispatchThreadPool::run_worker(പൂർണ്ണസംഖ്യ thread_അക്കം)
{
#ifdef YOSYS_ENABLE_THREADS
	ആകുമ്പോൾ (സത്യം)
	{
		worker_wait_for_start(thread_അക്കം);
		എങ്കിൽ (current_work == nullptr)
			നിർത്തുക;
		പൂർണ്ണസംഖ്യ അക്കം_active_worker_threads_2 = അക്കം_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_അക്കം + 1}, അക്കം_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#അല്ലെങ്കിൽ
	(ശൂന്യം)thread_അക്കം;
	(ശൂന്യം)current_work;
#endif
}

YOSYS_NAMESPACE_END
