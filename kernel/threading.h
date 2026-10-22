#অন্তর্ভাৱ <deque>

#অন্তর্ভাৱ "kernel/yosys_common.h"
#অন্তর্ভাৱ "kernel/log.h"
#অন্তর্ভাৱ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#অন্তর্ভাৱ <condition_variable>
#অন্তর্ভাৱ <mutex>
#অন্তর্ভাৱ <thread>
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
#অন্যথা
সংরচনা Mutex {
	শূন্য lock() {}
	শূন্য unlock() {}
	bool try_lock() { ঘূৰাই true; }
};
সংরচনা CondVar {
	template <class L> শূন্য wait(L &) {}
	template <class L, class P> শূন্য wait(L &, P) {}
	শূন্য notify_one() {}
	শূন্য notify_all() {}
};
সংরচনা UniqueLock {
	UniqueLock(Mutex &) {}
};
সংরচনা LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
সার্ৱজনিক:
	ConcurrentQueue(পূর্ণাঙ্ক capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	শূন্য ঠেলো_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { ঘূৰাই static_cast<পূর্ণাঙ্ক>(contents.size()) < capacity; });
		যদি (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.ঠেলো_back(std::move(t));
		যদি (static_cast<পূর্ণাঙ্ক>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	শূন্য close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> বেৰ_কৰো_front()
	{
		ঘূৰাই বেৰ_কৰো_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_বেৰ_কৰো_front()
	{
		ঘূৰাই বেৰ_কৰো_front_internal(false);
	}
গুপ্ত:
	std::optional<T> বেৰ_কৰো_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		যদি (wait) {
			not_empty_condition.wait(lock, [this] { ঘূৰাই !contents.empty() || closed; });
		}
		যদি (contents.empty())
			ঘূৰাই std::nullopt;
		যদি (static_cast<পূর্ণাঙ্ক>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.বেৰ_কৰো_front();
		যদি (!contents.empty())
			not_empty_condition.notify_one();
		ঘূৰাই std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	পূর্ণাঙ্ক capacity;
	bool closed = false;
};

class DeferredLogs
{
সার্ৱজনিক:
	template <typename... Args>
	শূন্য log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ঠেলো_back({fmt.format(args...), false});
	}
	template <typename... Args>
	শূন্য log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ঠেলো_back({fmt.format(args...), true});
	}
	শূন্য flush();
গুপ্ত:
	সংরচনা Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
সার্ৱজনিক:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	স্থির পূর্ণাঙ্ক pool_size(পূর্ণাঙ্ক reserved_cores, পূর্ণাঙ্ক max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	স্থির পূর্ণাঙ্ক work_pool_size(পূর্ণাঙ্ক reserved_cores, পূর্ণাঙ্ক work_units, পূর্ণাঙ্ক work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(পূর্ণাঙ্ক pool_size, std::function<শূন্য(পূর্ণাঙ্ক)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	পূর্ণাঙ্ক অঙ্ক_threads() ধ্রুৱ
	{
#ifdef YOSYS_ENABLE_THREADS
		ঘূৰাই threads.size();
#অন্যথা
		ঘূৰাই 0;
#endif
	}
গুপ্ত:
	std::function<শূন্য(পূর্ণাঙ্ক)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(পূর্ণাঙ্ক অঙ্ক_বস্তুসমূহ, পূর্ণাঙ্ক thread_অঙ্ক, পূর্ণাঙ্ক অঙ্ক_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
সংরচনা ThreadIndex {
	পূর্ণাঙ্ক thread_অঙ্ক;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
সার্ৱজনিক:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(পূর্ণাঙ্ক pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	সংরচনা RunCtx : সার্ৱজনিক ThreadIndex {
		পূর্ণাঙ্ক অঙ্ক_threads;
		IntRange item_range(পূর্ণাঙ্ক অঙ্ক_বস্তুসমূহ) ধ্রুৱ {
			ঘূৰাই item_range_for_worker(অঙ্ক_বস্তুসমূহ, thread_অঙ্ক, অঙ্ক_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	সার্ৱজনিক:
		Subpool(ParallelDispatchThreadPool &parent, পূর্ণাঙ্ক max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		পূর্ণাঙ্ক অঙ্ক_threads() ধ্রুৱ {
			ঘূৰাই parent.অঙ্ক_threads(max_threads);
		}
		শূন্য run(std::function<শূন্য(ধ্রুৱ RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { ঘূৰাই parent; }
	গুপ্ত:
		ParallelDispatchThreadPool &parent;
		পূর্ণাঙ্ক max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	শূন্য run(std::function<শূন্য(ধ্রুৱ RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	পূর্ণাঙ্ক অঙ্ক_threads() ধ্রুৱ {
		ঘূৰাই অঙ্ক_threads(INT_MAX);
	}
গুপ্ত:
	friend class Subpool;

	শূন্য run(std::function<শূন্য(ধ্রুৱ RunCtx &)> work, পূর্ণাঙ্ক max_threads);
	পূর্ণাঙ্ক অঙ্ক_threads(পূর্ণাঙ্ক max_threads) ধ্রুৱ {
		ঘূৰাই std::min(অঙ্ক_worker_threads + 1, std::max(1, max_threads));
	}
	শূন্য run_worker(পূর্ণাঙ্ক thread_অঙ্ক);

	std::function<শূন্য(ধ্রুৱ RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	পূর্ণাঙ্ক অঙ্ক_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<পূর্ণাঙ্ক> অঙ্ক_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	শূন্য signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		পূর্ণাঙ্ক অঙ্ক_active_worker_threads_2 = অঙ্ক_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + অঙ্ক_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	শূন্য worker_wait_for_start(পূর্ণাঙ্ক thread_অঙ্ক) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_অঙ্ক] { ঘূৰাই main_to_workers_signal[thread_অঙ্ক] > 0; });
		main_to_workers_signal[thread_অঙ্ক] = 0;
	}

	std::atomic<পূর্ণাঙ্ক> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	শূন্য signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		পূর্ণাঙ্ক অঙ্ক_active_worker_threads_2 = অঙ্ক_active_worker_threads.load(std::memory_order_relaxed);
		পূর্ণাঙ্ক d = done_workers.fetch_add(1, std::memory_order_release);
		যদি (d + 1 == অঙ্ক_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	শূন্য wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			পূর্ণাঙ্ক অঙ্ক_active_worker_threads_2 = অঙ্ক_active_worker_threads.load(std::memory_order_relaxed);
			ঘূৰাই done_workers.load(std::memory_order_acquire) == অঙ্ক_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_স্তূপ
{
সার্ৱজনিক:
	শূন্য ঠেলো_back(T &&t) {
		LockGuard lock(mutex);
		contents.ঠেলো_back(std::move(t));
	}
	std::optional<T> try_বেৰ_কৰো_back() {
		LockGuard lock(mutex);
		যদি (contents.empty())
			ঘূৰাই std::nullopt;
		T result = std::move(contents.back());
		contents.বেৰ_কৰো_back();
		ঘূৰাই result;
	}
গুপ্ত:
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
সার্ৱজনিক:
	ShardedVector(ধ্রুৱ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.অঙ্ক_threads());
	}
	ShardedVector(ধ্রুৱ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.অঙ্ক_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	শূন্য ঢোকাও(ধ্রুৱ ThreadIndex &thread, T মান) {
		buckets[thread.thread_অঙ্ক].emplace_back(std::move(মান));
	}

	bool empty() ধ্রুৱ {
		বাবে (ধ্রুৱ std::vector<T> &bucket : buckets)
			যদি (!bucket.empty())
				ঘূৰাই false;
		ঘূৰাই true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	সার্ৱজনিক:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			যদি (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() ধ্রুৱ { ঘূৰাই *inner_it.মান(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			ঘূৰাই *this;
		}
		bool operator!=(ধ্রুৱ iterator &other) ধ্রুৱ {
			ঘূৰাই bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	গুপ্ত:
		শূন্য normalize() {
			যদি (bucket_it == bucket_end)
				ঘূৰাই;
			যেতিয়ালৈ (inner_it == bucket_it->end()) {
				++bucket_it;
				যদি (bucket_it == bucket_end) {
					inner_it.reset();
					ঘূৰাই;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { ঘূৰাই iterator(buckets.begin(), buckets.end()); }
	iterator end() { ঘূৰাই iterator(buckets.end(), buckets.end()); }
গুপ্ত:
	শূন্য init(পূর্ণাঙ্ক অঙ্ক_threads) {
		buckets.resize(অঙ্ক_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
সংরচনা SetCollisionHandler {
	শূন্য operator()(typename V::Accumulated &, typename V::Accumulated &) ধ্রুৱ {}
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
template <typename V, typename চাবি_Equality, typename CollisionHandler>
class ShardedHashtable {
সার্ৱজনিক:
	// A combination of a `V` and its hash value.
	সংরচনা মান_3 {
		মান_3(V মান, unsigned পূর্ণাঙ্ক hash) : মান(std::move(মান)), hash(hash) {}
		মান_3(মান_3 &&) = স্ৱভাৱ;
		মান_3(ধ্রুৱ মান_3 &) = delete;
		মান_3 &operator=(ধ্রুৱ মান_3 &) = delete;
		V মান;
		unsigned পূর্ণাঙ্ক hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	সংরচনা Accumulated_মান {
		Accumulated_মান(typename V::Accumulated মান, unsigned পূর্ণাঙ্ক hash) : মান(std::move(মান)), hash(hash) {}
		Accumulated_মান(Accumulated_মান &&) = স্ৱভাৱ;
#যদি defined(_MSC_VER)
		Accumulated_মান(ধ্রুৱ Accumulated_মান &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_মান &operator=(ধ্রুৱ Accumulated_মান &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			ঘূৰাই *this;
		}
#অন্যথা
		Accumulated_মান(ধ্রুৱ Accumulated_মান &) = delete;
		Accumulated_মান &operator=(ধ্রুৱ Accumulated_মান &) = delete;
#endif
		typename V::Accumulated মান;
		unsigned পূর্ণাঙ্ক hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	সংরচনা Accumulated_মান_Equality {
		চাবি_Equality inner;
		Accumulated_মান_Equality(ধ্রুৱ চাবি_Equality &inner) : inner(inner) {}
		bool operator()(ধ্রুৱ Accumulated_মান &v1, ধ্রুৱ Accumulated_মান &v2) ধ্রুৱ {
			ঘূৰাই inner(v1.মান, v2.মান);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	সংরচনা Accumulated_মান_Hash_Op {
		size_t operator()(ধ্রুৱ Accumulated_মান &v) ধ্রুৱ {
			ঘূৰাই static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_মান, Accumulated_মান_Hash_Op, Accumulated_মান_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	সার্ৱজনিক:
		Builder(ধ্রুৱ ParallelDispatchThreadPool &thread_pool, চাবি_Equality equality = চাবি_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.অঙ্ক_threads(), std::move(equality));
		}
		Builder(ধ্রুৱ ParallelDispatchThreadPool::Subpool &thread_pool, চাবি_Equality equality = চাবি_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.অঙ্ক_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		শূন্য ঢোকাও(ধ্রুৱ ThreadIndex &thread, মান_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<মান_3>> &buckets = all_buckets[thread.thread_অঙ্ক];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		শূন্য process(ধ্রুৱ ThreadIndex &thread) {
			পূর্ণাঙ্ক size = 0;
			বাবে (std::vector<std::vector<মান_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_অঙ্ক]);
			Shard &shard = shards[thread.thread_অঙ্ক];
			shard.reserve(size);
			বাবে (std::vector<std::vector<মান_3>> &buckets : all_buckets) {
				বাবে (মান_3 &মান : buckets[thread.thread_অঙ্ক])
					accumulate(মান, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<মান_3>().swap(buckets[thread.thread_অঙ্ক]);
			}
		}
	গুপ্ত:
		friend class ShardedHashtable<V, চাবি_Equality, CollisionHandler>;
		শূন্য accumulate(মান_3 &মান, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_মান accumulated_মান{std::move(মান.মান), মান.hash};
			auto [it, inserted] = shard.ঢোকাও(std::move(accumulated_মান));
			যদি (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->মান), accumulated_মান.মান);
		}
		শূন্য init(পূর্ণাঙ্ক অঙ্ক_threads, চাবি_Equality equality) {
			all_buckets.resize(অঙ্ক_threads);
			বাবে (std::vector<std::vector<মান_3>> &buckets : all_buckets)
				buckets.resize(অঙ্ক_threads);
			বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_threads; ++i)
				shards.emplace_back(0, Accumulated_মান_Hash_Op(), Accumulated_মান_Equality(equality));
		}
		ধ্রুৱ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<মান_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		বাবে (std::vector<std::vector<মান_3>> &buckets : builder.all_buckets)
			বাবে (std::vector<মান_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<মান_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = স্ৱভাৱ;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = স্ৱভাৱ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ধ্রুৱ typename V::Accumulated *find(ধ্রুৱ Accumulated_মান &v) ধ্রুৱ {
		size_t অঙ্ক_shards = shards.size();
		যদি (অঙ্ক_shards == 0)
			ঘূৰাই nullptr;
		size_t shard = static_cast<size_t>(v.hash) % অঙ্ক_shards;
		auto it = shards[shard].find(v);
		যদি (it == shards[shard].end())
			ঘূৰাই nullptr;
		ঘূৰাই &it->মান;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	শূন্য ঢোকাও(Accumulated_মান v) {
		size_t অঙ্ক_shards = shards.size();
		যদি (অঙ্ক_shards == 0)
			ঘূৰাই;
		size_t shard = static_cast<size_t>(v.hash) % অঙ্ক_shards;
		shards[shard].ঢোকাও(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	শূন্য clear(ধ্রুৱ ThreadIndex &shard) {
		Accumulated_মান_Equality equality = shards[shard.thread_অঙ্ক].চাবি_eq();
		shards[shard.thread_অঙ্ক] = Shard(0, Accumulated_মান_Hash_Op(), equality);
	}
গুপ্ত:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
সার্ৱজনিক:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(পূর্ণাঙ্ক অঙ্ক_threads, পূর্ণাঙ্ক batch_size = 100)
		: batch_size(batch_size), thread_states(অঙ্ক_threads) {}
	পূর্ণাঙ্ক অঙ্ক_threads() ধ্রুৱ { ঘূৰাই GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	শূন্য ঠেলো(ধ্রুৱ ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_অঙ্ক];
		thread_state.next_batch.emplace_back(std::move(work));
		যদি (GetSize(thread_state.next_batch) < batch_size)
			ঘূৰাই;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.ঠেলো_back(std::move(thread_state.next_batch));
		}
		যদি (was_empty) {
			UniqueLock lock(waiters_lock);
			যদি (অঙ্ক_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> বেৰ_কৰো_batch(ধ্রুৱ ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_অঙ্ক];
		যদি (!thread_state.next_batch.empty())
			ঘূৰাই std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			যদি (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.বেৰ_কৰো_back();
				ঘূৰাই batch;
			}
		}
		// From here on in this function, our work queue is empty.
		যেতিয়ালৈ (true) {
			std::vector<T> batch = try_steal(thread);
			যদি (!batch.empty()) {
				ঘূৰাই std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++অঙ্ক_waiters;
			যদি (অঙ্ক_waiters == অঙ্ক_threads()) {
				waiters_cv.notify_all();
				ঘূৰাই {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			যদি (অঙ্ক_waiters == অঙ্ক_threads())
				ঘূৰাই {};
			--অঙ্ক_waiters;
		}
	}
গুপ্ত:
	std::vector<T> try_steal(ধ্রুৱ ThreadIndex &thread) {
		বাবে (পূর্ণাঙ্ক i = 1; i < অঙ্ক_threads(); i++) {
			পূর্ণাঙ্ক other_thread_অঙ্ক = (thread.thread_অঙ্ক + i) % অঙ্ক_threads();
			ThreadState &other_thread_state = thread_states[other_thread_অঙ্ক];
			UniqueLock lock(other_thread_state.batches_lock);
			যদি (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.বেৰ_কৰো_front();
				ঘূৰাই batch;
			}
		}
		ঘূৰাই {};
	}

	পূর্ণাঙ্ক batch_size;

	সংরচনা ThreadState {
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
	পূর্ণাঙ্ক অঙ্ক_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
সার্ৱজনিক:
	MonotonicFlag() : মান(false) {}
	bool load() ধ্রুৱ { ঘূৰাই মান.load(std::memory_order_relaxed); }
	শূন্য set() { মান.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		ঘূৰাই মান.exchange(true, std::memory_order_relaxed);
	}
গুপ্ত:
	std::atomic<bool> মান;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
