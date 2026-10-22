#அந்தர்பாவ <deque>

#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/log.h"
#அந்தர்பாவ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#அந்தர்பாவ <condition_variable>
#அந்தர்பாவ <mutex>
#அந்தர்பாவ <thread>
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
#இல்லையெனில்
கட்டமைப்பு Mutex {
	வெறுமை lock() {}
	வெறுமை unlock() {}
	bool try_lock() { திரும்பு true; }
};
கட்டமைப்பு CondVar {
	template <class L> வெறுமை wait(L &) {}
	template <class L, class P> வெறுமை wait(L &, P) {}
	வெறுமை notify_one() {}
	வெறுமை notify_all() {}
};
கட்டமைப்பு UniqueLock {
	UniqueLock(Mutex &) {}
};
கட்டமைப்பு LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
பொது:
	ConcurrentQueue(முழு capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	வெறுமை தள்ளு_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { திரும்பு static_cast<முழு>(contents.size()) < capacity; });
		எனில் (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.தள்ளு_back(std::move(t));
		எனில் (static_cast<முழு>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	வெறுமை close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> எடு_front()
	{
		திரும்பு எடு_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_எடு_front()
	{
		திரும்பு எடு_front_internal(false);
	}
தனிப்பட்ட:
	std::optional<T> எடு_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		எனில் (wait) {
			not_empty_condition.wait(lock, [this] { திரும்பு !contents.empty() || closed; });
		}
		எனில் (contents.empty())
			திரும்பு std::nullopt;
		எனில் (static_cast<முழு>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.எடு_front();
		எனில் (!contents.empty())
			not_empty_condition.notify_one();
		திரும்பு std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	முழு capacity;
	bool closed = false;
};

class DeferredLogs
{
பொது:
	template <typename... Args>
	வெறுமை log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.தள்ளு_back({fmt.format(args...), false});
	}
	template <typename... Args>
	வெறுமை log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.தள்ளு_back({fmt.format(args...), true});
	}
	வெறுமை flush();
தனிப்பட்ட:
	கட்டமைப்பு Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
பொது:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	நிலையான முழு pool_size(முழு reserved_cores, முழு max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	நிலையான முழு work_pool_size(முழு reserved_cores, முழு work_units, முழு work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(முழு pool_size, std::function<வெறுமை(முழு)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	முழு இலக்கம்_threads() மாறிலி
	{
#ifdef YOSYS_ENABLE_THREADS
		திரும்பு threads.size();
#இல்லையெனில்
		திரும்பு 0;
#endif
	}
தனிப்பட்ட:
	std::function<வெறுமை(முழு)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(முழு இலக்கம்_பொருட்கள், முழு thread_இலக்கம், முழு இலக்கம்_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
கட்டமைப்பு ThreadIndex {
	முழு thread_இலக்கம்;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
பொது:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(முழு pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	கட்டமைப்பு RunCtx : பொது ThreadIndex {
		முழு இலக்கம்_threads;
		IntRange item_range(முழு இலக்கம்_பொருட்கள்) மாறிலி {
			திரும்பு item_range_for_worker(இலக்கம்_பொருட்கள், thread_இலக்கம், இலக்கம்_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	பொது:
		Subpool(ParallelDispatchThreadPool &parent, முழு max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		முழு இலக்கம்_threads() மாறிலி {
			திரும்பு parent.இலக்கம்_threads(max_threads);
		}
		வெறுமை run(std::function<வெறுமை(மாறிலி RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { திரும்பு parent; }
	தனிப்பட்ட:
		ParallelDispatchThreadPool &parent;
		முழு max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	வெறுமை run(std::function<வெறுமை(மாறிலி RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	முழு இலக்கம்_threads() மாறிலி {
		திரும்பு இலக்கம்_threads(INT_MAX);
	}
தனிப்பட்ட:
	friend class Subpool;

	வெறுமை run(std::function<வெறுமை(மாறிலி RunCtx &)> work, முழு max_threads);
	முழு இலக்கம்_threads(முழு max_threads) மாறிலி {
		திரும்பு std::min(இலக்கம்_worker_threads + 1, std::max(1, max_threads));
	}
	வெறுமை run_worker(முழு thread_இலக்கம்);

	std::function<வெறுமை(மாறிலி RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	முழு இலக்கம்_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<முழு> இலக்கம்_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	வெறுமை signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		முழு இலக்கம்_active_worker_threads_2 = இலக்கம்_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + இலக்கம்_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	வெறுமை worker_wait_for_start(முழு thread_இலக்கம்) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_இலக்கம்] { திரும்பு main_to_workers_signal[thread_இலக்கம்] > 0; });
		main_to_workers_signal[thread_இலக்கம்] = 0;
	}

	std::atomic<முழு> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	வெறுமை signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		முழு இலக்கம்_active_worker_threads_2 = இலக்கம்_active_worker_threads.load(std::memory_order_relaxed);
		முழு d = done_workers.fetch_add(1, std::memory_order_release);
		எனில் (d + 1 == இலக்கம்_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	வெறுமை wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			முழு இலக்கம்_active_worker_threads_2 = இலக்கம்_active_worker_threads.load(std::memory_order_relaxed);
			திரும்பு done_workers.load(std::memory_order_acquire) == இலக்கம்_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_அடுக்கு
{
பொது:
	வெறுமை தள்ளு_back(T &&t) {
		LockGuard lock(mutex);
		contents.தள்ளு_back(std::move(t));
	}
	std::optional<T> try_எடு_back() {
		LockGuard lock(mutex);
		எனில் (contents.empty())
			திரும்பு std::nullopt;
		T result = std::move(contents.back());
		contents.எடு_back();
		திரும்பு result;
	}
தனிப்பட்ட:
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
பொது:
	ShardedVector(மாறிலி ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.இலக்கம்_threads());
	}
	ShardedVector(மாறிலி ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.இலக்கம்_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	வெறுமை செருகு(மாறிலி ThreadIndex &thread, T மதிப்பு) {
		buckets[thread.thread_இலக்கம்].emplace_back(std::move(மதிப்பு));
	}

	bool empty() மாறிலி {
		க்கு (மாறிலி std::vector<T> &bucket : buckets)
			எனில் (!bucket.empty())
				திரும்பு false;
		திரும்பு true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	பொது:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			எனில் (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() மாறிலி { திரும்பு *inner_it.மதிப்பு(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			திரும்பு *this;
		}
		bool operator!=(மாறிலி iterator &other) மாறிலி {
			திரும்பு bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	தனிப்பட்ட:
		வெறுமை normalize() {
			எனில் (bucket_it == bucket_end)
				திரும்பு;
			போது (inner_it == bucket_it->end()) {
				++bucket_it;
				எனில் (bucket_it == bucket_end) {
					inner_it.reset();
					திரும்பு;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { திரும்பு iterator(buckets.begin(), buckets.end()); }
	iterator end() { திரும்பு iterator(buckets.end(), buckets.end()); }
தனிப்பட்ட:
	வெறுமை init(முழு இலக்கம்_threads) {
		buckets.resize(இலக்கம்_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
கட்டமைப்பு SetCollisionHandler {
	வெறுமை operator()(typename V::Accumulated &, typename V::Accumulated &) மாறிலி {}
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
template <typename V, typename சாவி_Equality, typename CollisionHandler>
class ShardedHashtable {
பொது:
	// A combination of a `V` and its hash value.
	கட்டமைப்பு மதிப்பு_3 {
		மதிப்பு_3(V மதிப்பு, unsigned முழு hash) : மதிப்பு(std::move(மதிப்பு)), hash(hash) {}
		மதிப்பு_3(மதிப்பு_3 &&) = இயல்பு;
		மதிப்பு_3(மாறிலி மதிப்பு_3 &) = delete;
		மதிப்பு_3 &operator=(மாறிலி மதிப்பு_3 &) = delete;
		V மதிப்பு;
		unsigned முழு hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	கட்டமைப்பு Accumulated_மதிப்பு {
		Accumulated_மதிப்பு(typename V::Accumulated மதிப்பு, unsigned முழு hash) : மதிப்பு(std::move(மதிப்பு)), hash(hash) {}
		Accumulated_மதிப்பு(Accumulated_மதிப்பு &&) = இயல்பு;
#எனில் defined(_MSC_VER)
		Accumulated_மதிப்பு(மாறிலி Accumulated_மதிப்பு &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_மதிப்பு &operator=(மாறிலி Accumulated_மதிப்பு &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			திரும்பு *this;
		}
#இல்லையெனில்
		Accumulated_மதிப்பு(மாறிலி Accumulated_மதிப்பு &) = delete;
		Accumulated_மதிப்பு &operator=(மாறிலி Accumulated_மதிப்பு &) = delete;
#endif
		typename V::Accumulated மதிப்பு;
		unsigned முழு hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	கட்டமைப்பு Accumulated_மதிப்பு_Equality {
		சாவி_Equality inner;
		Accumulated_மதிப்பு_Equality(மாறிலி சாவி_Equality &inner) : inner(inner) {}
		bool operator()(மாறிலி Accumulated_மதிப்பு &v1, மாறிலி Accumulated_மதிப்பு &v2) மாறிலி {
			திரும்பு inner(v1.மதிப்பு, v2.மதிப்பு);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	கட்டமைப்பு Accumulated_மதிப்பு_Hash_Op {
		size_t operator()(மாறிலி Accumulated_மதிப்பு &v) மாறிலி {
			திரும்பு static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_மதிப்பு, Accumulated_மதிப்பு_Hash_Op, Accumulated_மதிப்பு_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	பொது:
		Builder(மாறிலி ParallelDispatchThreadPool &thread_pool, சாவி_Equality equality = சாவி_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.இலக்கம்_threads(), std::move(equality));
		}
		Builder(மாறிலி ParallelDispatchThreadPool::Subpool &thread_pool, சாவி_Equality equality = சாவி_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.இலக்கம்_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		வெறுமை செருகு(மாறிலி ThreadIndex &thread, மதிப்பு_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<மதிப்பு_3>> &buckets = all_buckets[thread.thread_இலக்கம்];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		வெறுமை process(மாறிலி ThreadIndex &thread) {
			முழு size = 0;
			க்கு (std::vector<std::vector<மதிப்பு_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_இலக்கம்]);
			Shard &shard = shards[thread.thread_இலக்கம்];
			shard.reserve(size);
			க்கு (std::vector<std::vector<மதிப்பு_3>> &buckets : all_buckets) {
				க்கு (மதிப்பு_3 &மதிப்பு : buckets[thread.thread_இலக்கம்])
					accumulate(மதிப்பு, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<மதிப்பு_3>().swap(buckets[thread.thread_இலக்கம்]);
			}
		}
	தனிப்பட்ட:
		friend class ShardedHashtable<V, சாவி_Equality, CollisionHandler>;
		வெறுமை accumulate(மதிப்பு_3 &மதிப்பு, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_மதிப்பு accumulated_மதிப்பு{std::move(மதிப்பு.மதிப்பு), மதிப்பு.hash};
			auto [it, inserted] = shard.செருகு(std::move(accumulated_மதிப்பு));
			எனில் (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->மதிப்பு), accumulated_மதிப்பு.மதிப்பு);
		}
		வெறுமை init(முழு இலக்கம்_threads, சாவி_Equality equality) {
			all_buckets.resize(இலக்கம்_threads);
			க்கு (std::vector<std::vector<மதிப்பு_3>> &buckets : all_buckets)
				buckets.resize(இலக்கம்_threads);
			க்கு (முழு i = 0; i < இலக்கம்_threads; ++i)
				shards.emplace_back(0, Accumulated_மதிப்பு_Hash_Op(), Accumulated_மதிப்பு_Equality(equality));
		}
		மாறிலி CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<மதிப்பு_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		க்கு (std::vector<std::vector<மதிப்பு_3>> &buckets : builder.all_buckets)
			க்கு (std::vector<மதிப்பு_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<மதிப்பு_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = இயல்பு;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = இயல்பு;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	மாறிலி typename V::Accumulated *find(மாறிலி Accumulated_மதிப்பு &v) மாறிலி {
		size_t இலக்கம்_shards = shards.size();
		எனில் (இலக்கம்_shards == 0)
			திரும்பு nullptr;
		size_t shard = static_cast<size_t>(v.hash) % இலக்கம்_shards;
		auto it = shards[shard].find(v);
		எனில் (it == shards[shard].end())
			திரும்பு nullptr;
		திரும்பு &it->மதிப்பு;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	வெறுமை செருகு(Accumulated_மதிப்பு v) {
		size_t இலக்கம்_shards = shards.size();
		எனில் (இலக்கம்_shards == 0)
			திரும்பு;
		size_t shard = static_cast<size_t>(v.hash) % இலக்கம்_shards;
		shards[shard].செருகு(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	வெறுமை clear(மாறிலி ThreadIndex &shard) {
		Accumulated_மதிப்பு_Equality equality = shards[shard.thread_இலக்கம்].சாவி_eq();
		shards[shard.thread_இலக்கம்] = Shard(0, Accumulated_மதிப்பு_Hash_Op(), equality);
	}
தனிப்பட்ட:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
பொது:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(முழு இலக்கம்_threads, முழு batch_size = 100)
		: batch_size(batch_size), thread_states(இலக்கம்_threads) {}
	முழு இலக்கம்_threads() மாறிலி { திரும்பு GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	வெறுமை தள்ளு(மாறிலி ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_இலக்கம்];
		thread_state.next_batch.emplace_back(std::move(work));
		எனில் (GetSize(thread_state.next_batch) < batch_size)
			திரும்பு;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.தள்ளு_back(std::move(thread_state.next_batch));
		}
		எனில் (was_empty) {
			UniqueLock lock(waiters_lock);
			எனில் (இலக்கம்_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> எடு_batch(மாறிலி ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_இலக்கம்];
		எனில் (!thread_state.next_batch.empty())
			திரும்பு std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			எனில் (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.எடு_back();
				திரும்பு batch;
			}
		}
		// From here on in this function, our work queue is empty.
		போது (true) {
			std::vector<T> batch = try_steal(thread);
			எனில் (!batch.empty()) {
				திரும்பு std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++இலக்கம்_waiters;
			எனில் (இலக்கம்_waiters == இலக்கம்_threads()) {
				waiters_cv.notify_all();
				திரும்பு {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			எனில் (இலக்கம்_waiters == இலக்கம்_threads())
				திரும்பு {};
			--இலக்கம்_waiters;
		}
	}
தனிப்பட்ட:
	std::vector<T> try_steal(மாறிலி ThreadIndex &thread) {
		க்கு (முழு i = 1; i < இலக்கம்_threads(); i++) {
			முழு other_thread_இலக்கம் = (thread.thread_இலக்கம் + i) % இலக்கம்_threads();
			ThreadState &other_thread_state = thread_states[other_thread_இலக்கம்];
			UniqueLock lock(other_thread_state.batches_lock);
			எனில் (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.எடு_front();
				திரும்பு batch;
			}
		}
		திரும்பு {};
	}

	முழு batch_size;

	கட்டமைப்பு ThreadState {
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
	முழு இலக்கம்_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
பொது:
	MonotonicFlag() : மதிப்பு(false) {}
	bool load() மாறிலி { திரும்பு மதிப்பு.load(std::memory_order_relaxed); }
	வெறுமை set() { மதிப்பு.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		திரும்பு மதிப்பு.exchange(true, std::memory_order_relaxed);
	}
தனிப்பட்ட:
	std::atomic<bool> மதிப்பு;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
