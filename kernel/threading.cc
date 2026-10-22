#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_ڌاڳا()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	إذا (v == خالي_اشارو)
		أرجع INT32_MAX;
	أرجع atoi(v);
}

ساكن صحيح get_max_ڌاڳا()
{
	ساكن صحيح max_ڌاڳا = init_max_ڌاڳا();
	أرجع max_ڌاڳا;
}

ساكن صحيح init_work_units_في_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	إذا (v == خالي_اشارو)
		أرجع 0;
	أرجع atoi(v);
}

ساكن صحيح get_work_units_في_thread_override()
{
	ساكن صحيح work_units_في_thread = init_work_units_في_thread_override();
	أرجع work_units_في_thread;
}

فراغ DeferredLogs::flush()
{
	لكل (خودڪار &m : logs)
		إذا (m.error)
			YOSYS_NAMESPACE_اڳياڙي log_error("%s", m.متن.c_str());
		وإلا
			YOSYS_NAMESPACE_اڳياڙي لاگرٿم("%s", m.متن.c_str());
}

صحيح ThreadPool::pool_سائيز(صحيح reserved_cores, صحيح max_ڪم_ڪندڙ_ڌاڳا)
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
	صحيح موجود_ڌاڳا = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_ڌاڳا());
	صحيح انگ_ڌاڳا = std::min(موجود_ڌاڳا - reserved_cores, max_ڪم_ڪندڙ_ڌاڳا);
	أرجع std::max(0, انگ_ڌاڳا);
#وإلا
	(فراغ)reserved_cores;
	(فراغ)max_ڪم_ڪندڙ_ڌاڳا;
	(فراغ)get_max_ڌاڳا();
	أرجع 0;
#جيڪڏهن_پورو
}

صحيح ThreadPool::work_pool_سائيز(صحيح reserved_cores, صحيح work_units, صحيح work_units_في_thread)
{
	صحيح work_units_في_thread_override = get_work_units_في_thread_override();
	إذا (work_units_في_thread_override > 0)
		work_units_في_thread = work_units_في_thread_override;
	أرجع pool_سائيز(reserved_cores, work_units / work_units_في_thread);
}

ThreadPool::ThreadPool(صحيح pool_سائيز, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
	ڌاڳا.reserve(pool_سائيز);
	لكل (صحيح i = 0; i < pool_سائيز; i++)
		ڌاڳا.emplace_پٺ([i, هذا]{ body(i); });
#وإلا
	(فراغ)pool_سائيز;
#جيڪڏهن_پورو
}

ThreadPool::~ThreadPool()
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
	لكل (خودڪار &t : ڌاڳا)
		t.join();
#جيڪڏهن_پورو
}

IntRange شي_range_for_ڪم_ڪندڙ(صحيح انگ_شيون, صحيح thread_انگ, صحيح انگ_ڌاڳا)
{
	إذا (انگ_ڌاڳا <= 1) {
		أرجع {0, انگ_شيون};
	}
	صحيح شيون_في_thread = انگ_شيون / انگ_ڌاڳا;
	صحيح extra_شيون = انگ_شيون % انگ_ڌاڳا;
	// The first `extra_items` threads get one extra item.
	صحيح شروعات = thread_انگ * شيون_في_thread + std::min(thread_انگ, extra_شيون);
	صحيح پڇاڙي = (thread_انگ + 1) * شيون_في_thread + std::min(thread_انگ + 1, extra_شيون);
	أرجع {شروعات, پڇاڙي};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_سائيز)
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
		: انگ_ڪم_ڪندڙ_ڌاڳا(std::max(1, pool_سائيز) - 1)
#وإلا
		: انگ_ڪم_ڪندڙ_ڌاڳا(0)
#جيڪڏهن_پورو
{
#جيڪڏهن_وصف_نه_ٿيل YOSYS_ENABLE_ڌاڳا
	(فراغ)pool_سائيز;
#جيڪڏهن_پورو
	main_منزل_workers_signal.resize(انگ_ڪم_ڪندڙ_ڌاڳا, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(انگ_ڪم_ڪندڙ_ڌاڳا, [هذا](صحيح thread_انگ){
		لاڳيتو_ڪم_ڪندڙ(thread_انگ);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (انگ_ڪم_ڪندڙ_ڌاڳا == 0)
		أرجع;
	current_work = خالي_اشارو;
	انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.store(انگ_ڪم_ڪندڙ_ڌاڳا, std::يادگيري_ترتيب_relaxed);
	signal_workers_شروعات();
	wait_for_workers_پورو();
}

فراغ ParallelDispatchThreadPool::لاڳيتو(std::function<فراغ(ثابت لاڳيتو_Ctx &)> work, صحيح max_ڌاڳا)
{
	Multithreading multithreading;
	صحيح انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 = انگ_ڌاڳا(max_ڌاڳا) - 1;
	إذا (انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.store(انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2, std::يادگيري_ترتيب_relaxed);
	current_work = &work;
	signal_workers_شروعات();
	work({{0}, انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 + 1});
	wait_for_workers_پورو();
}

فراغ ParallelDispatchThreadPool::لاڳيتو_ڪم_ڪندڙ(صحيح thread_انگ)
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
	طالما (سچ)
	{
		ڪم_ڪندڙ_wait_for_شروعات(thread_انگ);
		إذا (current_work == خالي_اشارو)
			اكسر;
		صحيح انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 = انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.ڀريو(std::يادگيري_ترتيب_relaxed);
		(*current_work)({{thread_انگ + 1}, انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 + 1});
		signal_ڪم_ڪندڙ_پورو();
	}
	signal_ڪم_ڪندڙ_پورو();
#وإلا
	(فراغ)thread_انگ;
	(فراغ)current_work;
#جيڪڏهن_پورو
}

YOSYS_NAMESPACE_پڇاڙي
