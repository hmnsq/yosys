#ଅନ୍ତର୍ଭାଵ <deque>

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#ଅନ୍ତର୍ଭାଵ <condition_variable>
#ଅନ୍ତର୍ଭାଵ <mutex>
#ଅନ୍ତର୍ଭାଵ <thread>
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
#ଅନ୍ୟଥା
ସଂରଚନା Mutex {
	ଶୂନ୍ୟ lock() {}
	ଶୂନ୍ୟ unlock() {}
	bool try_lock() { ଫେରନ୍ତୁ true; }
};
ସଂରଚନା CondVar {
	template <class L> ଶୂନ୍ୟ wait(L &) {}
	template <class L, class P> ଶୂନ୍ୟ wait(L &, P) {}
	ଶୂନ୍ୟ notify_one() {}
	ଶୂନ୍ୟ notify_all() {}
};
ସଂରଚନା UniqueLock {
	UniqueLock(Mutex &) {}
};
ସଂରଚନା LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
ସର୍ବସାଧାରଣ:
	ConcurrentQueue(ପୂର୍ଣ୍ଣସଂଖ୍ୟା capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	ଶୂନ୍ୟ ଠେଲ_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { ଫେରନ୍ତୁ static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(contents.size()) < capacity; });
		ଯଦି (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.ଠେଲ_back(std::move(t));
		ଯଦି (static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	ଶୂନ୍ୟ close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> କାଢ଼_front()
	{
		ଫେରନ୍ତୁ କାଢ଼_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_କାଢ଼_front()
	{
		ଫେରନ୍ତୁ କାଢ଼_front_internal(false);
	}
ବ୍ୟକ୍ତିଗତ:
	std::optional<T> କାଢ଼_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		ଯଦି (wait) {
			not_empty_condition.wait(lock, [this] { ଫେରନ୍ତୁ !contents.empty() || closed; });
		}
		ଯଦି (contents.empty())
			ଫେରନ୍ତୁ std::nullopt;
		ଯଦି (static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.କାଢ଼_front();
		ଯଦି (!contents.empty())
			not_empty_condition.notify_one();
		ଫେରନ୍ତୁ std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା capacity;
	bool closed = false;
};

class DeferredLogs
{
ସର୍ବସାଧାରଣ:
	template <typename... Args>
	ଶୂନ୍ୟ log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ଠେଲ_back({fmt.format(args...), false});
	}
	template <typename... Args>
	ଶୂନ୍ୟ log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ଠେଲ_back({fmt.format(args...), true});
	}
	ଶୂନ୍ୟ flush();
ବ୍ୟକ୍ତିଗତ:
	ସଂରଚନା Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
ସର୍ବସାଧାରଣ:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserved_cores, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_pool_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserved_cores, ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units, ପୂର୍ଣ୍ଣସଂଖ୍ୟା work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size, std::function<ଶୂନ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads() ଅଚଳ
	{
#ifdef YOSYS_ENABLE_THREADS
		ଫେରନ୍ତୁ threads.size();
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ 0;
#endif
	}
ବ୍ୟକ୍ତିଗତ:
	std::function<ଶୂନ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ସଂରଚନା ThreadIndex {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
ସର୍ବସାଧାରଣ:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ସଂରଚନା RunCtx : ସର୍ବସାଧାରଣ ThreadIndex {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads;
		IntRange item_range(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ) ଅଚଳ {
			ଫେରନ୍ତୁ item_range_for_worker(ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, thread_ଅଙ୍କ, ଅଙ୍କ_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	ସର୍ବସାଧାରଣ:
		Subpool(ParallelDispatchThreadPool &parent, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads() ଅଚଳ {
			ଫେରନ୍ତୁ parent.ଅଙ୍କ_threads(max_threads);
		}
		ଶୂନ୍ୟ run(std::function<ଶୂନ୍ୟ(ଅଚଳ RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { ଫେରନ୍ତୁ parent; }
	ବ୍ୟକ୍ତିଗତ:
		ParallelDispatchThreadPool &parent;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	ଶୂନ୍ୟ run(std::function<ଶୂନ୍ୟ(ଅଚଳ RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads() ଅଚଳ {
		ଫେରନ୍ତୁ ଅଙ୍କ_threads(INT_MAX);
	}
ବ୍ୟକ୍ତିଗତ:
	friend class Subpool;

	ଶୂନ୍ୟ run(std::function<ଶୂନ୍ୟ(ଅଚଳ RunCtx &)> work, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_threads) ଅଚଳ {
		ଫେରନ୍ତୁ std::min(ଅଙ୍କ_worker_threads + 1, std::max(1, max_threads));
	}
	ଶୂନ୍ୟ run_worker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ);

	std::function<ଶୂନ୍ୟ(ଅଚଳ RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଙ୍କ_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	ଶୂନ୍ୟ signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_active_worker_threads_2 = ଅଙ୍କ_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + ଅଙ୍କ_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	ଶୂନ୍ୟ worker_wait_for_start(ପୂର୍ଣ୍ଣସଂଖ୍ୟା thread_ଅଙ୍କ) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_ଅଙ୍କ] { ଫେରନ୍ତୁ main_to_workers_signal[thread_ଅଙ୍କ] > 0; });
		main_to_workers_signal[thread_ଅଙ୍କ] = 0;
	}

	std::atomic<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	ଶୂନ୍ୟ signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_active_worker_threads_2 = ଅଙ୍କ_active_worker_threads.load(std::memory_order_relaxed);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = done_workers.fetch_add(1, std::memory_order_release);
		ଯଦି (d + 1 == ଅଙ୍କ_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	ଶୂନ୍ୟ wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_active_worker_threads_2 = ଅଙ୍କ_active_worker_threads.load(std::memory_order_relaxed);
			ଫେରନ୍ତୁ done_workers.load(std::memory_order_acquire) == ଅଙ୍କ_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_ଗଦା
{
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ ଠେଲ_back(T &&t) {
		LockGuard lock(mutex);
		contents.ଠେଲ_back(std::move(t));
	}
	std::optional<T> try_କାଢ଼_back() {
		LockGuard lock(mutex);
		ଯଦି (contents.empty())
			ଫେରନ୍ତୁ std::nullopt;
		T result = std::move(contents.back());
		contents.କାଢ଼_back();
		ଫେରନ୍ତୁ result;
	}
ବ୍ୟକ୍ତିଗତ:
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
ସର୍ବସାଧାରଣ:
	ShardedVector(ଅଚଳ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.ଅଙ୍କ_threads());
	}
	ShardedVector(ଅଚଳ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.ଅଙ୍କ_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ଅଚଳ ThreadIndex &thread, T ମୂଲ୍ୟ) {
		buckets[thread.thread_ଅଙ୍କ].emplace_back(std::move(ମୂଲ୍ୟ));
	}

	bool empty() ଅଚଳ {
		ପାଇଁ (ଅଚଳ std::vector<T> &bucket : buckets)
			ଯଦି (!bucket.empty())
				ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	ସର୍ବସାଧାରଣ:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			ଯଦି (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() ଅଚଳ { ଫେରନ୍ତୁ *inner_it.ମୂଲ୍ୟ(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			ଫେରନ୍ତୁ *this;
		}
		bool operator!=(ଅଚଳ iterator &other) ଅଚଳ {
			ଫେରନ୍ତୁ bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	ବ୍ୟକ୍ତିଗତ:
		ଶୂନ୍ୟ normalize() {
			ଯଦି (bucket_it == bucket_end)
				ଫେରନ୍ତୁ;
			ଯେତେବେଳେ (inner_it == bucket_it->end()) {
				++bucket_it;
				ଯଦି (bucket_it == bucket_end) {
					inner_it.reset();
					ଫେରନ୍ତୁ;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { ଫେରନ୍ତୁ iterator(buckets.begin(), buckets.end()); }
	iterator end() { ଫେରନ୍ତୁ iterator(buckets.end(), buckets.end()); }
ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads) {
		buckets.resize(ଅଙ୍କ_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
ସଂରଚନା SetCollisionHandler {
	ଶୂନ୍ୟ operator()(typename V::Accumulated &, typename V::Accumulated &) ଅଚଳ {}
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
template <typename V, typename ଚାବି_Equality, typename CollisionHandler>
class ShardedHashtable {
ସର୍ବସାଧାରଣ:
	// A combination of a `V` and its hash value.
	ସଂରଚନା ମୂଲ୍ୟ_3 {
		ମୂଲ୍ୟ_3(V ମୂଲ୍ୟ, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash) : ମୂଲ୍ୟ(std::move(ମୂଲ୍ୟ)), hash(hash) {}
		ମୂଲ୍ୟ_3(ମୂଲ୍ୟ_3 &&) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
		ମୂଲ୍ୟ_3(ଅଚଳ ମୂଲ୍ୟ_3 &) = delete;
		ମୂଲ୍ୟ_3 &operator=(ଅଚଳ ମୂଲ୍ୟ_3 &) = delete;
		V ମୂଲ୍ୟ;
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ସଂରଚନା Accumulated_ମୂଲ୍ୟ {
		Accumulated_ମୂଲ୍ୟ(typename V::Accumulated ମୂଲ୍ୟ, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash) : ମୂଲ୍ୟ(std::move(ମୂଲ୍ୟ)), hash(hash) {}
		Accumulated_ମୂଲ୍ୟ(Accumulated_ମୂଲ୍ୟ &&) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
#ଯଦି defined(_MSC_VER)
		Accumulated_ମୂଲ୍ୟ(ଅଚଳ Accumulated_ମୂଲ୍ୟ &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_ମୂଲ୍ୟ &operator=(ଅଚଳ Accumulated_ମୂଲ୍ୟ &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			ଫେରନ୍ତୁ *this;
		}
#ଅନ୍ୟଥା
		Accumulated_ମୂଲ୍ୟ(ଅଚଳ Accumulated_ମୂଲ୍ୟ &) = delete;
		Accumulated_ମୂଲ୍ୟ &operator=(ଅଚଳ Accumulated_ମୂଲ୍ୟ &) = delete;
#endif
		typename V::Accumulated ମୂଲ୍ୟ;
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ସଂରଚନା Accumulated_ମୂଲ୍ୟ_Equality {
		ଚାବି_Equality inner;
		Accumulated_ମୂଲ୍ୟ_Equality(ଅଚଳ ଚାବି_Equality &inner) : inner(inner) {}
		bool operator()(ଅଚଳ Accumulated_ମୂଲ୍ୟ &v1, ଅଚଳ Accumulated_ମୂଲ୍ୟ &v2) ଅଚଳ {
			ଫେରନ୍ତୁ inner(v1.ମୂଲ୍ୟ, v2.ମୂଲ୍ୟ);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ସଂରଚନା Accumulated_ମୂଲ୍ୟ_Hash_Op {
		size_t operator()(ଅଚଳ Accumulated_ମୂଲ୍ୟ &v) ଅଚଳ {
			ଫେରନ୍ତୁ static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_ମୂଲ୍ୟ, Accumulated_ମୂଲ୍ୟ_Hash_Op, Accumulated_ମୂଲ୍ୟ_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	ସର୍ବସାଧାରଣ:
		Builder(ଅଚଳ ParallelDispatchThreadPool &thread_pool, ଚାବି_Equality equality = ଚାବି_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ଅଙ୍କ_threads(), std::move(equality));
		}
		Builder(ଅଚଳ ParallelDispatchThreadPool::Subpool &thread_pool, ଚାବି_Equality equality = ଚାବି_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ଅଙ୍କ_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		ଶୂନ୍ୟ ଭର୍ତ୍ତି(ଅଚଳ ThreadIndex &thread, ମୂଲ୍ୟ_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<ମୂଲ୍ୟ_3>> &buckets = all_buckets[thread.thread_ଅଙ୍କ];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		ଶୂନ୍ୟ process(ଅଚଳ ThreadIndex &thread) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = 0;
			ପାଇଁ (std::vector<std::vector<ମୂଲ୍ୟ_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_ଅଙ୍କ]);
			Shard &shard = shards[thread.thread_ଅଙ୍କ];
			shard.reserve(size);
			ପାଇଁ (std::vector<std::vector<ମୂଲ୍ୟ_3>> &buckets : all_buckets) {
				ପାଇଁ (ମୂଲ୍ୟ_3 &ମୂଲ୍ୟ : buckets[thread.thread_ଅଙ୍କ])
					accumulate(ମୂଲ୍ୟ, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<ମୂଲ୍ୟ_3>().swap(buckets[thread.thread_ଅଙ୍କ]);
			}
		}
	ବ୍ୟକ୍ତିଗତ:
		friend class ShardedHashtable<V, ଚାବି_Equality, CollisionHandler>;
		ଶୂନ୍ୟ accumulate(ମୂଲ୍ୟ_3 &ମୂଲ୍ୟ, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_ମୂଲ୍ୟ accumulated_ମୂଲ୍ୟ{std::move(ମୂଲ୍ୟ.ମୂଲ୍ୟ), ମୂଲ୍ୟ.hash};
			auto [it, inserted] = shard.ଭର୍ତ୍ତି(std::move(accumulated_ମୂଲ୍ୟ));
			ଯଦି (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->ମୂଲ୍ୟ), accumulated_ମୂଲ୍ୟ.ମୂଲ୍ୟ);
		}
		ଶୂନ୍ୟ init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads, ଚାବି_Equality equality) {
			all_buckets.resize(ଅଙ୍କ_threads);
			ପାଇଁ (std::vector<std::vector<ମୂଲ୍ୟ_3>> &buckets : all_buckets)
				buckets.resize(ଅଙ୍କ_threads);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_threads; ++i)
				shards.emplace_back(0, Accumulated_ମୂଲ୍ୟ_Hash_Op(), Accumulated_ମୂଲ୍ୟ_Equality(equality));
		}
		ଅଚଳ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<ମୂଲ୍ୟ_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		ପାଇଁ (std::vector<std::vector<ମୂଲ୍ୟ_3>> &buckets : builder.all_buckets)
			ପାଇଁ (std::vector<ମୂଲ୍ୟ_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<ମୂଲ୍ୟ_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ଅଚଳ typename V::Accumulated *find(ଅଚଳ Accumulated_ମୂଲ୍ୟ &v) ଅଚଳ {
		size_t ଅଙ୍କ_shards = shards.size();
		ଯଦି (ଅଙ୍କ_shards == 0)
			ଫେରନ୍ତୁ nullptr;
		size_t shard = static_cast<size_t>(v.hash) % ଅଙ୍କ_shards;
		auto it = shards[shard].find(v);
		ଯଦି (it == shards[shard].end())
			ଫେରନ୍ତୁ nullptr;
		ଫେରନ୍ତୁ &it->ମୂଲ୍ୟ;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	ଶୂନ୍ୟ ଭର୍ତ୍ତି(Accumulated_ମୂଲ୍ୟ v) {
		size_t ଅଙ୍କ_shards = shards.size();
		ଯଦି (ଅଙ୍କ_shards == 0)
			ଫେରନ୍ତୁ;
		size_t shard = static_cast<size_t>(v.hash) % ଅଙ୍କ_shards;
		shards[shard].ଭର୍ତ୍ତି(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	ଶୂନ୍ୟ clear(ଅଚଳ ThreadIndex &shard) {
		Accumulated_ମୂଲ୍ୟ_Equality equality = shards[shard.thread_ଅଙ୍କ].ଚାବି_eq();
		shards[shard.thread_ଅଙ୍କ] = Shard(0, Accumulated_ମୂଲ୍ୟ_Hash_Op(), equality);
	}
ବ୍ୟକ୍ତିଗତ:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
ସର୍ବସାଧାରଣ:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads, ପୂର୍ଣ୍ଣସଂଖ୍ୟା batch_size = 100)
		: batch_size(batch_size), thread_states(ଅଙ୍କ_threads) {}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_threads() ଅଚଳ { ଫେରନ୍ତୁ GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	ଶୂନ୍ୟ ଠେଲ(ଅଚଳ ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_ଅଙ୍କ];
		thread_state.next_batch.emplace_back(std::move(work));
		ଯଦି (GetSize(thread_state.next_batch) < batch_size)
			ଫେରନ୍ତୁ;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.ଠେଲ_back(std::move(thread_state.next_batch));
		}
		ଯଦି (was_empty) {
			UniqueLock lock(waiters_lock);
			ଯଦି (ଅଙ୍କ_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> କାଢ଼_batch(ଅଚଳ ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_ଅଙ୍କ];
		ଯଦି (!thread_state.next_batch.empty())
			ଫେରନ୍ତୁ std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			ଯଦି (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.କାଢ଼_back();
				ଫେରନ୍ତୁ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		ଯେତେବେଳେ (true) {
			std::vector<T> batch = try_steal(thread);
			ଯଦି (!batch.empty()) {
				ଫେରନ୍ତୁ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++ଅଙ୍କ_waiters;
			ଯଦି (ଅଙ୍କ_waiters == ଅଙ୍କ_threads()) {
				waiters_cv.notify_all();
				ଫେରନ୍ତୁ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			ଯଦି (ଅଙ୍କ_waiters == ଅଙ୍କ_threads())
				ଫେରନ୍ତୁ {};
			--ଅଙ୍କ_waiters;
		}
	}
ବ୍ୟକ୍ତିଗତ:
	std::vector<T> try_steal(ଅଚଳ ThreadIndex &thread) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < ଅଙ୍କ_threads(); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_thread_ଅଙ୍କ = (thread.thread_ଅଙ୍କ + i) % ଅଙ୍କ_threads();
			ThreadState &other_thread_state = thread_states[other_thread_ଅଙ୍କ];
			UniqueLock lock(other_thread_state.batches_lock);
			ଯଦି (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.କାଢ଼_front();
				ଫେରନ୍ତୁ batch;
			}
		}
		ଫେରନ୍ତୁ {};
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା batch_size;

	ସଂରଚନା ThreadState {
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
ସର୍ବସାଧାରଣ:
	MonotonicFlag() : ମୂଲ୍ୟ(false) {}
	bool load() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ.load(std::memory_order_relaxed); }
	ଶୂନ୍ୟ set() { ମୂଲ୍ୟ.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ.exchange(true, std::memory_order_relaxed);
	}
ବ୍ୟକ୍ତିଗତ:
	std::atomic<bool> ମୂଲ୍ୟ;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
