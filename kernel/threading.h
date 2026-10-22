#ضمن <deque>

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/utils.h"

#که_تعریف_شوی YOSYS_ENABLE_تارونه
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <thread>
#د_که_پای

#که_نه_تعریف_شوی YOSYS_THREADING_H
#تعریف_کړه YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#که_تعریف_شوی YOSYS_ENABLE_تارونه
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_قلف = std::unique_قلف<Mutex>;
using قلف_Guard = std::قلف_guard<Mutex>;
#وإلا
بنية Mutex {
	فراغ قلف() {}
	فراغ unlock() {}
	bool try_قلف() { أرجع true; }
};
بنية CondVar {
	کينډۍ <class L> فراغ wait(L &) {}
	کينډۍ <class L, class P> فراغ wait(L &, P) {}
	فراغ notify_one() {}
	فراغ notify_all() {}
};
بنية Unique_قلف {
	Unique_قلف(Mutex &) {}
};
بنية قلف_Guard {
	قلف_Guard(Mutex &) {}
};
#د_که_پای

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
کينډۍ <typename T>
class Concurrent_کتار
{
عام:
	Concurrent_کتار(صحيح ظرفیت = لوی_عدد)
		: ظرفیت(ظرفیت) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	فراغ ټېل_شا(T t)
	{
		Unique_قلف قلف(mutex);
		not_ډک_condition.wait(قلف, [this] { أرجع static_cast<صحيح>(contents.اندازه()) < ظرفیت; });
		إذا (contents.تشه())
			not_تشه_condition.notify_one();
		log_assert(!closed);
		contents.ټېل_شا(std::move(t));
		إذا (static_cast<صحيح>(contents.اندازه()) < ظرفیت)
			not_ډک_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	فراغ close()
	{
		Unique_قلف قلف(mutex);
		not_تشه_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> راوباسه_front()
	{
		أرجع راوباسه_front_دننی(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_راوباسه_front()
	{
		أرجع راوباسه_front_دننی(false);
	}
خاص:
	std::optional<T> راوباسه_front_دننی(bool wait)
	{
		Unique_قلف قلف(mutex);
		إذا (wait) {
			not_تشه_condition.wait(قلف, [this] { أرجع !contents.تشه() || closed; });
		}
		إذا (contents.تشه())
			أرجع std::nullopt;
		إذا (static_cast<صحيح>(contents.اندازه()) == ظرفیت)
			not_ډک_condition.notify_one();
		T پایله = std::move(contents.front());
		contents.راوباسه_front();
		إذا (!contents.تشه())
			not_تشه_condition.notify_one();
		أرجع std::move(پایله);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_ډک_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_تشه_condition;
	std::deque<T> contents;
	صحيح ظرفیت;
	bool closed = false;
};

class DeferredLogs
{
عام:
	کينډۍ <typename... دلیلونه_3>
	فراغ لوګاریتم(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, دلیلونه_3... دلیلونه_2)
	{
		logs.ټېل_شا({fmt.format(دلیلونه_2...), false});
	}
	کينډۍ <typename... دلیلونه_3>
	فراغ log_error(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, دلیلونه_3... دلیلونه_2)
	{
		logs.ټېل_شا({fmt.format(دلیلونه_2...), true});
	}
	فراغ flush();
خاص:
	بنية پیغام_3
	{
		std::string متن;
		bool error;
	};
	std::ویکټور<پیغام_3> logs;
};

class ThreadPool
{
عام:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساكن صحيح pool_اندازه(صحيح reserved_cores, صحيح max_کارګر_تارونه);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساكن صحيح work_pool_اندازه(صحيح reserved_cores, صحيح work_units, صحيح work_units_فی_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحيح pool_اندازه, std::function<فراغ(صحيح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحيح عدد_تارونه() ثابت
	{
#که_تعریف_شوی YOSYS_ENABLE_تارونه
		أرجع تارونه.اندازه();
#وإلا
		أرجع 0;
#د_که_پای
	}
خاص:
	std::function<فراغ(صحيح)> body;
#که_تعریف_شوی YOSYS_ENABLE_تارونه
	std::ویکټور<std::thread> تارونه;
#د_که_پای
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange توکی_range_for_کارګر(صحيح عدد_توکي, صحيح thread_عدد, صحيح عدد_تارونه);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
بنية Thread_شاخص {
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
	ParallelDispatchThreadPool(صحيح pool_اندازه);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	بنية پرلپسې_Ctx : عام Thread_شاخص {
		صحيح عدد_تارونه;
		IntRange توکی_range(صحيح عدد_توکي) ثابت {
			أرجع توکی_range_for_کارګر(عدد_توکي, thread_عدد, عدد_تارونه);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عام:
		Subpool(ParallelDispatchThreadPool &پلار, صحيح max_تارونه)
				: پلار(پلار), max_تارونه(max_تارونه) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحيح عدد_تارونه() ثابت {
			أرجع پلار.عدد_تارونه(max_تارونه);
		}
		فراغ پرلپسې(std::function<فراغ(ثابت پرلپسې_Ctx &)> work) {
			پلار.پرلپسې(std::move(work), max_تارونه);
		}
		ParallelDispatchThreadPool &thread_pool() { أرجع پلار; }
	خاص:
		ParallelDispatchThreadPool &پلار;
		صحيح max_تارونه;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	فراغ پرلپسې(std::function<فراغ(ثابت پرلپسې_Ctx &)> work) {
		پرلپسې(std::move(work), لوی_عدد);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحيح عدد_تارونه() ثابت {
		أرجع عدد_تارونه(لوی_عدد);
	}
خاص:
	ملګری class Subpool;

	فراغ پرلپسې(std::function<فراغ(ثابت پرلپسې_Ctx &)> work, صحيح max_تارونه);
	صحيح عدد_تارونه(صحيح max_تارونه) ثابت {
		أرجع std::min(عدد_کارګر_تارونه + 1, std::max(1, max_تارونه));
	}
	فراغ پرلپسې_کارګر(صحيح thread_عدد);

	std::function<فراغ(ثابت پرلپسې_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحيح عدد_کارګر_تارونه;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحيح> عدد_فعال_کارګر_تارونه = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_منزل_workers_signal_mutex;
	CondVar main_منزل_workers_signal_cv;
	std::ویکټور<uint8_t> main_منزل_workers_signal;
	فراغ signal_workers_پیل() {
		Unique_قلف قلف(main_منزل_workers_signal_mutex);
		صحيح عدد_فعال_کارګر_تارونه_2 = عدد_فعال_کارګر_تارونه.بارول(std::حافظه_ترتیب_relaxed);
		std::fill(main_منزل_workers_signal.begin(), main_منزل_workers_signal.begin() + عدد_فعال_کارګر_تارونه_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_منزل_workers_signal_cv.notify_all();
	}
	فراغ کارګر_wait_for_پیل(صحيح thread_عدد) {
		Unique_قلف قلف(main_منزل_workers_signal_mutex);
		main_منزل_workers_signal_cv.wait(قلف, [this, thread_عدد] { أرجع main_منزل_workers_signal[thread_عدد] > 0; });
		main_منزل_workers_signal[thread_عدد] = 0;
	}

	std::atomic<صحيح> بشپړ_workers = 0;
	Mutex workers_منزل_main_signal_mutex;
	CondVar workers_منزل_main_signal_cv;
	فراغ signal_کارګر_بشپړ() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحيح عدد_فعال_کارګر_تارونه_2 = عدد_فعال_کارګر_تارونه.بارول(std::حافظه_ترتیب_relaxed);
		صحيح d = بشپړ_workers.fetch_جمع_کړه(1, std::حافظه_ترتیب_release);
		إذا (d + 1 == عدد_فعال_کارګر_تارونه_2) {
			Unique_قلف قلف(workers_منزل_main_signal_mutex);
			workers_منزل_main_signal_cv.notify_all();
		}
	}
	فراغ wait_for_workers_بشپړ() {
		Unique_قلف قلف(workers_منزل_main_signal_mutex);
		workers_منزل_main_signal_cv.wait(قلف, [this] {
			صحيح عدد_فعال_کارګر_تارونه_2 = عدد_فعال_کارګر_تارونه.بارول(std::حافظه_ترتیب_relaxed);
			أرجع بشپړ_workers.بارول(std::حافظه_ترتیب_acquire) == عدد_فعال_کارګر_تارونه_2;
		});
		بشپړ_workers.store(0, std::حافظه_ترتیب_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

کينډۍ <class T>
class Concurrent_ډېری
{
عام:
	فراغ ټېل_شا(T &&t) {
		قلف_Guard قلف(mutex);
		contents.ټېل_شا(std::move(t));
	}
	std::optional<T> try_راوباسه_شا() {
		قلف_Guard قلف(mutex);
		إذا (contents.تشه())
			أرجع std::nullopt;
		T پایله = std::move(contents.شا());
		contents.راوباسه_شا();
		أرجع پایله;
	}
خاص:
	Mutex mutex;
	std::ویکټور<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
کينډۍ <typename T>
class Sharded_ویکټور {
عام:
	Sharded_ویکټور(ثابت ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.عدد_تارونه());
	}
	Sharded_ویکټور(ثابت ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.عدد_تارونه());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	فراغ ورننباسه(ثابت Thread_شاخص &thread, T ارزښت) {
		buckets[thread.thread_عدد].emplace_شا(std::move(ارزښت));
	}

	bool تشه() ثابت {
		لكل (ثابت std::ویکټور<T> &لوښی : buckets)
			إذا (!لوښی.تشه())
				أرجع false;
		أرجع true;
	}

	using Buckets = std::ویکټور<std::ویکټور<T>>;
	class iterator {
	عام:
		iterator(typename Buckets::iterator لوښی_it, typename Buckets::iterator لوښی_پای_ټکی)
			: لوښی_it(std::move(لوښی_it)), لوښی_پای_ټکی(std::move(لوښی_پای_ټکی)) {
			إذا (لوښی_it != لوښی_پای_ټکی)
				inner_it = لوښی_it->begin();
			normalize();
		}
		T& چلوونکی*() ثابت { أرجع *inner_it.ارزښت(); }
		iterator &چلوونکی++() {
			++*inner_it;
			normalize();
			أرجع *this;
		}
		bool چلوونکی!=(ثابت iterator &other) ثابت {
			أرجع لوښی_it != other.لوښی_it || inner_it != other.inner_it;
		}
	خاص:
		فراغ normalize() {
			إذا (لوښی_it == لوښی_پای_ټکی)
				أرجع;
			طالما (inner_it == لوښی_it->پای_ټکی()) {
				++لوښی_it;
				إذا (لوښی_it == لوښی_پای_ټکی) {
					inner_it.reset();
					أرجع;
				}
				inner_it = لوښی_it->begin();
			}
		}
		std::optional<typename std::ویکټور<T>::iterator> inner_it;
		typename Buckets::iterator لوښی_it;
		typename Buckets::iterator لوښی_پای_ټکی;
	};
	iterator begin() { أرجع iterator(buckets.begin(), buckets.پای_ټکی()); }
	iterator پای_ټکی() { أرجع iterator(buckets.پای_ټکی(), buckets.پای_ټکی()); }
خاص:
	فراغ init(صحيح عدد_تارونه) {
		buckets.resize(عدد_تارونه);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
کينډۍ <typename V>
بنية سېټ_Collision_Handler {
	فراغ چلوونکی()(typename V::Accumulated &, typename V::Accumulated &) ثابت {}
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
کينډۍ <typename V, typename کلی_Equality, typename CollisionHandler>
class ShardedHashtable {
عام:
	// A combination of a `V` and its hash value.
	بنية ارزښت_3 {
		ارزښت_3(V ارزښت, بینښه صحيح hash) : ارزښت(std::move(ارزښت)), hash(hash) {}
		ارزښت_3(ارزښت_3 &&) = افتراضي;
		ارزښت_3(ثابت ارزښت_3 &) = delete;
		ارزښت_3 &چلوونکی=(ثابت ارزښت_3 &) = delete;
		V ارزښت;
		بینښه صحيح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	بنية Accumulated_ارزښت {
		Accumulated_ارزښت(typename V::Accumulated ارزښت, بینښه صحيح hash) : ارزښت(std::move(ارزښت)), hash(hash) {}
		Accumulated_ارزښت(Accumulated_ارزښت &&) = افتراضي;
#إذا defined(_MSC_VER)
		Accumulated_ارزښت(ثابت Accumulated_ارزښت &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_ارزښت &چلوونکی=(ثابت Accumulated_ارزښت &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			أرجع *this;
		}
#وإلا
		Accumulated_ارزښت(ثابت Accumulated_ارزښت &) = delete;
		Accumulated_ارزښت &چلوونکی=(ثابت Accumulated_ارزښت &) = delete;
#د_که_پای
		typename V::Accumulated ارزښت;
		بینښه صحيح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_ارزښت_Equality {
		کلی_Equality inner;
		Accumulated_ارزښت_Equality(ثابت کلی_Equality &inner) : inner(inner) {}
		bool چلوونکی()(ثابت Accumulated_ارزښت &v1, ثابت Accumulated_ارزښت &v2) ثابت {
			أرجع inner(v1.ارزښت, v2.ارزښت);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_ارزښت_Hash_Op {
		اندازه_t چلوونکی()(ثابت Accumulated_ارزښت &v) ثابت {
			أرجع static_cast<اندازه_t>(v.hash);
		}
	};
	using Shard = std::unordered_سېټ<Accumulated_ارزښت, Accumulated_ارزښت_Hash_Op, Accumulated_ارزښت_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عام:
		Builder(ثابت ParallelDispatchThreadPool &thread_pool, کلی_Equality equality = کلی_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.عدد_تارونه(), std::move(equality));
		}
		Builder(ثابت ParallelDispatchThreadPool::Subpool &thread_pool, کلی_Equality equality = کلی_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.عدد_تارونه(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		فراغ ورننباسه(ثابت Thread_شاخص &thread, ارزښت_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::ویکټور<std::ویکټور<ارزښت_3>> &buckets = all_buckets[thread.thread_عدد];
			اندازه_t لوښی = static_cast<اندازه_t>(v.hash) % buckets.اندازه();
			buckets[لوښی].emplace_شا(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		فراغ process(ثابت Thread_شاخص &thread) {
			صحيح اندازه = 0;
			لكل (std::ویکټور<std::ویکټور<ارزښت_3>> &buckets : all_buckets)
				اندازه += Get_اندازه(buckets[thread.thread_عدد]);
			Shard &shard = shards[thread.thread_عدد];
			shard.reserve(اندازه);
			لكل (std::ویکټور<std::ویکټور<ارزښت_3>> &buckets : all_buckets) {
				لكل (ارزښت_3 &ارزښت : buckets[thread.thread_عدد])
					accumulate(ارزښت, shard);
				// Free as much memory as we can during the parallel phase.
				std::ویکټور<ارزښت_3>().بدلون(buckets[thread.thread_عدد]);
			}
		}
	خاص:
		ملګری class ShardedHashtable<V, کلی_Equality, CollisionHandler>;
		فراغ accumulate(ارزښت_3 &ارزښت, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_ارزښت accumulated_ارزښت{std::move(ارزښت.ارزښت), ارزښت.hash};
			خپلکاری [it, inserted] = shard.ورننباسه(std::move(accumulated_ارزښت));
			إذا (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->ارزښت), accumulated_ارزښت.ارزښت);
		}
		فراغ init(صحيح عدد_تارونه, کلی_Equality equality) {
			all_buckets.resize(عدد_تارونه);
			لكل (std::ویکټور<std::ویکټور<ارزښت_3>> &buckets : all_buckets)
				buckets.resize(عدد_تارونه);
			لكل (صحيح i = 0; i < عدد_تارونه; ++i)
				shards.emplace_شا(0, Accumulated_ارزښت_Hash_Op(), Accumulated_ارزښت_Equality(equality));
		}
		ثابت CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::ویکټور<std::ویکټور<std::ویکټور<ارزښت_3>>> all_buckets;
		std::ویکټور<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		لكل (std::ویکټور<std::ویکټور<ارزښت_3>> &buckets : builder.all_buckets)
			لكل (std::ویکټور<ارزښت_3> &لوښی : buckets)
				log_assert(لوښی.تشه());
		// Free memory.
		std::ویکټور<std::ویکټور<std::ویکټور<ارزښت_3>>>().بدلون(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = افتراضي;
	ShardedHashtable() {}

	ShardedHashtable &چلوونکی=(ShardedHashtable &&other) = افتراضي;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *پیدا_کړه(ثابت Accumulated_ارزښت &v) ثابت {
		اندازه_t عدد_shards = shards.اندازه();
		إذا (عدد_shards == 0)
			أرجع nullptr;
		اندازه_t shard = static_cast<اندازه_t>(v.hash) % عدد_shards;
		خپلکاری it = shards[shard].پیدا_کړه(v);
		إذا (it == shards[shard].پای_ټکی())
			أرجع nullptr;
		أرجع &it->ارزښت;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	فراغ ورننباسه(Accumulated_ارزښت v) {
		اندازه_t عدد_shards = shards.اندازه();
		إذا (عدد_shards == 0)
			أرجع;
		اندازه_t shard = static_cast<اندازه_t>(v.hash) % عدد_shards;
		shards[shard].ورننباسه(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	فراغ clear(ثابت Thread_شاخص &shard) {
		Accumulated_ارزښت_Equality equality = shards[shard.thread_عدد].کلی_eq();
		shards[shard.thread_عدد] = Shard(0, Accumulated_ارزښت_Hash_Op(), equality);
	}
خاص:
	std::ویکټور<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
کينډۍ <typename T>
class Concurrent_Work_کتار {
عام:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_کتار(صحيح عدد_تارونه, صحيح batch_اندازه = 100)
		: batch_اندازه(batch_اندازه), thread_حالتونه(عدد_تارونه) {}
	صحيح عدد_تارونه() ثابت { أرجع Get_اندازه(thread_حالتونه); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	فراغ ټېل(ثابت Thread_شاخص &thread, T work) {
		ThreadState &thread_state = thread_حالتونه[thread.thread_عدد];
		thread_state.بل_batch.emplace_شا(std::move(work));
		إذا (Get_اندازه(thread_state.بل_batch) < batch_اندازه)
			أرجع;
		bool was_تشه;
		{
			Unique_قلف قلف(thread_state.batches_قلف);
			was_تشه = thread_state.batches.تشه();
			thread_state.batches.ټېل_شا(std::move(thread_state.بل_batch));
		}
		إذا (was_تشه) {
			Unique_قلف قلف(waiters_قلف);
			إذا (عدد_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::ویکټور<T> راوباسه_batch(ثابت Thread_شاخص &thread) {
		ThreadState &thread_state = thread_حالتونه[thread.thread_عدد];
		إذا (!thread_state.بل_batch.تشه())
			أرجع std::move(thread_state.بل_batch);
		// Empty our own work queue first.
		{
			Unique_قلف قلف(thread_state.batches_قلف);
			إذا (!thread_state.batches.تشه()) {
				std::ویکټور<T> batch = std::move(thread_state.batches.شا());
				thread_state.batches.راوباسه_شا();
				أرجع batch;
			}
		}
		// From here on in this function, our work queue is empty.
		طالما (true) {
			std::ویکټور<T> batch = try_steal(thread);
			إذا (!batch.تشه()) {
				أرجع std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_قلف قلف(waiters_قلف);
			++عدد_waiters;
			إذا (عدد_waiters == عدد_تارونه()) {
				waiters_cv.notify_all();
				أرجع {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(قلف);
			إذا (عدد_waiters == عدد_تارونه())
				أرجع {};
			--عدد_waiters;
		}
	}
خاص:
	std::ویکټور<T> try_steal(ثابت Thread_شاخص &thread) {
		لكل (صحيح i = 1; i < عدد_تارونه(); i++) {
			صحيح other_thread_عدد = (thread.thread_عدد + i) % عدد_تارونه();
			ThreadState &other_thread_state = thread_حالتونه[other_thread_عدد];
			Unique_قلف قلف(other_thread_state.batches_قلف);
			إذا (!other_thread_state.batches.تشه()) {
				std::ویکټور<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.راوباسه_front();
				أرجع batch;
			}
		}
		أرجع {};
	}

	صحيح batch_اندازه;

	بنية ThreadState {
		// Entirely thread-local.
		std::ویکټور<T> بل_batch;

		Mutex batches_قلف;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::ویکټور<T>> batches;
	};
	std::ویکټور<ThreadState> thread_حالتونه;

	Mutex waiters_قلف;
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
	MonotonicFlag() : ارزښت(false) {}
	bool بارول() ثابت { أرجع ارزښت.بارول(std::حافظه_ترتیب_relaxed); }
	فراغ سېټ() { ارزښت.store(true, std::حافظه_ترتیب_relaxed); }
	bool سېټ_and_return_old() {
		أرجع ارزښت.exchange(true, std::حافظه_ترتیب_relaxed);
	}
خاص:
	std::atomic<bool> ارزښت;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای // YOSYS_THREADING_H
