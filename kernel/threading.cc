#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ init_max_ಎಳೆಗಳು()
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = getenv("YOSYS_MAX_THREADS");
	ಆದರೆ (v == ಶೂನ್ಯಸೂಚಕ)
		ಹಿಂತಿರುಗಿ INT32_MAX;
	ಹಿಂತಿರುಗಿ atoi(v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_max_ಎಳೆಗಳು()
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು = init_max_ಎಳೆಗಳು();
	ಹಿಂತಿರುಗಿ max_ಎಳೆಗಳು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ init_work_units_ಪ್ರತಿ_thread_override()
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ಆದರೆ (v == ಶೂನ್ಯಸೂಚಕ)
		ಹಿಂತಿರುಗಿ 0;
	ಹಿಂತಿರುಗಿ atoi(v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_work_units_ಪ್ರತಿ_thread_override()
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ work_units_ಪ್ರತಿ_thread = init_work_units_ಪ್ರತಿ_thread_override();
	ಹಿಂತಿರುಗಿ work_units_ಪ್ರತಿ_thread;
}

ಶೂನ್ಯ DeferredLogs::flush()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &m : logs)
		ಆದರೆ (m.error)
			YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ log_error("%s", m.ಪಠ್ಯ.c_str());
		ಇಲ್ಲದಿದ್ದರೆ
			YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ ಲಘುಗಣಕ("%s", m.ಪಠ್ಯ.c_str());
}

ಪೂರ್ಣಾಂಕ ThreadPool::pool_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ reserved_cores, ಪೂರ್ಣಾಂಕ max_ಕೆಲಸಗಾರ_ಎಳೆಗಳು)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	ಪೂರ್ಣಾಂಕ ಲಭ್ಯ_ಎಳೆಗಳು = std::min<ಪೂರ್ಣಾಂಕ>(std::thread::hardware_concurrency(), get_max_ಎಳೆಗಳು());
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು = std::min(ಲಭ್ಯ_ಎಳೆಗಳು - reserved_cores, max_ಕೆಲಸಗಾರ_ಎಳೆಗಳು);
	ಹಿಂತಿರುಗಿ std::max(0, ಅಂಕೆ_ಎಳೆಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)reserved_cores;
	(ಶೂನ್ಯ)max_ಕೆಲಸಗಾರ_ಎಳೆಗಳು;
	(ಶೂನ್ಯ)get_max_ಎಳೆಗಳು();
	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ ThreadPool::work_pool_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ reserved_cores, ಪೂರ್ಣಾಂಕ work_units, ಪೂರ್ಣಾಂಕ work_units_ಪ್ರತಿ_thread)
{
	ಪೂರ್ಣಾಂಕ work_units_ಪ್ರತಿ_thread_override = get_work_units_ಪ್ರತಿ_thread_override();
	ಆದರೆ (work_units_ಪ್ರತಿ_thread_override > 0)
		work_units_ಪ್ರತಿ_thread = work_units_ಪ್ರತಿ_thread_override;
	ಹಿಂತಿರುಗಿ pool_ಗಾತ್ರ(reserved_cores, work_units / work_units_ಪ್ರತಿ_thread);
}

ThreadPool::ThreadPool(ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ, std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಂಕ)> b)
	: body(std::move(b))
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	ಎಳೆಗಳು.reserve(pool_ಗಾತ್ರ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pool_ಗಾತ್ರ; i++)
		ಎಳೆಗಳು.emplace_ಹಿಂಭಾಗ([i, ಇದು]{ body(i); });
#ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)pool_ಗಾತ್ರ;
#ಆದರೆ_ಅಂತ್ಯ
}

ThreadPool::~ThreadPool()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &t : ಎಳೆಗಳು)
		t.join();
#ಆದರೆ_ಅಂತ್ಯ
}

IntRange ಅಂಶ_range_for_ಕೆಲಸಗಾರ(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ವಸ್ತುಗಳು, ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು)
{
	ಆದರೆ (ಅಂಕೆ_ಎಳೆಗಳು <= 1) {
		ಹಿಂತಿರುಗಿ {0, ಅಂಕೆ_ವಸ್ತುಗಳು};
	}
	ಪೂರ್ಣಾಂಕ ವಸ್ತುಗಳು_ಪ್ರತಿ_thread = ಅಂಕೆ_ವಸ್ತುಗಳು / ಅಂಕೆ_ಎಳೆಗಳು;
	ಪೂರ್ಣಾಂಕ extra_ವಸ್ತುಗಳು = ಅಂಕೆ_ವಸ್ತುಗಳು % ಅಂಕೆ_ಎಳೆಗಳು;
	// The first `extra_items` threads get one extra item.
	ಪೂರ್ಣಾಂಕ ಆರಂಭ = thread_ಅಂಕೆ * ವಸ್ತುಗಳು_ಪ್ರತಿ_thread + std::min(thread_ಅಂಕೆ, extra_ವಸ್ತುಗಳು);
	ಪೂರ್ಣಾಂಕ ಕೊನೆ = (thread_ಅಂಕೆ + 1) * ವಸ್ತುಗಳು_ಪ್ರತಿ_thread + std::min(thread_ಅಂಕೆ + 1, extra_ವಸ್ತುಗಳು);
	ಹಿಂತಿರುಗಿ {ಆರಂಭ, ಕೊನೆ};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
		: ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು(std::max(1, pool_ಗಾತ್ರ) - 1)
#ಇಲ್ಲದಿದ್ದರೆ
		: ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು(0)
#ಆದರೆ_ಅಂತ್ಯ
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	(ಶೂನ್ಯ)pool_ಗಾತ್ರ;
#ಆದರೆ_ಅಂತ್ಯ
	main_ಗಮ್ಯ_workers_signal.resize(ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು, [ಇದು](ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ){
		ಸತತ_ಕೆಲಸಗಾರ(thread_ಅಂಕೆ);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ಆದರೆ (ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು == 0)
		ಹಿಂತಿರುಗಿ;
	current_work = ಶೂನ್ಯಸೂಚಕ;
	ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.store(ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
	signal_workers_ಆರಂಭ();
	wait_for_workers_ಮುಗಿದ();
}

ಶೂನ್ಯ ParallelDispatchThreadPool::ಸತತ(std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ ಸತತ_Ctx &)> work, ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು)
{
	Multithreading multithreading;
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 = ಅಂಕೆ_ಎಳೆಗಳು(max_ಎಳೆಗಳು) - 1;
	ಆದರೆ (ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 == 0) {
		work({{0}, 1});
		ಹಿಂತಿರುಗಿ;
	}
	ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.store(ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
	current_work = &work;
	signal_workers_ಆರಂಭ();
	work({{0}, ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 + 1});
	wait_for_workers_ಮುಗಿದ();
}

ಶೂನ್ಯ ParallelDispatchThreadPool::ಸತತ_ಕೆಲಸಗಾರ(ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	ಆಗ (ನಿಜ)
	{
		ಕೆಲಸಗಾರ_wait_for_ಆರಂಭ(thread_ಅಂಕೆ);
		ಆದರೆ (current_work == ಶೂನ್ಯಸೂಚಕ)
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 = ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
		(*current_work)({{thread_ಅಂಕೆ + 1}, ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 + 1});
		signal_ಕೆಲಸಗಾರ_ಮುಗಿದ();
	}
	signal_ಕೆಲಸಗಾರ_ಮುಗಿದ();
#ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)thread_ಅಂಕೆ;
	(ಶೂನ್ಯ)current_work;
#ಆದರೆ_ಅಂತ್ಯ
}

YOSYS_NAMESPACE_ಕೊನೆ
