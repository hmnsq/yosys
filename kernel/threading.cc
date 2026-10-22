#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

स्थिर पूर्णाङ्क init_max_तन्तवः()
{
	ध्रुव वर्ण *v = getenv("YOSYS_MAX_THREADS");
	यदि (v == शून्यसूचकः)
		प्रत्यागम INT32_MAX;
	प्रत्यागम atoi(v);
}

स्थिर पूर्णाङ्क get_max_तन्तवः()
{
	स्थिर पूर्णाङ्क max_तन्तवः = init_max_तन्तवः();
	प्रत्यागम max_तन्तवः;
}

स्थिर पूर्णाङ्क init_work_units_प्रत्येकम्_thread_override()
{
	ध्रुव वर्ण *v = getenv("YOSYS_WORK_UNITS_PER_THREAD");
	यदि (v == शून्यसूचकः)
		प्रत्यागम 0;
	प्रत्यागम atoi(v);
}

स्थिर पूर्णाङ्क get_work_units_प्रत्येकम्_thread_override()
{
	स्थिर पूर्णाङ्क work_units_प्रत्येकम्_thread = init_work_units_प्रत्येकम्_thread_override();
	प्रत्यागम work_units_प्रत्येकम्_thread;
}

शून्य DeferredLogs::flush()
{
	प्रति (स्वचालित &m : logs)
		यदि (m.error)
			YOSYS_NAMESPACE_उपसर्गः log_error("%s", m.text.c_str());
		अन्यथा
			YOSYS_NAMESPACE_उपसर्गः लघुगणकः("%s", m.text.c_str());
}

पूर्णाङ्क ThreadPool::pool_आकार(पूर्णाङ्क reserved_cores, पूर्णाङ्क max_कार्यकर्ता_तन्तवः)
{
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
	पूर्णाङ्क उपलब्धम्_तन्तवः = std::min<पूर्णाङ्क>(std::thread::hardware_concurrency(), get_max_तन्तवः());
	पूर्णाङ्क अङ्कः_तन्तवः = std::min(उपलब्धम्_तन्तवः - reserved_cores, max_कार्यकर्ता_तन्तवः);
	प्रत्यागम std::max(0, अङ्कः_तन्तवः);
#अन्यथा
	(शून्य)reserved_cores;
	(शून्य)max_कार्यकर्ता_तन्तवः;
	(शून्य)get_max_तन्तवः();
	प्रत्यागम 0;
#यदि_अन्तः
}

पूर्णाङ्क ThreadPool::work_pool_आकार(पूर्णाङ्क reserved_cores, पूर्णाङ्क work_units, पूर्णाङ्क work_units_प्रत्येकम्_thread)
{
	पूर्णाङ्क work_units_प्रत्येकम्_thread_override = get_work_units_प्रत्येकम्_thread_override();
	यदि (work_units_प्रत्येकम्_thread_override > 0)
		work_units_प्रत्येकम्_thread = work_units_प्रत्येकम्_thread_override;
	प्रत्यागम pool_आकार(reserved_cores, work_units / work_units_प्रत्येकम्_thread);
}

ThreadPool::ThreadPool(पूर्णाङ्क pool_आकार, std::function<शून्य(पूर्णाङ्क)> b)
	: body(std::move(b))
{
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
	तन्तवः.reserve(pool_आकार);
	प्रति (पूर्णाङ्क i = 0; i < pool_आकार; i++)
		तन्तवः.emplace_पृष्ठभागः([i, एतत्]{ body(i); });
#अन्यथा
	(शून्य)pool_आकार;
#यदि_अन्तः
}

ThreadPool::~ThreadPool()
{
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
	प्रति (स्वचालित &t : तन्तवः)
		t.join();
#यदि_अन्तः
}

