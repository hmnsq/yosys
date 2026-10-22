#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_نخها()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	إذا (v == اشاره_تهی)
		أرجع INT32_MAX;
	أرجع atoi(v);
}

ساكن صحيح get_max_نخها()
{
	ساكن صحيح max_نخها = init_max_نخها();
	أرجع max_نخها;
}

ساكن صحيح init_work_units_در_هر_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	إذا (v == اشاره_تهی)
		أرجع 0;
	أرجع atoi(v);
}

ساكن صحيح get_work_units_در_هر_thread_override()
{
	ساكن صحيح work_units_در_هر_thread = init_work_units_در_هر_thread_override();
	أرجع work_units_در_هر_thread;
}

فراغ DeferredLogs::flush()
{
	لكل (خودکار &m : logs)
		إذا (m.error)
			YOSYS_NAMESPACE_پیشوند log_error("%s", m.متن.c_str());
		وإلا
			YOSYS_NAMESPACE_پیشوند لگاریتم("%s", m.متن.c_str());
}

صحيح ThreadPool::pool_اندازه(صحيح reserved_cores, صحيح max_کارگر_نخها)
{
#اگر_تعریف_شده YOSYS_ENABLE_نخها
	صحيح موجود_نخها = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_نخها());
	صحيح رقم_نخها = std::min(موجود_نخها - reserved_cores, max_کارگر_نخها);
	أرجع std::max(0, رقم_نخها);
#وإلا
	(فراغ)reserved_cores;
	(فراغ)max_کارگر_نخها;
	(فراغ)get_max_نخها();
	أرجع 0;
#پایان_اگر
}

صحيح ThreadPool::work_pool_اندازه(صحيح reserved_cores, صحيح work_units, صحيح work_units_در_هر_thread)
{
	صحيح work_units_در_هر_thread_override = get_work_units_در_هر_thread_override();
	إذا (work_units_در_هر_thread_override > 0)
		work_units_در_هر_thread = work_units_در_هر_thread_override;
	أرجع pool_اندازه(reserved_cores, work_units / work_units_در_هر_thread);
}

ThreadPool::ThreadPool(صحيح pool_اندازه, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#اگر_تعریف_شده YOSYS_ENABLE_نخها
	نخها.reserve(pool_اندازه);
	لكل (صحيح i = 0; i < pool_اندازه; i++)
		نخها.emplace_پشت([i, هذا]{ body(i); });
#وإلا
	(فراغ)pool_اندازه;
#پایان_اگر
}

ThreadPool::~ThreadPool()
{
#اگر_تعریف_شده YOSYS_ENABLE_نخها
	لكل (خودکار &t : نخها)
		t.join();
#پایان_اگر
}

IntRange قلم_range_for_کارگر(صحيح رقم_اقلام, صحيح thread_رقم, صحيح رقم_نخها)
{
	إذا (رقم_نخها <= 1) {
		أرجع {0, رقم_اقلام};
	}
	صحيح اقلام_در_هر_thread = رقم_اقلام / رقم_نخها;
	صحيح extra_اقلام = رقم_اقلام % رقم_نخها;
	// The first `extra_items` threads get one extra item.
	صحيح شروع = thread_رقم * اقلام_در_هر_thread + std::min(thread_رقم, extra_اقلام);
	صحيح انتها = (thread_رقم + 1) * اقلام_در_هر_thread + std::min(thread_رقم + 1, extra_اقلام);
	أرجع {شروع, انتها};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_اندازه)
#اگر_تعریف_شده YOSYS_ENABLE_نخها
		: رقم_کارگر_نخها(std::max(1, pool_اندازه) - 1)
#وإلا
		: رقم_کارگر_نخها(0)
#پایان_اگر
{
#اگر_تعریف_نشده YOSYS_ENABLE_نخها
	(فراغ)pool_اندازه;
#پایان_اگر
	main_مقصد_workers_signal.resize(رقم_کارگر_نخها, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(رقم_کارگر_نخها, [هذا](صحيح thread_رقم){
		پیاپی_کارگر(thread_رقم);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (رقم_کارگر_نخها == 0)
		أرجع;
	current_work = اشاره_تهی;
	رقم_فعال_کارگر_نخها.store(رقم_کارگر_نخها, std::حافظه_ترتیب_relaxed);
	signal_workers_شروع();
	wait_for_workers_انجامشده();
}

فراغ ParallelDispatchThreadPool::پیاپی(std::function<فراغ(ثابت پیاپی_Ctx &)> work, صحيح max_نخها)
{
	Multithreading multithreading;
	صحيح رقم_فعال_کارگر_نخها_2 = رقم_نخها(max_نخها) - 1;
	إذا (رقم_فعال_کارگر_نخها_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	رقم_فعال_کارگر_نخها.store(رقم_فعال_کارگر_نخها_2, std::حافظه_ترتیب_relaxed);
	current_work = &work;
	signal_workers_شروع();
	work({{0}, رقم_فعال_کارگر_نخها_2 + 1});
	wait_for_workers_انجامشده();
}

فراغ ParallelDispatchThreadPool::پیاپی_کارگر(صحيح thread_رقم)
{
#اگر_تعریف_شده YOSYS_ENABLE_نخها
	طالما (درست)
	{
		کارگر_wait_for_شروع(thread_رقم);
		إذا (current_work == اشاره_تهی)
			اكسر;
		صحيح رقم_فعال_کارگر_نخها_2 = رقم_فعال_کارگر_نخها.بارگیری(std::حافظه_ترتیب_relaxed);
		(*current_work)({{thread_رقم + 1}, رقم_فعال_کارگر_نخها_2 + 1});
		signal_کارگر_انجامشده();
	}
	signal_کارگر_انجامشده();
#وإلا
	(فراغ)thread_رقم;
	(فراغ)current_work;
#پایان_اگر
}

YOSYS_NAMESPACE_انتها
