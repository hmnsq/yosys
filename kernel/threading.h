#include <deque>

#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#include <condition_variable>
#include <mutex>
#include <thread>
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
#інакше
struct Mutex {
	void lock() {}
	void unlock() {}
	bool try_lock() { повернути true; }
};
struct CondVar {
	template <class L> void wait(L &) {}
	template <class L, class P> void wait(L &, P) {}
	void notify_one() {}
	void notify_all() {}
};
struct UniqueLock {
	UniqueLock(Mutex &) {}
};
struct LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
public:
	ConcurrentQueue(ціле capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	void push_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { повернути static_cast<ціле>(contents.size()) < capacity; });
		якщо (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.push_back(std::move(t));
		якщо (static_cast<ціле>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	void close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> pop_front()
	{
		повернути pop_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_pop_front()
	{
		повернути pop_front_internal(false);
	}
private:
	std::optional<T> pop_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		якщо (wait) {
			not_empty_condition.wait(lock, [this] { повернути !contents.empty() || closed; });
		}
		якщо (contents.empty())
			повернути std::nullopt;
		якщо (static_cast<ціле>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.pop_front();
		якщо (!contents.empty())
			not_empty_condition.notify_one();
		повернути std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	ціле capacity;
	bool closed = false;
};

class DeferredLogs
{
public:
	template <typename... Args>
	void log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.push_back({fmt.format(args...), false});
	}
	template <typename... Args>
	void log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.push_back({fmt.format(args...), true});
	}
	void flush();
private:
	struct Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
public:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	static ціле pool_size(ціле reserved_cores, ціле max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	static ціле work_pool_size(ціле reserved_cores, ціле work_units, ціле work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(ціле pool_size, std::function<void(ціле)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	ціле num_threads() const
	{
#ifdef YOSYS_ENABLE_THREADS
		повернути threads.size();
#інакше
		повернути 0;
#endif
	}
private:
	std::function<void(ціле)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(ціле num_items, ціле thread_num, ціле num_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
struct ThreadIndex {
	ціле thread_num;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
public:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(ціле pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	struct RunCtx : public ThreadIndex {
		ціле num_threads;
		IntRange item_range(ціле num_items) const {
			повернути item_range_for_worker(num_items, thread_num, num_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	public:
		Subpool(ParallelDispatchThreadPool &parent, ціле max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		ціле num_threads() const {
			повернути parent.num_threads(max_threads);
		}
		void run(std::function<void(const RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { повернути parent; }
	private:
		ParallelDispatchThreadPool &parent;
		ціле max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	void run(std::function<void(const RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	ціле num_threads() const {
		повернути num_threads(INT_MAX);
	}
private:
	friend class Subpool;

	void run(std::function<void(const RunCtx &)> work, ціле max_threads);
	ціле num_threads(ціле max_threads) const {
		повернути std::min(num_worker_threads_ + 1, std::max(1, max_threads));
	}
	void run_worker(ціле thread_num);

	std::function<void(const RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	ціле num_worker_threads_;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<ціле> num_active_worker_threads_ = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	void signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		ціле num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + num_active_worker_threads, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	void worker_wait_for_start(ціле thread_num) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_num] { повернути main_to_workers_signal[thread_num] > 0; });
		main_to_workers_signal[thread_num] = 0;
	}

	std::atomic<ціле> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	void signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		ціле num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
		ціле d = done_workers.fetch_add(1, std::memory_order_release);
		якщо (d + 1 == num_active_worker_threads) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	void wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			ціле num_active_worker_threads = num_active_worker_threads_.load(std::memory_order_relaxed);
			повернути done_workers.load(std::memory_order_acquire) == num_active_worker_threads;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class ConcurrentStack
{
public:
	void push_back(T &&t) {
		LockGuard lock(mutex);
		contents.push_back(std::move(t));
	}
	std::optional<T> try_pop_back() {
		LockGuard lock(mutex);
		якщо (contents.empty())
			повернути std::nullopt;
		T result = std::move(contents.back());
		contents.pop_back();
		повернути result;
	}
private:
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
public:
	ShardedVector(const ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.num_threads());
	}
	ShardedVector(const ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.num_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	void insert(const ThreadIndex &thread, T value) {
		buckets[thread.thread_num].emplace_back(std::move(value));
	}

	bool empty() const {
		для (const std::vector<T> &bucket : buckets)
			якщо (!bucket.empty())
				повернути false;
		повернути true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	public:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			якщо (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() const { повернути *inner_it.value(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			повернути *this;
		}
		bool operator!=(const iterator &other) const {
			повернути bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	private:
		void normalize() {
			якщо (bucket_it == bucket_end)
				повернути;
			поки (inner_it == bucket_it->end()) {
				++bucket_it;
				якщо (bucket_it == bucket_end) {
					inner_it.reset();
					повернути;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { повернути iterator(buckets.begin(), buckets.end()); }
	iterator end() { повернути iterator(buckets.end(), buckets.end()); }
private:
	void init(ціле num_threads) {
		buckets.resize(num_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
struct SetCollisionHandler {
	void operator()(typename V::Accumulated &, typename V::Accumulated &) const {}
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
template <typename V, typename KeyEquality, typename CollisionHandler>
class ShardedHashtable {
public:
	// A combination of a `V` and its hash value.
	struct Value {
		Value(V value, unsigned ціле hash) : value(std::move(value)), hash(hash) {}
		Value(Value &&) = default;
		Value(const Value &) = delete;
		Value &operator=(const Value &) = delete;
		V value;
		unsigned ціле hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	struct AccumulatedValue {
		AccumulatedValue(typename V::Accumulated value, unsigned ціле hash) : value(std::move(value)), hash(hash) {}
		AccumulatedValue(AccumulatedValue &&) = default;
#якщо defined(_MSC_VER)
		AccumulatedValue(const AccumulatedValue &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		AccumulatedValue &operator=(const AccumulatedValue &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			повернути *this;
		}
#інакше
		AccumulatedValue(const AccumulatedValue &) = delete;
		AccumulatedValue &operator=(const AccumulatedValue &) = delete;
#endif
		typename V::Accumulated value;
		unsigned ціле hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	struct AccumulatedValueEquality {
		KeyEquality inner;
		AccumulatedValueEquality(const KeyEquality &inner) : inner(inner) {}
		bool operator()(const AccumulatedValue &v1, const AccumulatedValue &v2) const {
			повернути inner(v1.value, v2.value);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	struct AccumulatedValueHashOp {
		size_t operator()(const AccumulatedValue &v) const {
			повернути static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<AccumulatedValue, AccumulatedValueHashOp, AccumulatedValueEquality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	public:
		Builder(const ParallelDispatchThreadPool &thread_pool, KeyEquality equality = KeyEquality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.num_threads(), std::move(equality));
		}
		Builder(const ParallelDispatchThreadPool::Subpool &thread_pool, KeyEquality equality = KeyEquality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.num_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		void insert(const ThreadIndex &thread, Value v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<Value>> &buckets = all_buckets[thread.thread_num];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		void process(const ThreadIndex &thread) {
			ціле size = 0;
			для (std::vector<std::vector<Value>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_num]);
			Shard &shard = shards[thread.thread_num];
			shard.reserve(size);
			для (std::vector<std::vector<Value>> &buckets : all_buckets) {
				для (Value &value : buckets[thread.thread_num])
					accumulate(value, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<Value>().swap(buckets[thread.thread_num]);
			}
		}
	private:
		friend class ShardedHashtable<V, KeyEquality, CollisionHandler>;
		void accumulate(Value &value, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			AccumulatedValue accumulated_value{std::move(value.value), value.hash};
			auto [it, inserted] = shard.insert(std::move(accumulated_value));
			якщо (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->value), accumulated_value.value);
		}
		void init(ціле num_threads, KeyEquality equality) {
			all_buckets.resize(num_threads);
			для (std::vector<std::vector<Value>> &buckets : all_buckets)
				buckets.resize(num_threads);
			для (ціле i = 0; i < num_threads; ++i)
				shards.emplace_back(0, AccumulatedValueHashOp(), AccumulatedValueEquality(equality));
		}
		const CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<Value>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		для (std::vector<std::vector<Value>> &buckets : builder.all_buckets)
			для (std::vector<Value> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<Value>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = default;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = default;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	const typename V::Accumulated *find(const AccumulatedValue &v) const {
		size_t num_shards = shards.size();
		якщо (num_shards == 0)
			повернути nullptr;
		size_t shard = static_cast<size_t>(v.hash) % num_shards;
		auto it = shards[shard].find(v);
		якщо (it == shards[shard].end())
			повернути nullptr;
		повернути &it->value;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	void insert(AccumulatedValue v) {
		size_t num_shards = shards.size();
		якщо (num_shards == 0)
			повернути;
		size_t shard = static_cast<size_t>(v.hash) % num_shards;
		shards[shard].insert(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	void clear(const ThreadIndex &shard) {
		AccumulatedValueEquality equality = shards[shard.thread_num].key_eq();
		shards[shard.thread_num] = Shard(0, AccumulatedValueHashOp(), equality);
	}
private:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
public:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(ціле num_threads, ціле batch_size = 100)
		: batch_size(batch_size), thread_states(num_threads) {}
	ціле num_threads() const { повернути GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	void push(const ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_num];
		thread_state.next_batch.emplace_back(std::move(work));
		якщо (GetSize(thread_state.next_batch) < batch_size)
			повернути;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.push_back(std::move(thread_state.next_batch));
		}
		якщо (was_empty) {
			UniqueLock lock(waiters_lock);
			якщо (num_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> pop_batch(const ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_num];
		якщо (!thread_state.next_batch.empty())
			повернути std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			якщо (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.pop_back();
				повернути batch;
			}
		}
		// From here on in this function, our work queue is empty.
		поки (true) {
			std::vector<T> batch = try_steal(thread);
			якщо (!batch.empty()) {
				повернути std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++num_waiters;
			якщо (num_waiters == num_threads()) {
				waiters_cv.notify_all();
				повернути {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			якщо (num_waiters == num_threads())
				повернути {};
			--num_waiters;
		}
	}
private:
	std::vector<T> try_steal(const ThreadIndex &thread) {
		для (ціле i = 1; i < num_threads(); i++) {
			ціле other_thread_num = (thread.thread_num + i) % num_threads();
			ThreadState &other_thread_state = thread_states[other_thread_num];
			UniqueLock lock(other_thread_state.batches_lock);
			якщо (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.pop_front();
				повернути batch;
			}
		}
		повернути {};
	}

	ціле batch_size;

	struct ThreadState {
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
	ціле num_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
public:
	MonotonicFlag() : value(false) {}
	bool load() const { повернути value.load(std::memory_order_relaxed); }
	void set() { value.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		повернути value.exchange(true, std::memory_order_relaxed);
	}
private:
	std::atomic<bool> value;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