IntRange item_range_for_कार्यकर्ता(पूर्णाङ्क अङ्कः_वस्तूनि, पूर्णाङ्क thread_अङ्कः, पूर्णाङ्क अङ्कः_तन्तवः)
{
	यदि (अङ्कः_तन्तवः <= 1) {
		प्रत्यागम {0, अङ्कः_वस्तूनि};
	}
	पूर्णाङ्क वस्तूनि_प्रत्येकम्_thread = अङ्कः_वस्तूनि / अङ्कः_तन्तवः;
	पूर्णाङ्क extra_वस्तूनि = अङ्कः_वस्तूनि % अङ्कः_तन्तवः;
	// The first `extra_items` threads get one extra item.
	पूर्णाङ्क start = thread_अङ्कः * वस्तूनि_प्रत्येकम्_thread + std::min(thread_अङ्कः, extra_वस्तूनि);
	पूर्णाङ्क अन्तः = (thread_अङ्कः + 1) * वस्तूनि_प्रत्येकम्_thread + std::min(thread_अङ्कः + 1, extra_वस्तूनि);
	प्रत्यागम {start, अन्तः};
}

ParallelDispatchThreadPool::ParallelDispatchThreadPool(पूर्णाङ्क pool_आकार)
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
		: अङ्कः_कार्यकर्ता_तन्तवः(std::max(1, pool_आकार) - 1)
#अन्यथा
		: अङ्कः_कार्यकर्ता_तन्तवः(0)
#यदि_अन्तः
{
#यदि_अपरिभाषितम् YOSYS_ENABLE_तन्तवः
	(शून्य)pool_आकार;
#यदि_अन्तः
	main_गन्तव्यम्_workers_signal.resize(अङ्कः_कार्यकर्ता_तन्तवः, 0);
	// Don't start the threads until we've constructed all our data members.
	thread_pool = std::make_unique<ThreadPool>(अङ्कः_कार्यकर्ता_तन्तवः, [एतत्](पूर्णाङ्क thread_अङ्कः){
		सततम्_कार्यकर्ता(thread_अङ्कः);
	});
}

ParallelDispatchThreadPool::~ParallelDispatchThreadPool()
{
	यदि (अङ्कः_कार्यकर्ता_तन्तवः == 0)
		प्रत्यागम;
	current_work = शून्यसूचकः;
	अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.store(अङ्कः_कार्यकर्ता_तन्तवः, std::स्मृतिः_क्रमः_relaxed);
	signal_workers_start();
	wait_for_workers_समाप्तम्();
}

शून्य ParallelDispatchThreadPool::सततम्(std::function<शून्य(ध्रुव सततम्_Ctx &)> work, पूर्णाङ्क max_तन्तवः)
{
	Multithreading multithreading;
	पूर्णाङ्क अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 = अङ्कः_तन्तवः(max_तन्तवः) - 1;
	यदि (अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 == 0) {
		work({{0}, 1});
		प्रत्यागम;
	}
	अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.store(अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2, std::स्मृतिः_क्रमः_relaxed);
	current_work = &work;
	signal_workers_start();
	work({{0}, अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 + 1});
	wait_for_workers_समाप्तम्();
}

शून्य ParallelDispatchThreadPool::सततम्_कार्यकर्ता(पूर्णाङ्क thread_अङ्कः)
{
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
	यावत् (सत्य)
	{
		कार्यकर्ता_wait_for_start(thread_अङ्कः);
		यदि (current_work == शून्यसूचकः)
			भङ्ग;
		पूर्णाङ्क अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 = अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.आरोपय(std::स्मृतिः_क्रमः_relaxed);
		(*current_work)({{thread_अङ्कः + 1}, अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 + 1});
		signal_कार्यकर्ता_समाप्तम्();
	}
	signal_कार्यकर्ता_समाप्तम्();
#अन्यथा
	(शून्य)thread_अङ्कः;
	(शून्य)current_work;
#यदि_अन्तः
}

YOSYS_NAMESPACE_अन्तः
