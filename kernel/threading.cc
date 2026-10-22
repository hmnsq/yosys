#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

நிலையான முழு init_max_threads()
{
	மாறிலி எழுத்து *v = getenv("YOSYS_MAX_THREADS");
	எனில் (v == nullptr)
		திரும்பு INT32_MAX;
	திரும்பு atoi(v);
}

நிலையான முழு get_max_threads()
{
	நிலையான முழு max_threads = init_max_threads();
	திரும்பு max_threads;
}

நிலையான முழு init_work_units_per_thread_override()
{
	மாறிலி எழுத்து *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	எனில் (v == nullptr)
		திரும்பு 0;
	திரும்பு atoi(v);
}

நிலையான முழு get_work_units_per_thread_override()
{
	நிலையான முழு work_units_per_thread = init_work_units_per_thread_override();
	திரும்பு work_units_per_thread;
}

வெறுமை DeferredLogs::flush()
{
	க்கு (auto &m : logs)
		எனில் (m.error)
			YOSYS_NAMESPACE_PREFIX log_error("%s", m.text.c_str());
		இல்லையெனில்
			YOSYS_NAMESPACE_PREFIX log("%s", m.text.c_str());
}

முழு ThreadPool::pool_size(முழு reserved_cores, முழு max_worker_threads)
{
#ifdef YOSYS_ENABLE_THREADS
	முழு available_threads = std::min<முழு>(std::thread::hardware_concurrency(), get_max_threads());
	முழு இலக்கம்_threads = std::min(available_threads - reserved_cores, max_worker_threads);
	திரும்பு std::max(0, இலக்கம்_threads);
#இல்லையெனில்
	(வெறுமை)reserved_cores;
	(வெறுமை)max_worker_threads;
	(வெறுமை)get_max_threads();
	திரும்பு 0;
#endif
}

முழு ThreadPool::work_pool_size(முழு reserved_cores, முழு work_units, முழு work_units_per_thread)
{
	முழு work_units_per_thread_override = get_work_units_per_thread_override();
	எனில் (work_units_per_thread_override > 0)
		work_units_per_thread = work_units_per_thread_override;
	திரும்பு pool_size(reserved_cores, work_units / work_units_per_thread);
}

ThreadPool::ThreadPool(முழு pool_size, std::function<வெறுமை(முழு)> b)
	: body(std::move(b))
{
#ifdef YOSYS_ENABLE_THREADS
	threads.reserve(pool_size);
	க்கு (முழு i = 0; i < pool_size; i++)
		threads.emplace_back([i, இது]{ body(i); });
#இல்லையெனில்
	(வெறுமை)pool_size;
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef YOSYS_ENABLE_THREADS
	க்கு (auto &t : threads)
		t.join();
#endif
}

IntRange item_range_for_worker(முழு இலக்கம்_பொருட்கள், முழு thread_இலக்கம், முழு இலக்கம்_threads)
{
	எனில் (இலக்கம்_threads <= 1) {
		திரும்பு {0, இலக்கம்_பொருட்கள்};
	}
	முழு பொருட்கள்_per_thread = இலக்கம்_பொருட்கள் / இலக்கம்_threads;
	முழு extra_பொருட்கள் = இலக்கம்_பொருட்கள் % இலக்கம்_threads;
	// The first `extra_items` threads get one extra item.
	முழு start = thread_இலக்கம் * பொருட்கள்_per_thread + std::min(thread_இலக்கம், extra_பொருட்கள்);
	முழு end = (thread_இலக்கம் + 1) * பொருட்கள்_per_thread + std::min(thread_இலக்கம் + 1, extra_பொருட்கள்);
	திரும்பு {start, end};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(முழு pool_size)
#ifdef YOSYS_ENABLE_THREADS
		: இலக்கம்_worker_threads(std::max(1, pool_size) - 1)
#இல்லையெனில்
		: இலக்கம்_worker_threads(0)
#endif
{
#ifndef YOSYS_ENABLE_THREADS
	(வெறுமை)pool_size;
#endif
	main_to_workers_signal.resize(இலக்கம்_worker_threads, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(இலக்கம்_worker_threads, [இது](முழு thread_இலக்கம்){
		run_worker(thread_இலக்கம்);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	எனில் (இலக்கம்_worker_threads == 0)
		திரும்பு;
	current_work = nullptr;
	இலக்கம்_active_worker_threads.store(இலக்கம்_worker_threads, std::memory_order_relaxed);
	signal_workers_start();
	wait_for_workers_done();
}

வெறுமை ParallelDispatchThreadPool::run(std::function<வெறுமை(மாறிலி RunCtx &)> work, முழு max_threads)
{
	Multithreading multithreading;
	முழு இலக்கம்_active_worker_threads_2 = இலக்கம்_threads(max_threads) - 1;
	எனில் (இலக்கம்_active_worker_threads_2 == 0) {
		work({{0}, 1});
		திரும்பு;
	}
	இலக்கம்_active_worker_threads.store(இலக்கம்_active_worker_threads_2, std::memory_order_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, இலக்கம்_active_worker_threads_2 + 1});
	wait_for_workers_done();
}

வெறுமை ParallelDispatchThreadPool::run_worker(முழு thread_இலக்கம்)
{
#ifdef YOSYS_ENABLE_THREADS
	போது (உண்மை)
	{
		worker_wait_for_start(thread_இலக்கம்);
		எனில் (current_work == nullptr)
			நிறுத்து;
		முழு இலக்கம்_active_worker_threads_2 = இலக்கம்_active_worker_threads.load(std::memory_order_relaxed);
		(*current_work)({{thread_இலக்கம் + 1}, இலக்கம்_active_worker_threads_2 + 1});
		signal_worker_done();
	}
	signal_worker_done();
#இல்லையெனில்
	(வெறுமை)thread_இலக்கம்;
	(வெறுமை)current_work;
#endif
}

YOSYS_NAMESPACE_END
