#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

స్థిర పూర్ణాంకం init_max_దారాలు()
{
	స్థిరరాశి అక్షరం *v = getenv("YOSYS_MAX_THREADS");
	ఒకవేళ (v == శూన్యసూచిక)
		తిరిగి INT32_MAX;
	తిరిగి atoi(v);
}

స్థిర పూర్ణాంకం get_max_దారాలు()
{
	స్థిర పూర్ణాంకం max_దారాలు = init_max_దారాలు();
	తిరిగి max_దారాలు;
}

స్థిర పూర్ణాంకం init_work_units_చొప్పున_thread_override()
{
	స్థిరరాశి అక్షరం *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	ఒకవేళ (v == శూన్యసూచిక)
		తిరిగి 0;
	తిరిగి atoi(v);
}

స్థిర పూర్ణాంకం get_work_units_చొప్పున_thread_override()
{
	స్థిర పూర్ణాంకం work_units_చొప్పున_thread = init_work_units_చొప్పున_thread_override();
	తిరిగి work_units_చొప్పున_thread;
}

శూన్య DeferredLogs::flush()
{
	కొరకు (స్వయంచాలక &m : logs)
		ఒకవేళ (m.error)
			YOSYS_NAMESPACE_ఉపసర్గ log_error("%s", m.పాఠ్యం.c_str());
		లేకపోతే
			YOSYS_NAMESPACE_ఉపసర్గ సంవర్గమానం("%s", m.పాఠ్యం.c_str());
}

పూర్ణాంకం ThreadPool::pool_కొలత(పూర్ణాంకం reserved_cores, పూర్ణాంకం max_కార్మికుడు_దారాలు)
{
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
	పూర్ణాంకం అందుబాటు_దారాలు = std::min<పూర్ణాంకం>(std::thread::hardware_concurrency(), get_max_దారాలు());
	పూర్ణాంకం అంకె_దారాలు = std::min(అందుబాటు_దారాలు - reserved_cores, max_కార్మికుడు_దారాలు);
	తిరిగి std::max(0, అంకె_దారాలు);
#లేకపోతే
	(శూన్య)reserved_cores;
	(శూన్య)max_కార్మికుడు_దారాలు;
	(శూన్య)get_max_దారాలు();
	తిరిగి 0;
#అయితే_ముగింపు
}

పూర్ణాంకం ThreadPool::work_pool_కొలత(పూర్ణాంకం reserved_cores, పూర్ణాంకం work_units, పూర్ణాంకం work_units_చొప్పున_thread)
{
	పూర్ణాంకం work_units_చొప్పున_thread_override = get_work_units_చొప్పున_thread_override();
	ఒకవేళ (work_units_చొప్పున_thread_override > 0)
		work_units_చొప్పున_thread = work_units_చొప్పున_thread_override;
	తిరిగి pool_కొలత(reserved_cores, work_units / work_units_చొప్పున_thread);
}

ThreadPool::ThreadPool(పూర్ణాంకం pool_కొలత, std::function<శూన్య(పూర్ణాంకం)> b)
	: body(std::move(b))
{
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
	దారాలు.reserve(pool_కొలత);
	కొరకు (పూర్ణాంకం i = 0; i < pool_కొలత; i++)
		దారాలు.emplace_వెనుక([i, ఇది]{ body(i); });
#లేకపోతే
	(శూన్య)pool_కొలత;
#అయితే_ముగింపు
}

ThreadPool::~ThreadPool()
{
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
	కొరకు (స్వయంచాలక &t : దారాలు)
		t.join();
#అయితే_ముగింపు
}

