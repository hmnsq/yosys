#ضمن <deque>

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <thread>
#endif

#ifndef YOSYS_THREADING_H
#define YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#ifdef YOSYS_ENABLE_THREADS
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using UniqueLock = std::unique_lock<Mutex>;
using LockGuard = std::lock_guard<Mutex>;
#وإلا
بنية Mutex {
	فراغ lock() {}
	فراغ unlock() {}
	bool try_lock() { أرجع true; }
};
بنية CondVar {
	template <class L> فراغ wait(L &) {}
	template <class L, class P> فراغ wait(L &, P) {}
	فراغ notify_one() {}
	فراغ notify_all() {}
};
بنية UniqueLock {
	UniqueLock(Mutex &) {}
};
بنية LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
عام:
	ConcurrentQueue(صحيح capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	فراغ تولق_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { أرجع static_cast<صحيح>(contents.size()) < capacity; });
		إذا (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.تولق_back(std::move(t));
		إذا (static_cast<صحيح>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	فراغ close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> کلوار_front()
	{
		أرجع کلوار_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_کلوار_front()
	{
		أرجع کلوار_front_internal(false);
	}
خاص:
	std::optional<T> کلوار_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		إذا (wait) {
			not_empty_condition.wait(lock, [this] { أرجع !contents.empty() || closed; });
		}
		إذا (contents.empty())
			أرجع std::nullopt;
		إذا (static_cast<صحيح>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.کلوار_front();
		إذا (!contents.empty())
			not_empty_condition.notify_one();
		أرجع std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	صحيح capacity;
	bool closed = false;
};

class DeferredLogs
{
عام:
	template <typename... Args>
	فراغ log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.تولق_back({fmt.format(args...), false});
	}
	template <typename... Args>
	فراغ log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.تولق_back({fmt.format(args...), true});
	}
	فراغ flush();
خاص:
	بنية Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
عام:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ساكن صحيح pool_size(صحيح reserved_cores, صحيح max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ساكن صحيح work_pool_size(صحيح reserved_cores, صحيح work_units, صحيح work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(صحيح pool_size, std::function<فراغ(صحيح)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	صحيح اڠک_threads() ثابت
	{
#ifdef YOSYS_ENABLE_THREADS
		أرجع threads.size();
#وإلا
		أرجع 0;
#endif
	}
خاص:
	std::function<فراغ(صحيح)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(صحيح اڠک_بارڠن, صحيح thread_اڠک, صحيح اڠک_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
بنية ThreadIndex {
	صحيح thread_اڠک;
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
	ParallelDispatchThreadPool(صحيح pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	بنية RunCtx : عام ThreadIndex {
		صحيح اڠک_threads;
		IntRange item_range(صحيح اڠک_بارڠن) ثابت {
			أرجع item_range_for_worker(اڠک_بارڠن, thread_اڠک, اڠک_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	عام:
		Subpool(ParallelDispatchThreadPool &parent, صحيح max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		صحيح اڠک_threads() ثابت {
			أرجع parent.اڠک_threads(max_threads);
		}
		فراغ run(std::function<فراغ(ثابت RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { أرجع parent; }
	خاص:
		ParallelDispatchThreadPool &parent;
		صحيح max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	فراغ run(std::function<فراغ(ثابت RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	صحيح اڠک_threads() ثابت {
		أرجع اڠک_threads(INT_MAX);
	}
خاص:
	friend class Subpool;

	فراغ run(std::function<فراغ(ثابت RunCtx &)> work, صحيح max_threads);
	صحيح اڠک_threads(صحيح max_threads) ثابت {
		أرجع std::min(اڠک_worker_threads + 1, std::max(1, max_threads));
	}
	فراغ run_worker(صحيح thread_اڠک);

	std::function<فراغ(ثابت RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	صحيح اڠک_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<صحيح> اڠک_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	فراغ signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		صحيح اڠک_active_worker_threads_2 = اڠک_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + اڠک_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	فراغ worker_wait_for_start(صحيح thread_اڠک) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_اڠک] { أرجع main_to_workers_signal[thread_اڠک] > 0; });
		main_to_workers_signal[thread_اڠک] = 0;
	}

	std::atomic<صحيح> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	فراغ signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		صحيح اڠک_active_worker_threads_2 = اڠک_active_worker_threads.load(std::memory_order_relaxed);
		صحيح d = done_workers.fetch_add(1, std::memory_order_release);
		إذا (d + 1 == اڠک_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	فراغ wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			صحيح اڠک_active_worker_threads_2 = اڠک_active_worker_threads.load(std::memory_order_relaxed);
			أرجع done_workers.load(std::memory_order_acquire) == اڠک_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_تيندنن
{
عام:
	فراغ تولق_back(T &&t) {
		LockGuard lock(mutex);
		contents.تولق_back(std::move(t));
	}
	std::optional<T> try_کلوار_back() {
		LockGuard lock(mutex);
		إذا (contents.empty())
			أرجع std::nullopt;
		T result = std::move(contents.back());
		contents.کلوار_back();
		أرجع result;
	}
خاص:
	Mutex mutex;
	std::vector<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
template <typename T>
class ShardedVector {
عام:
	ShardedVector(ثابت ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.اڠک_threads());
	}
	ShardedVector(ثابت ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.اڠک_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	فراغ سيسيڤ(ثابت ThreadIndex &thread, T نيلاي) {
		buckets[thread.thread_اڠک].emplace_back(std::move(نيلاي));
	}

	bool empty() ثابت {
		لكل (ثابت std::vector<T> &bucket : buckets)
			إذا (!bucket.empty())
				أرجع false;
		أرجع true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	عام:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			إذا (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() ثابت { أرجع *inner_it.نيلاي(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			أرجع *this;
		}
		bool operator!=(ثابت iterator &other) ثابت {
			أرجع bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	خاص:
		فراغ normalize() {
			إذا (bucket_it == bucket_end)
				أرجع;
			طالما (inner_it == bucket_it->end()) {
				++bucket_it;
				إذا (bucket_it == bucket_end) {
					inner_it.reset();
					أرجع;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { أرجع iterator(buckets.begin(), buckets.end()); }
	iterator end() { أرجع iterator(buckets.end(), buckets.end()); }
خاص:
	فراغ init(صحيح اڠک_threads) {
		buckets.resize(اڠک_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
بنية SetCollisionHandler {
	فراغ operator()(typename V::Accumulated &, typename V::Accumulated &) ثابت {}
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
template <typename V, typename کونچي_Equality, typename CollisionHandler>
class ShardedHashtable {
عام:
	// A combination of a `V` and its hash value.
	بنية نيلاي_3 {
		نيلاي_3(V نيلاي, unsigned صحيح hash) : نيلاي(std::move(نيلاي)), hash(hash) {}
		نيلاي_3(نيلاي_3 &&) = افتراضي;
		نيلاي_3(ثابت نيلاي_3 &) = delete;
		نيلاي_3 &operator=(ثابت نيلاي_3 &) = delete;
		V نيلاي;
		unsigned صحيح hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	بنية Accumulated_نيلاي {
		Accumulated_نيلاي(typename V::Accumulated نيلاي, unsigned صحيح hash) : نيلاي(std::move(نيلاي)), hash(hash) {}
		Accumulated_نيلاي(Accumulated_نيلاي &&) = افتراضي;
#إذا defined(_MSC_VER)
		Accumulated_نيلاي(ثابت Accumulated_نيلاي &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_نيلاي &operator=(ثابت Accumulated_نيلاي &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			أرجع *this;
		}
#وإلا
		Accumulated_نيلاي(ثابت Accumulated_نيلاي &) = delete;
		Accumulated_نيلاي &operator=(ثابت Accumulated_نيلاي &) = delete;
#endif
		typename V::Accumulated نيلاي;
		unsigned صحيح hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_نيلاي_Equality {
		کونچي_Equality inner;
		Accumulated_نيلاي_Equality(ثابت کونچي_Equality &inner) : inner(inner) {}
		bool operator()(ثابت Accumulated_نيلاي &v1, ثابت Accumulated_نيلاي &v2) ثابت {
			أرجع inner(v1.نيلاي, v2.نيلاي);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	بنية Accumulated_نيلاي_Hash_Op {
		size_t operator()(ثابت Accumulated_نيلاي &v) ثابت {
			أرجع static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_نيلاي, Accumulated_نيلاي_Hash_Op, Accumulated_نيلاي_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	عام:
		Builder(ثابت ParallelDispatchThreadPool &thread_pool, کونچي_Equality equality = کونچي_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.اڠک_threads(), std::move(equality));
		}
		Builder(ثابت ParallelDispatchThreadPool::Subpool &thread_pool, کونچي_Equality equality = کونچي_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.اڠک_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		فراغ سيسيڤ(ثابت ThreadIndex &thread, نيلاي_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<نيلاي_3>> &buckets = all_buckets[thread.thread_اڠک];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		فراغ process(ثابت ThreadIndex &thread) {
			صحيح size = 0;
			لكل (std::vector<std::vector<نيلاي_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_اڠک]);
			Shard &shard = shards[thread.thread_اڠک];
			shard.reserve(size);
			لكل (std::vector<std::vector<نيلاي_3>> &buckets : all_buckets) {
				لكل (نيلاي_3 &نيلاي : buckets[thread.thread_اڠک])
					accumulate(نيلاي, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<نيلاي_3>().swap(buckets[thread.thread_اڠک]);
			}
		}
	خاص:
		friend class ShardedHashtable<V, کونچي_Equality, CollisionHandler>;
		فراغ accumulate(نيلاي_3 &نيلاي, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_نيلاي accumulated_نيلاي{std::move(نيلاي.نيلاي), نيلاي.hash};
			auto [it, inserted] = shard.سيسيڤ(std::move(accumulated_نيلاي));
			إذا (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->نيلاي), accumulated_نيلاي.نيلاي);
		}
		فراغ init(صحيح اڠک_threads, کونچي_Equality equality) {
			all_buckets.resize(اڠک_threads);
			لكل (std::vector<std::vector<نيلاي_3>> &buckets : all_buckets)
				buckets.resize(اڠک_threads);
			لكل (صحيح i = 0; i < اڠک_threads; ++i)
				shards.emplace_back(0, Accumulated_نيلاي_Hash_Op(), Accumulated_نيلاي_Equality(equality));
		}
		ثابت CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<نيلاي_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		لكل (std::vector<std::vector<نيلاي_3>> &buckets : builder.all_buckets)
			لكل (std::vector<نيلاي_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<نيلاي_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = افتراضي;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = افتراضي;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ثابت typename V::Accumulated *find(ثابت Accumulated_نيلاي &v) ثابت {
		size_t اڠک_shards = shards.size();
		إذا (اڠک_shards == 0)
			أرجع nullptr;
		size_t shard = static_cast<size_t>(v.hash) % اڠک_shards;
		auto it = shards[shard].find(v);
		إذا (it == shards[shard].end())
			أرجع nullptr;
		أرجع &it->نيلاي;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	فراغ سيسيڤ(Accumulated_نيلاي v) {
		size_t اڠک_shards = shards.size();
		إذا (اڠک_shards == 0)
			أرجع;
		size_t shard = static_cast<size_t>(v.hash) % اڠک_shards;
		shards[shard].سيسيڤ(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	فراغ clear(ثابت ThreadIndex &shard) {
		Accumulated_نيلاي_Equality equality = shards[shard.thread_اڠک].کونچي_eq();
		shards[shard.thread_اڠک] = Shard(0, Accumulated_نيلاي_Hash_Op(), equality);
	}
خاص:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
عام:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(صحيح اڠک_threads, صحيح batch_size = 100)
		: batch_size(batch_size), thread_states(اڠک_threads) {}
	صحيح اڠک_threads() ثابت { أرجع GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	فراغ تولق(ثابت ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_اڠک];
		thread_state.next_batch.emplace_back(std::move(work));
		إذا (GetSize(thread_state.next_batch) < batch_size)
			أرجع;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.تولق_back(std::move(thread_state.next_batch));
		}
		إذا (was_empty) {
			UniqueLock lock(waiters_lock);
			إذا (اڠک_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> کلوار_batch(ثابت ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_اڠک];
		إذا (!thread_state.next_batch.empty())
			أرجع std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			إذا (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.کلوار_back();
				أرجع batch;
			}
		}
		// From here on in this function, our work queue is empty.
		طالما (true) {
			std::vector<T> batch = try_steal(thread);
			إذا (!batch.empty()) {
				أرجع std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++اڠک_waiters;
			إذا (اڠک_waiters == اڠک_threads()) {
				waiters_cv.notify_all();
				أرجع {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			إذا (اڠک_waiters == اڠک_threads())
				أرجع {};
			--اڠک_waiters;
		}
	}
خاص:
	std::vector<T> try_steal(ثابت ThreadIndex &thread) {
		لكل (صحيح i = 1; i < اڠک_threads(); i++) {
			صحيح other_thread_اڠک = (thread.thread_اڠک + i) % اڠک_threads();
			ThreadState &other_thread_state = thread_states[other_thread_اڠک];
			UniqueLock lock(other_thread_state.batches_lock);
			إذا (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.کلوار_front();
				أرجع batch;
			}
		}
		أرجع {};
	}

	صحيح batch_size;

	بنية ThreadState {
		// Entirely thread-local.
		std::vector<T> next_batch;

		Mutex batches_lock;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::vector<T>> batches;
	};
	std::vector<ThreadState> thread_states;

	Mutex waiters_lock;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	صحيح اڠک_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
عام:
	MonotonicFlag() : نيلاي(false) {}
	bool load() ثابت { أرجع نيلاي.load(std::memory_order_relaxed); }
	فراغ set() { نيلاي.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		أرجع نيلاي.exchange(true, std::memory_order_relaxed);
	}
خاص:
	std::atomic<bool> نيلاي;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
