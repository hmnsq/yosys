#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ init_max_threads()
{
	ಧ್ರುವ ವರ್ಣ *v = getenv("YOSYS_MAX_THREADS");
	ಯದಿ (v == nullptr)
		ಪ್ರತ್ಯಾಗಮ INT32_MAX;
	ಪ್ರತ್ಯಾಗಮ atoi(v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_max_threads()
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ max_threads = init_max_threads();
	ಪ್ರತ್ಯಾಗಮ max_threads;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ init_work_units_per_thread_override()
{
	ಧ್ರುವ ವರ್ಣ *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ಯದಿ (v == nullptr)
		ಪ್ರತ್ಯಾಗಮ 0;
	ಪ್ರತ್ಯಾಗಮ atoi(v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_work_units_per_thread_override()
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ work_units_per_thread = init_work_units_per_thread_override();
	ಪ್ರತ್ಯಾಗಮ work_units_per_thread;
}

ಶೂನ್ಯ DeferredLogs::flush()
{
	ಪ್ರತಿ (auto &m : logs)
		ಯದಿ (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		ಅನ್ಯಥಾ
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

ಪೂರ್ಣಾಙ್ಕ ThreadPool::pool_size(ಪೂರ್ಣಾಙ್ಕ reserved_cores, ಪೂರ್ಣಾಙ್ಕ max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	ಪೂರ್ಣಾಙ್ಕ available_threads = std::min<ಪೂರ್ಣಾಙ್ಕ>(std::thread::hardware_concurrency(), get_max_threads());
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	ಪ್ರತ್ಯಾಗಮ std::max(0, ಅಂಕೆ_threads);
#ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)reserved_cores;
	(ಶೂನ್ಯ)max_worker_threads;
	(ಶೂನ್ಯ)get_max_threads();
	ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಪೂರ್ಣಾಙ್ಕ ThreadPool::work_pool_size(ಪೂರ್ಣಾಙ್ಕ reserved_cores, ಪೂರ್ಣಾಙ್ಕ work_units, ಪೂರ್ಣಾಙ್ಕ work_units_per_thread)
{
	ಪೂರ್ಣಾಙ್ಕ work_units_per_thread_override = get_work_units_per_thread_override();
	ಯದಿ (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	ಪ್ರತ್ಯಾಗಮ pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(ಪೂರ್ಣಾಙ್ಕ pool_size, std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಙ್ಕ)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pool_size; i++)
		threads.emplace_back([i, ಏತತ್]{ body(i); });
#ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	ಪ್ರತಿ (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ವಸ್ತುಗಳು, ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads)
{
	ಯದಿ (ಅಂಕೆ_threads <= 1) {
		ಪ್ರತ್ಯಾಗಮ {0, ಅಂಕೆ_ವಸ್ತುಗಳು};
	}
	ಪೂರ್ಣಾಙ್ಕ ವಸ್ತುಗಳು_per_thread = ಅಂಕೆ_ವಸ್ತುಗಳು / ಅಂಕೆ_threads;
	ಪೂರ್ಣಾಙ್ಕ extra_ವಸ್ತುಗಳು = ಅಂಕೆ_ವಸ್ತುಗಳು % ಅಂಕೆ_threads;
	// The first `extra_items` threads get one extra item.
	ಪೂರ್ಣಾಙ್ಕ start = thread_ಅಂಕೆ * ವಸ್ತುಗಳು_per_thread + std::min(thread_ಅಂಕೆ, extra_ವಸ್ತುಗಳು);
	ಪೂರ್ಣಾಙ್ಕ end = (thread_ಅಂಕೆ + 1) * ವಸ್ತುಗಳು_per_thread + std::min(thread_ಅಂಕೆ + 1, extra_ವಸ್ತುಗಳು);
	ಪ್ರತ್ಯಾಗಮ {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ಪೂರ್ಣಾಙ್ಕ pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: ಅಂಕೆ_worker_threads(std::max(1, pool_size) - 1)
#ಅನ್ಯಥಾ
		: ಅಂಕೆ_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(ಶೂನ್ಯ)pool_size;
#endif
	main_to_workers_signal.resize(ಅಂಕೆ_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(ಅಂಕೆ_worker_threads, [ಏತತ್](ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ){
		run_worker(thread_ಅಂಕೆ);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ಯದಿ (ಅಂಕೆ_worker_threads == 0)
		ಪ್ರತ್ಯಾಗಮ;
	current_work = nullptr;
	ಅಂಕೆ_active_worker_threads.store(ಅಂಕೆ_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

ಶೂನ್ಯ ParallelDispatchThreadPool::run(std::function<ಶೂನ್ಯ(ಧ್ರುವ RunCtx &)> work, ಪೂರ್ಣಾಙ್ಕ max_threads)
{
	Multithreading multithreading;
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_active_worker_threads_2 = ಅಂಕೆ_threads(max_threads) - 1;
	ಯದಿ (ಅಂಕೆ_active_worker_threads_2 == 0) {
		work({{0}, 1});
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಅಂಕೆ_active_worker_threads.store(ಅಂಕೆ_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, ಅಂಕೆ_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

ಶೂನ್ಯ ParallelDispatchThreadPool::run_worker(ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ)
{
#ifdef YOSYS_ENABLE_THREADS
	ಯಾವತ್ (ಸತ್ಯ)
	{
		worker_wait_for_start(thread_ಅಂಕೆ);
		ಯದಿ (current_work == nullptr)
			ಭಙ್ಗ;
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_active_worker_threads_2 = ಅಂಕೆ_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_ಅಂಕೆ + 1}, ಅಂಕೆ_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)thread_ಅಂಕೆ;
	(ಶೂನ್ಯ)current_work;
#endif
}

YOSYS_NAMESPACE_END
