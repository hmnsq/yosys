#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ਸਥਿਰ ਪੂਰਨ init_max_ਧਾਗੇ()
{
	ਅਚਲ ਅੱਖਰ *v = getenv("YOSYS_MAX_THREADS");
	ਜੇ (v == ਖਾਲੀ_ਸੂਚਕ)
		ਵਾਪਸ INT32_MAX;
	ਵਾਪਸ atoi(v);
}

ਸਥਿਰ ਪੂਰਨ get_max_ਧਾਗੇ()
{
	ਸਥਿਰ ਪੂਰਨ max_ਧਾਗੇ = init_max_ਧਾਗੇ();
	ਵਾਪਸ max_ਧਾਗੇ;
}

ਸਥਿਰ ਪੂਰਨ init_work_units_ਪ੍ਰਤੀ_thread_override()
{
	ਅਚਲ ਅੱਖਰ *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ਜੇ (v == ਖਾਲੀ_ਸੂਚਕ)
		ਵਾਪਸ 0;
	ਵਾਪਸ atoi(v);
}

ਸਥਿਰ ਪੂਰਨ get_work_units_ਪ੍ਰਤੀ_thread_override()
{
	ਸਥਿਰ ਪੂਰਨ work_units_ਪ੍ਰਤੀ_thread = init_work_units_ਪ੍ਰਤੀ_thread_override();
	ਵਾਪਸ work_units_ਪ੍ਰਤੀ_thread;
}

ਖਾਲੀ DeferredLogs::flush()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &m : logs)
		ਜੇ (m.error)
			YOSYS_NAMESPACE_ਅਗੇਤਰ log_error("%s", m.ਪਾਠ.c_str());
		ਨਹੀਂਤਾਂ
			YOSYS_NAMESPACE_ਅਗੇਤਰ ਲਘੂਗਣਕ("%s", m.ਪਾਠ.c_str());
}

ਪੂਰਨ ThreadPool::pool_ਮਾਪ(ਪੂਰਨ reserved_cores, ਪੂਰਨ max_ਕਾਮਾ_ਧਾਗੇ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	ਪੂਰਨ ਉਪਲਬਧ_ਧਾਗੇ = std::min<ਪੂਰਨ>(std::thread::hardware_concurrency(), get_max_ਧਾਗੇ());
	ਪੂਰਨ ਅੰਕ_ਧਾਗੇ = std::min(ਉਪਲਬਧ_ਧਾਗੇ - reserved_cores, max_ਕਾਮਾ_ਧਾਗੇ);
	ਵਾਪਸ std::max(0, ਅੰਕ_ਧਾਗੇ);
#ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)reserved_cores;
	(ਖਾਲੀ)max_ਕਾਮਾ_ਧਾਗੇ;
	(ਖਾਲੀ)get_max_ਧਾਗੇ();
	ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ ThreadPool::work_pool_ਮਾਪ(ਪੂਰਨ reserved_cores, ਪੂਰਨ work_units, ਪੂਰਨ work_units_ਪ੍ਰਤੀ_thread)
{
	ਪੂਰਨ work_units_ਪ੍ਰਤੀ_thread_override = get_work_units_ਪ੍ਰਤੀ_thread_override();
	ਜੇ (work_units_ਪ੍ਰਤੀ_thread_override > 0)
		work_units_ਪ੍ਰਤੀ_thread = work_units_ਪ੍ਰਤੀ_thread_override;
	ਵਾਪਸ pool_ਮਾਪ(reserved_cores, work_units / work_units_ਪ੍ਰਤੀ_thread);
}

ThreadPool::ThreadPool(ਪੂਰਨ pool_ਮਾਪ, std::function<ਖਾਲੀ(ਪੂਰਨ)> b)
	: body(std::move(b))
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	ਧਾਗੇ.reserve(pool_ਮਾਪ);
	ਲਈ (ਪੂਰਨ i = 0; i < pool_ਮਾਪ; i++)
		ਧਾਗੇ.emplace_ਪਿੱਠ([i, ਇਹ]{ body(i); });
#ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)pool_ਮਾਪ;
#ਜੇ_ਅੰਤ
}

ThreadPool::~ThreadPool()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	ਲਈ (ਸਵੈਚਾਲਿਤ &t : ਧਾਗੇ)
		t.join();
