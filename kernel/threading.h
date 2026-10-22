#അന്തര്ഭാവ <deque>

#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/log.h"
#അന്തര്ഭാവ "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#അന്തര്ഭാവ <condition_variable>
#അന്തര്ഭാവ <mutex>
#അന്തര്ഭാവ <thread>
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
#അല്ലെങ്കിൽ
ഘടന Mutex {
	ശൂന്യം lock() {}
	ശൂന്യം unlock() {}
	bool try_lock() { തിരികെ true; }
};
ഘടന CondVar {
	template <class L> ശൂന്യം wait(L &) {}
	template <class L, class P> ശൂന്യം wait(L &, P) {}
	ശൂന്യം notify_one() {}
	ശൂന്യം notify_all() {}
};
ഘടന UniqueLock {
	UniqueLock(Mutex &) {}
};
ഘടന LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
template <typename T>
class ConcurrentQueue
{
പൊതു:
	ConcurrentQueue(പൂർണ്ണസംഖ്യ capacity = INT_MAX)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	ശൂന്യം തള്ളുക_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.wait(lock, [this] { തിരികെ static_cast<പൂർണ്ണസംഖ്യ>(contents.size()) < capacity; });
		എങ്കിൽ (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.തള്ളുക_back(std::move(t));
		എങ്കിൽ (static_cast<പൂർണ്ണസംഖ്യ>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	ശൂന്യം close()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> എടുക്കുക_front()
	{
		തിരികെ എടുക്കുക_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_എടുക്കുക_front()
	{
		തിരികെ എടുക്കുക_front_internal(false);
	}
സ്വകാര്യ:
	std::optional<T> എടുക്കുക_front_internal(bool wait)
	{
		UniqueLock lock(mutex);
		എങ്കിൽ (wait) {
			not_empty_condition.wait(lock, [this] { തിരികെ !contents.empty() || closed; });
		}
		എങ്കിൽ (contents.empty())
			തിരികെ std::nullopt;
		എങ്കിൽ (static_cast<പൂർണ്ണസംഖ്യ>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.എടുക്കുക_front();
		എങ്കിൽ (!contents.empty())
			not_empty_condition.notify_one();
		തിരികെ std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	പൂർണ്ണസംഖ്യ capacity;
	bool closed = false;
};

class DeferredLogs
{
പൊതു:
	template <typename... Args>
	ശൂന്യം log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.തള്ളുക_back({fmt.format(args...), false});
	}
	template <typename... Args>
	ശൂന്യം log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.തള്ളുക_back({fmt.format(args...), true});
	}
	ശൂന്യം flush();
സ്വകാര്യ:
	ഘടന Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
പൊതു:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	സ്ഥിതം പൂർണ്ണസംഖ്യ pool_size(പൂർണ്ണസംഖ്യ reserved_cores, പൂർണ്ണസംഖ്യ max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	സ്ഥിതം പൂർണ്ണസംഖ്യ work_pool_size(പൂർണ്ണസംഖ്യ reserved_cores, പൂർണ്ണസംഖ്യ work_units, പൂർണ്ണസംഖ്യ work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(പൂർണ്ണസംഖ്യ pool_size, std::function<ശൂന്യം(പൂർണ്ണസംഖ്യ)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	പൂർണ്ണസംഖ്യ അക്കം_threads() സ്ഥിരം
	{
#ifdef YOSYS_ENABLE_THREADS
		തിരികെ threads.size();
#അല്ലെങ്കിൽ
		തിരികെ 0;
#endif
	}
സ്വകാര്യ:
	std::function<ശൂന്യം(പൂർണ്ണസംഖ്യ)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(പൂർണ്ണസംഖ്യ അക്കം_ഇനങ്ങൾ, പൂർണ്ണസംഖ്യ thread_അക്കം, പൂർണ്ണസംഖ്യ അക്കം_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ഘടന ThreadIndex {
	പൂർണ്ണസംഖ്യ thread_അക്കം;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
പൊതു:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(പൂർണ്ണസംഖ്യ pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ഘടന RunCtx : പൊതു ThreadIndex {
		പൂർണ്ണസംഖ്യ അക്കം_threads;
		IntRange item_range(പൂർണ്ണസംഖ്യ അക്കം_ഇനങ്ങൾ) സ്ഥിരം {
			തിരികെ item_range_for_worker(അക്കം_ഇനങ്ങൾ, thread_അക്കം, അക്കം_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	പൊതു:
		Subpool(ParallelDispatchThreadPool &parent, പൂർണ്ണസംഖ്യ max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		പൂർണ്ണസംഖ്യ അക്കം_threads() സ്ഥിരം {
			തിരികെ parent.അക്കം_threads(max_threads);
		}
		ശൂന്യം run(std::function<ശൂന്യം(സ്ഥിരം RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { തിരികെ parent; }
	സ്വകാര്യ:
		ParallelDispatchThreadPool &parent;
		പൂർണ്ണസംഖ്യ max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	ശൂന്യം run(std::function<ശൂന്യം(സ്ഥിരം RunCtx &)> work) {
		run(std::move(work), INT_MAX);
	}

	// Returns the number of threads that will be used when calling `run()`.
	പൂർണ്ണസംഖ്യ അക്കം_threads() സ്ഥിരം {
		തിരികെ അക്കം_threads(INT_MAX);
	}
സ്വകാര്യ:
	friend class Subpool;

	ശൂന്യം run(std::function<ശൂന്യം(സ്ഥിരം RunCtx &)> work, പൂർണ്ണസംഖ്യ max_threads);
	പൂർണ്ണസംഖ്യ അക്കം_threads(പൂർണ്ണസംഖ്യ max_threads) സ്ഥിരം {
		തിരികെ std::min(അക്കം_worker_threads + 1, std::max(1, max_threads));
	}
	ശൂന്യം run_worker(പൂർണ്ണസംഖ്യ thread_അക്കം);

	std::function<ശൂന്യം(സ്ഥിരം RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	പൂർണ്ണസംഖ്യ അക്കം_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<പൂർണ്ണസംഖ്യ> അക്കം_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	ശൂന്യം signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		പൂർണ്ണസംഖ്യ അക്കം_active_worker_threads_2 = അക്കം_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + അക്കം_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	ശൂന്യം worker_wait_for_start(പൂർണ്ണസംഖ്യ thread_അക്കം) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.wait(lock, [this, thread_അക്കം] { തിരികെ main_to_workers_signal[thread_അക്കം] > 0; });
		main_to_workers_signal[thread_അക്കം] = 0;
	}

	std::atomic<പൂർണ്ണസംഖ്യ> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	ശൂന്യം signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		പൂർണ്ണസംഖ്യ അക്കം_active_worker_threads_2 = അക്കം_active_worker_threads.load(std::memory_order_relaxed);
		പൂർണ്ണസംഖ്യ d = done_workers.fetch_add(1, std::memory_order_release);
		എങ്കിൽ (d + 1 == അക്കം_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	ശൂന്യം wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.wait(lock, [this] {
			പൂർണ്ണസംഖ്യ അക്കം_active_worker_threads_2 = അക്കം_active_worker_threads.load(std::memory_order_relaxed);
			തിരികെ done_workers.load(std::memory_order_acquire) == അക്കം_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

template <class T>
class Concurrent_അട്ടി
{
പൊതു:
	ശൂന്യം തള്ളുക_back(T &&t) {
		LockGuard lock(mutex);
		contents.തള്ളുക_back(std::move(t));
	}
	std::optional<T> try_എടുക്കുക_back() {
		LockGuard lock(mutex);
		എങ്കിൽ (contents.empty())
			തിരികെ std::nullopt;
		T result = std::move(contents.back());
		contents.എടുക്കുക_back();
		തിരികെ result;
	}
സ്വകാര്യ:
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
പൊതു:
	ShardedVector(സ്ഥിരം ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.അക്കം_threads());
	}
	ShardedVector(സ്ഥിരം ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.അക്കം_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	ശൂന്യം തിരുകുക(സ്ഥിരം ThreadIndex &thread, T മൂല്യം) {
		buckets[thread.thread_അക്കം].emplace_back(std::move(മൂല്യം));
	}

	bool empty() സ്ഥിരം {
		വേണ്ടി (സ്ഥിരം std::vector<T> &bucket : buckets)
			എങ്കിൽ (!bucket.empty())
				തിരികെ false;
		തിരികെ true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	പൊതു:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			എങ്കിൽ (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& operator*() സ്ഥിരം { തിരികെ *inner_it.മൂല്യം(); }
		iterator &operator++() {
			++*inner_it;
			normalize();
			തിരികെ *this;
		}
		bool operator!=(സ്ഥിരം iterator &other) സ്ഥിരം {
			തിരികെ bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	സ്വകാര്യ:
		ശൂന്യം normalize() {
			എങ്കിൽ (bucket_it == bucket_end)
				തിരികെ;
			ആകുമ്പോൾ (inner_it == bucket_it->end()) {
				++bucket_it;
				എങ്കിൽ (bucket_it == bucket_end) {
					inner_it.reset();
					തിരികെ;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { തിരികെ iterator(buckets.begin(), buckets.end()); }
	iterator end() { തിരികെ iterator(buckets.end(), buckets.end()); }
സ്വകാര്യ:
	ശൂന്യം init(പൂർണ്ണസംഖ്യ അക്കം_threads) {
		buckets.resize(അക്കം_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
template <typename V>
ഘടന SetCollisionHandler {
	ശൂന്യം operator()(typename V::Accumulated &, typename V::Accumulated &) സ്ഥിരം {}
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
template <typename V, typename താക്കോൽ_Equality, typename CollisionHandler>
class ShardedHashtable {
പൊതു:
	// A combination of a `V` and its hash value.
	ഘടന മൂല്യം_3 {
		മൂല്യം_3(V മൂല്യം, unsigned പൂർണ്ണസംഖ്യ hash) : മൂല്യം(std::move(മൂല്യം)), hash(hash) {}
		മൂല്യം_3(മൂല്യം_3 &&) = സ്ഥിരസ്ഥിതി;
		മൂല്യം_3(സ്ഥിരം മൂല്യം_3 &) = delete;
		മൂല്യം_3 &operator=(സ്ഥിരം മൂല്യം_3 &) = delete;
		V മൂല്യം;
		unsigned പൂർണ്ണസംഖ്യ hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ഘടന Accumulated_മൂല്യം {
		Accumulated_മൂല്യം(typename V::Accumulated മൂല്യം, unsigned പൂർണ്ണസംഖ്യ hash) : മൂല്യം(std::move(മൂല്യം)), hash(hash) {}
		Accumulated_മൂല്യം(Accumulated_മൂല്യം &&) = സ്ഥിരസ്ഥിതി;
#എങ്കിൽ defined(_MSC_VER)
		Accumulated_മൂല്യം(സ്ഥിരം Accumulated_മൂല്യം &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_മൂല്യം &operator=(സ്ഥിരം Accumulated_മൂല്യം &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			തിരികെ *this;
		}
#അല്ലെങ്കിൽ
		Accumulated_മൂല്യം(സ്ഥിരം Accumulated_മൂല്യം &) = delete;
		Accumulated_മൂല്യം &operator=(സ്ഥിരം Accumulated_മൂല്യം &) = delete;
#endif
		typename V::Accumulated മൂല്യം;
		unsigned പൂർണ്ണസംഖ്യ hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ഘടന Accumulated_മൂല്യം_Equality {
		താക്കോൽ_Equality inner;
		Accumulated_മൂല്യം_Equality(സ്ഥിരം താക്കോൽ_Equality &inner) : inner(inner) {}
		bool operator()(സ്ഥിരം Accumulated_മൂല്യം &v1, സ്ഥിരം Accumulated_മൂല്യം &v2) സ്ഥിരം {
			തിരികെ inner(v1.മൂല്യം, v2.മൂല്യം);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ഘടന Accumulated_മൂല്യം_Hash_Op {
		size_t operator()(സ്ഥിരം Accumulated_മൂല്യം &v) സ്ഥിരം {
			തിരികെ static_cast<size_t>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_മൂല്യം, Accumulated_മൂല്യം_Hash_Op, Accumulated_മൂല്യം_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	പൊതു:
		Builder(സ്ഥിരം ParallelDispatchThreadPool &thread_pool, താക്കോൽ_Equality equality = താക്കോൽ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.അക്കം_threads(), std::move(equality));
		}
		Builder(സ്ഥിരം ParallelDispatchThreadPool::Subpool &thread_pool, താക്കോൽ_Equality equality = താക്കോൽ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.അക്കം_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		ശൂന്യം തിരുകുക(സ്ഥിരം ThreadIndex &thread, മൂല്യം_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<മൂല്യം_3>> &buckets = all_buckets[thread.thread_അക്കം];
			size_t bucket = static_cast<size_t>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		ശൂന്യം process(സ്ഥിരം ThreadIndex &thread) {
			പൂർണ്ണസംഖ്യ size = 0;
			വേണ്ടി (std::vector<std::vector<മൂല്യം_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_അക്കം]);
			Shard &shard = shards[thread.thread_അക്കം];
			shard.reserve(size);
			വേണ്ടി (std::vector<std::vector<മൂല്യം_3>> &buckets : all_buckets) {
				വേണ്ടി (മൂല്യം_3 &മൂല്യം : buckets[thread.thread_അക്കം])
					accumulate(മൂല്യം, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<മൂല്യം_3>().swap(buckets[thread.thread_അക്കം]);
			}
		}
	സ്വകാര്യ:
		friend class ShardedHashtable<V, താക്കോൽ_Equality, CollisionHandler>;
		ശൂന്യം accumulate(മൂല്യം_3 &മൂല്യം, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_മൂല്യം accumulated_മൂല്യം{std::move(മൂല്യം.മൂല്യം), മൂല്യം.hash};
			auto [it, inserted] = shard.തിരുകുക(std::move(accumulated_മൂല്യം));
			എങ്കിൽ (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->മൂല്യം), accumulated_മൂല്യം.മൂല്യം);
		}
		ശൂന്യം init(പൂർണ്ണസംഖ്യ അക്കം_threads, താക്കോൽ_Equality equality) {
			all_buckets.resize(അക്കം_threads);
			വേണ്ടി (std::vector<std::vector<മൂല്യം_3>> &buckets : all_buckets)
				buckets.resize(അക്കം_threads);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_threads; ++i)
				shards.emplace_back(0, Accumulated_മൂല്യം_Hash_Op(), Accumulated_മൂല്യം_Equality(equality));
		}
		സ്ഥിരം CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<മൂല്യം_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		വേണ്ടി (std::vector<std::vector<മൂല്യം_3>> &buckets : builder.all_buckets)
			വേണ്ടി (std::vector<മൂല്യം_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<മൂല്യം_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = സ്ഥിരസ്ഥിതി;
	ShardedHashtable() {}

	ShardedHashtable &operator=(ShardedHashtable &&other) = സ്ഥിരസ്ഥിതി;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	സ്ഥിരം typename V::Accumulated *find(സ്ഥിരം Accumulated_മൂല്യം &v) സ്ഥിരം {
		size_t അക്കം_shards = shards.size();
		എങ്കിൽ (അക്കം_shards == 0)
			തിരികെ nullptr;
		size_t shard = static_cast<size_t>(v.hash) % അക്കം_shards;
		auto it = shards[shard].find(v);
		എങ്കിൽ (it == shards[shard].end())
			തിരികെ nullptr;
		തിരികെ &it->മൂല്യം;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	ശൂന്യം തിരുകുക(Accumulated_മൂല്യം v) {
		size_t അക്കം_shards = shards.size();
		എങ്കിൽ (അക്കം_shards == 0)
			തിരികെ;
		size_t shard = static_cast<size_t>(v.hash) % അക്കം_shards;
		shards[shard].തിരുകുക(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	ശൂന്യം clear(സ്ഥിരം ThreadIndex &shard) {
		Accumulated_മൂല്യം_Equality equality = shards[shard.thread_അക്കം].താക്കോൽ_eq();
		shards[shard.thread_അക്കം] = Shard(0, Accumulated_മൂല്യം_Hash_Op(), equality);
	}
സ്വകാര്യ:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
template <typename T>
class ConcurrentWorkQueue {
പൊതു:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(പൂർണ്ണസംഖ്യ അക്കം_threads, പൂർണ്ണസംഖ്യ batch_size = 100)
		: batch_size(batch_size), thread_states(അക്കം_threads) {}
	പൂർണ്ണസംഖ്യ അക്കം_threads() സ്ഥിരം { തിരികെ GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	ശൂന്യം തള്ളുക(സ്ഥിരം ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_അക്കം];
		thread_state.next_batch.emplace_back(std::move(work));
		എങ്കിൽ (GetSize(thread_state.next_batch) < batch_size)
			തിരികെ;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.തള്ളുക_back(std::move(thread_state.next_batch));
		}
		എങ്കിൽ (was_empty) {
			UniqueLock lock(waiters_lock);
			എങ്കിൽ (അക്കം_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> എടുക്കുക_batch(സ്ഥിരം ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_അക്കം];
		എങ്കിൽ (!thread_state.next_batch.empty())
			തിരികെ std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			എങ്കിൽ (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.എടുക്കുക_back();
				തിരികെ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		ആകുമ്പോൾ (true) {
			std::vector<T> batch = try_steal(thread);
			എങ്കിൽ (!batch.empty()) {
				തിരികെ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++അക്കം_waiters;
			എങ്കിൽ (അക്കം_waiters == അക്കം_threads()) {
				waiters_cv.notify_all();
				തിരികെ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(lock);
			എങ്കിൽ (അക്കം_waiters == അക്കം_threads())
				തിരികെ {};
			--അക്കം_waiters;
		}
	}
സ്വകാര്യ:
	std::vector<T> try_steal(സ്ഥിരം ThreadIndex &thread) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < അക്കം_threads(); i++) {
			പൂർണ്ണസംഖ്യ other_thread_അക്കം = (thread.thread_അക്കം + i) % അക്കം_threads();
			ThreadState &other_thread_state = thread_states[other_thread_അക്കം];
			UniqueLock lock(other_thread_state.batches_lock);
			എങ്കിൽ (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.എടുക്കുക_front();
				തിരികെ batch;
			}
		}
		തിരികെ {};
	}

	പൂർണ്ണസംഖ്യ batch_size;

	ഘടന ThreadState {
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
	പൂർണ്ണസംഖ്യ അക്കം_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
പൊതു:
	MonotonicFlag() : മൂല്യം(false) {}
	bool load() സ്ഥിരം { തിരികെ മൂല്യം.load(std::memory_order_relaxed); }
	ശൂന്യം set() { മൂല്യം.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		തിരികെ മൂല്യം.exchange(true, std::memory_order_relaxed);
	}
സ്വകാര്യ:
	std::atomic<bool> മൂല്യം;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
