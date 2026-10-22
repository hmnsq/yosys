#شامل "kernel/yosys_common.h"
#شامل "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساکن صحیح init_max_دھاگے()
{
	ثابت حرف *v = getenv("YOSYS_MAX_THREADS");
	اگر (v == خالی_اشاریہ)
		واپس INT32_MAX;
	واپس atoi(v);
}

ساکن صحیح get_max_دھاگے()
{
	ساکن صحیح max_دھاگے = init_max_دھاگے();
	واپس max_دھاگے;
}

ساکن صحیح init_work_units_فی_thread_override()
{
	ثابت حرف *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	اگر (v == خالی_اشاریہ)
		واپس 0;
	واپس atoi(v);
}

ساکن صحیح get_work_units_فی_thread_override()
{
	ساکن صحیح work_units_فی_thread = init_work_units_فی_thread_override();
	واپس work_units_فی_thread;
}

خالی DeferredLogs::flush()
{
	برائے (خودکار &m : logs)
		اگر (m.error)
			YOSYS_NAMESPACE_سابقہ log_error("%s", m.متن.c_str());
		ورنہ
			YOSYS_NAMESPACE_سابقہ لوگارتھم("%s", m.متن.c_str());
}

صحیح ThreadPool::pool_سائز(صحیح reserved_cores, صحیح max_کارکن_دھاگے)
{
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	صحیح دستیاب_دھاگے = std::min<صحیح>(std::thread::hardware_concurrency(), get_max_دھاگے());
	صحیح عدد_دھاگے = std::min(دستیاب_دھاگے - reserved_cores, max_کارکن_دھاگے);
	واپس std::max(0, عدد_دھاگے);
#ورنہ
	(خالی)reserved_cores;
	(خالی)max_کارکن_دھاگے;
	(خالی)get_max_دھاگے();
	واپس 0;
#اگر_ختم
}

صحیح ThreadPool::work_pool_سائز(صحیح reserved_cores, صحیح work_units, صحیح work_units_فی_thread)
{
	صحیح work_units_فی_thread_override = get_work_units_فی_thread_override();
	اگر (work_units_فی_thread_override > 0)
		work_units_فی_thread = work_units_فی_thread_override;
	واپس pool_سائز(reserved_cores, work_units / work_units_فی_thread);
}

ThreadPool::ThreadPool(صحیح pool_سائز, std::function<خالی(صحیح)> b)
	: body(std::move(b))
{
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	دھاگے.reserve(pool_سائز);
	برائے (صحیح i = 0; i < pool_سائز; i++)
		دھاگے.emplace_عقب([i, یہ]{ body(i); });
#ورنہ
	(خالی)pool_سائز;
#اگر_ختم
}

ThreadPool::~ThreadPool()
{
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	برائے (خودکار &t : دھاگے)
		t.join();
#اگر_ختم
}

IntRange شے_range_for_کارکن(صحیح عدد_اشیاء, صحیح thread_عدد, صحیح عدد_دھاگے)
{
	اگر (عدد_دھاگے <= 1) {
		واپس {0, عدد_اشیاء};
	}
	صحیح اشیاء_فی_thread = عدد_اشیاء / عدد_دھاگے;
	صحیح extra_اشیاء = عدد_اشیاء % عدد_دھاگے;
	// The first `extra_items` threads get one extra item.
	صحیح آغاز = thread_عدد * اشیاء_فی_thread + std::min(thread_عدد, extra_اشیاء);
	صحیح انتہا = (thread_عدد + 1) * اشیاء_فی_thread + std::min(thread_عدد + 1, extra_اشیاء);
	واپس {آغاز, انتہا};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(صحیح pool_سائز)
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
		: عدد_کارکن_دھاگے(std::max(1, pool_سائز) - 1)
#ورنہ
		: عدد_کارکن_دھاگے(0)
#اگر_ختم
{
#اگر_غیر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	(خالی)pool_سائز;
#اگر_ختم
	main_مقصد_workers_signal.resize(عدد_کارکن_دھاگے, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(عدد_کارکن_دھاگے, [یہ](صحیح thread_عدد){
		تسلسل_کارکن(thread_عدد);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	اگر (عدد_کارکن_دھاگے == 0)
		واپس;
	current_work = خالی_اشاریہ;
	عدد_فعال_کارکن_دھاگے.store(عدد_کارکن_دھاگے, std::حافظہ_ترتیب_relaxed);
	signal_workers_آغاز();
	wait_for_workers_مکمل();
}

خالی ParallelDispatchThreadPool::تسلسل(std::function<خالی(ثابت تسلسل_Ctx &)> work, صحیح max_دھاگے)
{
	Multithreading multithreading;
	صحیح عدد_فعال_کارکن_دھاگے_2 = عدد_دھاگے(max_دھاگے) - 1;
	اگر (عدد_فعال_کارکن_دھاگے_2 == 0) {
		work({{0}, 1});
		واپس;
	}
	عدد_فعال_کارکن_دھاگے.store(عدد_فعال_کارکن_دھاگے_2, std::حافظہ_ترتیب_relaxed);
	current_work = &work;
	signal_workers_آغاز();
	work({{0}, عدد_فعال_کارکن_دھاگے_2 + 1});
	wait_for_workers_مکمل();
}

خالی ParallelDispatchThreadPool::تسلسل_کارکن(صحیح thread_عدد)
{
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	جب (سچ)
	{
		کارکن_wait_for_آغاز(thread_عدد);
		اگر (current_work == خالی_اشاریہ)
			توڑو;
		صحیح عدد_فعال_کارکن_دھاگے_2 = عدد_فعال_کارکن_دھاگے.بھرو(std::حافظہ_ترتیب_relaxed);
		(*current_work)({{thread_عدد + 1}, عدد_فعال_کارکن_دھاگے_2 + 1});
		signal_کارکن_مکمل();
	}
	signal_کارکن_مکمل();
#ورنہ
	(خالی)thread_عدد;
	(خالی)current_work;
#اگر_ختم
}

YOSYS_NAMESPACE_انتہا
