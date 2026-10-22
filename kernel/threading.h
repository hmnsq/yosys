#అన్తర్భావ <deque>

#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/log.h"
#అన్తర్భావ "kernel/utils.h"

#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
#అన్తర్భావ <condition_variable>
#అన్తర్భావ <mutex>
#అన్తర్భావ <thread>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే YOSYS_THREADING_H
#నిర్వచించు YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_తాళం = std::unique_తాళం<Mutex>;
using తాళం_Guard = std::తాళం_guard<Mutex>;
#లేకపోతే
నిర్మాణం Mutex {
	శూన్య తాళం() {}
	శూన్య unlock() {}
	bool try_తాళం() { తిరిగి true; }
};
నిర్మాణం CondVar {
	మూస <class L> శూన్య wait(L &) {}
	మూస <class L, class P> శూన్య wait(L &, P) {}
	శూన్య notify_one() {}
	శూన్య notify_all() {}
};
నిర్మాణం Unique_తాళం {
	Unique_తాళం(Mutex &) {}
};
నిర్మాణం తాళం_Guard {
	తాళం_Guard(Mutex &) {}
};
#అయితే_ముగింపు

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
మూస <typename T>
class Concurrent_వరుస
{
బహిరంగ:
	Concurrent_వరుస(పూర్ణాంకం సామర్థ్యం = గరిష్ఠ_పూర్ణాంకం)
		: సామర్థ్యం(సామర్థ్యం) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	శూన్య నెట్టు_వెనుక(T t)
	{
		Unique_తాళం తాళం(mutex);
		not_నిండిన_condition.wait(తాళం, [this] { తిరిగి static_cast<పూర్ణాంకం>(contents.కొలత()) < సామర్థ్యం; });
		ఒకవేళ (contents.ఖాళీలు())
			not_ఖాళీలు_condition.notify_one();
		log_assert(!closed);
		contents.నెట్టు_వెనుక(std::move(t));
		ఒకవేళ (static_cast<పూర్ణాంకం>(contents.కొలత()) < సామర్థ్యం)
			not_నిండిన_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	శూన్య close()
	{
		Unique_తాళం తాళం(mutex);
		not_ఖాళీలు_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> తీయి_front()
	{
		తిరిగి తీయి_front_అంతర్గత(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_తీయి_front()
	{
		తిరిగి తీయి_front_అంతర్గత(false);
	}
అంతరంగ:
	std::optional<T> తీయి_front_అంతర్గత(bool wait)
	{
		Unique_తాళం తాళం(mutex);
		ఒకవేళ (wait) {
			not_ఖాళీలు_condition.wait(తాళం, [this] { తిరిగి !contents.ఖాళీలు() || closed; });
		}
		ఒకవేళ (contents.ఖాళీలు())
			తిరిగి std::nullopt;
		ఒకవేళ (static_cast<పూర్ణాంకం>(contents.కొలత()) == సామర్థ్యం)
			not_నిండిన_condition.notify_one();
		T ఫలితం = std::move(contents.front());
		contents.తీయి_front();
		ఒకవేళ (!contents.ఖాళీలు())
			not_ఖాళీలు_condition.notify_one();
		తిరిగి std::move(ఫలితం);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_నిండిన_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_ఖాళీలు_condition;
	std::deque<T> contents;
	పూర్ణాంకం సామర్థ్యం;
	bool closed = false;
};

class DeferredLogs
{
బహిరంగ:
	మూస <typename... వాదనలు_3>
	శూన్య సంవర్గమానం(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, వాదనలు_3... వాదనలు_2)
	{
		logs.నెట్టు_వెనుక({fmt.format(వాదనలు_2...), false});
	}
	మూస <typename... వాదనలు_3>
	శూన్య log_error(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, వాదనలు_3... వాదనలు_2)
	{
		logs.నెట్టు_వెనుక({fmt.format(వాదనలు_2...), true});
	}
	శూన్య flush();
అంతరంగ:
	నిర్మాణం సందేశం_3
	{
		std::string పాఠ్యం;
		bool error;
	};
	std::సదిశ<సందేశం_3> logs;
};

class ThreadPool
{
బహిరంగ:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	స్థిర పూర్ణాంకం pool_కొలత(పూర్ణాంకం reserved_cores, పూర్ణాంకం max_కార్మికుడు_దారాలు);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	స్థిర పూర్ణాంకం work_pool_కొలత(పూర్ణాంకం reserved_cores, పూర్ణాంకం work_units, పూర్ణాంకం work_units_చొప్పున_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(పూర్ణాంకం pool_కొలత, std::function<శూన్య(పూర్ణాంకం)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	పూర్ణాంకం అంకె_దారాలు() స్థిరరాశి
	{
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
		తిరిగి దారాలు.కొలత();
#లేకపోతే
		తిరిగి 0;
#అయితే_ముగింపు
	}
అంతరంగ:
	std::function<శూన్య(పూర్ణాంకం)> body;
#నిర్వచితమైతే YOSYS_ENABLE_దారాలు
	std::సదిశ<std::thread> దారాలు;
#అయితే_ముగింపు
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange అంశం_range_for_కార్మికుడు(పూర్ణాంకం అంకె_వస్తువులు, పూర్ణాంకం thread_అంకె, పూర్ణాంకం అంకె_దారాలు);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
నిర్మాణం Thread_సూచీ {
	పూర్ణాంకం thread_అంకె;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
బహిరంగ:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(పూర్ణాంకం pool_కొలత);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	నిర్మాణం నిరంతరం_Ctx : బహిరంగ Thread_సూచీ {
		పూర్ణాంకం అంకె_దారాలు;
		IntRange అంశం_range(పూర్ణాంకం అంకె_వస్తువులు) స్థిరరాశి {
			తిరిగి అంశం_range_for_కార్మికుడు(అంకె_వస్తువులు, thread_అంకె, అంకె_దారాలు);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	బహిరంగ:
		Subpool(ParallelDispatchThreadPool &మాతృ, పూర్ణాంకం max_దారాలు)
				: మాతృ(మాతృ), max_దారాలు(max_దారాలు) {}
		// Returns the number of threads that will be used when calling `run()`.
		పూర్ణాంకం అంకె_దారాలు() స్థిరరాశి {
			తిరిగి మాతృ.అంకె_దారాలు(max_దారాలు);
		}
		శూన్య నిరంతరం(std::function<శూన్య(స్థిరరాశి నిరంతరం_Ctx &)> work) {
			మాతృ.నిరంతరం(std::move(work), max_దారాలు);
		}
		ParallelDispatchThreadPool &thread_pool() { తిరిగి మాతృ; }
	అంతరంగ:
		ParallelDispatchThreadPool &మాతృ;
		పూర్ణాంకం max_దారాలు;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	శూన్య నిరంతరం(std::function<శూన్య(స్థిరరాశి నిరంతరం_Ctx &)> work) {
		నిరంతరం(std::move(work), గరిష్ఠ_పూర్ణాంకం);
	}

	// Returns the number of threads that will be used when calling `run()`.
	పూర్ణాంకం అంకె_దారాలు() స్థిరరాశి {
		తిరిగి అంకె_దారాలు(గరిష్ఠ_పూర్ణాంకం);
	}
అంతరంగ:
	మిత్ర class Subpool;

	శూన్య నిరంతరం(std::function<శూన్య(స్థిరరాశి నిరంతరం_Ctx &)> work, పూర్ణాంకం max_దారాలు);
	పూర్ణాంకం అంకె_దారాలు(పూర్ణాంకం max_దారాలు) స్థిరరాశి {
		తిరిగి std::min(అంకె_కార్మికుడు_దారాలు + 1, std::max(1, max_దారాలు));
	}
	శూన్య నిరంతరం_కార్మికుడు(పూర్ణాంకం thread_అంకె);

	std::function<శూన్య(స్థిరరాశి నిరంతరం_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	పూర్ణాంకం అంకె_కార్మికుడు_దారాలు;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<పూర్ణాంకం> అంకె_చురుకైన_కార్మికుడు_దారాలు = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_గమ్యం_workers_signal_mutex;
	CondVar main_గమ్యం_workers_signal_cv;
	std::సదిశ<uint8_t> main_గమ్యం_workers_signal;
	శూన్య signal_workers_ప్రారంభం() {
		Unique_తాళం తాళం(main_గమ్యం_workers_signal_mutex);
		పూర్ణాంకం అంకె_చురుకైన_కార్మికుడు_దారాలు_2 = అంకె_చురుకైన_కార్మికుడు_దారాలు.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed);
		std::fill(main_గమ్యం_workers_signal.begin(), main_గమ్యం_workers_signal.begin() + అంకె_చురుకైన_కార్మికుడు_దారాలు_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_గమ్యం_workers_signal_cv.notify_all();
	}
	శూన్య కార్మికుడు_wait_for_ప్రారంభం(పూర్ణాంకం thread_అంకె) {
		Unique_తాళం తాళం(main_గమ్యం_workers_signal_mutex);
		main_గమ్యం_workers_signal_cv.wait(తాళం, [this, thread_అంకె] { తిరిగి main_గమ్యం_workers_signal[thread_అంకె] > 0; });
		main_గమ్యం_workers_signal[thread_అంకె] = 0;
	}

	std::atomic<పూర్ణాంకం> పూర్తి_workers = 0;
	Mutex workers_గమ్యం_main_signal_mutex;
	CondVar workers_గమ్యం_main_signal_cv;
	శూన్య signal_కార్మికుడు_పూర్తి() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		పూర్ణాంకం అంకె_చురుకైన_కార్మికుడు_దారాలు_2 = అంకె_చురుకైన_కార్మికుడు_దారాలు.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed);
		పూర్ణాంకం d = పూర్తి_workers.fetch_కూడు(1, std::జ్ఞాపకం_క్రమం_release);
		ఒకవేళ (d + 1 == అంకె_చురుకైన_కార్మికుడు_దారాలు_2) {
			Unique_తాళం తాళం(workers_గమ్యం_main_signal_mutex);
			workers_గమ్యం_main_signal_cv.notify_all();
		}
	}
	శూన్య wait_for_workers_పూర్తి() {
		Unique_తాళం తాళం(workers_గమ్యం_main_signal_mutex);
		workers_గమ్యం_main_signal_cv.wait(తాళం, [this] {
			పూర్ణాంకం అంకె_చురుకైన_కార్మికుడు_దారాలు_2 = అంకె_చురుకైన_కార్మికుడు_దారాలు.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed);
			తిరిగి పూర్తి_workers.ఎక్కించు(std::జ్ఞాపకం_క్రమం_acquire) == అంకె_చురుకైన_కార్మికుడు_దారాలు_2;
		});
		పూర్తి_workers.store(0, std::జ్ఞాపకం_క్రమం_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

మూస <class T>
class Concurrent_దొంతర
{
బహిరంగ:
	శూన్య నెట్టు_వెనుక(T &&t) {
		తాళం_Guard తాళం(mutex);
		contents.నెట్టు_వెనుక(std::move(t));
	}
	std::optional<T> try_తీయి_వెనుక() {
		తాళం_Guard తాళం(mutex);
		ఒకవేళ (contents.ఖాళీలు())
			తిరిగి std::nullopt;
		T ఫలితం = std::move(contents.వెనుక());
		contents.తీయి_వెనుక();
		తిరిగి ఫలితం;
	}
అంతరంగ:
	Mutex mutex;
	std::సదిశ<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
మూస <typename T>
class Sharded_సదిశ {
బహిరంగ:
	Sharded_సదిశ(స్థిరరాశి ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.అంకె_దారాలు());
	}
	Sharded_సదిశ(స్థిరరాశి ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.అంకె_దారాలు());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	శూన్య చొప్పించు(స్థిరరాశి Thread_సూచీ &thread, T విలువ) {
		buckets[thread.thread_అంకె].emplace_వెనుక(std::move(విలువ));
	}

	bool ఖాళీలు() స్థిరరాశి {
		కొరకు (స్థిరరాశి std::సదిశ<T> &పాత్ర : buckets)
			ఒకవేళ (!పాత్ర.ఖాళీలు())
				తిరిగి false;
		తిరిగి true;
	}

	using Buckets = std::సదిశ<std::సదిశ<T>>;
	class iterator {
	బహిరంగ:
		iterator(typename Buckets::iterator పాత్ర_it, typename Buckets::iterator పాత్ర_చివర)
			: పాత్ర_it(std::move(పాత్ర_it)), పాత్ర_చివర(std::move(పాత్ర_చివర)) {
			ఒకవేళ (పాత్ర_it != పాత్ర_చివర)
				inner_it = పాత్ర_it->begin();
			normalize();
		}
		T& క్రియాకారకం*() స్థిరరాశి { తిరిగి *inner_it.విలువ(); }
		iterator &క్రియాకారకం++() {
			++*inner_it;
			normalize();
			తిరిగి *this;
		}
		bool క్రియాకారకం!=(స్థిరరాశి iterator &other) స్థిరరాశి {
			తిరిగి పాత్ర_it != other.పాత్ర_it || inner_it != other.inner_it;
		}
	అంతరంగ:
		శూన్య normalize() {
			ఒకవేళ (పాత్ర_it == పాత్ర_చివర)
				తిరిగి;
			అయితే (inner_it == పాత్ర_it->చివర()) {
				++పాత్ర_it;
				ఒకవేళ (పాత్ర_it == పాత్ర_చివర) {
					inner_it.reset();
					తిరిగి;
				}
				inner_it = పాత్ర_it->begin();
			}
		}
		std::optional<typename std::సదిశ<T>::iterator> inner_it;
		typename Buckets::iterator పాత్ర_it;
		typename Buckets::iterator పాత్ర_చివర;
	};
	iterator begin() { తిరిగి iterator(buckets.begin(), buckets.చివర()); }
	iterator చివర() { తిరిగి iterator(buckets.చివర(), buckets.చివర()); }
అంతరంగ:
	శూన్య init(పూర్ణాంకం అంకె_దారాలు) {
		buckets.resize(అంకె_దారాలు);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
మూస <typename V>
నిర్మాణం సమితి_Collision_Handler {
	శూన్య క్రియాకారకం()(typename V::Accumulated &, typename V::Accumulated &) స్థిరరాశి {}
};

// A hashtable that can be efficiently built in parallel and then looked up concurrently.
// `V` is the type of elements that will be added to the hashtable. It must have a
// member type `Accumulated` representing the combination of multiple `V` elements. This
// can be the same as `V`, but for example `V` could contain a Wire* and `V::Accumulated`
// could contain a `pool<Wire*>`. `KeyEquality` is a class containing an `operator()` that
// returns true of two `V` elements have equal keys.
// `CollisionHandler` is used to reduce two `V::Accumulated` values into a single value.
//
// To use this, first construct a `Builder` and fill it in (in parallel), then construct
// a `ShardedHashtable` from the `Builder`.
మూస <typename V, typename కీలకం_Equality, typename CollisionHandler>
class ShardedHashtable {
బహిరంగ:
	// A combination of a `V` and its hash value.
	నిర్మాణం విలువ_3 {
		విలువ_3(V విలువ, గుర్తులేని పూర్ణాంకం hash) : విలువ(std::move(విలువ)), hash(hash) {}
		విలువ_3(విలువ_3 &&) = అప్రమేయ;
		విలువ_3(స్థిరరాశి విలువ_3 &) = delete;
		విలువ_3 &క్రియాకారకం=(స్థిరరాశి విలువ_3 &) = delete;
		V విలువ;
		గుర్తులేని పూర్ణాంకం hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	నిర్మాణం Accumulated_విలువ {
		Accumulated_విలువ(typename V::Accumulated విలువ, గుర్తులేని పూర్ణాంకం hash) : విలువ(std::move(విలువ)), hash(hash) {}
		Accumulated_విలువ(Accumulated_విలువ &&) = అప్రమేయ;
#ఒకవేళ defined(_MSC_VER)
		Accumulated_విలువ(స్థిరరాశి Accumulated_విలువ &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_విలువ &క్రియాకారకం=(స్థిరరాశి Accumulated_విలువ &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			తిరిగి *this;
		}
#లేకపోతే
		Accumulated_విలువ(స్థిరరాశి Accumulated_విలువ &) = delete;
		Accumulated_విలువ &క్రియాకారకం=(స్థిరరాశి Accumulated_విలువ &) = delete;
#అయితే_ముగింపు
		typename V::Accumulated విలువ;
		గుర్తులేని పూర్ణాంకం hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	నిర్మాణం Accumulated_విలువ_Equality {
		కీలకం_Equality inner;
		Accumulated_విలువ_Equality(స్థిరరాశి కీలకం_Equality &inner) : inner(inner) {}
		bool క్రియాకారకం()(స్థిరరాశి Accumulated_విలువ &v1, స్థిరరాశి Accumulated_విలువ &v2) స్థిరరాశి {
			తిరిగి inner(v1.విలువ, v2.విలువ);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	నిర్మాణం Accumulated_విలువ_Hash_Op {
		కొలత_t క్రియాకారకం()(స్థిరరాశి Accumulated_విలువ &v) స్థిరరాశి {
			తిరిగి static_cast<కొలత_t>(v.hash);
		}
	};
	using Shard = std::unordered_సమితి<Accumulated_విలువ, Accumulated_విలువ_Hash_Op, Accumulated_విలువ_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	బహిరంగ:
		Builder(స్థిరరాశి ParallelDispatchThreadPool &thread_pool, కీలకం_Equality equality = కీలకం_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.అంకె_దారాలు(), std::move(equality));
		}
		Builder(స్థిరరాశి ParallelDispatchThreadPool::Subpool &thread_pool, కీలకం_Equality equality = కీలకం_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.అంకె_దారాలు(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		శూన్య చొప్పించు(స్థిరరాశి Thread_సూచీ &thread, విలువ_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::సదిశ<std::సదిశ<విలువ_3>> &buckets = all_buckets[thread.thread_అంకె];
			కొలత_t పాత్ర = static_cast<కొలత_t>(v.hash) % buckets.కొలత();
			buckets[పాత్ర].emplace_వెనుక(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		శూన్య process(స్థిరరాశి Thread_సూచీ &thread) {
			పూర్ణాంకం కొలత = 0;
			కొరకు (std::సదిశ<std::సదిశ<విలువ_3>> &buckets : all_buckets)
				కొలత += Get_కొలత(buckets[thread.thread_అంకె]);
			Shard &shard = shards[thread.thread_అంకె];
			shard.reserve(కొలత);
			కొరకు (std::సదిశ<std::సదిశ<విలువ_3>> &buckets : all_buckets) {
				కొరకు (విలువ_3 &విలువ : buckets[thread.thread_అంకె])
					accumulate(విలువ, shard);
				// Free as much memory as we can during the parallel phase.
				std::సదిశ<విలువ_3>().మార్పిడి(buckets[thread.thread_అంకె]);
			}
		}
	అంతరంగ:
		మిత్ర class ShardedHashtable<V, కీలకం_Equality, CollisionHandler>;
		శూన్య accumulate(విలువ_3 &విలువ, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_విలువ accumulated_విలువ{std::move(విలువ.విలువ), విలువ.hash};
			స్వయంచాలక [it, inserted] = shard.చొప్పించు(std::move(accumulated_విలువ));
			ఒకవేళ (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->విలువ), accumulated_విలువ.విలువ);
		}
		శూన్య init(పూర్ణాంకం అంకె_దారాలు, కీలకం_Equality equality) {
			all_buckets.resize(అంకె_దారాలు);
			కొరకు (std::సదిశ<std::సదిశ<విలువ_3>> &buckets : all_buckets)
				buckets.resize(అంకె_దారాలు);
			కొరకు (పూర్ణాంకం i = 0; i < అంకె_దారాలు; ++i)
				shards.emplace_వెనుక(0, Accumulated_విలువ_Hash_Op(), Accumulated_విలువ_Equality(equality));
		}
		స్థిరరాశి CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::సదిశ<std::సదిశ<std::సదిశ<విలువ_3>>> all_buckets;
		std::సదిశ<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		కొరకు (std::సదిశ<std::సదిశ<విలువ_3>> &buckets : builder.all_buckets)
			కొరకు (std::సదిశ<విలువ_3> &పాత్ర : buckets)
				log_assert(పాత్ర.ఖాళీలు());
		// Free memory.
		std::సదిశ<std::సదిశ<std::సదిశ<విలువ_3>>>().మార్పిడి(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = అప్రమేయ;
	ShardedHashtable() {}

	ShardedHashtable &క్రియాకారకం=(ShardedHashtable &&other) = అప్రమేయ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	స్థిరరాశి typename V::Accumulated *కనుగొను(స్థిరరాశి Accumulated_విలువ &v) స్థిరరాశి {
		కొలత_t అంకె_shards = shards.కొలత();
		ఒకవేళ (అంకె_shards == 0)
			తిరిగి nullptr;
		కొలత_t shard = static_cast<కొలత_t>(v.hash) % అంకె_shards;
		స్వయంచాలక it = shards[shard].కనుగొను(v);
		ఒకవేళ (it == shards[shard].చివర())
			తిరిగి nullptr;
		తిరిగి &it->విలువ;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	శూన్య చొప్పించు(Accumulated_విలువ v) {
		కొలత_t అంకె_shards = shards.కొలత();
		ఒకవేళ (అంకె_shards == 0)
			తిరిగి;
		కొలత_t shard = static_cast<కొలత_t>(v.hash) % అంకె_shards;
		shards[shard].చొప్పించు(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	శూన్య clear(స్థిరరాశి Thread_సూచీ &shard) {
		Accumulated_విలువ_Equality equality = shards[shard.thread_అంకె].కీలకం_eq();
		shards[shard.thread_అంకె] = Shard(0, Accumulated_విలువ_Hash_Op(), equality);
	}
అంతరంగ:
	std::సదిశ<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
మూస <typename T>
class Concurrent_Work_వరుస {
బహిరంగ:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_వరుస(పూర్ణాంకం అంకె_దారాలు, పూర్ణాంకం batch_కొలత = 100)
		: batch_కొలత(batch_కొలత), thread_స్థితులు(అంకె_దారాలు) {}
	పూర్ణాంకం అంకె_దారాలు() స్థిరరాశి { తిరిగి Get_కొలత(thread_స్థితులు); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	శూన్య నెట్టు(స్థిరరాశి Thread_సూచీ &thread, T work) {
		ThreadState &thread_state = thread_స్థితులు[thread.thread_అంకె];
		thread_state.తదుపరి_batch.emplace_వెనుక(std::move(work));
		ఒకవేళ (Get_కొలత(thread_state.తదుపరి_batch) < batch_కొలత)
			తిరిగి;
		bool was_ఖాళీలు;
		{
			Unique_తాళం తాళం(thread_state.batches_తాళం);
			was_ఖాళీలు = thread_state.batches.ఖాళీలు();
			thread_state.batches.నెట్టు_వెనుక(std::move(thread_state.తదుపరి_batch));
		}
		ఒకవేళ (was_ఖాళీలు) {
			Unique_తాళం తాళం(waiters_తాళం);
			ఒకవేళ (అంకె_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::సదిశ<T> తీయి_batch(స్థిరరాశి Thread_సూచీ &thread) {
		ThreadState &thread_state = thread_స్థితులు[thread.thread_అంకె];
		ఒకవేళ (!thread_state.తదుపరి_batch.ఖాళీలు())
			తిరిగి std::move(thread_state.తదుపరి_batch);
		// Empty our own work queue first.
		{
			Unique_తాళం తాళం(thread_state.batches_తాళం);
			ఒకవేళ (!thread_state.batches.ఖాళీలు()) {
				std::సదిశ<T> batch = std::move(thread_state.batches.వెనుక());
				thread_state.batches.తీయి_వెనుక();
				తిరిగి batch;
			}
		}
		// From here on in this function, our work queue is empty.
		అయితే (true) {
			std::సదిశ<T> batch = try_steal(thread);
			ఒకవేళ (!batch.ఖాళీలు()) {
				తిరిగి std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_తాళం తాళం(waiters_తాళం);
			++అంకె_waiters;
			ఒకవేళ (అంకె_waiters == అంకె_దారాలు()) {
				waiters_cv.notify_all();
				తిరిగి {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(తాళం);
			ఒకవేళ (అంకె_waiters == అంకె_దారాలు())
				తిరిగి {};
			--అంకె_waiters;
		}
	}
అంతరంగ:
	std::సదిశ<T> try_steal(స్థిరరాశి Thread_సూచీ &thread) {
		కొరకు (పూర్ణాంకం i = 1; i < అంకె_దారాలు(); i++) {
			పూర్ణాంకం other_thread_అంకె = (thread.thread_అంకె + i) % అంకె_దారాలు();
			ThreadState &other_thread_state = thread_స్థితులు[other_thread_అంకె];
			Unique_తాళం తాళం(other_thread_state.batches_తాళం);
			ఒకవేళ (!other_thread_state.batches.ఖాళీలు()) {
				std::సదిశ<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.తీయి_front();
				తిరిగి batch;
			}
		}
		తిరిగి {};
	}

	పూర్ణాంకం batch_కొలత;

	నిర్మాణం ThreadState {
		// Entirely thread-local.
		std::సదిశ<T> తదుపరి_batch;

		Mutex batches_తాళం;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::సదిశ<T>> batches;
	};
	std::సదిశ<ThreadState> thread_స్థితులు;

	Mutex waiters_తాళం;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	పూర్ణాంకం అంకె_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
బహిరంగ:
	MonotonicFlag() : విలువ(false) {}
	bool ఎక్కించు() స్థిరరాశి { తిరిగి విలువ.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed); }
	శూన్య సమితి() { విలువ.store(true, std::జ్ఞాపకం_క్రమం_relaxed); }
	bool సమితి_and_return_old() {
		తిరిగి విలువ.exchange(true, std::జ్ఞాపకం_క్రమం_relaxed);
	}
అంతరంగ:
	std::atomic<bool> విలువ;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు // YOSYS_THREADING_H
