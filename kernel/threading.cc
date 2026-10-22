#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_تارونه()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	إذا (v == تش_اشاره)
		أرجع INT32_MAX;
	أرجع atoi(v);
}

ساكن صحيح get_max_تارونه()
{
	ساكن صحيح max_تارونه = init_max_تارونه();
	أرجع max_تارونه;
}

ساكن صحيح init_work_units_فی_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	إذا (v == تش_اشاره)
		أرجع 0;
	أرجع atoi(v);
}

ساكن صحيح get_work_units_فی_thread_override()
{
	ساكن صحيح work_units_فی_thread = init_work_units_فی_thread_override();
	أرجع work_units_فی_thread;
}

فراغ DeferredLogs::flush()
{
	لكل (خپلکاری &m : logs)
		إذا (m.error)
			YOSYS_NAMESPACE_مختاړی log_error("%s", m.متن.c_str());
		وإلا
			YOSYS_NAMESPACE_مختاړی لوګاریتم("%s", m.متن.c_str());
}

صحيح ThreadPool::pool_اندازه(صحيح reserved_cores, صحيح max_کارګر_تارونه)
{
#که_تعریف_شوی YOSYS_ENABLE_تارونه
	صحيح شته_تارونه = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_تارونه());
	صحيح عدد_تارونه = std::min(شته_تارونه - reserved_cores, max_کارګر_تارونه);
	أرجع std::max(0, عدد_تارونه);
#وإلا
	(فراغ)reserved_cores;
	(فراغ)max_کارګر_تارونه;
	(فراغ)get_max_تارونه();
	أرجع 0;
#د_که_پای
}

صحيح ThreadPool::work_pool_اندازه(صحيح reserved_cores, صحيح work_units, صحيح work_units_فی_thread)
{
	صحيح work_units_فی_thread_override = get_work_units_فی_thread_override();
	إذا (work_units_فی_thread_override > 0)
		work_units_فی_thread = work_units_فی_thread_override;
	أرجع pool_اندازه(reserved_cores, work_units / work_units_فی_thread);
}

ThreadPool::ThreadPool(صحيح pool_اندازه, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#که_تعریف_شوی YOSYS_ENABLE_تارونه
	تارونه.reserve(pool_اندازه);
	لكل (صحيح i = 0; i < pool_اندازه; i++)
		تارونه.emplace_شا([i, هذا]{ body(i); });
#وإلا
	(فراغ)pool_اندازه;
#د_که_پای
}

ThreadPool::~ThreadPool()
{
#که_تعریف_شوی YOSYS_ENABLE_تارونه
	لكل (خپلکاری &t : تارونه)
		t.join();
#د_که_پای
}

IntRange توکی_range_for_کارګر(صحيح عدد_توکي, صحيح thread_عدد, صحيح عدد_تارونه)
{
	إذا (عدد_تارونه <= 1) {
		أرجع {0, عدد_توکي};
	}
	صحيح توکي_فی_thread = عدد_توکي / عدد_تارونه;
	صحيح extra_توکي = عدد_توکي % عدد_تارونه;
	// The first `extra_items` threads get one extra item.
	صحيح پیل = thread_عدد * توکي_فی_thread + std::min(thread_عدد, extra_توکي);
	صحيح پای_ټکی = (thread_عدد + 1) * توکي_فی_thread + std::min(thread_عدد + 1, extra_توکي);
	أرجع {پیل, پای_ټکی};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_اندازه)
#که_تعریف_شوی YOSYS_ENABLE_تارونه
		: عدد_کارګر_تارونه(std::max(1, pool_اندازه) - 1)
#وإلا
		: عدد_کارګر_تارونه(0)
#د_که_پای
{
#که_نه_تعریف_شوی YOSYS_ENABLE_تارونه
	(فراغ)pool_اندازه;
#د_که_پای
	main_منزل_workers_signal.resize(عدد_کارګر_تارونه, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(عدد_کارګر_تارونه, [هذا](صحيح thread_عدد){
		پرلپسې_کارګر(thread_عدد);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (عدد_کارګر_تارونه == 0)
		أرجع;
	current_work = تش_اشاره;
	عدد_فعال_کارګر_تارونه.store(عدد_کارګر_تارونه, std::حافظه_ترتیب_relaxed);
	signal_workers_پیل();
	wait_for_workers_بشپړ();
}

فراغ ParallelDispatchThreadPool::پرلپسې(std::function<فراغ(ثابت پرلپسې_Ctx &)> work, صحيح max_تارونه)
{
	Multithreading multithreading;
	صحيح عدد_فعال_کارګر_تارونه_2 = عدد_تارونه(max_تارونه) - 1;
	إذا (عدد_فعال_کارګر_تارونه_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	عدد_فعال_کارګر_تارونه.store(عدد_فعال_کارګر_تارونه_2, std::حافظه_ترتیب_relaxed);
	current_work = &work;
	signal_workers_پیل();
	work({{0}, عدد_فعال_کارګر_تارونه_2 + 1});
	wait_for_workers_بشپړ();
}

فراغ ParallelDispatchThreadPool::پرلپسې_کارګر(صحيح thread_عدد)
{
#که_تعریف_شوی YOSYS_ENABLE_تارونه
	طالما (ریښتیا)
	{
		کارګر_wait_for_پیل(thread_عدد);
		إذا (current_work == تش_اشاره)
			اكسر;
		صحيح عدد_فعال_کارګر_تارونه_2 = عدد_فعال_کارګر_تارونه.بارول(std::حافظه_ترتیب_relaxed);
		(*current_work)({{thread_عدد + 1}, عدد_فعال_کارګر_تارونه_2 + 1});
		signal_کارګر_بشپړ();
	}
	signal_کارګر_بشپړ();
#وإلا
	(فراغ)thread_عدد;
	(فراغ)current_work;
#د_که_پای
}

YOSYS_NAMESPACE_پای_ټکی