#ਜੇ_ਅੰਤ
}

IntRange ਵਸਤੂ_range_for_ਕਾਮਾ(ਪੂਰਨ ਅੰਕ_ਚੀਜ਼ਾਂ, ਪੂਰਨ thread_ਅੰਕ, ਪੂਰਨ ਅੰਕ_ਧਾਗੇ)
{
	ਜੇ (ਅੰਕ_ਧਾਗੇ <= 1) {
		ਵਾਪਸ {0, ਅੰਕ_ਚੀਜ਼ਾਂ};
	}
	ਪੂਰਨ ਚੀਜ਼ਾਂ_ਪ੍ਰਤੀ_thread = ਅੰਕ_ਚੀਜ਼ਾਂ / ਅੰਕ_ਧਾਗੇ;
	ਪੂਰਨ extra_ਚੀਜ਼ਾਂ = ਅੰਕ_ਚੀਜ਼ਾਂ % ਅੰਕ_ਧਾਗੇ;
	// The first `extra_items` threads get one extra item.
	ਪੂਰਨ ਸ਼ੁਰੂ = thread_ਅੰਕ * ਚੀਜ਼ਾਂ_ਪ੍ਰਤੀ_thread + std::min(thread_ਅੰਕ, extra_ਚੀਜ਼ਾਂ);
	ਪੂਰਨ ਸਿਰਾ = (thread_ਅੰਕ + 1) * ਚੀਜ਼ਾਂ_ਪ੍ਰਤੀ_thread + std::min(thread_ਅੰਕ + 1, extra_ਚੀਜ਼ਾਂ);
	ਵਾਪਸ {ਸ਼ੁਰੂ, ਸਿਰਾ};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ਪੂਰਨ pool_ਮਾਪ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
		: ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ(std::max(1, pool_ਮਾਪ) - 1)
#ਨਹੀਂਤਾਂ
		: ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ(0)
#ਜੇ_ਅੰਤ
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	(ਖਾਲੀ)pool_ਮਾਪ;
#ਜੇ_ਅੰਤ
	main_ਮੰਜ਼ਿਲ_workers_signal.resize(ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ, [ਇਹ](ਪੂਰਨ thread_ਅੰਕ){
		ਲੜੀਵਾਰ_ਕਾਮਾ(thread_ਅੰਕ);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ਜੇ (ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ == 0)
		ਵਾਪਸ;
	current_work = ਖਾਲੀ_ਸੂਚਕ;
	ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.store(ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
	signal_workers_ਸ਼ੁਰੂ();
	wait_for_workers_ਪੂਰਾ();
}

ਖਾਲੀ ParallelDispatchThreadPool::ਲੜੀਵਾਰ(std::function<ਖਾਲੀ(ਅਚਲ ਲੜੀਵਾਰ_Ctx &)> work, ਪੂਰਨ max_ਧਾਗੇ)
{
	Multithreading multithreading;
	ਪੂਰਨ ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 = ਅੰਕ_ਧਾਗੇ(max_ਧਾਗੇ) - 1;
	ਜੇ (ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 == 0) {
		work({{0}, 1});
		ਵਾਪਸ;
	}
	ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.store(ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
	current_work = &work;
	signal_workers_ਸ਼ੁਰੂ();
	work({{0}, ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 + 1});
	wait_for_workers_ਪੂਰਾ();
}

ਖਾਲੀ ParallelDispatchThreadPool::ਲੜੀਵਾਰ_ਕਾਮਾ(ਪੂਰਨ thread_ਅੰਕ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	ਜਦੋਂ (ਸੱਚ)
	{
		ਕਾਮਾ_wait_for_ਸ਼ੁਰੂ(thread_ਅੰਕ);
		ਜੇ (current_work == ਖਾਲੀ_ਸੂਚਕ)
			ਰੋਕੋ;
		ਪੂਰਨ ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 = ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
		(*current_work)({{thread_ਅੰਕ + 1}, ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 + 1});
		signal_ਕਾਮਾ_ਪੂਰਾ();
	}
	signal_ਕਾਮਾ_ਪੂਰਾ();
#ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)thread_ਅੰਕ;
	(ਖਾਲੀ)current_work;
#ਜੇ_ਅੰਤ
}

YOSYS_NAMESPACE_ਸਿਰਾ
