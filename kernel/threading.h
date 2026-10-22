#ضمن <deque>

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/utils.h"

#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <thread>
#جے_انت

#جے_بنا_تعریف YOSYS_THREADING_H
#تعریف_کرو YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_جندرا = std::unique_جندرا<Mutex>;
using جندرا_Guard = std::جندرا_guard<Mutex>;
#وإلا
بنية Mutex {
	فراغ جندرا() {}
	فراغ unlock() {}
	bool try_جندرا() { أرجع true; }
};
بنية CondVar {
	سانچا <class L> فراغ wait(L &) {}
	سانچا <class L, class P> فراغ wait(L &, P) {}
	فراغ notify_one() {}
	فراغ notify_all() {}
};
بنية Unique_جندرا {
	Unique_جندرا(Mutex &) {}
};
بنية جندرا_Guard {
	جندرا_Guard(Mutex &) {}
};
#جے_انت

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
سانچا <typename T>
class Concurrent_قطار
{
عام:
	Concurrent_قطار(صحيح سمرتھا = ودھ_توں_ودھ_عدد)
		: سمرتھا(سمرتھا) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	فراغ ڈالو_پٹھ(T t)
	{
		Unique_جندرا جندرا(mutex);
		not_بھریا_condition.wait(جندرا, [this] { أرجع static_cast<صحيح>(contents.ناپ()) < سمرتھا; });
		إذا (contents.خالی_تھاواں())
			not_خالی_تھاواں_condition.notify_one();
		log_assert(!closed);
		contents.ڈالو_پٹھ(std::move(t));
		إذا (static_cast<صحيح>(contents.ناپ()) < سمرتھا)
			not_بھریا_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	فراغ close()
	{
		Unique_جندرا جندرا(mutex);
		not_خالی_تھاواں_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> نکالو_front()
	{
		أرجع نکالو_front_اندرونی(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_نکالو_front()
	{
		أرجع نکالو_front_اندرونی(false);
	}
خاص:
	std::optional<T> نکالو_front_اندرونی(bool wait)
	{
		Unique_جندرا جندرا(mutex);
		إذا (wait) {
			not_خالی_تھاواں_condition.wait(جندرا, [this] { أرجع !contents.خالی_تھاواں() || closed; });
		}
		إذا (contents.خالی_تھاواں())
			أرجع std::nullopt;
		إذا (static_cast<صحيح>(contents.ناپ()) == سمرتھا)
			not_بھریا_condition.notify_one();
		T نتیجہ = std::move(contents.front());
		contents.نکالو_front();
		إذا (!contents.خالی_تھاواں())
			not_خالی_تھاواں_condition.notify_one();
		أرجع std::move(نتیجہ);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_بھریا_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_خالی_تھاواں_condition;
	std::deque<T> contents;
	صحيح سمرتھا;
	bool closed = false;
};

class DeferredLogs
{
عام:
	سانچا <typename... دلیلاں_3>
	فراغ لوگارتھم(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, دلیلاں_3... دلیلاں_2)
	{
		logs.ڈالو_پٹھ({fmt.format(دلیلاں_2...), false});
	}
	سانچا <typename... دلیلاں_3>
	فراغ log_error(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, دلیلاں_3... دلیلاں_2)
	{
		logs.ڈالو_پٹھ({fmt.format(دلیلاں_2...), true});
	}
	فراغ flush();
خاص:
	بنية سنیہا_2
	{
		std::string متن;
		bool error;
	};
	std::سمتیہ<سنیہا_2> logs;
};

class ThreadPool
{
عام:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساكن صحيح pool_ناپ(صحيح reserved_cores, صحيح max_کامہ_دھاگے);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساكن صحيح work_pool_ناپ(صحيح reserved_cores, صحيح work_units, صحيح work_units_فی_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحيح pool_ناپ, std::function<فراغ(صحيح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحيح عدد_دھاگے() ثابت
	{
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
		أرجع دھاگے.ناپ();
#وإلا
		أرجع 0;
#جے_انت
	}
خاص:
	std::function<فراغ(صحيح)> body;
#جے_تعریف_شدہ YOSYS_ENABLE_دھاگے
	std::سمتیہ<std::thread> دھاگے;
#جے_انت
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange شے_range_for_کامہ(صحيح عدد_اشیاء, صحيح thread_عدد, صحيح عدد_دھاگے);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
بنية Thread_اشاریہ {
	صحيح thread_عدد;
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
	ParallelDispatchThreadPool(صحيح pool_ناپ);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	بنية لگاتار_Ctx : عام Thread_اشاریہ {
		صحيح عدد_دھاگے;
		IntRange شے_range(صحيح عدد_اشیاء) ثابت {
			أرجع شے_range_for_کامہ(عدد_اشیاء, thread_عدد, عدد_دھاگے);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عام:
		Subpool(ParallelDispatchThreadPool &ماپا, صحيح max_دھاگے)
				: ماپا(ماپا), max_دھاگے(max_دھاگے) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحيح عدد_دھاگے() ثابت {
			أرجع ماپا.عدد_دھاگے(max_دھاگے);
		}
		فراغ لگاتار(std::function<فراغ(ثابت لگاتار_Ctx &)> work) {
			ماپا.لگاتار(std::move(work), max_دھاگے);
		}
		ParallelDispatchThreadPool &thread_pool() { أرجع ماپا; }
	خاص:
		ParallelDispatchThreadPool &ماپا;
		صحيح max_دھاگے;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	فراغ لگاتار(std::function<فراغ(ثابت لگاتار_Ctx &)> work) {
		لگاتار(std::move(work), ودھ_توں_ودھ_عدد);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحيح عدد_دھاگے() ثابت {
		أرجع عدد_دھاگے(ودھ_توں_ودھ_عدد);
	}
خاص:
	یار class Subpool;

	فراغ لگاتار(std::function<فراغ(ثابت لگاتار_Ctx &)> work, صحيح max_دھاگے);
	صحيح عدد_دھاگے(صحيح max_دھاگے) ثابت {
		أرجع std::min(عدد_کامہ_دھاگے + 1, std::max(1, max_دھاگے));
	}
	فراغ لگاتار_کامہ(صحيح thread_عدد);

	std::function<فراغ(ثابت لگاتار_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحيح عدد_کامہ_دھاگے;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحيح> عدد_سرگرم_کامہ_دھاگے = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_منزل_workers_signal_mutex;
	CondVar main_منزل_workers_signal_cv;
	std::سمتیہ<uint8_t> main_منزل_workers_signal;
	فراغ signal_workers_شروع() {
		Unique_جندرا جندرا(main_منزل_workers_signal_mutex);
		صحيح عدد_سرگرم_کامہ_دھاگے_2 = عدد_سرگرم_کامہ_دھاگے.لدو(std::حافظہ_ترتیب_relaxed);
		std::fill(main_منزل_workers_signal.begin(), main_منزل_workers_signal.begin() + عدد_سرگرم_کامہ_دھاگے_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_منزل_workers_signal_cv.notify_all();
	}
	فراغ کامہ_wait_for_شروع(صحيح thread_عدد) {
		Unique_جندرا جندرا(main_منزل_workers_signal_mutex);
		main_منزل_workers_signal_cv.wait(جندرا, [this, thread_عدد] { أرجع main_منزل_workers_signal[thread_عدد] > 0; });
		main_منزل_workers_signal[thread_عدد] = 0;
	}

	std::atomic<صحيح> پورا_workers = 0;
	Mutex workers_منزل_main_signal_mutex;
	CondVar workers_منزل_main_signal_cv;
	فراغ signal_کامہ_پورا() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحيح عدد_سرگرم_کامہ_دھاگے_2 = عدد_سرگرم_کامہ_دھاگے.لدو(std::حافظہ_ترتیب_relaxed);
		صحيح d = پورا_workers.fetch_جوڑو(1, std::حافظہ_ترتیب_release);
		إذا (d + 1 == عدد_سرگرم_کامہ_دھاگے_2) {
			Unique_جندرا جندرا(workers_منزل_main_signal_mutex);
			workers_منزل_main_signal_cv.notify_all();
		}
	}
	فراغ wait_for_workers_پورا() {
		Unique_جندرا جندرا(workers_منزل_main_signal_mutex);
		workers_منزل_main_signal_cv.wait(جندرا, [this] {
			صحيح عدد_سرگرم_کامہ_دھاگے_2 = عدد_سرگرم_کامہ_دھاگے.لدو(std::حافظہ_ترتیب_relaxed);
			أرجع پورا_workers.لدو(std::حافظہ_ترتیب_acquire) == عدد_سرگرم_کامہ_دھاگے_2;
		});
		پورا_workers.store(0, std::حافظہ_ترتیب_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

سانچا <class T>
class Concurrent_ڈھیر
{
عام:
	فراغ ڈالو_پٹھ(T &&t) {
		جندرا_Guard جندرا(mutex);
		contents.ڈالو_پٹھ(std::move(t));
	}
	std::optional<T> try_نکالو_پٹھ() {
		جندرا_Guard جندرا(mutex);
		إذا (contents.خالی_تھاواں())
			أرجع std::nullopt;
		T نتیجہ = std::move(contents.پٹھ());
		contents.نکالو_پٹھ();
		أرجع نتیجہ;
	}
خاص:
	Mutex mutex;
	std::سمتیہ<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
سانچا <typename T>
class Sharded_سمتیہ {
عام:
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
	فراغ داخل_کرو(ثابت Thread_اشاریہ &thread, T قدر) {
		buckets[thread.thread_عدد].emplace_پٹھ(std::move(قدر));
	}

	bool خالی_تھاواں() ثابت {
		لكل (ثابت std::سمتیہ<T> &بالٹی : buckets)
			إذا (!بالٹی.خالی_تھاواں())
				أرجع false;
		أرجع true;
	}

	using Buckets = std::سمتیہ<std::سمتیہ<T>>;
	class iterator {
	عام:
		iterator(typename Buckets::iterator بالٹی_it, typename Buckets::iterator بالٹی_سرا)
			: بالٹی_it(std::move(بالٹی_it)), بالٹی_سرا(std::move(بالٹی_سرا)) {
			إذا (بالٹی_it != بالٹی_سرا)
				inner_it = بالٹی_it->begin();
			normalize();
		}
		T& عامل*() ثابت { أرجع *inner_it.قدر(); }
		iterator &عامل++() {
			++*inner_it;
			normalize();
			أرجع *this;
		}
		bool عامل!=(ثابت iterator &other) ثابت {
			أرجع بالٹی_it != other.بالٹی_it || inner_it != other.inner_it;
		}
	خاص:
		فراغ normalize() {
			إذا (بالٹی_it == بالٹی_سرا)
				أرجع;
			طالما (inner_it == بالٹی_it->سرا()) {
				++بالٹی_it;
				إذا (بالٹی_it == بالٹی_سرا) {
					inner_it.reset();
					أرجع;
				}
				inner_it = بالٹی_it->begin();
			}
		}
		std::optional<typename std::سمتیہ<T>::iterator> inner_it;
		typename Buckets::iterator بالٹی_it;
		typename Buckets::iterator بالٹی_سرا;
	};
	iterator begin() { أرجع iterator(buckets.begin(), buckets.سرا()); }
	iterator سرا() { أرجع iterator(buckets.سرا(), buckets.سرا()); }
خاص:
	فراغ init(صحيح عدد_دھاگے) {
		buckets.resize(عدد_دھاگے);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
سانچا <typename V>
بنية سیٹ_Collision_Handler {
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
سانچا <typename V, typename کلید_Equality, typename CollisionHandler>
class ShardedHashtable {
عام:
	// A combination of a `V` and its hash value.
	بنية قدر_3 {
		قدر_3(V قدر, بنانشانی صحيح hash) : قدر(std::move(قدر)), hash(hash) {}
		قدر_3(قدر_3 &&) = افتراضي;
		قدر_3(ثابت قدر_3 &) = delete;
		قدر_3 &عامل=(ثابت قدر_3 &) = delete;
		V قدر;
		بنانشانی صحيح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	بنية Accumulated_قدر {
		Accumulated_قدر(typename V::Accumulated قدر, بنانشانی صحيح hash) : قدر(std::move(قدر)), hash(hash) {}
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
#جے_انت
		typename V::Accumulated قدر;
		بنانشانی صحيح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_قدر_Equality {
		کلید_Equality inner;
		Accumulated_قدر_Equality(ثابت کلید_Equality &inner) : inner(inner) {}
		bool عامل()(ثابت Accumulated_قدر &v1, ثابت Accumulated_قدر &v2) ثابت {
			أرجع inner(v1.قدر, v2.قدر);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_قدر_Hash_Op {
		ناپ_t عامل()(ثابت Accumulated_قدر &v) ثابت {
			أرجع static_cast<ناپ_t>(v.hash);
		}
	};
	using Shard = std::unordered_سیٹ<Accumulated_قدر, Accumulated_قدر_Hash_Op, Accumulated_قدر_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عام:
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
		فراغ داخل_کرو(ثابت Thread_اشاریہ &thread, قدر_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::سمتیہ<std::سمتیہ<قدر_3>> &buckets = all_buckets[thread.thread_عدد];
			ناپ_t بالٹی = static_cast<ناپ_t>(v.hash) % buckets.ناپ();
			buckets[بالٹی].emplace_پٹھ(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		فراغ process(ثابت Thread_اشاریہ &thread) {
			صحيح ناپ = 0;
			لكل (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets)
				ناپ += Get_ناپ(buckets[thread.thread_عدد]);
			Shard &shard = shards[thread.thread_عدد];
			shard.reserve(ناپ);
			لكل (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets) {
				لكل (قدر_3 &قدر : buckets[thread.thread_عدد])
					accumulate(قدر, shard);
				// Free as much memory as we can during the parallel phase.
				std::سمتیہ<قدر_3>().وٹاندرا(buckets[thread.thread_عدد]);
			}
		}
	خاص:
		یار class ShardedHashtable<V, کلید_Equality, CollisionHandler>;
		فراغ accumulate(قدر_3 &قدر, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_قدر accumulated_قدر{std::move(قدر.قدر), قدر.hash};
			خودکار [it, inserted] = shard.داخل_کرو(std::move(accumulated_قدر));
			إذا (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->قدر), accumulated_قدر.قدر);
		}
		فراغ init(صحيح عدد_دھاگے, کلید_Equality equality) {
			all_buckets.resize(عدد_دھاگے);
			لكل (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : all_buckets)
				buckets.resize(عدد_دھاگے);
			لكل (صحيح i = 0; i < عدد_دھاگے; ++i)
				shards.emplace_پٹھ(0, Accumulated_قدر_Hash_Op(), Accumulated_قدر_Equality(equality));
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
		لكل (std::سمتیہ<std::سمتیہ<قدر_3>> &buckets : builder.all_buckets)
			لكل (std::سمتیہ<قدر_3> &بالٹی : buckets)
				log_assert(بالٹی.خالی_تھاواں());
		// Free memory.
		std::سمتیہ<std::سمتیہ<std::سمتیہ<قدر_3>>>().وٹاندرا(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = افتراضي;
	ShardedHashtable() {}

	ShardedHashtable &عامل=(ShardedHashtable &&other) = افتراضي;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *لبھو(ثابت Accumulated_قدر &v) ثابت {
		ناپ_t عدد_shards = shards.ناپ();
		إذا (عدد_shards == 0)
			أرجع nullptr;
		ناپ_t shard = static_cast<ناپ_t>(v.hash) % عدد_shards;
		خودکار it = shards[shard].لبھو(v);
		إذا (it == shards[shard].سرا())
			أرجع nullptr;
		أرجع &it->قدر;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	فراغ داخل_کرو(Accumulated_قدر v) {
		ناپ_t عدد_shards = shards.ناپ();
		إذا (عدد_shards == 0)
			أرجع;
		ناپ_t shard = static_cast<ناپ_t>(v.hash) % عدد_shards;
		shards[shard].داخل_کرو(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	فراغ clear(ثابت Thread_اشاریہ &shard) {
		Accumulated_قدر_Equality equality = shards[shard.thread_عدد].کلید_eq();
		shards[shard.thread_عدد] = Shard(0, Accumulated_قدر_Hash_Op(), equality);
	}
خاص:
	std::سمتیہ<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
سانچا <typename T>
class Concurrent_Work_قطار {
عام:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_قطار(صحيح عدد_دھاگے, صحيح batch_ناپ = 100)
		: batch_ناپ(batch_ناپ), thread_حالتاں(عدد_دھاگے) {}
	صحيح عدد_دھاگے() ثابت { أرجع Get_ناپ(thread_حالتاں); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	فراغ ڈالو(ثابت Thread_اشاریہ &thread, T work) {
		ThreadState &thread_state = thread_حالتاں[thread.thread_عدد];
		thread_state.اگلا_batch.emplace_پٹھ(std::move(work));
		إذا (Get_ناپ(thread_state.اگلا_batch) < batch_ناپ)
			أرجع;
		bool was_خالی_تھاواں;
		{
			Unique_جندرا جندرا(thread_state.batches_جندرا);
			was_خالی_تھاواں = thread_state.batches.خالی_تھاواں();
			thread_state.batches.ڈالو_پٹھ(std::move(thread_state.اگلا_batch));
		}
		إذا (was_خالی_تھاواں) {
			Unique_جندرا جندرا(waiters_جندرا);
			إذا (عدد_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::سمتیہ<T> نکالو_batch(ثابت Thread_اشاریہ &thread) {
		ThreadState &thread_state = thread_حالتاں[thread.thread_عدد];
		إذا (!thread_state.اگلا_batch.خالی_تھاواں())
			أرجع std::move(thread_state.اگلا_batch);
		// Empty our own work queue first.
		{
			Unique_جندرا جندرا(thread_state.batches_جندرا);
			إذا (!thread_state.batches.خالی_تھاواں()) {
				std::سمتیہ<T> batch = std::move(thread_state.batches.پٹھ());
				thread_state.batches.نکالو_پٹھ();
				أرجع batch;
			}
		}
		// From here on in this function, our work queue is empty.
		طالما (true) {
			std::سمتیہ<T> batch = try_steal(thread);
			إذا (!batch.خالی_تھاواں()) {
				أرجع std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_جندرا جندرا(waiters_جندرا);
			++عدد_waiters;
			إذا (عدد_waiters == عدد_دھاگے()) {
				waiters_cv.notify_all();
				أرجع {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(جندرا);
			إذا (عدد_waiters == عدد_دھاگے())
				أرجع {};
			--عدد_waiters;
		}
	}
خاص:
	std::سمتیہ<T> try_steal(ثابت Thread_اشاریہ &thread) {
		لكل (صحيح i = 1; i < عدد_دھاگے(); i++) {
			صحيح other_thread_عدد = (thread.thread_عدد + i) % عدد_دھاگے();
			ThreadState &other_thread_state = thread_حالتاں[other_thread_عدد];
			Unique_جندرا جندرا(other_thread_state.batches_جندرا);
			إذا (!other_thread_state.batches.خالی_تھاواں()) {
				std::سمتیہ<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.نکالو_front();
				أرجع batch;
			}
		}
		أرجع {};
	}

	صحيح batch_ناپ;

	بنية ThreadState {
		// Entirely thread-local.
		std::سمتیہ<T> اگلا_batch;

		Mutex batches_جندرا;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::سمتیہ<T>> batches;
	};
	std::سمتیہ<ThreadState> thread_حالتاں;

	Mutex waiters_جندرا;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	صحيح عدد_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
عام:
	MonotonicFlag() : قدر(false) {}
	bool لدو() ثابت { أرجع قدر.لدو(std::حافظہ_ترتیب_relaxed); }
	فراغ سیٹ() { قدر.store(true, std::حافظہ_ترتیب_relaxed); }
	bool سیٹ_and_return_old() {
		أرجع قدر.exchange(true, std::حافظہ_ترتیب_relaxed);
	}
خاص:
	std::atomic<bool> قدر;
};

YOSYS_NAMESPACE_سرا

#جے_انت // YOSYS_THREADING_H
