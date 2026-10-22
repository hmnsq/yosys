#ضمن <deque>

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/utils.h"

#اگر_تعریف_شده YOSYS_ENABLE_نخها
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <thread>
#پایان_اگر

#اگر_تعریف_نشده YOSYS_THREADING_H
#تعریف_کن YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#اگر_تعریف_شده YOSYS_ENABLE_نخها
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_قفل = std::unique_قفل<Mutex>;
using قفل_Guard = std::قفل_guard<Mutex>;
#وإلا
بنية Mutex {
	فراغ قفل() {}
	فراغ unlock() {}
	bool try_قفل() { أرجع true; }
};
بنية CondVar {
	قالب <class L> فراغ wait(L &) {}
	قالب <class L, class P> فراغ wait(L &, P) {}
	فراغ notify_one() {}
	فراغ notify_all() {}
};
بنية Unique_قفل {
	Unique_قفل(Mutex &) {}
};
بنية قفل_Guard {
	قفل_Guard(Mutex &) {}
};
#پایان_اگر

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
قالب <typename T>
class Concurrent_صف
{
عام:
	Concurrent_صف(صحيح ظرفیت = بیشترین_صحیح)
		: ظرفیت(ظرفیت) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	فراغ افزودن_پشت(T t)
	{
		Unique_قفل قفل(mutex);
		not_پر_condition.wait(قفل, [this] { أرجع static_cast<صحيح>(contents.اندازه()) < ظرفیت; });
		إذا (contents.خالی())
			not_خالی_condition.notify_one();
		log_assert(!closed);
		contents.افزودن_پشت(std::move(t));
		إذا (static_cast<صحيح>(contents.اندازه()) < ظرفیت)
			not_پر_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	فراغ close()
	{
		Unique_قفل قفل(mutex);
		not_خالی_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> برداشت_front()
	{
		أرجع برداشت_front_داخلی(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_برداشت_front()
	{
		أرجع برداشت_front_داخلی(false);
	}
خاص:
	std::optional<T> برداشت_front_داخلی(bool wait)
	{
		Unique_قفل قفل(mutex);
		إذا (wait) {
			not_خالی_condition.wait(قفل, [this] { أرجع !contents.خالی() || closed; });
		}
		إذا (contents.خالی())
			أرجع std::nullopt;
		إذا (static_cast<صحيح>(contents.اندازه()) == ظرفیت)
			not_پر_condition.notify_one();
		T نتیجه = std::move(contents.front());
		contents.برداشت_front();
		إذا (!contents.خالی())
			not_خالی_condition.notify_one();
		أرجع std::move(نتیجه);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_پر_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_خالی_condition;
	std::deque<T> contents;
	صحيح ظرفیت;
	bool closed = false;
};

class DeferredLogs
{
عام:
	قالب <typename... آرگومانها_3>
	فراغ لگاریتم(FmtString<Type_عنصر_خنثی<آرگومانها_3>...> fmt, آرگومانها_3... آرگومانها_2)
	{
		logs.افزودن_پشت({fmt.format(آرگومانها_2...), false});
	}
	قالب <typename... آرگومانها_3>
	فراغ log_error(FmtString<Type_عنصر_خنثی<آرگومانها_3>...> fmt, آرگومانها_3... آرگومانها_2)
	{
		logs.افزودن_پشت({fmt.format(آرگومانها_2...), true});
	}
	فراغ flush();
خاص:
	بنية پیام_3
	{
		std::string متن;
		bool error;
	};
	std::وکتور<پیام_3> logs;
};

class ThreadPool
{
عام:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساكن صحيح pool_اندازه(صحيح reserved_cores, صحيح max_کارگر_نخها);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساكن صحيح work_pool_اندازه(صحيح reserved_cores, صحيح work_units, صحيح work_units_در_هر_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحيح pool_اندازه, std::function<فراغ(صحيح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحيح رقم_نخها() ثابت
	{
#اگر_تعریف_شده YOSYS_ENABLE_نخها
		أرجع نخها.اندازه();
#وإلا
		أرجع 0;
#پایان_اگر
	}
خاص:
	std::function<فراغ(صحيح)> body;
#اگر_تعریف_شده YOSYS_ENABLE_نخها
	std::وکتور<std::thread> نخها;
#پایان_اگر
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange قلم_range_for_کارگر(صحيح رقم_اقلام, صحيح thread_رقم, صحيح رقم_نخها);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
بنية Thread_شاخص {
	صحيح thread_رقم;
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
	ParallelDispatchThreadPool(صحيح pool_اندازه);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	بنية پیاپی_Ctx : عام Thread_شاخص {
		صحيح رقم_نخها;
		IntRange قلم_range(صحيح رقم_اقلام) ثابت {
			أرجع قلم_range_for_کارگر(رقم_اقلام, thread_رقم, رقم_نخها);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عام:
		Subpool(ParallelDispatchThreadPool &والد, صحيح max_نخها)
				: والد(والد), max_نخها(max_نخها) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحيح رقم_نخها() ثابت {
			أرجع والد.رقم_نخها(max_نخها);
		}
		فراغ پیاپی(std::function<فراغ(ثابت پیاپی_Ctx &)> work) {
			والد.پیاپی(std::move(work), max_نخها);
		}
		ParallelDispatchThreadPool &thread_pool() { أرجع والد; }
	خاص:
		ParallelDispatchThreadPool &والد;
		صحيح max_نخها;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	فراغ پیاپی(std::function<فراغ(ثابت پیاپی_Ctx &)> work) {
		پیاپی(std::move(work), بیشترین_صحیح);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحيح رقم_نخها() ثابت {
		أرجع رقم_نخها(بیشترین_صحیح);
	}
خاص:
	دوست class Subpool;

	فراغ پیاپی(std::function<فراغ(ثابت پیاپی_Ctx &)> work, صحيح max_نخها);
	صحيح رقم_نخها(صحيح max_نخها) ثابت {
		أرجع std::min(رقم_کارگر_نخها + 1, std::max(1, max_نخها));
	}
	فراغ پیاپی_کارگر(صحيح thread_رقم);

	std::function<فراغ(ثابت پیاپی_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحيح رقم_کارگر_نخها;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحيح> رقم_فعال_کارگر_نخها = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_مقصد_workers_signal_mutex;
	CondVar main_مقصد_workers_signal_cv;
	std::وکتور<uint8_t> main_مقصد_workers_signal;
	فراغ signal_workers_شروع() {
		Unique_قفل قفل(main_مقصد_workers_signal_mutex);
		صحيح رقم_فعال_کارگر_نخها_2 = رقم_فعال_کارگر_نخها.بارگیری(std::حافظه_ترتیب_relaxed);
		std::fill(main_مقصد_workers_signal.begin(), main_مقصد_workers_signal.begin() + رقم_فعال_کارگر_نخها_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_مقصد_workers_signal_cv.notify_all();
	}
	فراغ کارگر_wait_for_شروع(صحيح thread_رقم) {
		Unique_قفل قفل(main_مقصد_workers_signal_mutex);
		main_مقصد_workers_signal_cv.wait(قفل, [this, thread_رقم] { أرجع main_مقصد_workers_signal[thread_رقم] > 0; });
		main_مقصد_workers_signal[thread_رقم] = 0;
	}

	std::atomic<صحيح> انجامشده_workers = 0;
	Mutex workers_مقصد_main_signal_mutex;
	CondVar workers_مقصد_main_signal_cv;
	فراغ signal_کارگر_انجامشده() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحيح رقم_فعال_کارگر_نخها_2 = رقم_فعال_کارگر_نخها.بارگیری(std::حافظه_ترتیب_relaxed);
		صحيح d = انجامشده_workers.fetch_جمع_کن(1, std::حافظه_ترتیب_release);
		إذا (d + 1 == رقم_فعال_کارگر_نخها_2) {
			Unique_قفل قفل(workers_مقصد_main_signal_mutex);
			workers_مقصد_main_signal_cv.notify_all();
		}
	}
	فراغ wait_for_workers_انجامشده() {
		Unique_قفل قفل(workers_مقصد_main_signal_mutex);
		workers_مقصد_main_signal_cv.wait(قفل, [this] {
			صحيح رقم_فعال_کارگر_نخها_2 = رقم_فعال_کارگر_نخها.بارگیری(std::حافظه_ترتیب_relaxed);
			أرجع انجامشده_workers.بارگیری(std::حافظه_ترتیب_acquire) == رقم_فعال_کارگر_نخها_2;
		});
		انجامشده_workers.store(0, std::حافظه_ترتیب_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

قالب <class T>
class Concurrent_پشته
{
عام:
	فراغ افزودن_پشت(T &&t) {
		قفل_Guard قفل(mutex);
		contents.افزودن_پشت(std::move(t));
	}
	std::optional<T> try_برداشت_پشت() {
		قفل_Guard قفل(mutex);
		إذا (contents.خالی())
			أرجع std::nullopt;
		T نتیجه = std::move(contents.پشت());
		contents.برداشت_پشت();
		أرجع نتیجه;
	}
خاص:
	Mutex mutex;
	std::وکتور<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
قالب <typename T>
class Sharded_وکتور {
عام:
	Sharded_وکتور(ثابت ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.رقم_نخها());
	}
	Sharded_وکتور(ثابت ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.رقم_نخها());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	فراغ درج(ثابت Thread_شاخص &thread, T مقدار) {
		buckets[thread.thread_رقم].emplace_پشت(std::move(مقدار));
	}

	bool خالی() ثابت {
		لكل (ثابت std::وکتور<T> &سطل : buckets)
			إذا (!سطل.خالی())
				أرجع false;
		أرجع true;
	}

	using Buckets = std::وکتور<std::وکتور<T>>;
	class iterator {
	عام:
		iterator(typename Buckets::iterator سطل_it, typename Buckets::iterator سطل_انتها)
			: سطل_it(std::move(سطل_it)), سطل_انتها(std::move(سطل_انتها)) {
			إذا (سطل_it != سطل_انتها)
				inner_it = سطل_it->begin();
			normalize();
		}
		T& عملگر*() ثابت { أرجع *inner_it.مقدار(); }
		iterator &عملگر++() {
			++*inner_it;
			normalize();
			أرجع *this;
		}
		bool عملگر!=(ثابت iterator &other) ثابت {
			أرجع سطل_it != other.سطل_it || inner_it != other.inner_it;
		}
	خاص:
		فراغ normalize() {
			إذا (سطل_it == سطل_انتها)
				أرجع;
			طالما (inner_it == سطل_it->انتها()) {
				++سطل_it;
				إذا (سطل_it == سطل_انتها) {
					inner_it.reset();
					أرجع;
				}
				inner_it = سطل_it->begin();
			}
		}
		std::optional<typename std::وکتور<T>::iterator> inner_it;
		typename Buckets::iterator سطل_it;
		typename Buckets::iterator سطل_انتها;
	};
	iterator begin() { أرجع iterator(buckets.begin(), buckets.انتها()); }
	iterator انتها() { أرجع iterator(buckets.انتها(), buckets.انتها()); }
خاص:
	فراغ init(صحيح رقم_نخها) {
		buckets.resize(رقم_نخها);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
قالب <typename V>
بنية مجموعه_Collision_Handler {
	فراغ عملگر()(typename V::Accumulated &, typename V::Accumulated &) ثابت {}
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
قالب <typename V, typename کلید_Equality, typename CollisionHandler>
class ShardedHashtable {
عام:
	// A combination of a `V` and its hash value.
	بنية مقدار_3 {
		مقدار_3(V مقدار, بیعلامت صحيح hash) : مقدار(std::move(مقدار)), hash(hash) {}
		مقدار_3(مقدار_3 &&) = افتراضي;
		مقدار_3(ثابت مقدار_3 &) = delete;
		مقدار_3 &عملگر=(ثابت مقدار_3 &) = delete;
		V مقدار;
		بیعلامت صحيح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	بنية Accumulated_مقدار {
		Accumulated_مقدار(typename V::Accumulated مقدار, بیعلامت صحيح hash) : مقدار(std::move(مقدار)), hash(hash) {}
		Accumulated_مقدار(Accumulated_مقدار &&) = افتراضي;
#إذا defined(_MSC_VER)
		Accumulated_مقدار(ثابت Accumulated_مقدار &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_مقدار &عملگر=(ثابت Accumulated_مقدار &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			أرجع *this;
		}
#وإلا
		Accumulated_مقدار(ثابت Accumulated_مقدار &) = delete;
		Accumulated_مقدار &عملگر=(ثابت Accumulated_مقدار &) = delete;
#پایان_اگر
		typename V::Accumulated مقدار;
		بیعلامت صحيح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_مقدار_Equality {
		کلید_Equality inner;
		Accumulated_مقدار_Equality(ثابت کلید_Equality &inner) : inner(inner) {}
		bool عملگر()(ثابت Accumulated_مقدار &v1, ثابت Accumulated_مقدار &v2) ثابت {
			أرجع inner(v1.مقدار, v2.مقدار);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_مقدار_Hash_Op {
		اندازه_t عملگر()(ثابت Accumulated_مقدار &v) ثابت {
			أرجع static_cast<اندازه_t>(v.hash);
		}
	};
	using Shard = std::unordered_مجموعه<Accumulated_مقدار, Accumulated_مقدار_Hash_Op, Accumulated_مقدار_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عام:
		Builder(ثابت ParallelDispatchThreadPool &thread_pool, کلید_Equality equality = کلید_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.رقم_نخها(), std::move(equality));
		}
		Builder(ثابت ParallelDispatchThreadPool::Subpool &thread_pool, کلید_Equality equality = کلید_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.رقم_نخها(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		فراغ درج(ثابت Thread_شاخص &thread, مقدار_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::وکتور<std::وکتور<مقدار_3>> &buckets = all_buckets[thread.thread_رقم];
			اندازه_t سطل = static_cast<اندازه_t>(v.hash) % buckets.اندازه();
			buckets[سطل].emplace_پشت(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		فراغ process(ثابت Thread_شاخص &thread) {
			صحيح اندازه = 0;
			لكل (std::وکتور<std::وکتور<مقدار_3>> &buckets : all_buckets)
				اندازه += Get_اندازه(buckets[thread.thread_رقم]);
			Shard &shard = shards[thread.thread_رقم];
			shard.reserve(اندازه);
			لكل (std::وکتور<std::وکتور<مقدار_3>> &buckets : all_buckets) {
				لكل (مقدار_3 &مقدار : buckets[thread.thread_رقم])
					accumulate(مقدار, shard);
				// Free as much memory as we can during the parallel phase.
				std::وکتور<مقدار_3>().جابجا(buckets[thread.thread_رقم]);
			}
		}
	خاص:
		دوست class ShardedHashtable<V, کلید_Equality, CollisionHandler>;
		فراغ accumulate(مقدار_3 &مقدار, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_مقدار accumulated_مقدار{std::move(مقدار.مقدار), مقدار.hash};
			خودکار [it, inserted] = shard.درج(std::move(accumulated_مقدار));
			إذا (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->مقدار), accumulated_مقدار.مقدار);
		}
		فراغ init(صحيح رقم_نخها, کلید_Equality equality) {
			all_buckets.resize(رقم_نخها);
			لكل (std::وکتور<std::وکتور<مقدار_3>> &buckets : all_buckets)
				buckets.resize(رقم_نخها);
			لكل (صحيح i = 0; i < رقم_نخها; ++i)
				shards.emplace_پشت(0, Accumulated_مقدار_Hash_Op(), Accumulated_مقدار_Equality(equality));
		}
		ثابت CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::وکتور<std::وکتور<std::وکتور<مقدار_3>>> all_buckets;
		std::وکتور<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		لكل (std::وکتور<std::وکتور<مقدار_3>> &buckets : builder.all_buckets)
			لكل (std::وکتور<مقدار_3> &سطل : buckets)
				log_assert(سطل.خالی());
		// Free memory.
		std::وکتور<std::وکتور<std::وکتور<مقدار_3>>>().جابجا(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = افتراضي;
	ShardedHashtable() {}

	ShardedHashtable &عملگر=(ShardedHashtable &&other) = افتراضي;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *پیدا_کن(ثابت Accumulated_مقدار &v) ثابت {
		اندازه_t رقم_shards = shards.اندازه();
		إذا (رقم_shards == 0)
			أرجع nullptr;
		اندازه_t shard = static_cast<اندازه_t>(v.hash) % رقم_shards;
		خودکار it = shards[shard].پیدا_کن(v);
		إذا (it == shards[shard].انتها())
			أرجع nullptr;
		أرجع &it->مقدار;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	فراغ درج(Accumulated_مقدار v) {
		اندازه_t رقم_shards = shards.اندازه();
		إذا (رقم_shards == 0)
			أرجع;
		اندازه_t shard = static_cast<اندازه_t>(v.hash) % رقم_shards;
		shards[shard].درج(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	فراغ clear(ثابت Thread_شاخص &shard) {
		Accumulated_مقدار_Equality equality = shards[shard.thread_رقم].کلید_eq();
		shards[shard.thread_رقم] = Shard(0, Accumulated_مقدار_Hash_Op(), equality);
	}
خاص:
	std::وکتور<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
قالب <typename T>
class Concurrent_Work_صف {
عام:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_صف(صحيح رقم_نخها, صحيح batch_اندازه = 100)
		: batch_اندازه(batch_اندازه), thread_حالتها(رقم_نخها) {}
	صحيح رقم_نخها() ثابت { أرجع Get_اندازه(thread_حالتها); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	فراغ افزودن(ثابت Thread_شاخص &thread, T work) {
		ThreadState &thread_state = thread_حالتها[thread.thread_رقم];
		thread_state.بعدی_batch.emplace_پشت(std::move(work));
		إذا (Get_اندازه(thread_state.بعدی_batch) < batch_اندازه)
			أرجع;
		bool was_خالی;
		{
			Unique_قفل قفل(thread_state.batches_قفل);
			was_خالی = thread_state.batches.خالی();
			thread_state.batches.افزودن_پشت(std::move(thread_state.بعدی_batch));
		}
		إذا (was_خالی) {
			Unique_قفل قفل(waiters_قفل);
			إذا (رقم_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::وکتور<T> برداشت_batch(ثابت Thread_شاخص &thread) {
		ThreadState &thread_state = thread_حالتها[thread.thread_رقم];
		إذا (!thread_state.بعدی_batch.خالی())
			أرجع std::move(thread_state.بعدی_batch);
		// Empty our own work queue first.
		{
			Unique_قفل قفل(thread_state.batches_قفل);
			إذا (!thread_state.batches.خالی()) {
				std::وکتور<T> batch = std::move(thread_state.batches.پشت());
				thread_state.batches.برداشت_پشت();
				أرجع batch;
			}
		}
		// From here on in this function, our work queue is empty.
		طالما (true) {
			std::وکتور<T> batch = try_steal(thread);
			إذا (!batch.خالی()) {
				أرجع std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_قفل قفل(waiters_قفل);
			++رقم_waiters;
			إذا (رقم_waiters == رقم_نخها()) {
				waiters_cv.notify_all();
				أرجع {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(قفل);
			إذا (رقم_waiters == رقم_نخها())
				أرجع {};
			--رقم_waiters;
		}
	}
خاص:
	std::وکتور<T> try_steal(ثابت Thread_شاخص &thread) {
		لكل (صحيح i = 1; i < رقم_نخها(); i++) {
			صحيح other_thread_رقم = (thread.thread_رقم + i) % رقم_نخها();
			ThreadState &other_thread_state = thread_حالتها[other_thread_رقم];
			Unique_قفل قفل(other_thread_state.batches_قفل);
			إذا (!other_thread_state.batches.خالی()) {
				std::وکتور<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.برداشت_front();
				أرجع batch;
			}
		}
		أرجع {};
	}

	صحيح batch_اندازه;

	بنية ThreadState {
		// Entirely thread-local.
		std::وکتور<T> بعدی_batch;

		Mutex batches_قفل;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::وکتور<T>> batches;
	};
	std::وکتور<ThreadState> thread_حالتها;

	Mutex waiters_قفل;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	صحيح رقم_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
عام:
	MonotonicFlag() : مقدار(false) {}
	bool بارگیری() ثابت { أرجع مقدار.بارگیری(std::حافظه_ترتیب_relaxed); }
	فراغ مجموعه() { مقدار.store(true, std::حافظه_ترتیب_relaxed); }
	bool مجموعه_and_return_old() {
		أرجع مقدار.exchange(true, std::حافظه_ترتیب_relaxed);
	}
خاص:
	std::atomic<bool> مقدار;
};

YOSYS_NAMESPACE_انتها

#پایان_اگر // YOSYS_THREADING_H
