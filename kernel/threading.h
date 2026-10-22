#अन्तर्भाव <deque>

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/utils.h"

#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
#अन्तर्भाव <condition_variable>
#अन्तर्भाव <mutex>
#अन्तर्भाव <thread>
#यदि_अन्तः

#यदि_अपरिभाषितम् YOSYS_THREADING_H
#परिभाषय YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_तालकम् = std::unique_तालकम्<Mutex>;
using तालकम्_Guard = std::तालकम्_guard<Mutex>;
#अन्यथा
संरचना Mutex {
	शून्य तालकम्() {}
	शून्य unlock() {}
	bool try_तालकम्() { प्रत्यागम true; }
};
संरचना CondVar {
	प्रतिरूपम् <class L> शून्य प्रतीक्षस्व(L &) {}
	प्रतिरूपम् <class L, class P> शून्य प्रतीक्षस्व(L &, P) {}
	शून्य notify_one() {}
	शून्य notify_all() {}
};
संरचना Unique_तालकम् {
	Unique_तालकम्(Mutex &) {}
};
संरचना तालकम्_Guard {
	तालकम्_Guard(Mutex &) {}
};
#यदि_अन्तः

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
प्रतिरूपम् <typename T>
class ConcurrentQueue
{
सार्वजनिक:
	ConcurrentQueue(पूर्णाङ्क क्षमता = अधिकतम_पूर्णाङ्कः)
		: क्षमता(क्षमता) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	शून्य प्रक्षिप_पृष्ठभागः(T t)
	{
		Unique_तालकम् तालकम्(mutex);
		not_पूर्णम्_condition.प्रतीक्षस्व(तालकम्, [this] { प्रत्यागम static_cast<पूर्णाङ्क>(contents.आकार()) < क्षमता; });
		यदि (contents.रिक्तानि())
			not_रिक्तानि_condition.notify_one();
		log_assert(!closed);
		contents.प्रक्षिप_पृष्ठभागः(std::move(t));
		यदि (static_cast<पूर्णाङ्क>(contents.आकार()) < क्षमता)
			not_पूर्णम्_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	शून्य close()
	{
		Unique_तालकम् तालकम्(mutex);
		not_रिक्तानि_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> निष्कासय_front()
	{
		प्रत्यागम निष्कासय_front_आन्तरिकम्(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_निष्कासय_front()
	{
		प्रत्यागम निष्कासय_front_आन्तरिकम्(false);
	}
गुप्त:
	std::optional<T> निष्कासय_front_आन्तरिकम्(bool प्रतीक्षस्व)
	{
		Unique_तालकम् तालकम्(mutex);
		यदि (प्रतीक्षस्व) {
			not_रिक्तानि_condition.प्रतीक्षस्व(तालकम्, [this] { प्रत्यागम !contents.रिक्तानि() || closed; });
		}
		यदि (contents.रिक्तानि())
			प्रत्यागम std::nullopt;
		यदि (static_cast<पूर्णाङ्क>(contents.आकार()) == क्षमता)
			not_पूर्णम्_condition.notify_one();
		T परिणाम = std::move(contents.front());
		contents.निष्कासय_front();
		यदि (!contents.रिक्तानि())
			not_रिक्तानि_condition.notify_one();
		प्रत्यागम std::move(परिणाम);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_पूर्णम्_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_रिक्तानि_condition;
	std::deque<T> contents;
	पूर्णाङ्क क्षमता;
	bool closed = false;
};

class DeferredLogs
{
सार्वजनिक:
	प्रतिरूपम् <typename... तर्काः_3>
	शून्य लघुगणकः(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, तर्काः_3... तर्काः_2)
	{
		logs.प्रक्षिप_पृष्ठभागः({fmt.format(तर्काः_2...), false});
	}
	प्रतिरूपम् <typename... तर्काः_3>
	शून्य log_error(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, तर्काः_3... तर्काः_2)
	{
		logs.प्रक्षिप_पृष्ठभागः({fmt.format(तर्काः_2...), true});
	}
	शून्य flush();
गुप्त:
	संरचना सन्देशः_3
	{
		std::string text;
		bool error;
	};
	std::सदिशम्<सन्देशः_3> logs;
};

class ThreadPool
{
सार्वजनिक:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	स्थिर पूर्णाङ्क pool_आकार(पूर्णाङ्क reserved_cores, पूर्णाङ्क max_कार्यकर्ता_तन्तवः);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	स्थिर पूर्णाङ्क work_pool_आकार(पूर्णाङ्क reserved_cores, पूर्णाङ्क work_units, पूर्णाङ्क work_units_प्रत्येकम्_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(पूर्णाङ्क pool_आकार, std::function<शून्य(पूर्णाङ्क)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	पूर्णाङ्क अङ्कः_तन्तवः() ध्रुव
	{
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
		प्रत्यागम तन्तवः.आकार();
#अन्यथा
		प्रत्यागम 0;
#यदि_अन्तः
	}
गुप्त:
	std::function<शून्य(पूर्णाङ्क)> body;
#यदि_परिभाषितम् YOSYS_ENABLE_तन्तवः
	std::सदिशम्<std::thread> तन्तवः;
#यदि_अन्तः
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_कार्यकर्ता(पूर्णाङ्क अङ्कः_वस्तूनि, पूर्णाङ्क thread_अङ्कः, पूर्णाङ्क अङ्कः_तन्तवः);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
संरचना Thread_सूचकांक {
	पूर्णाङ्क thread_अङ्कः;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
सार्वजनिक:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(पूर्णाङ्क pool_आकार);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	संरचना सततम्_Ctx : सार्वजनिक Thread_सूचकांक {
		पूर्णाङ्क अङ्कः_तन्तवः;
		IntRange item_range(पूर्णाङ्क अङ्कः_वस्तूनि) ध्रुव {
			प्रत्यागम item_range_for_कार्यकर्ता(अङ्कः_वस्तूनि, thread_अङ्कः, अङ्कः_तन्तवः);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	सार्वजनिक:
		Subpool(ParallelDispatchThreadPool &parent, पूर्णाङ्क max_तन्तवः)
				: parent(parent), max_तन्तवः(max_तन्तवः) {}
		// Returns the number of threads that will be used when calling `run()`.
		पूर्णाङ्क अङ्कः_तन्तवः() ध्रुव {
			प्रत्यागम parent.अङ्कः_तन्तवः(max_तन्तवः);
		}
		शून्य सततम्(std::function<शून्य(ध्रुव सततम्_Ctx &)> work) {
			parent.सततम्(std::move(work), max_तन्तवः);
		}
		ParallelDispatchThreadPool &thread_pool() { प्रत्यागम parent; }
	गुप्त:
		ParallelDispatchThreadPool &parent;
		पूर्णाङ्क max_तन्तवः;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	शून्य सततम्(std::function<शून्य(ध्रुव सततम्_Ctx &)> work) {
		सततम्(std::move(work), अधिकतम_पूर्णाङ्कः);
	}

	// Returns the number of threads that will be used when calling `run()`.
	पूर्णाङ्क अङ्कः_तन्तवः() ध्रुव {
		प्रत्यागम अङ्कः_तन्तवः(अधिकतम_पूर्णाङ्कः);
	}
गुप्त:
	मित्रम् class Subpool;

	शून्य सततम्(std::function<शून्य(ध्रुव सततम्_Ctx &)> work, पूर्णाङ्क max_तन्तवः);
	पूर्णाङ्क अङ्कः_तन्तवः(पूर्णाङ्क max_तन्तवः) ध्रुव {
		प्रत्यागम std::min(अङ्कः_कार्यकर्ता_तन्तवः + 1, std::max(1, max_तन्तवः));
	}
	शून्य सततम्_कार्यकर्ता(पूर्णाङ्क thread_अङ्कः);

	std::function<शून्य(ध्रुव सततम्_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	पूर्णाङ्क अङ्कः_कार्यकर्ता_तन्तवः;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<पूर्णाङ्क> अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_गन्तव्यम्_workers_signal_mutex;
	CondVar main_गन्तव्यम्_workers_signal_cv;
	std::सदिशम्<uint8_t> main_गन्तव्यम्_workers_signal;
	शून्य signal_workers_start() {
		Unique_तालकम् तालकम्(main_गन्तव्यम्_workers_signal_mutex);
		पूर्णाङ्क अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 = अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.आरोपय(std::स्मृतिः_क्रमः_relaxed);
		std::fill(main_गन्तव्यम्_workers_signal.begin(), main_गन्तव्यम्_workers_signal.begin() + अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_गन्तव्यम्_workers_signal_cv.notify_all();
	}
	शून्य कार्यकर्ता_wait_for_start(पूर्णाङ्क thread_अङ्कः) {
		Unique_तालकम् तालकम्(main_गन्तव्यम्_workers_signal_mutex);
		main_गन्तव्यम्_workers_signal_cv.प्रतीक्षस्व(तालकम्, [this, thread_अङ्कः] { प्रत्यागम main_गन्तव्यम्_workers_signal[thread_अङ्कः] > 0; });
		main_गन्तव्यम्_workers_signal[thread_अङ्कः] = 0;
	}

	std::atomic<पूर्णाङ्क> समाप्तम्_workers = 0;
	Mutex workers_गन्तव्यम्_main_signal_mutex;
	CondVar workers_गन्तव्यम्_main_signal_cv;
	शून्य signal_कार्यकर्ता_समाप्तम्() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		पूर्णाङ्क अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 = अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.आरोपय(std::स्मृतिः_क्रमः_relaxed);
		पूर्णाङ्क d = समाप्तम्_workers.fetch_सङ्कलय(1, std::स्मृतिः_क्रमः_release);
		यदि (d + 1 == अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2) {
			Unique_तालकम् तालकम्(workers_गन्तव्यम्_main_signal_mutex);
			workers_गन्तव्यम्_main_signal_cv.notify_all();
		}
	}
	शून्य wait_for_workers_समाप्तम्() {
		Unique_तालकम् तालकम्(workers_गन्तव्यम्_main_signal_mutex);
		workers_गन्तव्यम्_main_signal_cv.प्रतीक्षस्व(तालकम्, [this] {
			पूर्णाङ्क अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2 = अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः.आरोपय(std::स्मृतिः_क्रमः_relaxed);
			प्रत्यागम समाप्तम्_workers.आरोपय(std::स्मृतिः_क्रमः_acquire) == अङ्कः_सक्रिय_कार्यकर्ता_तन्तवः_2;
		});
		समाप्तम्_workers.store(0, std::स्मृतिः_क्रमः_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

प्रतिरूपम् <class T>
class Concurrent_राशिः
{
सार्वजनिक:
	शून्य प्रक्षिप_पृष्ठभागः(T &&t) {
		तालकम्_Guard तालकम्(mutex);
		contents.प्रक्षिप_पृष्ठभागः(std::move(t));
	}
	std::optional<T> try_निष्कासय_पृष्ठभागः() {
		तालकम्_Guard तालकम्(mutex);
		यदि (contents.रिक्तानि())
			प्रत्यागम std::nullopt;
		T परिणाम = std::move(contents.पृष्ठभागः());
		contents.निष्कासय_पृष्ठभागः();
		प्रत्यागम परिणाम;
	}
गुप्त:
	Mutex mutex;
	std::सदिशम्<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
प्रतिरूपम् <typename T>
class Sharded_सदिशम् {
सार्वजनिक:
	Sharded_सदिशम्(ध्रुव ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.अङ्कः_तन्तवः());
	}
	Sharded_सदिशम्(ध्रुव ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.अङ्कः_तन्तवः());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	शून्य निवेशय(ध्रुव Thread_सूचकांक &thread, T मानम्) {
		buckets[thread.thread_अङ्कः].emplace_पृष्ठभागः(std::move(मानम्));
	}

	bool रिक्तानि() ध्रुव {
		प्रति (ध्रुव std::सदिशम्<T> &bucket : buckets)
			यदि (!bucket.रिक्तानि())
				प्रत्यागम false;
		प्रत्यागम true;
	}

	using Buckets = std::सदिशम्<std::सदिशम्<T>>;
	class iterator {
	सार्वजनिक:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_अन्तः)
			: bucket_it(std::move(bucket_it)), bucket_अन्तः(std::move(bucket_अन्तः)) {
			यदि (bucket_it != bucket_अन्तः)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& संक्रियकः*() ध्रुव { प्रत्यागम *inner_it.मानम्(); }
		iterator &संक्रियकः++() {
			++*inner_it;
			normalize();
			प्रत्यागम *this;
		}
		bool संक्रियकः!=(ध्रुव iterator &other) ध्रुव {
			प्रत्यागम bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	गुप्त:
		शून्य normalize() {
			यदि (bucket_it == bucket_अन्तः)
				प्रत्यागम;
			यावत् (inner_it == bucket_it->अन्तः()) {
				++bucket_it;
				यदि (bucket_it == bucket_अन्तः) {
					inner_it.reset();
					प्रत्यागम;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::सदिशम्<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_अन्तः;
	};
	iterator begin() { प्रत्यागम iterator(buckets.begin(), buckets.अन्तः()); }
	iterator अन्तः() { प्रत्यागम iterator(buckets.अन्तः(), buckets.अन्तः()); }
गुप्त:
	शून्य init(पूर्णाङ्क अङ्कः_तन्तवः) {
		buckets.resize(अङ्कः_तन्तवः);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
प्रतिरूपम् <typename V>
संरचना समुच्चयः_Collision_Handler {
	शून्य संक्रियकः()(typename V::Accumulated &, typename V::Accumulated &) ध्रुव {}
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
प्रतिरूपम् <typename V, typename कुञ्चिका_Equality, typename CollisionHandler>
class ShardedHashtable {
सार्वजनिक:
	// A combination of a `V` and its hash value.
	संरचना मानम्_3 {
		मानम्_3(V मानम्, अचिह्नित पूर्णाङ्क hash) : मानम्(std::move(मानम्)), hash(hash) {}
		मानम्_3(मानम्_3 &&) = स्वभाव;
		मानम्_3(ध्रुव मानम्_3 &) = delete;
		मानम्_3 &संक्रियकः=(ध्रुव मानम्_3 &) = delete;
		V मानम्;
		अचिह्नित पूर्णाङ्क hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	संरचना Accumulated_मानम् {
		Accumulated_मानम्(typename V::Accumulated मानम्, अचिह्नित पूर्णाङ्क hash) : मानम्(std::move(मानम्)), hash(hash) {}
		Accumulated_मानम्(Accumulated_मानम् &&) = स्वभाव;
#यदि defined(_MSC_VER)
		Accumulated_मानम्(ध्रुव Accumulated_मानम् &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_मानम् &संक्रियकः=(ध्रुव Accumulated_मानम् &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			प्रत्यागम *this;
		}
#अन्यथा
		Accumulated_मानम्(ध्रुव Accumulated_मानम् &) = delete;
		Accumulated_मानम् &संक्रियकः=(ध्रुव Accumulated_मानम् &) = delete;
#यदि_अन्तः
		typename V::Accumulated मानम्;
		अचिह्नित पूर्णाङ्क hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मानम्_Equality {
		कुञ्चिका_Equality inner;
		Accumulated_मानम्_Equality(ध्रुव कुञ्चिका_Equality &inner) : inner(inner) {}
		bool संक्रियकः()(ध्रुव Accumulated_मानम् &v1, ध्रुव Accumulated_मानम् &v2) ध्रुव {
			प्रत्यागम inner(v1.मानम्, v2.मानम्);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मानम्_Hash_Op {
		आकार_t संक्रियकः()(ध्रुव Accumulated_मानम् &v) ध्रुव {
			प्रत्यागम static_cast<आकार_t>(v.hash);
		}
	};
	using Shard = std::unordered_समुच्चयः<Accumulated_मानम्, Accumulated_मानम्_Hash_Op, Accumulated_मानम्_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	सार्वजनिक:
		Builder(ध्रुव ParallelDispatchThreadPool &thread_pool, कुञ्चिका_Equality equality = कुञ्चिका_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अङ्कः_तन्तवः(), std::move(equality));
		}
		Builder(ध्रुव ParallelDispatchThreadPool::Subpool &thread_pool, कुञ्चिका_Equality equality = कुञ्चिका_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अङ्कः_तन्तवः(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		शून्य निवेशय(ध्रुव Thread_सूचकांक &thread, मानम्_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::सदिशम्<std::सदिशम्<मानम्_3>> &buckets = all_buckets[thread.thread_अङ्कः];
			आकार_t bucket = static_cast<आकार_t>(v.hash) % buckets.आकार();
			buckets[bucket].emplace_पृष्ठभागः(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		शून्य process(ध्रुव Thread_सूचकांक &thread) {
			पूर्णाङ्क आकार = 0;
			प्रति (std::सदिशम्<std::सदिशम्<मानम्_3>> &buckets : all_buckets)
				आकार += Get_आकार(buckets[thread.thread_अङ्कः]);
			Shard &shard = shards[thread.thread_अङ्कः];
			shard.reserve(आकार);
			प्रति (std::सदिशम्<std::सदिशम्<मानम्_3>> &buckets : all_buckets) {
				प्रति (मानम्_3 &मानम् : buckets[thread.thread_अङ्कः])
					accumulate(मानम्, shard);
				// Free as much memory as we can during the parallel phase.
				std::सदिशम्<मानम्_3>().swap(buckets[thread.thread_अङ्कः]);
			}
		}
	गुप्त:
		मित्रम् class ShardedHashtable<V, कुञ्चिका_Equality, CollisionHandler>;
		शून्य accumulate(मानम्_3 &मानम्, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_मानम् accumulated_मानम्{std::move(मानम्.मानम्), मानम्.hash};
			स्वचालित [it, inserted] = shard.निवेशय(std::move(accumulated_मानम्));
			यदि (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->मानम्), accumulated_मानम्.मानम्);
		}
		शून्य init(पूर्णाङ्क अङ्कः_तन्तवः, कुञ्चिका_Equality equality) {
			all_buckets.resize(अङ्कः_तन्तवः);
			प्रति (std::सदिशम्<std::सदिशम्<मानम्_3>> &buckets : all_buckets)
				buckets.resize(अङ्कः_तन्तवः);
			प्रति (पूर्णाङ्क i = 0; i < अङ्कः_तन्तवः; ++i)
				shards.emplace_पृष्ठभागः(0, Accumulated_मानम्_Hash_Op(), Accumulated_मानम्_Equality(equality));
		}
		ध्रुव CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::सदिशम्<std::सदिशम्<std::सदिशम्<मानम्_3>>> all_buckets;
		std::सदिशम्<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		प्रति (std::सदिशम्<std::सदिशम्<मानम्_3>> &buckets : builder.all_buckets)
			प्रति (std::सदिशम्<मानम्_3> &bucket : buckets)
				log_assert(bucket.रिक्तानि());
		// Free memory.
		std::सदिशम्<std::सदिशम्<std::सदिशम्<मानम्_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = स्वभाव;
	ShardedHashtable() {}

	ShardedHashtable &संक्रियकः=(ShardedHashtable &&other) = स्वभाव;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ध्रुव typename V::Accumulated *find(ध्रुव Accumulated_मानम् &v) ध्रुव {
		आकार_t अङ्कः_shards = shards.आकार();
		यदि (अङ्कः_shards == 0)
			प्रत्यागम nullptr;
		आकार_t shard = static_cast<आकार_t>(v.hash) % अङ्कः_shards;
		स्वचालित it = shards[shard].find(v);
		यदि (it == shards[shard].अन्तः())
			प्रत्यागम nullptr;
		प्रत्यागम &it->मानम्;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	शून्य निवेशय(Accumulated_मानम् v) {
		आकार_t अङ्कः_shards = shards.आकार();
		यदि (अङ्कः_shards == 0)
			प्रत्यागम;
		आकार_t shard = static_cast<आकार_t>(v.hash) % अङ्कः_shards;
		shards[shard].निवेशय(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	शून्य clear(ध्रुव Thread_सूचकांक &shard) {
		Accumulated_मानम्_Equality equality = shards[shard.thread_अङ्कः].कुञ्चिका_eq();
		shards[shard.thread_अङ्कः] = Shard(0, Accumulated_मानम्_Hash_Op(), equality);
	}
गुप्त:
	std::सदिशम्<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
प्रतिरूपम् <typename T>
class ConcurrentWorkQueue {
सार्वजनिक:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(पूर्णाङ्क अङ्कः_तन्तवः, पूर्णाङ्क batch_आकार = 100)
		: batch_आकार(batch_आकार), thread_अवस्थाः(अङ्कः_तन्तवः) {}
	पूर्णाङ्क अङ्कः_तन्तवः() ध्रुव { प्रत्यागम Get_आकार(thread_अवस्थाः); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	शून्य प्रक्षिप(ध्रुव Thread_सूचकांक &thread, T work) {
		ThreadState &thread_state = thread_अवस्थाः[thread.thread_अङ्कः];
		thread_state.अग्र_batch.emplace_पृष्ठभागः(std::move(work));
		यदि (Get_आकार(thread_state.अग्र_batch) < batch_आकार)
			प्रत्यागम;
		bool was_रिक्तानि;
		{
			Unique_तालकम् तालकम्(thread_state.batches_तालकम्);
			was_रिक्तानि = thread_state.batches.रिक्तानि();
			thread_state.batches.प्रक्षिप_पृष्ठभागः(std::move(thread_state.अग्र_batch));
		}
		यदि (was_रिक्तानि) {
			Unique_तालकम् तालकम्(waiters_तालकम्);
			यदि (अङ्कः_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::सदिशम्<T> निष्कासय_batch(ध्रुव Thread_सूचकांक &thread) {
		ThreadState &thread_state = thread_अवस्थाः[thread.thread_अङ्कः];
		यदि (!thread_state.अग्र_batch.रिक्तानि())
			प्रत्यागम std::move(thread_state.अग्र_batch);
		// Empty our own work queue first.
		{
			Unique_तालकम् तालकम्(thread_state.batches_तालकम्);
			यदि (!thread_state.batches.रिक्तानि()) {
				std::सदिशम्<T> batch = std::move(thread_state.batches.पृष्ठभागः());
				thread_state.batches.निष्कासय_पृष्ठभागः();
				प्रत्यागम batch;
			}
		}
		// From here on in this function, our work queue is empty.
		यावत् (true) {
			std::सदिशम्<T> batch = try_steal(thread);
			यदि (!batch.रिक्तानि()) {
				प्रत्यागम std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_तालकम् तालकम्(waiters_तालकम्);
			++अङ्कः_waiters;
			यदि (अङ्कः_waiters == अङ्कः_तन्तवः()) {
				waiters_cv.notify_all();
				प्रत्यागम {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.प्रतीक्षस्व(तालकम्);
			यदि (अङ्कः_waiters == अङ्कः_तन्तवः())
				प्रत्यागम {};
			--अङ्कः_waiters;
		}
	}
गुप्त:
	std::सदिशम्<T> try_steal(ध्रुव Thread_सूचकांक &thread) {
		प्रति (पूर्णाङ्क i = 1; i < अङ्कः_तन्तवः(); i++) {
			पूर्णाङ्क other_thread_अङ्कः = (thread.thread_अङ्कः + i) % अङ्कः_तन्तवः();
			ThreadState &other_thread_state = thread_अवस्थाः[other_thread_अङ्कः];
			Unique_तालकम् तालकम्(other_thread_state.batches_तालकम्);
			यदि (!other_thread_state.batches.रिक्तानि()) {
				std::सदिशम्<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.निष्कासय_front();
				प्रत्यागम batch;
			}
		}
		प्रत्यागम {};
	}

	पूर्णाङ्क batch_आकार;

	संरचना ThreadState {
		// Entirely thread-local.
		std::सदिशम्<T> अग्र_batch;

		Mutex batches_तालकम्;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::सदिशम्<T>> batches;
	};
	std::सदिशम्<ThreadState> thread_अवस्थाः;

	Mutex waiters_तालकम्;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	पूर्णाङ्क अङ्कः_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
सार्वजनिक:
	MonotonicFlag() : मानम्(false) {}
	bool आरोपय() ध्रुव { प्रत्यागम मानम्.आरोपय(std::स्मृतिः_क्रमः_relaxed); }
	शून्य समुच्चयः() { मानम्.store(true, std::स्मृतिः_क्रमः_relaxed); }
	bool समुच्चयः_and_return_old() {
		प्रत्यागम मानम्.exchange(true, std::स्मृतिः_क्रमः_relaxed);
	}
गुप्त:
	std::atomic<bool> मानम्;
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः // YOSYS_THREADING_H
