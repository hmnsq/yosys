#ضمن "kernel/yosys_common.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساكن صحيح init_max_دھاگے()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	إذا (v == خالی_سوچک)
		أرجع INT32_MAX;
	أرجع atoi(v);
}

ساكن صحيح get_max_دھاگے()
{
	ساكن صحيح max_دھاگے = init_max_دھاگے();
	أرجع max_دھاگے;
}

ساكن صحيح init_work_units_فی_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	إذا (v == خالی_سوچک)
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
	لكل (خودکار &m : logs)
		إذا (m.error)
			YOSYS_NAMESPACE_اگیتر log_error("%s", m.متن.c_str());
		وإلا
			YOSYS_NAMESPACE_اگیتر لوگارتھم("%s", m.متن.c_str());
}

صحيح ThreadPool::pool_ناپ(صحيح reserved_cores, صحيح max_کامہ_دھاگے)
{
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
	صحيح دستیاب_دھاگے = std::min<صحيح>(std::thread::hardware_concurrency(), get_max_دھاگے());
	صحيح عدد_دھاگے = std::min(دستیاب_دھاگے - reserved_cores, max_کامہ_دھاگے);
	أرجع std::max(0, عدد_دھاگے);
#وإلا
	(فراغ)reserved_cores;
	(فراغ)max_کامہ_دھاگے;
	(فراغ)get_max_دھاگے();
	أرجع 0;
#جے_انت
}

صحيح ThreadPool::work_pool_ناپ(صحيح reserved_cores, صحيح work_units, صحيح work_units_فی_thread)
{
	صحيح work_units_فی_thread_override = get_work_units_فی_thread_override();
	إذا (work_units_فی_thread_override > 0)
		work_units_فی_thread = work_units_فی_thread_override;
	أرجع pool_ناپ(reserved_cores, work_units / work_units_فی_thread);
}

ThreadPool::ThreadPool(صحيح pool_ناپ, std::function<فراغ(صحيح)> b)
	: body(std::move(b))
{
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
	دھاگے.reserve(pool_ناپ);
	لكل (صحيح i = 0; i < pool_ناپ; i++)
		دھاگے.emplace_پٹھ([i, هذا]{ body(i); });
#وإلا
	(فراغ)pool_ناپ;
#جے_انت
}

ThreadPool::~ThreadPool()
{
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
	لكل (خودکار &t : دھاگے)
		t.join();
#جے_انت
}

IntRange شے_range_for_کامہ(صحيح عدد_اشیاء, صحيح thread_عدد, صحيح عدد_دھاگے)
{
	إذا (عدد_دھاگے <= 1) {
		أرجع {0, عدد_اشیاء};
	}
	صحيح اشیاء_فی_thread = عدد_اشیاء / عدد_دھاگے;
	صحيح extra_اشیاء = عدد_اشیاء % عدد_دھاگے;
	// The first `extra_items` threads get one extra item.
	صحيح شروع = thread_عدد * اشیاء_فی_thread + std::min(thread_عدد, extra_اشیاء);
	صحيح سرا = (thread_عدد + 1) * اشیاء_فی_thread + std::min(thread_عدد + 1, extra_اشیاء);
	أرجع {شروع, سرا};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحيح pool_ناپ)
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
		: عدد_کامہ_دھاگے(std::max(1, pool_ناپ) - 1)
#وإلا
		: عدد_کامہ_دھاگے(0)
#جے_انت
{
#جے_بنا_تعریف YOSYS_ENABLE_دھاگے
	(فراغ)pool_ناپ;
#جے_انت
	main_منزل_workers_signal.resize(عدد_کامہ_دھاگے, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(عدد_کامہ_دھاگے, [هذا](صحيح thread_عدد){
		لگاتار_کامہ(thread_عدد);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	إذا (عدد_کامہ_دھاگے == 0)
		أرجع;
	current_work = خالی_سوچک;
	عدد_سرگرم_کامہ_دھاگے.store(عدد_کامہ_دھاگے, std::حافظہ_ترتیب_relaxed);
	signal_workers_شروع();
	wait_for_workers_پورا();
}

فراغ ParallelDispatchThreadPool::لگاتار(std::function<فراغ(ثابت لگاتار_Ctx &)> work, صحيح max_دھاگے)
{
	Multithreading multithreading;
	صحيح عدد_سرگرم_کامہ_دھاگے_2 = عدد_دھاگے(max_دھاگے) - 1;
	إذا (عدد_سرگرم_کامہ_دھاگے_2 == 0) {
		work({{0}, 1});
		أرجع;
	}
	عدد_سرگرم_کامہ_دھاگے.store(عدد_سرگرم_کامہ_دھاگے_2, std::حافظہ_ترتیب_relaxed);
	current_work = &work;
	signal_workers_شروع();
	work({{0}, عدد_سرگرم_کامہ_دھاگے_2 + 1});
	wait_for_workers_پورا();
}

فراغ ParallelDispatchThreadPool::لگاتار_کامہ(صحيح thread_عدد)
{
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
	طالما (سچ)
	{
		کامہ_wait_for_شروع(thread_عدد);
		إذا (current_work == خالی_سوچک)
			اكسر;
		صحيح عدد_سرگرم_کامہ_دھاگے_2 = عدد_سرگرم_کامہ_دھاگے.لدو(std::حافظہ_ترتیب_relaxed);
		(*current_work)({{thread_عدد + 1}, عدد_سرگرم_کامہ_دھاگے_2 + 1});
		signal_کامہ_پورا();
	}
	signal_کامہ_پورا();
#وإلا
	(فراغ)thread_عدد;
	(فراغ)current_work;
#جے_انت
}

YOSYS_NAMESPACE_سرا
