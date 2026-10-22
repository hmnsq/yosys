#અન્તર્ભાવ <deque>

#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/log.h"
#અન્તર્ભાવ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#અન્તર્ભાવ <condition_variable>
#અન્તર્ભાવ <mutex>
#અન્તર્ભાવ <thread>
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
#નહીંતર
સંરચના Mutex {
	શૂન્ય lock() {}
	શૂન્ય unlock() {}
	bool try_lock() { પાછા true; }
};
સંરચના CondVar {
	template <class L> શૂન્ય wait(L &) {}
	template <class L, class P> શૂન્ય wait(L &, P) {}
	શૂન્ય notify_one() {}
	શૂન્ય notify_all() {}
};
સંરચના UniqueLock {
	UniqueLock(Mutex &) {}
};
સંરચના LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
જાહેર:
	ConcurrentQueue(પૂર્ણાંક capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	શૂન્ય ધકેલો_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { પાછા static_cast<પૂર્ણાંક>(contents.size()) < capacity; });
		જો (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.ધકેલો_back(std::move(t));
		જો (static_cast<પૂર્ણાંક>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	શૂન્ય close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> કાઢો_front()
	{
		પાછા કાઢો_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_કાઢો_front()
	{
		પાછા કાઢો_front_internal(false);
	}
ખાનગી:
	std::optional<T> કાઢો_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		જો (wait) {
			not_empty_condition.wait(lock, [this] { પાછા !contents.empty() || closed; });
		}
		જો (contents.empty())
			પાછા std::nullopt;
		જો (static_cast<પૂર્ણાંક>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.કાઢો_front();
		જો (!contents.empty())
			not_empty_condition.notify_one();
		પાછા std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	પૂર્ણાંક capacity;
	bool closed = false;
};

class DeferredLogs
{
જાહેર:
	template <typename... Args>
	શૂન્ય log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ધકેલો_back({fmt.format(args...), false});
	}
	template <typename... Args>
	શૂન્ય log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ધકેલો_back({fmt.format(args...), true});
	}
	શૂન્ય flush();
ખાનગી:
	સંરચના Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
જાહેર:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	સ્થિર પૂર્ણાંક pool_size(પૂર્ણાંક reserved_cores, પૂર્ણાંક max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	સ્થિર પૂર્ણાંક work_pool_size(પૂર્ણાંક reserved_cores, પૂર્ણાંક work_units, પૂર્ણાંક work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(પૂર્ણાંક pool_size, std::function<શૂન્ય(પૂર્ણાંક)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	પૂર્ણાંક અંક_threads() અચલ
	{
#ifdef YOSYS_ENABLE_THREADS
		પાછા threads.size();
#નહીંતર
		પાછા 0;
#endif
	}
ખાનગી:
	std::function<શૂન્ય(પૂર્ણાંક)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(પૂર્ણાંક અંક_વસ્તુઓ, પૂર્ણાંક thread_અંક, પૂર્ણાંક અંક_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
સંરચના ThreadIndex {
	પૂર્ણાંક thread_અંક;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
જાહેર:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(પૂર્ણાંક pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	સંરચના RunCtx : જાહેર ThreadIndex {
		પૂર્ણાંક અંક_threads;
		IntRange item_range(પૂર્ણાંક અંક_વસ્તુઓ) અચલ {
			પાછા item_range_for_worker(અંક_વસ્તુઓ, thread_અંક, અંક_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	જાહેર:
		Subpool(ParallelDispatchThreadPool &parent, પૂર્ણાંક max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		પૂર્ણાંક અંક_threads() અચલ {
			પાછા parent.અંક_threads(max_threads);
		}
		શૂન્ય run(std::function<શૂન્ય(અચલ RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { પાછા parent; }
	ખાનગી:
		ParallelDispatchThreadPool &parent;
		પૂર્ણાંક max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	શૂન્ય run(std::function<શૂન્ય(અચલ RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	પૂર્ણાંક અંક_threads() અચલ {
		પાછા અંક_threads(INT_MAX);
	}
ખાનગી:
	friend class Subpool;

	શૂન્ય run(std::function<શૂન્ય(અચલ RunCtx &)> work, પૂર્ણાંક max_threads);
	પૂર્ણાંક અંક_threads(પૂર્ણાંક max_threads) અચલ {
		પાછા std::min(અંક_worker_threads + 1, std::max(1, max_threads));
	}
	શૂન્ય run_worker(પૂર્ણાંક thread_અંક);

	std::function<શૂન્ય(અચલ RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	પૂર્ણાંક અંક_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<પૂર્ણાંક> અંક_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	શૂન્ય signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		પૂર્ણાંક અંક_active_worker_threads_2 = અંક_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + અંક_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	શૂન્ય worker_wait_for_start(પૂર્ણાંક thread_અંક) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_અંક] { પાછા main_to_workers_signal[thread_અંક] > 0; });
		main_to_workers_signal[thread_અંક] = 0;
	}

	std::atomic<પૂર્ણાંક> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	શૂન્ય signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		પૂર્ણાંક અંક_active_worker_threads_2 = અંક_active_worker_threads.load(std::memory_order_relaxed);
		પૂર્ણાંક d = done_workers.fetch_add(1, std::memory_order_release);
		જો (d + 1 == અંક_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	શૂન્ય wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			પૂર્ણાંક અંક_active_worker_threads_2 = અંક_active_worker_threads.load(std::memory_order_relaxed);
			પાછા done_workers.load(std::memory_order_acquire) == અંક_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_ઢગલો
{
જાહેર:
	શૂન્ય ધકેલો_back(T &&t) {
		LockGuard lock(mutex);
		contents.ધકેલો_back(std::move(t));
	}
	std::optional<T> try_કાઢો_back() {
		LockGuard lock(mutex);
		જો (contents.empty())
			પાછા std::nullopt;
		T result = std::move(contents.back());
		contents.કાઢો_back();
		પાછા result;
	}
ખાનગી:
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
જાહેર:
	ShardedVector(અચલ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.અંક_threads());
	}
	ShardedVector(અચલ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.અંક_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	શૂન્ય ઉમેરો(અચલ ThreadIndex &thread, T કિંમત) {
		buckets[thread.thread_અંક].emplace_back(std::move(કિંમત));
	}

	bool empty() અચલ {
		માટે (અચલ std::vector<T> &bucket : buckets)
			જો (!bucket.empty())
				પાછા false;
		પાછા true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	જાહેર:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			જો (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() અચલ { પાછા *inner_it.કિંમત(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			પાછા *this;
		}
		bool operator!=(અચલ iterator &other) અચલ {
			પાછા bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	ખાનગી:
		શૂન્ય normalize() {
			જો (bucket_it == bucket_end)
				પાછા;
			જ્યારે (inner_it == bucket_it->end()) {
				++bucket_it;
				જો (bucket_it == bucket_end) {
					inner_it.reset();
					પાછા;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { પાછા iterator(buckets.begin(), buckets.end()); }
	iterator end() { પાછા iterator(buckets.end(), buckets.end()); }
ખાનગી:
	શૂન્ય init(પૂર્ણાંક અંક_threads) {
		buckets.resize(અંક_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
સંરચના SetCollisionHandler {
	શૂન્ય operator()(typename V::Accumulated &, typename V::Accumulated &) અચલ {}
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
template <typename V, typename ચાવી_Equality, typename CollisionHandler>
class ShardedHashtable {
જાહેર:
	// A combination of a `V` and its hash value.
	સંરચના કિંમત_3 {
		કિંમત_3(V કિંમત, unsigned પૂર્ણાંક hash) : કિંમત(std::move(કિંમત)), hash(hash) {}
		કિંમત_3(કિંમત_3 &&) = મૂળભૂત;
		કિંમત_3(અચલ કિંમત_3 &) = delete;
		કિંમત_3 &operator=(અચલ કિંમત_3 &) = delete;
		V કિંમત;
		unsigned પૂર્ણાંક hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	સંરચના Accumulated_કિંમત {
		Accumulated_કિંમત(typename V::Accumulated કિંમત, unsigned પૂર્ણાંક hash) : કિંમત(std::move(કિંમત)), hash(hash) {}
		Accumulated_કિંમત(Accumulated_કિંમત &&) = મૂળભૂત;
#જો defined(_MSC_VER)
		Accumulated_કિંમત(અચલ Accumulated_કિંમત &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_કિંમત &operator=(અચલ Accumulated_કિંમત &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			પાછા *this;
		}
#નહીંતર
		Accumulated_કિંમત(અચલ Accumulated_કિંમત &) = delete;
		Accumulated_કિંમત &operator=(અચલ Accumulated_કિંમત &) = delete;
#endif
		typename V::Accumulated કિંમત;
		unsigned પૂર્ણાંક hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	સંરચના Accumulated_કિંમત_Equality {
		ચાવી_Equality inner;
		Accumulated_કિંમત_Equality(અચલ ચાવી_Equality &inner) : inner(inner) {}
		bool operator()(અચલ Accumulated_કિંમત &v1, અચલ Accumulated_કિંમત &v2) અચલ {
			પાછા inner(v1.કિંમત, v2.કિંમત);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	સંરચના Accumulated_કિંમત_Hash_Op {
		size_t operator()(અચલ Accumulated_કિંમત &v) અચલ {
			પાછા static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_કિંમત, Accumulated_કિંમત_Hash_Op, Accumulated_કિંમત_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	જાહેર:
		Builder(અચલ ParallelDispatchThreadPool &thread_pool, ચાવી_Equality equality = ચાવી_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.અંક_threads(), std::move(equality));
		}
		Builder(અચલ ParallelDispatchThreadPool::Subpool &thread_pool, ચાવી_Equality equality = ચાવી_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.અંક_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		શૂન્ય ઉમેરો(અચલ ThreadIndex &thread, કિંમત_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<કિંમત_3>> &buckets = all_buckets[thread.thread_અંક];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		શૂન્ય process(અચલ ThreadIndex &thread) {
			પૂર્ણાંક size = 0;
			માટે (std::vector<std::vector<કિંમત_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_અંક]);
			Shard &shard = shards[thread.thread_અંક];
			shard.reserve(size);
			માટે (std::vector<std::vector<કિંમત_3>> &buckets : all_buckets) {
				માટે (કિંમત_3 &કિંમત : buckets[thread.thread_અંક])
					accumulate(કિંમત, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<કિંમત_3>().swap(buckets[thread.thread_અંક]);
			}
		}
	ખાનગી:
		friend class ShardedHashtable<V, ચાવી_Equality, CollisionHandler>;
		શૂન્ય accumulate(કિંમત_3 &કિંમત, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_કિંમત accumulated_કિંમત{std::move(કિંમત.કિંમત), કિંમત.hash};
			auto [it, inserted] = shard.ઉમેરો(std::move(accumulated_કિંમત));
			જો (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->કિંમત), accumulated_કિંમત.કિંમત);
		}
		શૂન્ય init(પૂર્ણાંક અંક_threads, ચાવી_Equality equality) {
			all_buckets.resize(અંક_threads);
			માટે (std::vector<std::vector<કિંમત_3>> &buckets : all_buckets)
				buckets.resize(અંક_threads);
			માટે (પૂર્ણાંક i = 0; i < અંક_threads; ++i)
				shards.emplace_back(0, Accumulated_કિંમત_Hash_Op(), Accumulated_કિંમત_Equality(equality));
		}
		અચલ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<કિંમત_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		માટે (std::vector<std::vector<કિંમત_3>> &buckets : builder.all_buckets)
			માટે (std::vector<કિંમત_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<કિંમત_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = મૂળભૂત;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = મૂળભૂત;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	અચલ typename V::Accumulated *find(અચલ Accumulated_કિંમત &v) અચલ {
		size_t અંક_shards = shards.size();
		જો (અંક_shards == 0)
			પાછા nullptr;
		size_t shard = static_cast<size_t>(v.hash) % અંક_shards;
		auto it = shards[shard].find(v);
		જો (it == shards[shard].end())
			પાછા nullptr;
		પાછા &it->કિંમત;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	શૂન્ય ઉમેરો(Accumulated_કિંમત v) {
		size_t અંક_shards = shards.size();
		જો (અંક_shards == 0)
			પાછા;
		size_t shard = static_cast<size_t>(v.hash) % અંક_shards;
		shards[shard].ઉમેરો(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	શૂન્ય clear(અચલ ThreadIndex &shard) {
		Accumulated_કિંમત_Equality equality = shards[shard.thread_અંક].ચાવી_eq();
		shards[shard.thread_અંક] = Shard(0, Accumulated_કિંમત_Hash_Op(), equality);
	}
ખાનગી:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
જાહેર:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(પૂર્ણાંક અંક_threads, પૂર્ણાંક batch_size = 100)
		: batch_size(batch_size), thread_states(અંક_threads) {}
	પૂર્ણાંક અંક_threads() અચલ { પાછા GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	શૂન્ય ધકેલો(અચલ ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_અંક];
		thread_state.next_batch.emplace_back(std::move(work));
		જો (GetSize(thread_state.next_batch) < batch_size)
			પાછા;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.ધકેલો_back(std::move(thread_state.next_batch));
		}
		જો (was_empty) {
			UniqueLock lock(waiters_lock);
			જો (અંક_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> કાઢો_batch(અચલ ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_અંક];
		જો (!thread_state.next_batch.empty())
			પાછા std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			જો (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.કાઢો_back();
				પાછા batch;
			}
		}
		// From here on in this function, our work queue is empty.
		જ્યારે (true) {
			std::vector<T> batch = try_steal(thread);
			જો (!batch.empty()) {
				પાછા std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++અંક_waiters;
			જો (અંક_waiters == અંક_threads()) {
				waiters_cv.notify_all();
				પાછા {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			જો (અંક_waiters == અંક_threads())
				પાછા {};
			--અંક_waiters;
		}
	}
ખાનગી:
	std::vector<T> try_steal(અચલ ThreadIndex &thread) {
		માટે (પૂર્ણાંક i = 1; i < અંક_threads(); i++) {
			પૂર્ણાંક other_thread_અંક = (thread.thread_અંક + i) % અંક_threads();
			ThreadState &other_thread_state = thread_states[other_thread_અંક];
			UniqueLock lock(other_thread_state.batches_lock);
			જો (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.કાઢો_front();
				પાછા batch;
			}
		}
		પાછા {};
	}

	પૂર્ણાંક batch_size;

	સંરચના ThreadState {
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
	પૂર્ણાંક અંક_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
જાહેર:
	MonotonicFlag() : કિંમત(false) {}
	bool load() અચલ { પાછા કિંમત.load(std::memory_order_relaxed); }
	શૂન્ય set() { કિંમત.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		પાછા કિંમત.exchange(true, std::memory_order_relaxed);
	}
ખાનગી:
	std::atomic<bool> કિંમત;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
