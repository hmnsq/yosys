#ಅನ್ತರ್ಭಾವ <deque>

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#ಅನ್ತರ್ಭಾವ <condition_variable>
#ಅನ್ತರ್ಭಾವ <mutex>
#ಅನ್ತರ್ಭಾವ <thread>
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
#ಅನ್ಯಥಾ
ಸಂರಚನಾ Mutex {
	ಶೂನ್ಯ lock() {}
	ಶೂನ್ಯ unlock() {}
	bool try_lock() { ಪ್ರತ್ಯಾಗಮ true; }
};
ಸಂರಚನಾ CondVar {
	template <class L> ಶೂನ್ಯ wait(L &) {}
	template <class L, class P> ಶೂನ್ಯ wait(L &, P) {}
	ಶೂನ್ಯ notify_one() {}
	ಶೂನ್ಯ notify_all() {}
};
ಸಂರಚನಾ UniqueLock {
	UniqueLock(Mutex &) {}
};
ಸಂರಚನಾ LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
ಸಾರ್ವಜನಿಕ:
	ConcurrentQueue(ಪೂರ್ಣಾಙ್ಕ capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	ಶೂನ್ಯ ತಳ್ಳು_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { ಪ್ರತ್ಯಾಗಮ static_cast<ಪೂರ್ಣಾಙ್ಕ>(contents.size()) < capacity; });
		ಯದಿ (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.ತಳ್ಳು_back(std::move(t));
		ಯದಿ (static_cast<ಪೂರ್ಣಾಙ್ಕ>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	ಶೂನ್ಯ close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> ತೆಗೆ_front()
	{
		ಪ್ರತ್ಯಾಗಮ ತೆಗೆ_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_ತೆಗೆ_front()
	{
		ಪ್ರತ್ಯಾಗಮ ತೆಗೆ_front_internal(false);
	}
ಗುಪ್ತ:
	std::optional<T> ತೆಗೆ_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		ಯದಿ (wait) {
			not_empty_condition.wait(lock, [this] { ಪ್ರತ್ಯಾಗಮ !contents.empty() || closed; });
		}
		ಯದಿ (contents.empty())
			ಪ್ರತ್ಯಾಗಮ std::nullopt;
		ಯದಿ (static_cast<ಪೂರ್ಣಾಙ್ಕ>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.ತೆಗೆ_front();
		ಯದಿ (!contents.empty())
			not_empty_condition.notify_one();
		ಪ್ರತ್ಯಾಗಮ std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	ಪೂರ್ಣಾಙ್ಕ capacity;
	bool closed = false;
};

class DeferredLogs
{
ಸಾರ್ವಜನಿಕ:
	template <typename... Args>
	ಶೂನ್ಯ log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ತಳ್ಳು_back({fmt.format(args...), false});
	}
	template <typename... Args>
	ಶೂನ್ಯ log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ತಳ್ಳು_back({fmt.format(args...), true});
	}
	ಶೂನ್ಯ flush();
ಗುಪ್ತ:
	ಸಂರಚನಾ Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
ಸಾರ್ವಜನಿಕ:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pool_size(ಪೂರ್ಣಾಙ್ಕ reserved_cores, ಪೂರ್ಣಾಙ್ಕ max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ work_pool_size(ಪೂರ್ಣಾಙ್ಕ reserved_cores, ಪೂರ್ಣಾಙ್ಕ work_units, ಪೂರ್ಣಾಙ್ಕ work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(ಪೂರ್ಣಾಙ್ಕ pool_size, std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಙ್ಕ)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads() ಧ್ರುವ
	{
#ifdef YOSYS_ENABLE_THREADS
		ಪ್ರತ್ಯಾಗಮ threads.size();
#ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ 0;
#endif
	}
ಗುಪ್ತ:
	std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಙ್ಕ)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ವಸ್ತುಗಳು, ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ಸಂರಚನಾ ThreadIndex {
	ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
ಸಾರ್ವಜನಿಕ:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(ಪೂರ್ಣಾಙ್ಕ pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ಸಂರಚನಾ RunCtx : ಸಾರ್ವಜನಿಕ ThreadIndex {
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads;
		IntRange item_range(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ವಸ್ತುಗಳು) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ item_range_for_worker(ಅಂಕೆ_ವಸ್ತುಗಳು, thread_ಅಂಕೆ, ಅಂಕೆ_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	ಸಾರ್ವಜನಿಕ:
		Subpool(ParallelDispatchThreadPool &parent, ಪೂರ್ಣಾಙ್ಕ max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads() ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ parent.ಅಂಕೆ_threads(max_threads);
		}
		ಶೂನ್ಯ run(std::function<ಶೂನ್ಯ(ಧ್ರುವ RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { ಪ್ರತ್ಯಾಗಮ parent; }
	ಗುಪ್ತ:
		ParallelDispatchThreadPool &parent;
		ಪೂರ್ಣಾಙ್ಕ max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	ಶೂನ್ಯ run(std::function<ಶೂನ್ಯ(ಧ್ರುವ RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_threads(INT_MAX);
	}
ಗುಪ್ತ:
	friend class Subpool;

	ಶೂನ್ಯ run(std::function<ಶೂನ್ಯ(ಧ್ರುವ RunCtx &)> work, ಪೂರ್ಣಾಙ್ಕ max_threads);
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads(ಪೂರ್ಣಾಙ್ಕ max_threads) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ std::min(ಅಂಕೆ_worker_threads + 1, std::max(1, max_threads));
	}
	ಶೂನ್ಯ run_worker(ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ);

	std::function<ಶೂನ್ಯ(ಧ್ರುವ RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<ಪೂರ್ಣಾಙ್ಕ> ಅಂಕೆ_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	ಶೂನ್ಯ signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_active_worker_threads_2 = ಅಂಕೆ_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + ಅಂಕೆ_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	ಶೂನ್ಯ worker_wait_for_start(ಪೂರ್ಣಾಙ್ಕ thread_ಅಂಕೆ) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_ಅಂಕೆ] { ಪ್ರತ್ಯಾಗಮ main_to_workers_signal[thread_ಅಂಕೆ] > 0; });
		main_to_workers_signal[thread_ಅಂಕೆ] = 0;
	}

	std::atomic<ಪೂರ್ಣಾಙ್ಕ> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	ಶೂನ್ಯ signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_active_worker_threads_2 = ಅಂಕೆ_active_worker_threads.load(std::memory_order_relaxed);
		ಪೂರ್ಣಾಙ್ಕ d = done_workers.fetch_add(1, std::memory_order_release);
		ಯದಿ (d + 1 == ಅಂಕೆ_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	ಶೂನ್ಯ wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_active_worker_threads_2 = ಅಂಕೆ_active_worker_threads.load(std::memory_order_relaxed);
			ಪ್ರತ್ಯಾಗಮ done_workers.load(std::memory_order_acquire) == ಅಂಕೆ_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_ರಾಶಿ
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ತಳ್ಳು_back(T &&t) {
		LockGuard lock(mutex);
		contents.ತಳ್ಳು_back(std::move(t));
	}
	std::optional<T> try_ತೆಗೆ_back() {
		LockGuard lock(mutex);
		ಯದಿ (contents.empty())
			ಪ್ರತ್ಯಾಗಮ std::nullopt;
		T result = std::move(contents.back());
		contents.ತೆಗೆ_back();
		ಪ್ರತ್ಯಾಗಮ result;
	}
ಗುಪ್ತ:
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
ಸಾರ್ವಜನಿಕ:
	ShardedVector(ಧ್ರುವ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.ಅಂಕೆ_threads());
	}
	ShardedVector(ಧ್ರುವ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.ಅಂಕೆ_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	ಶೂನ್ಯ ಸೇರಿಸು(ಧ್ರುವ ThreadIndex &thread, T ಮೌಲ್ಯ) {
		buckets[thread.thread_ಅಂಕೆ].emplace_back(std::move(ಮೌಲ್ಯ));
	}

	bool empty() ಧ್ರುವ {
		ಪ್ರತಿ (ಧ್ರುವ std::vector<T> &bucket : buckets)
			ಯದಿ (!bucket.empty())
				ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	ಸಾರ್ವಜನಿಕ:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			ಯದಿ (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *inner_it.ಮೌಲ್ಯ(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			ಪ್ರತ್ಯಾಗಮ *this;
		}
		bool operator!=(ಧ್ರುವ iterator &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	ಗುಪ್ತ:
		ಶೂನ್ಯ normalize() {
			ಯದಿ (bucket_it == bucket_end)
				ಪ್ರತ್ಯಾಗಮ;
			ಯಾವತ್ (inner_it == bucket_it->end()) {
				++bucket_it;
				ಯದಿ (bucket_it == bucket_end) {
					inner_it.reset();
					ಪ್ರತ್ಯಾಗಮ;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { ಪ್ರತ್ಯಾಗಮ iterator(buckets.begin(), buckets.end()); }
	iterator end() { ಪ್ರತ್ಯಾಗಮ iterator(buckets.end(), buckets.end()); }
ಗುಪ್ತ:
	ಶೂನ್ಯ init(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads) {
		buckets.resize(ಅಂಕೆ_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
ಸಂರಚನಾ SetCollisionHandler {
	ಶೂನ್ಯ operator()(typename V::Accumulated &, typename V::Accumulated &) ಧ್ರುವ {}
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
template <typename V, typename ಕೀಲಿ_Equality, typename CollisionHandler>
class ShardedHashtable {
ಸಾರ್ವಜನಿಕ:
	// A combination of a `V` and its hash value.
	ಸಂರಚನಾ ಮೌಲ್ಯ_3 {
		ಮೌಲ್ಯ_3(V ಮೌಲ್ಯ, unsigned ಪೂರ್ಣಾಙ್ಕ hash) : ಮೌಲ್ಯ(std::move(ಮೌಲ್ಯ)), hash(hash) {}
		ಮೌಲ್ಯ_3(ಮೌಲ್ಯ_3 &&) = ಸ್ವಭಾವ;
		ಮೌಲ್ಯ_3(ಧ್ರುವ ಮೌಲ್ಯ_3 &) = delete;
		ಮೌಲ್ಯ_3 &operator=(ಧ್ರುವ ಮೌಲ್ಯ_3 &) = delete;
		V ಮೌಲ್ಯ;
		unsigned ಪೂರ್ಣಾಙ್ಕ hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ಸಂರಚನಾ Accumulated_ಮೌಲ್ಯ {
		Accumulated_ಮೌಲ್ಯ(typename V::Accumulated ಮೌಲ್ಯ, unsigned ಪೂರ್ಣಾಙ್ಕ hash) : ಮೌಲ್ಯ(std::move(ಮೌಲ್ಯ)), hash(hash) {}
		Accumulated_ಮೌಲ್ಯ(Accumulated_ಮೌಲ್ಯ &&) = ಸ್ವಭಾವ;
#ಯದಿ defined(_MSC_VER)
		Accumulated_ಮೌಲ್ಯ(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_ಮೌಲ್ಯ &operator=(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			ಪ್ರತ್ಯಾಗಮ *this;
		}
#ಅನ್ಯಥಾ
		Accumulated_ಮೌಲ್ಯ(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &) = delete;
		Accumulated_ಮೌಲ್ಯ &operator=(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &) = delete;
#endif
		typename V::Accumulated ಮೌಲ್ಯ;
		unsigned ಪೂರ್ಣಾಙ್ಕ hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ಸಂರಚನಾ Accumulated_ಮೌಲ್ಯ_Equality {
		ಕೀಲಿ_Equality inner;
		Accumulated_ಮೌಲ್ಯ_Equality(ಧ್ರುವ ಕೀಲಿ_Equality &inner) : inner(inner) {}
		bool operator()(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &v1, ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &v2) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ inner(v1.ಮೌಲ್ಯ, v2.ಮೌಲ್ಯ);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ಸಂರಚನಾ Accumulated_ಮೌಲ್ಯ_Hash_Op {
		size_t operator()(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &v) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_ಮೌಲ್ಯ, Accumulated_ಮೌಲ್ಯ_Hash_Op, Accumulated_ಮೌಲ್ಯ_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	ಸಾರ್ವಜನಿಕ:
		Builder(ಧ್ರುವ ParallelDispatchThreadPool &thread_pool, ಕೀಲಿ_Equality equality = ಕೀಲಿ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ಅಂಕೆ_threads(), std::move(equality));
		}
		Builder(ಧ್ರುವ ParallelDispatchThreadPool::Subpool &thread_pool, ಕೀಲಿ_Equality equality = ಕೀಲಿ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ಅಂಕೆ_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		ಶೂನ್ಯ ಸೇರಿಸು(ಧ್ರುವ ThreadIndex &thread, ಮೌಲ್ಯ_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<ಮೌಲ್ಯ_3>> &buckets = all_buckets[thread.thread_ಅಂಕೆ];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		ಶೂನ್ಯ process(ಧ್ರುವ ThreadIndex &thread) {
			ಪೂರ್ಣಾಙ್ಕ size = 0;
			ಪ್ರತಿ (std::vector<std::vector<ಮೌಲ್ಯ_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_ಅಂಕೆ]);
			Shard &shard = shards[thread.thread_ಅಂಕೆ];
			shard.reserve(size);
			ಪ್ರತಿ (std::vector<std::vector<ಮೌಲ್ಯ_3>> &buckets : all_buckets) {
				ಪ್ರತಿ (ಮೌಲ್ಯ_3 &ಮೌಲ್ಯ : buckets[thread.thread_ಅಂಕೆ])
					accumulate(ಮೌಲ್ಯ, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<ಮೌಲ್ಯ_3>().swap(buckets[thread.thread_ಅಂಕೆ]);
			}
		}
	ಗುಪ್ತ:
		friend class ShardedHashtable<V, ಕೀಲಿ_Equality, CollisionHandler>;
		ಶೂನ್ಯ accumulate(ಮೌಲ್ಯ_3 &ಮೌಲ್ಯ, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_ಮೌಲ್ಯ accumulated_ಮೌಲ್ಯ{std::move(ಮೌಲ್ಯ.ಮೌಲ್ಯ), ಮೌಲ್ಯ.hash};
			auto [it, inserted] = shard.ಸೇರಿಸು(std::move(accumulated_ಮೌಲ್ಯ));
			ಯದಿ (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->ಮೌಲ್ಯ), accumulated_ಮೌಲ್ಯ.ಮೌಲ್ಯ);
		}
		ಶೂನ್ಯ init(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads, ಕೀಲಿ_Equality equality) {
			all_buckets.resize(ಅಂಕೆ_threads);
			ಪ್ರತಿ (std::vector<std::vector<ಮೌಲ್ಯ_3>> &buckets : all_buckets)
				buckets.resize(ಅಂಕೆ_threads);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_threads; ++i)
				shards.emplace_back(0, Accumulated_ಮೌಲ್ಯ_Hash_Op(), Accumulated_ಮೌಲ್ಯ_Equality(equality));
		}
		ಧ್ರುವ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<ಮೌಲ್ಯ_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		ಪ್ರತಿ (std::vector<std::vector<ಮೌಲ್ಯ_3>> &buckets : builder.all_buckets)
			ಪ್ರತಿ (std::vector<ಮೌಲ್ಯ_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<ಮೌಲ್ಯ_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = ಸ್ವಭಾವ;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = ಸ್ವಭಾವ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ಧ್ರುವ typename V::Accumulated *find(ಧ್ರುವ Accumulated_ಮೌಲ್ಯ &v) ಧ್ರುವ {
		size_t ಅಂಕೆ_shards = shards.size();
		ಯದಿ (ಅಂಕೆ_shards == 0)
			ಪ್ರತ್ಯಾಗಮ nullptr;
		size_t shard = static_cast<size_t>(v.hash) % ಅಂಕೆ_shards;
		auto it = shards[shard].find(v);
		ಯದಿ (it == shards[shard].end())
			ಪ್ರತ್ಯಾಗಮ nullptr;
		ಪ್ರತ್ಯಾಗಮ &it->ಮೌಲ್ಯ;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	ಶೂನ್ಯ ಸೇರಿಸು(Accumulated_ಮೌಲ್ಯ v) {
		size_t ಅಂಕೆ_shards = shards.size();
		ಯದಿ (ಅಂಕೆ_shards == 0)
			ಪ್ರತ್ಯಾಗಮ;
		size_t shard = static_cast<size_t>(v.hash) % ಅಂಕೆ_shards;
		shards[shard].ಸೇರಿಸು(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	ಶೂನ್ಯ clear(ಧ್ರುವ ThreadIndex &shard) {
		Accumulated_ಮೌಲ್ಯ_Equality equality = shards[shard.thread_ಅಂಕೆ].ಕೀಲಿ_eq();
		shards[shard.thread_ಅಂಕೆ] = Shard(0, Accumulated_ಮೌಲ್ಯ_Hash_Op(), equality);
	}
ಗುಪ್ತ:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
ಸಾರ್ವಜನಿಕ:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads, ಪೂರ್ಣಾಙ್ಕ batch_size = 100)
		: batch_size(batch_size), thread_states(ಅಂಕೆ_threads) {}
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_threads() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	ಶೂನ್ಯ ತಳ್ಳು(ಧ್ರುವ ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_ಅಂಕೆ];
		thread_state.next_batch.emplace_back(std::move(work));
		ಯದಿ (GetSize(thread_state.next_batch) < batch_size)
			ಪ್ರತ್ಯಾಗಮ;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.ತಳ್ಳು_back(std::move(thread_state.next_batch));
		}
		ಯದಿ (was_empty) {
			UniqueLock lock(waiters_lock);
			ಯದಿ (ಅಂಕೆ_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> ತೆಗೆ_batch(ಧ್ರುವ ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_ಅಂಕೆ];
		ಯದಿ (!thread_state.next_batch.empty())
			ಪ್ರತ್ಯಾಗಮ std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			ಯದಿ (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.ತೆಗೆ_back();
				ಪ್ರತ್ಯಾಗಮ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		ಯಾವತ್ (true) {
			std::vector<T> batch = try_steal(thread);
			ಯದಿ (!batch.empty()) {
				ಪ್ರತ್ಯಾಗಮ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++ಅಂಕೆ_waiters;
			ಯದಿ (ಅಂಕೆ_waiters == ಅಂಕೆ_threads()) {
				waiters_cv.notify_all();
				ಪ್ರತ್ಯಾಗಮ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			ಯದಿ (ಅಂಕೆ_waiters == ಅಂಕೆ_threads())
				ಪ್ರತ್ಯಾಗಮ {};
			--ಅಂಕೆ_waiters;
		}
	}
ಗುಪ್ತ:
	std::vector<T> try_steal(ಧ್ರುವ ThreadIndex &thread) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < ಅಂಕೆ_threads(); i++) {
			ಪೂರ್ಣಾಙ್ಕ other_thread_ಅಂಕೆ = (thread.thread_ಅಂಕೆ + i) % ಅಂಕೆ_threads();
			ThreadState &other_thread_state = thread_states[other_thread_ಅಂಕೆ];
			UniqueLock lock(other_thread_state.batches_lock);
			ಯದಿ (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.ತೆಗೆ_front();
				ಪ್ರತ್ಯಾಗಮ batch;
			}
		}
		ಪ್ರತ್ಯಾಗಮ {};
	}

	ಪೂರ್ಣಾಙ್ಕ batch_size;

	ಸಂರಚನಾ ThreadState {
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
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
ಸಾರ್ವಜನಿಕ:
	MonotonicFlag() : ಮೌಲ್ಯ(false) {}
	bool load() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ.load(std::memory_order_relaxed); }
	ಶೂನ್ಯ set() { ಮೌಲ್ಯ.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ.exchange(true, std::memory_order_relaxed);
	}
ಗುಪ್ತ:
	std::atomic<bool> ಮೌಲ್ಯ;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
