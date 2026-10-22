#ضمن <deque>

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/utils.h"

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <thread>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل YOSYS_THREADING_H
#وصف_ڪريو YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_تالو = std::unique_تالو<Mutex>;
using تالو_Guard = std::تالو_guard<Mutex>;
#وإلا
بنية Mutex {
	فراغ تالو() {}
	فراغ unlock() {}
	bool try_تالو() { أرجع true; }
};
بنية CondVar {
	سانچو <class L> فراغ wait(L &) {}
	سانچو <class L, class P> فراغ wait(L &, P) {}
	فراغ notify_one() {}
	فراغ notify_all() {}
};
بنية Unique_تالو {
	Unique_تالو(Mutex &) {}
};
بنية تالو_Guard {
	تالو_Guard(Mutex &) {}
};
#جيڪڏهن_پورو

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
سانچو <typename T>
class Concurrent_قطار
{
عام:
	Concurrent_قطار(صحيح صلاحيت = وڌ_کان_وڌ_عدد)
		: صلاحيت(صلاحيت) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	فراغ ڌڪو_پٺ(T t)
	{
		Unique_تالو تالو(mutex);
		not_ڀريل_condition.wait(تالو, [this] { أرجع static_cast<صحيح>(contents.سائيز()) < صلاحيت; });
		إذا (contents.خالي_جايون())
			not_خالي_جايون_condition.notify_one();
		log_assert(!closed);
		contents.ڌڪو_پٺ(std::move(t));
		إذا (static_cast<صحيح>(contents.سائيز()) < صلاحيت)
			not_ڀريل_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	فراغ close()
	{
		Unique_تالو تالو(mutex);
		not_خالي_جايون_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> ڪڍو_front()
	{
		أرجع ڪڍو_front_اندروني(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_ڪڍو_front()
	{
		أرجع ڪڍو_front_اندروني(false);
	}
خاص:
	std::optional<T> ڪڍو_front_اندروني(bool wait)
	{
		Unique_تالو تالو(mutex);
		إذا (wait) {
			not_خالي_جايون_condition.wait(تالو, [this] { أرجع !contents.خالي_جايون() || closed; });
		}
		إذا (contents.خالي_جايون())
			أرجع std::nullopt;
		إذا (static_cast<صحيح>(contents.سائيز()) == صلاحيت)
			not_ڀريل_condition.notify_one();
		T نتيجو = std::move(contents.front());
		contents.ڪڍو_front();
		إذا (!contents.خالي_جايون())
			not_خالي_جايون_condition.notify_one();
		أرجع std::move(نتيجو);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_ڀريل_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_خالي_جايون_condition;
	std::deque<T> contents;
	صحيح صلاحيت;
	bool closed = false;
};

class DeferredLogs
{
عام:
	سانچو <typename... دليلون_3>
	فراغ لاگرٿم(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, دليلون_3... دليلون_2)
	{
		logs.ڌڪو_پٺ({fmt.format(دليلون_2...), false});
	}
	سانچو <typename... دليلون_3>
	فراغ log_error(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, دليلون_3... دليلون_2)
	{
		logs.ڌڪو_پٺ({fmt.format(دليلون_2...), true});
	}
	فراغ flush();
خاص:
	بنية پيغام_2
	{
		std::string متن;
		bool error;
	};
	std::ويڪٽر<پيغام_2> logs;
};

class ThreadPool
{
عام:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساكن صحيح pool_سائيز(صحيح reserved_cores, صحيح max_ڪم_ڪندڙ_ڌاڳا);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساكن صحيح work_pool_سائيز(صحيح reserved_cores, صحيح work_units, صحيح work_units_في_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحيح pool_سائيز, std::function<فراغ(صحيح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحيح انگ_ڌاڳا() ثابت
	{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
		أرجع ڌاڳا.سائيز();
#وإلا
		أرجع 0;
#جيڪڏهن_پورو
	}
خاص:
	std::function<فراغ(صحيح)> body;
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ڌاڳا
	std::ويڪٽر<std::thread> ڌاڳا;
#جيڪڏهن_پورو
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange شي_range_for_ڪم_ڪندڙ(صحيح انگ_شيون, صحيح thread_انگ, صحيح انگ_ڌاڳا);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
بنية Thread_اشاريو {
	صحيح thread_انگ;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
عام:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(صحيح pool_سائيز);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	بنية لاڳيتو_Ctx : عام Thread_اشاريو {
		صحيح انگ_ڌاڳا;
		IntRange شي_range(صحيح انگ_شيون) ثابت {
			أرجع شي_range_for_ڪم_ڪندڙ(انگ_شيون, thread_انگ, انگ_ڌاڳا);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عام:
		Subpool(ParallelDispatchThreadPool &والد, صحيح max_ڌاڳا)
				: والد(والد), max_ڌاڳا(max_ڌاڳا) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحيح انگ_ڌاڳا() ثابت {
			أرجع والد.انگ_ڌاڳا(max_ڌاڳا);
		}
		فراغ لاڳيتو(std::function<فراغ(ثابت لاڳيتو_Ctx &)> work) {
			والد.لاڳيتو(std::move(work), max_ڌاڳا);
		}
		ParallelDispatchThreadPool &thread_pool() { أرجع والد; }
	خاص:
		ParallelDispatchThreadPool &والد;
		صحيح max_ڌاڳا;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	فراغ لاڳيتو(std::function<فراغ(ثابت لاڳيتو_Ctx &)> work) {
		لاڳيتو(std::move(work), وڌ_کان_وڌ_عدد);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحيح انگ_ڌاڳا() ثابت {
		أرجع انگ_ڌاڳا(وڌ_کان_وڌ_عدد);
	}
خاص:
	دوست class Subpool;

	فراغ لاڳيتو(std::function<فراغ(ثابت لاڳيتو_Ctx &)> work, صحيح max_ڌاڳا);
	صحيح انگ_ڌاڳا(صحيح max_ڌاڳا) ثابت {
		أرجع std::min(انگ_ڪم_ڪندڙ_ڌاڳا + 1, std::max(1, max_ڌاڳا));
	}
	فراغ لاڳيتو_ڪم_ڪندڙ(صحيح thread_انگ);

	std::function<فراغ(ثابت لاڳيتو_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحيح انگ_ڪم_ڪندڙ_ڌاڳا;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحيح> انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_منزل_workers_signal_mutex;
	CondVar main_منزل_workers_signal_cv;
	std::ويڪٽر<uint8_t> main_منزل_workers_signal;
	فراغ signal_workers_شروعات() {
		Unique_تالو تالو(main_منزل_workers_signal_mutex);
		صحيح انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 = انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.ڀريو(std::يادگيري_ترتيب_relaxed);
		std::fill(main_منزل_workers_signal.begin(), main_منزل_workers_signal.begin() + انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_منزل_workers_signal_cv.notify_all();
	}
	فراغ ڪم_ڪندڙ_wait_for_شروعات(صحيح thread_انگ) {
		Unique_تالو تالو(main_منزل_workers_signal_mutex);
		main_منزل_workers_signal_cv.wait(تالو, [this, thread_انگ] { أرجع main_منزل_workers_signal[thread_انگ] > 0; });
		main_منزل_workers_signal[thread_انگ] = 0;
	}

	std::atomic<صحيح> پورو_workers = 0;
	Mutex workers_منزل_main_signal_mutex;
	CondVar workers_منزل_main_signal_cv;
	فراغ signal_ڪم_ڪندڙ_پورو() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحيح انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 = انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.ڀريو(std::يادگيري_ترتيب_relaxed);
		صحيح d = پورو_workers.fetch_جوڙيو(1, std::يادگيري_ترتيب_release);
		إذا (d + 1 == انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2) {
			Unique_تالو تالو(workers_منزل_main_signal_mutex);
			workers_منزل_main_signal_cv.notify_all();
		}
	}
	فراغ wait_for_workers_پورو() {
		Unique_تالو تالو(workers_منزل_main_signal_mutex);
		workers_منزل_main_signal_cv.wait(تالو, [this] {
			صحيح انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2 = انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا.ڀريو(std::يادگيري_ترتيب_relaxed);
			أرجع پورو_workers.ڀريو(std::يادگيري_ترتيب_acquire) == انگ_سرگرم_ڪم_ڪندڙ_ڌاڳا_2;
		});
		پورو_workers.store(0, std::يادگيري_ترتيب_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

سانچو <class T>
class Concurrent_ڍير
{
عام:
	فراغ ڌڪو_پٺ(T &&t) {
		تالو_Guard تالو(mutex);
		contents.ڌڪو_پٺ(std::move(t));
	}
	std::optional<T> try_ڪڍو_پٺ() {
		تالو_Guard تالو(mutex);
		إذا (contents.خالي_جايون())
			أرجع std::nullopt;
		T نتيجو = std::move(contents.پٺ());
		contents.ڪڍو_پٺ();
		أرجع نتيجو;
	}
خاص:
	Mutex mutex;
	std::ويڪٽر<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
سانچو <typename T>
class Sharded_ويڪٽر {
عام:
	Sharded_ويڪٽر(ثابت ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.انگ_ڌاڳا());
	}
	Sharded_ويڪٽر(ثابت ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.انگ_ڌاڳا());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	فراغ وجهو(ثابت Thread_اشاريو &thread, T قدر) {
		buckets[thread.thread_انگ].emplace_پٺ(std::move(قدر));
	}

	bool خالي_جايون() ثابت {
		لكل (ثابت std::ويڪٽر<T> &ٻالٽي : buckets)
			إذا (!ٻالٽي.خالي_جايون())
				أرجع false;
		أرجع true;
	}

	using Buckets = std::ويڪٽر<std::ويڪٽر<T>>;
	class iterator {
	عام:
		iterator(typename Buckets::iterator ٻالٽي_it, typename Buckets::iterator ٻالٽي_پڇاڙي)
			: ٻالٽي_it(std::move(ٻالٽي_it)), ٻالٽي_پڇاڙي(std::move(ٻالٽي_پڇاڙي)) {
			إذا (ٻالٽي_it != ٻالٽي_پڇاڙي)
				inner_it = ٻالٽي_it->begin();
			normalize();
		}
		T& عامل*() ثابت { أرجع *inner_it.قدر(); }
		iterator &عامل++() {
			++*inner_it;
			normalize();
			أرجع *this;
		}
		bool عامل!=(ثابت iterator &other) ثابت {
			أرجع ٻالٽي_it != other.ٻالٽي_it || inner_it != other.inner_it;
		}
	خاص:
		فراغ normalize() {
			إذا (ٻالٽي_it == ٻالٽي_پڇاڙي)
				أرجع;
			طالما (inner_it == ٻالٽي_it->پڇاڙي()) {
				++ٻالٽي_it;
				إذا (ٻالٽي_it == ٻالٽي_پڇاڙي) {
					inner_it.reset();
					أرجع;
				}
				inner_it = ٻالٽي_it->begin();
			}
		}
		std::optional<typename std::ويڪٽر<T>::iterator> inner_it;
		typename Buckets::iterator ٻالٽي_it;
		typename Buckets::iterator ٻالٽي_پڇاڙي;
	};
	iterator begin() { أرجع iterator(buckets.begin(), buckets.پڇاڙي()); }
	iterator پڇاڙي() { أرجع iterator(buckets.پڇاڙي(), buckets.پڇاڙي()); }
خاص:
	فراغ init(صحيح انگ_ڌاڳا) {
		buckets.resize(انگ_ڌاڳا);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
سانچو <typename V>
بنية سيٽ_Collision_Handler {
	فراغ عامل()(typename V::Accumulated &, typename V::Accumulated &) ثابت {}
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
سانچو <typename V, typename ڪنجي_Equality, typename CollisionHandler>
class ShardedHashtable {
عام:
	// A combination of a `V` and its hash value.
	بنية قدر_3 {
		قدر_3(V قدر, بنانشان صحيح hash) : قدر(std::move(قدر)), hash(hash) {}
		قدر_3(قدر_3 &&) = افتراضي;
		قدر_3(ثابت قدر_3 &) = delete;
		قدر_3 &عامل=(ثابت قدر_3 &) = delete;
		V قدر;
		بنانشان صحيح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	بنية Accumulated_قدر {
		Accumulated_قدر(typename V::Accumulated قدر, بنانشان صحيح hash) : قدر(std::move(قدر)), hash(hash) {}
		Accumulated_قدر(Accumulated_قدر &&) = افتراضي;
#إذا defined(_MSC_VER)
		Accumulated_قدر(ثابت Accumulated_قدر &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_قدر &عامل=(ثابت Accumulated_قدر &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			أرجع *this;
		}
#وإلا
		Accumulated_قدر(ثابت Accumulated_قدر &) = delete;
		Accumulated_قدر &عامل=(ثابت Accumulated_قدر &) = delete;
#جيڪڏهن_پورو
		typename V::Accumulated قدر;
		بنانشان صحيح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_قدر_Equality {
		ڪنجي_Equality inner;
		Accumulated_قدر_Equality(ثابت ڪنجي_Equality &inner) : inner(inner) {}
		bool عامل()(ثابت Accumulated_قدر &v1, ثابت Accumulated_قدر &v2) ثابت {
			أرجع inner(v1.قدر, v2.قدر);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_قدر_Hash_Op {
		سائيز_t عامل()(ثابت Accumulated_قدر &v) ثابت {
			أرجع static_cast<سائيز_t>(v.hash);
		}
	};
	using Shard = std::unordered_سيٽ<Accumulated_قدر, Accumulated_قدر_Hash_Op, Accumulated_قدر_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عام:
		Builder(ثابت ParallelDispatchThreadPool &thread_pool, ڪنجي_Equality equality = ڪنجي_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.انگ_ڌاڳا(), std::move(equality));
		}
		Builder(ثابت ParallelDispatchThreadPool::Subpool &thread_pool, ڪنجي_Equality equality = ڪنجي_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.انگ_ڌاڳا(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		فراغ وجهو(ثابت Thread_اشاريو &thread, قدر_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::ويڪٽر<std::ويڪٽر<قدر_3>> &buckets = all_buckets[thread.thread_انگ];
			سائيز_t ٻالٽي = static_cast<سائيز_t>(v.hash) % buckets.سائيز();
			buckets[ٻالٽي].emplace_پٺ(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		فراغ process(ثابت Thread_اشاريو &thread) {
			صحيح سائيز = 0;
			لكل (std::ويڪٽر<std::ويڪٽر<قدر_3>> &buckets : all_buckets)
				سائيز += Get_سائيز(buckets[thread.thread_انگ]);
			Shard &shard = shards[thread.thread_انگ];
			shard.reserve(سائيز);
			لكل (std::ويڪٽر<std::ويڪٽر<قدر_3>> &buckets : all_buckets) {
				لكل (قدر_3 &قدر : buckets[thread.thread_انگ])
					accumulate(قدر, shard);
				// Free as much memory as we can during the parallel phase.
				std::ويڪٽر<قدر_3>().مٽا_سٽا(buckets[thread.thread_انگ]);
			}
		}
	خاص:
		دوست class ShardedHashtable<V, ڪنجي_Equality, CollisionHandler>;
		فراغ accumulate(قدر_3 &قدر, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_قدر accumulated_قدر{std::move(قدر.قدر), قدر.hash};
			خودڪار [it, inserted] = shard.وجهو(std::move(accumulated_قدر));
			إذا (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->قدر), accumulated_قدر.قدر);
		}
		فراغ init(صحيح انگ_ڌاڳا, ڪنجي_Equality equality) {
			all_buckets.resize(انگ_ڌاڳا);
			لكل (std::ويڪٽر<std::ويڪٽر<قدر_3>> &buckets : all_buckets)
				buckets.resize(انگ_ڌاڳا);
			لكل (صحيح i = 0; i < انگ_ڌاڳا; ++i)
				shards.emplace_پٺ(0, Accumulated_قدر_Hash_Op(), Accumulated_قدر_Equality(equality));
		}
		ثابت CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::ويڪٽر<std::ويڪٽر<std::ويڪٽر<قدر_3>>> all_buckets;
		std::ويڪٽر<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		لكل (std::ويڪٽر<std::ويڪٽر<قدر_3>> &buckets : builder.all_buckets)
			لكل (std::ويڪٽر<قدر_3> &ٻالٽي : buckets)
				log_assert(ٻالٽي.خالي_جايون());
		// Free memory.
		std::ويڪٽر<std::ويڪٽر<std::ويڪٽر<قدر_3>>>().مٽا_سٽا(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = افتراضي;
	ShardedHashtable() {}

	ShardedHashtable &عامل=(ShardedHashtable &&other) = افتراضي;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *ڳوليو(ثابت Accumulated_قدر &v) ثابت {
		سائيز_t انگ_shards = shards.سائيز();
		إذا (انگ_shards == 0)
			أرجع nullptr;
		سائيز_t shard = static_cast<سائيز_t>(v.hash) % انگ_shards;
		خودڪار it = shards[shard].ڳوليو(v);
		إذا (it == shards[shard].پڇاڙي())
			أرجع nullptr;
		أرجع &it->قدر;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	فراغ وجهو(Accumulated_قدر v) {
		سائيز_t انگ_shards = shards.سائيز();
		إذا (انگ_shards == 0)
			أرجع;
		سائيز_t shard = static_cast<سائيز_t>(v.hash) % انگ_shards;
		shards[shard].وجهو(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	فراغ clear(ثابت Thread_اشاريو &shard) {
		Accumulated_قدر_Equality equality = shards[shard.thread_انگ].ڪنجي_eq();
		shards[shard.thread_انگ] = Shard(0, Accumulated_قدر_Hash_Op(), equality);
	}
خاص:
	std::ويڪٽر<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
سانچو <typename T>
class Concurrent_Work_قطار {
عام:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_قطار(صحيح انگ_ڌاڳا, صحيح batch_سائيز = 100)
		: batch_سائيز(batch_سائيز), thread_حالتون(انگ_ڌاڳا) {}
	صحيح انگ_ڌاڳا() ثابت { أرجع Get_سائيز(thread_حالتون); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	فراغ ڌڪو(ثابت Thread_اشاريو &thread, T work) {
		ThreadState &thread_state = thread_حالتون[thread.thread_انگ];
		thread_state.ايندڙ_batch.emplace_پٺ(std::move(work));
		إذا (Get_سائيز(thread_state.ايندڙ_batch) < batch_سائيز)
			أرجع;
		bool was_خالي_جايون;
		{
			Unique_تالو تالو(thread_state.batches_تالو);
			was_خالي_جايون = thread_state.batches.خالي_جايون();
			thread_state.batches.ڌڪو_پٺ(std::move(thread_state.ايندڙ_batch));
		}
		إذا (was_خالي_جايون) {
			Unique_تالو تالو(waiters_تالو);
			إذا (انگ_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::ويڪٽر<T> ڪڍو_batch(ثابت Thread_اشاريو &thread) {
		ThreadState &thread_state = thread_حالتون[thread.thread_انگ];
		إذا (!thread_state.ايندڙ_batch.خالي_جايون())
			أرجع std::move(thread_state.ايندڙ_batch);
		// Empty our own work queue first.
		{
			Unique_تالو تالو(thread_state.batches_تالو);
			إذا (!thread_state.batches.خالي_جايون()) {
				std::ويڪٽر<T> batch = std::move(thread_state.batches.پٺ());
				thread_state.batches.ڪڍو_پٺ();
				أرجع batch;
			}
		}
		// From here on in this function, our work queue is empty.
		طالما (true) {
			std::ويڪٽر<T> batch = try_steal(thread);
			إذا (!batch.خالي_جايون()) {
				أرجع std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_تالو تالو(waiters_تالو);
			++انگ_waiters;
			إذا (انگ_waiters == انگ_ڌاڳا()) {
				waiters_cv.notify_all();
				أرجع {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(تالو);
			إذا (انگ_waiters == انگ_ڌاڳا())
				أرجع {};
			--انگ_waiters;
		}
	}
خاص:
	std::ويڪٽر<T> try_steal(ثابت Thread_اشاريو &thread) {
		لكل (صحيح i = 1; i < انگ_ڌاڳا(); i++) {
			صحيح other_thread_انگ = (thread.thread_انگ + i) % انگ_ڌاڳا();
			ThreadState &other_thread_state = thread_حالتون[other_thread_انگ];
			Unique_تالو تالو(other_thread_state.batches_تالو);
			إذا (!other_thread_state.batches.خالي_جايون()) {
				std::ويڪٽر<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.ڪڍو_front();
				أرجع batch;
			}
		}
		أرجع {};
	}

	صحيح batch_سائيز;

	بنية ThreadState {
		// Entirely thread-local.
		std::ويڪٽر<T> ايندڙ_batch;

		Mutex batches_تالو;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::ويڪٽر<T>> batches;
	};
	std::ويڪٽر<ThreadState> thread_حالتون;

	Mutex waiters_تالو;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	صحيح انگ_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
عام:
	MonotonicFlag() : قدر(false) {}
	bool ڀريو() ثابت { أرجع قدر.ڀريو(std::يادگيري_ترتيب_relaxed); }
	فراغ سيٽ() { قدر.store(true, std::يادگيري_ترتيب_relaxed); }
	bool سيٽ_and_return_old() {
		أرجع قدر.exchange(true, std::يادگيري_ترتيب_relaxed);
	}
خاص:
	std::atomic<bool> قدر;
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو // YOSYS_THREADING_H