IntRange అంశం_range_for_కార్మికుడు(పూర్ణాంకం అంకె_వస్తువులు, పూర్ణాంకం thread_అంకె, పూర్ణాంకం అంకె_దారాలు)
{
	ఒకవేళ (అంకె_దారాలు <= 1) {
		తిరిగి {0, అంకె_వస్తువులు};
	}
	పూర్ణాంకం వస్తువులు_చొప్పున_thread = అంకె_వస్తువులు / అంకె_దారాలు;
	పూర్ణాంకం extra_వస్తువులు = అంకె_వస్తువులు % అంకె_దారాలు;
	// The first `extra_items` threads get one extra item.
	పూర్ణాంకం ప్రారంభం = thread_అంకె * వస్తువులు_చొప్పున_thread + std::min(thread_అంకె, extra_వస్తువులు);
	పూర్ణాంకం చివర = (thread_అంకె + 1) * వస్తువులు_చొప్పున_thread + std::min(thread_అంకె + 1, extra_వస్తువులు);
	తిరిగి {ప్రారంభం, చివర};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(పూర్ణాంకం pool_కొలత)
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
		: అంకె_కార్మికుడు_దారాలు(std::max(1, pool_కొలత) - 1)
#లేకపోతే
		: అంకె_కార్మికుడు_దారాలు(0)
#అయితే_ముగింపు
{
#నిర్వచితం_కాకపోతే YOSYS_ENABLE_దారాలు
	(శూన్య)pool_కొలత;
#అయితే_ముగింపు
	main_గమ్యం_workers_signal.resize(అంకె_కార్మికుడు_దారాలు, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(అంకె_కార్మికుడు_దారాలు, [ఇది](పూర్ణాంకం thread_అంకె){
		నిరంతరం_కార్మికుడు(thread_అంకె);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	ఒకవేళ (అంకె_కార్మికుడు_దారాలు == 0)
		తిరిగి;
	current_work = శూన్యసూచిక;
	అంకె_చురుకైన_కార్మికుడు_దారాలు.store(అంకె_కార్మికుడు_దారాలు, std::జ్ఞాపకం_క్రమం_relaxed);
	signal_workers_ప్రారంభం();
	wait_for_workers_పూర్తి();
}

శూన్య ParallelDispatchThreadPool::నిరంతరం(std::function<శూన్య(స్థిరరాశి నిరంతరం_Ctx &)> work, పూర్ణాంకం max_దారాలు)
{
	Multithreading multithreading;
	పూర్ణాంకం అంకె_చురుకైన_కార్మికుడు_దారాలు_2 = అంకె_దారాలు(max_దారాలు) - 1;
	ఒకవేళ (అంకె_చురుకైన_కార్మికుడు_దారాలు_2 == 0) {
		work({{0}, 1});
		తిరిగి;
	}
	అంకె_చురుకైన_కార్మికుడు_దారాలు.store(అంకె_చురుకైన_కార్మికుడు_దారాలు_2, std::జ్ఞాపకం_క్రమం_relaxed);
	current_work = &work;
	signal_workers_ప్రారంభం();
	work({{0}, అంకె_చురుకైన_కార్మికుడు_దారాలు_2 + 1});
	wait_for_workers_పూర్తి();
}

శూన్య ParallelDispatchThreadPool::నిరంతరం_కార్మికుడు(పూర్ణాంకం thread_అంకె)
{
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
	అయితే (నిజం)
	{
		కార్మికుడు_wait_for_ప్రారంభం(thread_అంకె);
		ఒకవేళ (current_work == శూన్యసూచిక)
			ఆపు;
		పూర్ణాంకం అంకె_చురుకైన_కార్మికుడు_దారాలు_2 = అంకె_చురుకైన_కార్మికుడు_దారాలు.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed);
		(*current_work)({{thread_అంకె + 1}, అంకె_చురుకైన_కార్మికుడు_దారాలు_2 + 1});
		signal_కార్మికుడు_పూర్తి();
	}
	signal_కార్మికుడు_పూర్తి();
#లేకపోతే
	(శూన్య)thread_అంకె;
	(శూన్య)current_work;
#అయితే_ముగింపు
}

YOSYS_NAMESPACE_చివర
