#شامل <deque>

#شامل "kernel/yosys_common.h"
#شامل "kernel/log.h"
#شامل "kernel/utils.h"

#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
#شامل <condition_variable>
#شامل <mutex>
#شامل <thread>
#اگر_ختم

#اگر_غیر_تعریف_شدہ YOSYS_THREADING_H
#تعریف_کرو YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_تالا = std::unique_تالا<Mutex>;
using تالا_Guard = std::تالا_guard<Mutex>;
#ورنہ
ساخت Mutex {
	خالی تالا() {}
	خالی unlock() {}
	bool try_تالا() { واپس true; }
};
ساخت CondVar {
	سانچہ <class L> خالی wait(L &) {}
	سانچہ <class L, class P> خالی wait(L &, P) {}
	خالی notify_one() {}
	خالی notify_all() {}
};
ساخت Unique_تالا {
	Unique_تالا(Mutex &) {}
};
ساخت تالا_Guard {
	تالا_Guard(Mutex &) {}
};
#اگر_ختم

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
سانچہ <typename T>
class Concurrent_قطار
{
عوامی:
	Concurrent_قطار(صحیح استعداد = زیادہ_سے_زیادہ_عدد)
		: استعداد(استعداد) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	خالی ڈالو_عقب(T t)
	{
		Unique_تالا تالا(mutex);
		not_بھرا_condition.wait(تالا, [this] { واپس static_cast<صحیح>(contents.سائز()) < استعداد; });
		اگر (contents.خالی_خانے())
			not_خالی_خانے_condition.notify_one();
		log_assert(!closed);
		contents.ڈالو_عقب(std::move(t));
		اگر (static_cast<صحیح>(contents.سائز()) < استعداد)
			not_بھرا_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	خالی close()
	{
		Unique_تالا تالا(mutex);
		not_خالی_خانے_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> نکالو_front()
	{
		واپس نکالو_front_اندرونی(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_نکالو_front()
	{
		واپس نکالو_front_اندرونی(false);
	}
نجی:
	std::optional<T> نکالو_front_اندرونی(bool wait)
	{
		Unique_تالا تالا(mutex);
		اگر (wait) {
			not_خالی_خانے_condition.wait(تالا, [this] { واپس !contents.خالی_خانے() || closed; });
		}
		اگر (contents.خالی_خانے())
			واپس std::nullopt;
		اگر (static_cast<صحیح>(contents.سائز()) == استعداد)
			not_بھرا_condition.notify_one();
		T نتیجہ = std::move(contents.front());
		contents.نکالو_front();
		اگر (!contents.خالی_خانے())
			not_خالی_خانے_condition.notify_one();
		واپس std::move(نتیجہ);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_بھرا_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_خالی_خانے_condition;
	std::deque<T> contents;
	صحیح استعداد;
	bool closed = false;
};

class DeferredLogs
{
عوامی:
	سانچہ <typename... دلائل_3>
	خالی لوگارتھم(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, دلائل_3... دلائل_2)
	{
		logs.ڈالو_عقب({fmt.format(دلائل_2...), false});
	}
	سانچہ <typename... دلائل_3>
	خالی log_error(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, دلائل_3... دلائل_2)
	{
		logs.ڈالو_عقب({fmt.format(دلائل_2...), true});
	}
	خالی flush();
نجی:
	ساخت پیغام_3
	{
		std::string متن;
		bool error;
	};
	std::سمتیہ<پیغام_3> logs;
};

class ThreadPool
{
عوامی:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساکن صحیح pool_سائز(صحیح reserved_cores, صحیح max_کارکن_دھاگے);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساکن صحیح work_pool_سائز(صحیح reserved_cores, صحیح work_units, صحیح work_units_فی_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحیح pool_سائز, std::function<خالی(صحیح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحیح عدد_دھاگے() ثابت
	{
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
		واپس دھاگے.سائز();
#ورنہ
		واپس 0;
#اگر_ختم
	}
نجی:
	std::function<خالی(صحیح)> body;
#اگر_تعریف_شدہ YOSYS_ENABLE_دھاگے
	std::سمتیہ<std::thread> دھاگے;
#اگر_ختم
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange شے_range_for_کارکن(صحیح عدد_اشیاء, صحیح thread_عدد, صحیح عدد_دھاگے);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ساخت Thread_اشاریہ {
	صحیح thread_عدد;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
عوامی:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(صحیح pool_سائز);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ساخت تسلسل_Ctx : عوامی Thread_اشاریہ {
		صحیح عدد_دھاگے;
		IntRange شے_range(صحیح عدد_اشیاء) ثابت {
			واپس شے_range_for_کارکن(عدد_اشیاء, thread_عدد, عدد_دھاگے);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عوامی:
		Subpool(ParallelDispatchThreadPool &والد, صحیح max_دھاگے)
				: والد(والد), max_دھاگے(max_دھاگے) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحیح عدد_دھاگے() ثابت {
			واپس والد.عدد_دھاگے(max_دھاگے);
		}
		خالی تسلسل(std::function<خالی(ثابت تسلسل_Ctx &)> work) {
			والد.تسلسل(std::move(work), max_دھاگے);
		}
		ParallelDispatchThreadPool &thread_pool() { واپس والد; }
	نجی:
		ParallelDispatchThreadPool &والد;
		صحیح max_دھاگے;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	خالی تسلسل(std::function<خالی(ثابت تسلسل_Ctx &)> work) {
		تسلسل(std::move(work), زیادہ_سے_زیادہ_عدد);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحیح عدد_دھاگے() ثابت {
		واپس عدد_دھاگے(زیادہ_سے_زیادہ_عدد);
	}
نجی:
	دوست class Subpool;

	خالی تسلسل(std::function<خالی(ثابت تسلسل_Ctx &)> work, صحیح max_دھاگے);
	صحیح عدد_دھاگے(صحیح max_دھاگے) ثابت {
		واپس std::min(عدد_کارکن_دھاگے + 1, std::max(1, max_دھاگے));
	}
	خالی تسلسل_کارکن(صحیح thread_عدد);

	std::function<خالی(ثابت تسلسل_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحیح عدد_کارکن_دھاگے;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحیح> عدد_فعال_کارکن_دھاگے = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_مقصد_workers_signal_mutex;
	CondVar main_مقصد_workers_signal_cv;
	std::سمتیہ<uint8_t> main_مقصد_workers_signal;
	خالی signal_workers_آغاز() {
		Unique_تالا تالا(main_مقصد_workers_signal_mutex);
		صحیح عدد_فعال_کارکن_دھاگے_2 = عدد_فعال_کارکن_دھاگے.بھرو(std::حافظہ_ترتیب_relaxed);
		std::fill(main_مقصد_workers_signal.begin(), main_مقصد_workers_signal.begin() + عدد_فعال_کارکن_دھاگے_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_مقصد_workers_signal_cv.notify_all();
	}
	خالی کارکن_wait_for_آغاز(صحیح thread_عدد) {
		Unique_تالا تالا(main_مقصد_workers_signal_mutex);
		main_مقصد_workers_signal_cv.wait(تالا, [this, thread_عدد] { واپس main_مقصد_workers_signal[thread_عدد] > 0; });
		main_مقصد_workers_signal[thread_عدد] = 0;
	}

	std::atomic<صحیح> مکمل_workers = 0;
	Mutex workers_مقصد_main_signal_mutex;
	CondVar workers_مقصد_main_signal_cv;
	خالی signal_کارکن_مکمل() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحیح عدد_فعال_کارکن_دھاگے_2 = عدد_فعال_کارکن_دھاگے.بھرو(std::حافظہ_ترتیب_relaxed);
		صحیح d = مکمل_workers.fetch_جوڑو(1, std::حافظہ_ترتیب_release);
		اگر (d + 1 == عدد_فعال_کارکن_دھاگے_2) {
			Unique_تالا تالا(workers_مقصد_main_signal_mutex);
			workers_مقصد_main_signal_cv.notify_all();
		}
	}
	خالی wait_for_workers_مکمل() {
		Unique_تالا تالا(workers_مقصد_main_signal_mutex);
		workers_مقصد_main_signal_cv.wait(تالا, [this] {
			صحیح عدد_فعال_کارکن_دھاگے_2 = عدد_فعال_کارکن_دھاگے.بھرو(std::حافظہ_ترتیب_relaxed);
			واپس مکمل_workers.بھرو(std::حافظہ_ترتیب_acquire) == عدد_فعال_کارکن_دھاگے_2;
		});
		مکمل_workers.store(0, std::حافظہ_ترتیب_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

سانچہ <class T>
class Concurrent_ڈھیر
{
عوامی:
	خالی ڈالو_عقب(T &&t) {
		تالا_Guard تالا(mutex);
		contents.ڈالو_عقب(std::move(t));
	}
	std::optional<T> try_نکالو_عقب() {
		تالا_Guard تالا(mutex);
		اگر (contents.خالی_خانے())
			واپس std::nullopt;
		T نتیجہ = std::move(contents.عقب());
		contents.نکالو_عقب();
		واپس نتیجہ;
	}
نجی:
	Mutex mutex;
	std::سمتیہ<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
سانچہ <typename T>
class Sharded_سمتیہ {
عوامی:
	Sharded_سمتیہ(ثابت ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.عدد_دھاگے());
	}
	Sharded_سمتیہ(ثابت ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.عدد_دھاگے());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	خالی داخل_کرو(ثابت Thread_اشاریہ &thread, T قدر) {
		buckets[thread.thread_عدد].emplace_عقب(std::move(قدر));
	}

	bool خالی_خانے() ثابت {
		برائے (ثابت std::سمتیہ<T> &بالٹی : buckets)
			اگر (!بالٹی.خالی_خانے())
				واپس false;
		واپس true;
	}

	using Buckets = std::سمتیہ<std::سمتیہ<T>>;
	class iterator {
	عوامی:
		iterator(typename Buckets::iterator بالٹی_it, typename Buckets::iterator بالٹی_انتہا)
			: بالٹی_it(std::move(بالٹی_it)), بالٹی_انتہا(std::move(بالٹی_انتہا)) {
			اگر (بالٹی_it != بالٹی_انتہا)
				inner_it = بالٹی_it->begin();
			normalize();
		}
		T& عامل*() ثابت { واپس *inner_it.قدر(); }
		iterator &عامل++() {
			++*inner_it;
			normalize();
			واپس *this;
		}
		bool عامل!=(ثابت iterator &other) ثابت {
			واپس بالٹی_it != other.بالٹی_it || inner_it != other.inner_it;
		}
	نجی:
		خالی normalize() {
			اگر (بالٹی_it == بالٹی_انتہا)
				واپس;
			جب (inner_it == بالٹی_it->انتہا()) {
				++بالٹی_it;
				اگر (بالٹی_it == بالٹی_انتہا) {
					inner_it.reset();
					واپس;
				}
				inner_it = بالٹی_it->begin();
			}
		}
		std::optional<typename std::سمتیہ<T>::iterator> inner_it;
		typename Buckets::iterator بالٹی_it;
		typename Buckets::iterator بالٹی_انتہا;
	};
	iterator begin() { واپس iterator(buckets.begin(), buckets.انتہا()); }
	iterator انتہا() { واپس iterator(buckets.انتہا(), buckets.انتہا()); }
نجی:
	خالی init(صحیح عدد_دھاگے) {
		buckets.resize(عدد_دھاگے);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
سانچہ <typename V>
ساخت سیٹ_Collision_Handler {
	خالی عامل()(typename V::Accumulated &, typename V::Accumulated &) ثابت {}
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
سانچہ <typename V, typename کلید_Equality, typename CollisionHandler>
class ShardedHashtable {
عوامی:
	// A combination of a `V` and its hash value.
	ساخت قدر_3 {
		قدر_3(V قدر, غیرعلامتی صحیح hash) : قدر(std::move(قدر)), hash(hash) {}
		قدر_3(قدر_3 &&) = طےشدہ;
		قدر_3(ثابت قدر_3 &) = delete;
		قدر_3 &عامل=(ثابت قدر_3 &) = delete;
		V قدر;
		غیرعلامتی صحیح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ساخت Accumulated_قدر {
		Accumulated_قدر(typename V::Accumulated قدر, غیرعلامتی صحیح hash) : قدر(std::move(قدر)), hash(hash) {}
		Accumulated_قدر(Accumulated_قدر &&) = طےشدہ;
#اگر defined(_MSC_VER)
		Accumulated_قدر(ثابت Accumulated_قدر &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_قدر &عامل=(ثابت Accumulated_قدر &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			واپس *this;
		}
#ورنہ
		Accumulated_قدر(ثابت Accumulated_قدر &) = delete;
		Accumulated_قدر &عامل=(ثابت Accumulated_قدر &) = delete;
#اگر_ختم
		typename V::Accumulated قدر;
		غیرعلامتی صحیح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ساخت Accumulated_قدر_Equality {
		کلید_Equality inner;
		Accumulated_قدر_Equality(ثابت کلید_Equality &inner) : inner(inner) {}
		bool عامل()(ثابت Accumulated_قدر &v1, ثابت Accumulated_قدر &v2) ثابت {
			واپس inner(v1.قدر, v2.قدر);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ساخت Accumulated_قدر_Hash_Op {
		سائز_t عامل()(ثابت Accumulated_قدر &v) ثابت {
			واپس static_cast<سائز_t>(v.hash);
		}
	};
	using Shard = std::unordered_سیٹ<Accumulated_قدر, Accumulated_قدر_Hash_Op, Accumulated_قدر_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عوامی:
		Builder(ثابت ParallelDispatchThreadPool &thread_pool, کلید_Equality equality = کلید_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.عدد_دھاگے(), std::move(equality));
		}
		Builder(ثابت ParallelDispatchThreadPool::Subpool &thread_pool, کلید_Equality equality = کلید_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.عدد_دھاگے(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		خالی داخل_کرو(ثابت Thread_اشاریہ &thread, قدر_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::سمتیہ<std::سمتیہ<قدر_3>> &buckets = all_buckets[thread.thread_عدد];
			سائز_t بالٹی = static_cast<سائز_t>(v.hash) % buckets.سائز();
			buckets[بالٹی].emplace_عقب(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		خالی process(ثابت Thread_اشاریہ &thread) {
			صحیح سائز = 0;
			برائے (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets)
				سائز += Get_سائز(buckets[thread.thread_عدد]);
			Shard &shard = shards[thread.thread_عدد];
			shard.reserve(سائز);
			برائے (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets) {
				برائے (قدر_3 &قدر : buckets[thread.thread_عدد])
					accumulate(قدر, shard);
				// Free as much memory as we can during the parallel phase.
				std::سمتیہ<قدر_3>().تبادلہ(buckets[thread.thread_عدد]);
			}
		}
	نجی:
		دوست class ShardedHashtable<V, کلید_Equality, CollisionHandler>;
		خالی accumulate(قدر_3 &قدر, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_قدر accumulated_قدر{std::move(قدر.قدر), قدر.hash};
			خودکار [it, inserted] = shard.داخل_کرو(std::move(accumulated_قدر));
			اگر (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->قدر), accumulated_قدر.قدر);
		}
		خالی init(صحیح عدد_دھاگے, کلید_Equality equality) {
			all_buckets.resize(عدد_دھاگے);
			برائے (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets)
				buckets.resize(عدد_دھاگے);
			برائے (صحیح i = 0; i < عدد_دھاگے; ++i)
				shards.emplace_عقب(0, Accumulated_قدر_Hash_Op(), Accumulated_قدر_Equality(equality));
		}
		ثابت CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::سمتیہ<std::سمتیہ<std::سمتیہ<قدر_3>>> all_buckets;
		std::سمتیہ<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		برائے (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : builder.all_buckets)
			برائے (std::سمتیہ<قدر_3> &بالٹی : buckets)
				log_assert(بالٹی.خالی_خانے());
		// Free memory.
		std::سمتیہ<std::سمتیہ<std::سمتیہ<قدر_3>>>().تبادلہ(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = طےشدہ;
	ShardedHashtable() {}

	ShardedHashtable &عامل=(ShardedHashtable &&other) = طےشدہ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *ڈھونڈو(ثابت Accumulated_قدر &v) ثابت {
		سائز_t عدد_shards = shards.سائز();
		اگر (عدد_shards == 0)
			واپس nullptr;
		سائز_t shard = static_cast<سائز_t>(v.hash) % عدد_shards;
		خودکار it = shards[shard].ڈھونڈو(v);
		اگر (it == shards[shard].انتہا())
			واپس nullptr;
		واپس &it->قدر;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	خالی داخل_کرو(Accumulated_قدر v) {
		سائز_t عدد_shards = shards.سائز();
		اگر (عدد_shards == 0)
			واپس;
		سائز_t shard = static_cast<سائز_t>(v.hash) % عدد_shards;
		shards[shard].داخل_کرو(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	خالی clear(ثابت Thread_اشاریہ &shard) {
		Accumulated_قدر_Equality equality = shards[shard.thread_عدد].کلید_eq();
		shards[shard.thread_عدد] = Shard(0, Accumulated_قدر_Hash_Op(), equality);
	}
نجی:
	std::سمتیہ<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
سانچہ <typename T>
class Concurrent_Work_قطار {
عوامی:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_قطار(صحیح عدد_دھاگے, صحیح batch_سائز = 100)
		: batch_سائز(batch_سائز), thread_حالتیں(عدد_دھاگے) {}
	صحیح عدد_دھاگے() ثابت { واپس Get_سائز(thread_حالتیں); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	خالی ڈالو(ثابت Thread_اشاریہ &thread, T work) {
		ThreadState &thread_state = thread_حالتیں[thread.thread_عدد];
		thread_state.اگلا_batch.emplace_عقب(std::move(work));
		اگر (Get_سائز(thread_state.اگلا_batch) < batch_سائز)
			واپس;
		bool was_خالی_خانے;
		{
			Unique_تالا تالا(thread_state.batches_تالا);
			was_خالی_خانے = thread_state.batches.خالی_خانے();
			thread_state.batches.ڈالو_عقب(std::move(thread_state.اگلا_batch));
		}
		اگر (was_خالی_خانے) {
			Unique_تالا تالا(waiters_تالا);
			اگر (عدد_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::سمتیہ<T> نکالو_batch(ثابت Thread_اشاریہ &thread) {
		ThreadState &thread_state = thread_حالتیں[thread.thread_عدد];
		اگر (!thread_state.اگلا_batch.خالی_خانے())
			واپس std::move(thread_state.اگلا_batch);
		// Empty our own work queue first.
		{
			Unique_تالا تالا(thread_state.batches_تالا);
			اگر (!thread_state.batches.خالی_خانے()) {
				std::سمتیہ<T> batch = std::move(thread_state.batches.عقب());
				thread_state.batches.نکالو_عقب();
				واپس batch;
			}
		}
		// From here on in this function, our work queue is empty.
		جب (true) {
			std::سمتیہ<T> batch = try_steal(thread);
			اگر (!batch.خالی_خانے()) {
				واپس std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_تالا تالا(waiters_تالا);
			++عدد_waiters;
			اگر (عدد_waiters == عدد_دھاگے()) {
				waiters_cv.notify_all();
				واپس {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(تالا);
			اگر (عدد_waiters == عدد_دھاگے())
				واپس {};
			--عدد_waiters;
		}
	}
نجی:
	std::سمتیہ<T> try_steal(ثابت Thread_اشاریہ &thread) {
		برائے (صحیح i = 1; i < عدد_دھاگے(); i++) {
			صحیح other_thread_عدد = (thread.thread_عدد + i) % عدد_دھاگے();
			ThreadState &other_thread_state = thread_حالتیں[other_thread_عدد];
			Unique_تالا تالا(other_thread_state.batches_تالا);
			اگر (!other_thread_state.batches.خالی_خانے()) {
				std::سمتیہ<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.نکالو_front();
				واپس batch;
			}
		}
		واپس {};
	}

	صحیح batch_سائز;

	ساخت ThreadState {
		// Entirely thread-local.
		std::سمتیہ<T> اگلا_batch;

		Mutex batches_تالا;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::سمتیہ<T>> batches;
	};
	std::سمتیہ<ThreadState> thread_حالتیں;

	Mutex waiters_تالا;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	صحیح عدد_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
عوامی:
	MonotonicFlag() : قدر(false) {}
	bool بھرو() ثابت { واپس قدر.بھرو(std::حافظہ_ترتیب_relaxed); }
	خالی سیٹ() { قدر.store(true, std::حافظہ_ترتیب_relaxed); }
	bool سیٹ_and_return_old() {
		واپس قدر.exchange(true, std::حافظہ_ترتیب_relaxed);
	}
نجی:
	std::atomic<bool> قدر;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم // YOSYS_THREADING_H
