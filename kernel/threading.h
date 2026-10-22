#अन्तर्भाव <deque>

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/utils.h"

#ifdef YOSYS_ENABLE_THREADS
#अन्तर्भाव <condition_variable>
#अन्तर्भाव <mutex>
#अन्तर्भाव <thread>
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
#अन्यथा
संरचना Mutex {
	शून्य lock() {}
	शून्य unlock() {}
	bool try_lock() { फर्काऊ true; }
};
संरचना CondVar {
	ढाँचा <class L> शून्य रुको(L &) {}
	ढाँचा <class L, class P> शून्य रुको(L &, P) {}
	शून्य notify_one() {}
	शून्य notify_all() {}
};
संरचना UniqueLock {
	UniqueLock(Mutex &) {}
};
संरचना LockGuard {
	LockGuard(Mutex &) {}
};
#endif

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
ढाँचा <typename T>
class ConcurrentQueue
{
सार्वजनिक:
	ConcurrentQueue(पूर्णाङ्क capacity = पूर्णांक_उच्च)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	शून्य थप_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.रुको(lock, [this] { फर्काऊ static_cast<पूर्णाङ्क>(contents.size()) < capacity; });
		यदि (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.थप_back(std::move(t));
		यदि (static_cast<पूर्णाङ्क>(contents.size()) < capacity)
			not_full_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	शून्य बंद()
	{
		UniqueLock lock(mutex);
		not_empty_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> झिक_front()
	{
		फर्काऊ झिक_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_झिक_front()
	{
		फर्काऊ झिक_front_internal(false);
	}
गुप्त:
	std::optional<T> झिक_front_internal(bool रुको)
	{
		UniqueLock lock(mutex);
		यदि (रुको) {
			not_empty_condition.रुको(lock, [this] { फर्काऊ !contents.empty() || closed; });
		}
		यदि (contents.empty())
			फर्काऊ std::nullopt;
		यदि (static_cast<पूर्णाङ्क>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.झिक_front();
		यदि (!contents.empty())
			not_empty_condition.notify_one();
		फर्काऊ std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	पूर्णाङ्क capacity;
	bool closed = false;
};

class DeferredLogs
{
सार्वजनिक:
	ढाँचा <typename... Args>
	शून्य log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.थप_back({fmt.format(args...), false});
	}
	ढाँचा <typename... Args>
	शून्य log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.थप_back({fmt.format(args...), true});
	}
	शून्य flush();
गुप्त:
	संरचना Message
	{
		std::string text;
		bool error;
	};
	std::vector<Message> logs;
};

class ThreadPool
{
सार्वजनिक:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	स्थिर पूर्णाङ्क pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	स्थिर पूर्णाङ्क work_pool_size(पूर्णाङ्क reserved_cores, पूर्णाङ्क work_units, पूर्णाङ्क work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(पूर्णाङ्क pool_size, std::function<शून्य(पूर्णाङ्क)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	पूर्णाङ्क अङ्क_threads() अचर
	{
#ifdef YOSYS_ENABLE_THREADS
		फर्काऊ threads.size();
#अन्यथा
		फर्काऊ 0;
#endif
	}
गुप्त:
	std::function<शून्य(पूर्णाङ्क)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(पूर्णाङ्क अङ्क_वस्तुहरू, पूर्णाङ्क thread_अङ्क, पूर्णाङ्क अङ्क_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
संरचना ThreadIndex {
	पूर्णाङ्क thread_अङ्क;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
सार्वजनिक:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(पूर्णाङ्क pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	संरचना RunCtx : सार्वजनिक ThreadIndex {
		पूर्णाङ्क अङ्क_threads;
		IntRange item_range(पूर्णाङ्क अङ्क_वस्तुहरू) अचर {
			फर्काऊ item_range_for_worker(अङ्क_वस्तुहरू, thread_अङ्क, अङ्क_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	सार्वजनिक:
		Subpool(ParallelDispatchThreadPool &parent, पूर्णाङ्क max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		पूर्णाङ्क अङ्क_threads() अचर {
			फर्काऊ parent.अङ्क_threads(max_threads);
		}
		शून्य run(std::function<शून्य(अचर RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { फर्काऊ parent; }
	गुप्त:
		ParallelDispatchThreadPool &parent;
		पूर्णाङ्क max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	शून्य run(std::function<शून्य(अचर RunCtx &)> work) {
		run(std::move(work), पूर्णांक_उच्च);
	}

	// Returns the number of threads that will be used when calling `run()`.
	पूर्णाङ्क अङ्क_threads() अचर {
		फर्काऊ अङ्क_threads(पूर्णांक_उच्च);
	}
गुप्त:
	मित्र class Subpool;

	शून्य run(std::function<शून्य(अचर RunCtx &)> work, पूर्णाङ्क max_threads);
	पूर्णाङ्क अङ्क_threads(पूर्णाङ्क max_threads) अचर {
		फर्काऊ std::min(अङ्क_worker_threads + 1, std::max(1, max_threads));
	}
	शून्य run_worker(पूर्णाङ्क thread_अङ्क);

	std::function<शून्य(अचर RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	पूर्णाङ्क अङ्क_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<पूर्णाङ्क> अङ्क_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	शून्य signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		पूर्णाङ्क अङ्क_active_worker_threads_2 = अङ्क_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + अङ्क_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	शून्य worker_wait_for_start(पूर्णाङ्क thread_अङ्क) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.रुको(lock, [this, thread_अङ्क] { फर्काऊ main_to_workers_signal[thread_अङ्क] > 0; });
		main_to_workers_signal[thread_अङ्क] = 0;
	}

	std::atomic<पूर्णाङ्क> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	शून्य signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		पूर्णाङ्क अङ्क_active_worker_threads_2 = अङ्क_active_worker_threads.load(std::memory_order_relaxed);
		पूर्णाङ्क d = done_workers.fetch_add(1, std::memory_order_release);
		यदि (d + 1 == अङ्क_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	शून्य wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.रुको(lock, [this] {
			पूर्णाङ्क अङ्क_active_worker_threads_2 = अङ्क_active_worker_threads.load(std::memory_order_relaxed);
			फर्काऊ done_workers.load(std::memory_order_acquire) == अङ्क_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

ढाँचा <class T>
class Concurrent_थाक
{
सार्वजनिक:
	शून्य थप_back(T &&t) {
		LockGuard lock(mutex);
		contents.थप_back(std::move(t));
	}
	std::optional<T> try_झिक_back() {
		LockGuard lock(mutex);
		यदि (contents.empty())
			फर्काऊ std::nullopt;
		T result = std::move(contents.back());
		contents.झिक_back();
		फर्काऊ result;
	}
गुप्त:
	Mutex mutex;
	std::vector<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
ढाँचा <typename T>
class ShardedVector {
सार्वजनिक:
	ShardedVector(अचर ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.अङ्क_threads());
	}
	ShardedVector(अचर ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.अङ्क_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	शून्य घुसाउ(अचर ThreadIndex &thread, T मान) {
		buckets[thread.thread_अङ्क].emplace_back(std::move(मान));
	}

	bool empty() अचर {
		लागि (अचर std::vector<T> &bucket : buckets)
			यदि (!bucket.empty())
				फर्काऊ false;
		फर्काऊ true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	सार्वजनिक:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			यदि (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& चालक*() अचर { फर्काऊ *inner_it.मान(); }
		iterator &चालक++() {
			++*inner_it;
			normalize();
			फर्काऊ *this;
		}
		bool चालक!=(अचर iterator &other) अचर {
			फर्काऊ bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	गुप्त:
		शून्य normalize() {
			यदि (bucket_it == bucket_end)
				फर्काऊ;
			जबसम्म (inner_it == bucket_it->end()) {
				++bucket_it;
				यदि (bucket_it == bucket_end) {
					inner_it.reset();
					फर्काऊ;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { फर्काऊ iterator(buckets.begin(), buckets.end()); }
	iterator end() { फर्काऊ iterator(buckets.end(), buckets.end()); }
गुप्त:
	शून्य init(पूर्णाङ्क अङ्क_threads) {
		buckets.resize(अङ्क_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
ढाँचा <typename V>
संरचना SetCollisionHandler {
	शून्य चालक()(typename V::Accumulated &, typename V::Accumulated &) अचर {}
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
ढाँचा <typename V, typename साँचो_Equality, typename CollisionHandler>
class ShardedHashtable {
सार्वजनिक:
	// A combination of a `V` and its hash value.
	संरचना मान_3 {
		मान_3(V मान, अचिन्हित पूर्णाङ्क hash) : मान(std::move(मान)), hash(hash) {}
		मान_3(मान_3 &&) = स्वभाव;
		मान_3(अचर मान_3 &) = delete;
		मान_3 &चालक=(अचर मान_3 &) = delete;
		V मान;
		अचिन्हित पूर्णाङ्क hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	संरचना Accumulated_मान {
		Accumulated_मान(typename V::Accumulated मान, अचिन्हित पूर्णाङ्क hash) : मान(std::move(मान)), hash(hash) {}
		Accumulated_मान(Accumulated_मान &&) = स्वभाव;
#यदि defined(_MSC_VER)
		Accumulated_मान(अचर Accumulated_मान &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_मान &चालक=(अचर Accumulated_मान &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			फर्काऊ *this;
		}
#अन्यथा
		Accumulated_मान(अचर Accumulated_मान &) = delete;
		Accumulated_मान &चालक=(अचर Accumulated_मान &) = delete;
#endif
		typename V::Accumulated मान;
		अचिन्हित पूर्णाङ्क hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मान_Equality {
		साँचो_Equality inner;
		Accumulated_मान_Equality(अचर साँचो_Equality &inner) : inner(inner) {}
		bool चालक()(अचर Accumulated_मान &v1, अचर Accumulated_मान &v2) अचर {
			फर्काऊ inner(v1.मान, v2.मान);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मान_Hash_Op {
		माप_प्रकार चालक()(अचर Accumulated_मान &v) अचर {
			फर्काऊ static_cast<माप_प्रकार>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_मान, Accumulated_मान_Hash_Op, Accumulated_मान_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	सार्वजनिक:
		Builder(अचर ParallelDispatchThreadPool &thread_pool, साँचो_Equality equality = साँचो_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अङ्क_threads(), std::move(equality));
		}
		Builder(अचर ParallelDispatchThreadPool::Subpool &thread_pool, साँचो_Equality equality = साँचो_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अङ्क_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		शून्य घुसाउ(अचर ThreadIndex &thread, मान_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<मान_3>> &buckets = all_buckets[thread.thread_अङ्क];
			माप_प्रकार bucket = static_cast<माप_प्रकार>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		शून्य process(अचर ThreadIndex &thread) {
			पूर्णाङ्क size = 0;
			लागि (std::vector<std::vector<मान_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_अङ्क]);
			Shard &shard = shards[thread.thread_अङ्क];
			shard.reserve(size);
			लागि (std::vector<std::vector<मान_3>> &buckets : all_buckets) {
				लागि (मान_3 &मान : buckets[thread.thread_अङ्क])
					accumulate(मान, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<मान_3>().swap(buckets[thread.thread_अङ्क]);
			}
		}
	गुप्त:
		मित्र class ShardedHashtable<V, साँचो_Equality, CollisionHandler>;
		शून्य accumulate(मान_3 &मान, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_मान accumulated_मान{std::move(मान.मान), मान.hash};
			स्वतः [it, inserted] = shard.घुसाउ(std::move(accumulated_मान));
			यदि (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->मान), accumulated_मान.मान);
		}
		शून्य init(पूर्णाङ्क अङ्क_threads, साँचो_Equality equality) {
			all_buckets.resize(अङ्क_threads);
			लागि (std::vector<std::vector<मान_3>> &buckets : all_buckets)
				buckets.resize(अङ्क_threads);
			लागि (पूर्णाङ्क i = 0; i < अङ्क_threads; ++i)
				shards.emplace_back(0, Accumulated_मान_Hash_Op(), Accumulated_मान_Equality(equality));
		}
		अचर CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<मान_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		लागि (std::vector<std::vector<मान_3>> &buckets : builder.all_buckets)
			लागि (std::vector<मान_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<मान_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = स्वभाव;
	ShardedHashtable() {}

	ShardedHashtable &चालक=(ShardedHashtable &&other) = स्वभाव;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	अचर typename V::Accumulated *find(अचर Accumulated_मान &v) अचर {
		माप_प्रकार अङ्क_shards = shards.size();
		यदि (अङ्क_shards == 0)
			फर्काऊ nullptr;
		माप_प्रकार shard = static_cast<माप_प्रकार>(v.hash) % अङ्क_shards;
		स्वतः it = shards[shard].find(v);
		यदि (it == shards[shard].end())
			फर्काऊ nullptr;
		फर्काऊ &it->मान;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	शून्य घुसाउ(Accumulated_मान v) {
		माप_प्रकार अङ्क_shards = shards.size();
		यदि (अङ्क_shards == 0)
			फर्काऊ;
		माप_प्रकार shard = static_cast<माप_प्रकार>(v.hash) % अङ्क_shards;
		shards[shard].घुसाउ(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	शून्य clear(अचर ThreadIndex &shard) {
		Accumulated_मान_Equality equality = shards[shard.thread_अङ्क].साँचो_eq();
		shards[shard.thread_अङ्क] = Shard(0, Accumulated_मान_Hash_Op(), equality);
	}
गुप्त:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
ढाँचा <typename T>
class ConcurrentWorkQueue {
सार्वजनिक:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(पूर्णाङ्क अङ्क_threads, पूर्णाङ्क batch_size = 100)
		: batch_size(batch_size), thread_states(अङ्क_threads) {}
	पूर्णाङ्क अङ्क_threads() अचर { फर्काऊ GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	शून्य थप(अचर ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_अङ्क];
		thread_state.next_batch.emplace_back(std::move(work));
		यदि (GetSize(thread_state.next_batch) < batch_size)
			फर्काऊ;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.थप_back(std::move(thread_state.next_batch));
		}
		यदि (was_empty) {
			UniqueLock lock(waiters_lock);
			यदि (अङ्क_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> झिक_batch(अचर ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_अङ्क];
		यदि (!thread_state.next_batch.empty())
			फर्काऊ std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			यदि (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.झिक_back();
				फर्काऊ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		जबसम्म (true) {
			std::vector<T> batch = try_steal(thread);
			यदि (!batch.empty()) {
				फर्काऊ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++अङ्क_waiters;
			यदि (अङ्क_waiters == अङ्क_threads()) {
				waiters_cv.notify_all();
				फर्काऊ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.रुको(lock);
			यदि (अङ्क_waiters == अङ्क_threads())
				फर्काऊ {};
			--अङ्क_waiters;
		}
	}
गुप्त:
	std::vector<T> try_steal(अचर ThreadIndex &thread) {
		लागि (पूर्णाङ्क i = 1; i < अङ्क_threads(); i++) {
			पूर्णाङ्क other_thread_अङ्क = (thread.thread_अङ्क + i) % अङ्क_threads();
			ThreadState &other_thread_state = thread_states[other_thread_अङ्क];
			UniqueLock lock(other_thread_state.batches_lock);
			यदि (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.झिक_front();
				फर्काऊ batch;
			}
		}
		फर्काऊ {};
	}

	पूर्णाङ्क batch_size;

	संरचना ThreadState {
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
	पूर्णाङ्क अङ्क_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
सार्वजनिक:
	MonotonicFlag() : मान(false) {}
	bool load() अचर { फर्काऊ मान.load(std::memory_order_relaxed); }
	शून्य set() { मान.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		फर्काऊ मान.exchange(true, std::memory_order_relaxed);
	}
गुप्त:
	std::atomic<bool> मान;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
