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
	bool try_lock() { परत true; }
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
	ConcurrentQueue(पूर्णांक capacity = पूर्णांक_उच्च)
		: capacity(capacity) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	शून्य ढकला_back(T t)
	{
		UniqueLock lock(mutex);
		not_full_condition.रुको(lock, [this] { परत static_cast<पूर्णांक>(contents.size()) < capacity; });
		जर (contents.empty())
			not_empty_condition.notify_one();
		log_assert(!closed);
		contents.ढकला_back(std::move(t));
		जर (static_cast<पूर्णांक>(contents.size()) < capacity)
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
	std::optional<T> काढा_front()
	{
		परत काढा_front_internal(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_काढा_front()
	{
		परत काढा_front_internal(false);
	}
खाजगी:
	std::optional<T> काढा_front_internal(bool रुको)
	{
		UniqueLock lock(mutex);
		जर (रुको) {
			not_empty_condition.रुको(lock, [this] { परत !contents.empty() || closed; });
		}
		जर (contents.empty())
			परत std::nullopt;
		जर (static_cast<पूर्णांक>(contents.size()) == capacity)
			not_full_condition.notify_one();
		T result = std::move(contents.front());
		contents.काढा_front();
		जर (!contents.empty())
			not_empty_condition.notify_one();
		परत std::move(result);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_full_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_empty_condition;
	std::deque<T> contents;
	पूर्णांक capacity;
	bool closed = false;
};

class DeferredLogs
{
सार्वजनिक:
	ढाँचा <typename... Args>
	शून्य log(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ढकला_back({fmt.format(args...), false});
	}
	ढाँचा <typename... Args>
	शून्य log_error(FmtString<TypeIdentity<Args>...> fmt, Args... args)
	{
		logs.ढकला_back({fmt.format(args...), true});
	}
	शून्य flush();
खाजगी:
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
	स्थिर पूर्णांक pool_size(पूर्णांक reserved_cores, पूर्णांक max_worker_threads);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	स्थिर पूर्णांक work_pool_size(पूर्णांक reserved_cores, पूर्णांक work_units, पूर्णांक work_units_per_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(पूर्णांक pool_size, std::function<शून्य(पूर्णांक)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	पूर्णांक अंक_threads() स्थिरांक
	{
#ifdef YOSYS_ENABLE_THREADS
		परत threads.size();
#अन्यथा
		परत 0;
#endif
	}
खाजगी:
	std::function<शून्य(पूर्णांक)> body;
#ifdef YOSYS_ENABLE_THREADS
	std::vector<std::thread> threads;
#endif
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange item_range_for_worker(पूर्णांक अंक_वस्तुसंच, पूर्णांक thread_अंक, पूर्णांक अंक_threads);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
संरचना ThreadIndex {
	पूर्णांक thread_अंक;
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
	ParallelDispatchThreadPool(पूर्णांक pool_size);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	संरचना RunCtx : सार्वजनिक ThreadIndex {
		पूर्णांक अंक_threads;
		IntRange item_range(पूर्णांक अंक_वस्तुसंच) स्थिरांक {
			परत item_range_for_worker(अंक_वस्तुसंच, thread_अंक, अंक_threads);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	सार्वजनिक:
		Subpool(ParallelDispatchThreadPool &parent, पूर्णांक max_threads)
				: parent(parent), max_threads(max_threads) {}
		// Returns the number of threads that will be used when calling `run()`.
		पूर्णांक अंक_threads() स्थिरांक {
			परत parent.अंक_threads(max_threads);
		}
		शून्य run(std::function<शून्य(स्थिरांक RunCtx &)> work) {
			parent.run(std::move(work), max_threads);
		}
		ParallelDispatchThreadPool &thread_pool() { परत parent; }
	खाजगी:
		ParallelDispatchThreadPool &parent;
		पूर्णांक max_threads;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	शून्य run(std::function<शून्य(स्थिरांक RunCtx &)> work) {
		run(std::move(work), पूर्णांक_उच्च);
	}

	// Returns the number of threads that will be used when calling `run()`.
	पूर्णांक अंक_threads() स्थिरांक {
		परत अंक_threads(पूर्णांक_उच्च);
	}
खाजगी:
	मित्र class Subpool;

	शून्य run(std::function<शून्य(स्थिरांक RunCtx &)> work, पूर्णांक max_threads);
	पूर्णांक अंक_threads(पूर्णांक max_threads) स्थिरांक {
		परत std::min(अंक_worker_threads + 1, std::max(1, max_threads));
	}
	शून्य run_worker(पूर्णांक thread_अंक);

	std::function<शून्य(स्थिरांक RunCtx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	पूर्णांक अंक_worker_threads;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<पूर्णांक> अंक_active_worker_threads = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_to_workers_signal_mutex;
	CondVar main_to_workers_signal_cv;
	std::vector<uint8_t> main_to_workers_signal;
	शून्य signal_workers_start() {
		UniqueLock lock(main_to_workers_signal_mutex);
		पूर्णांक अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
		std::fill(main_to_workers_signal.begin(), main_to_workers_signal.begin() + अंक_active_worker_threads_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_to_workers_signal_cv.notify_all();
	}
	शून्य worker_wait_for_start(पूर्णांक thread_अंक) {
		UniqueLock lock(main_to_workers_signal_mutex);
		main_to_workers_signal_cv.रुको(lock, [this, thread_अंक] { परत main_to_workers_signal[thread_अंक] > 0; });
		main_to_workers_signal[thread_अंक] = 0;
	}

	std::atomic<पूर्णांक> done_workers = 0;
	Mutex workers_to_main_signal_mutex;
	CondVar workers_to_main_signal_cv;
	शून्य signal_worker_done() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		पूर्णांक अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
		पूर्णांक d = done_workers.fetch_add(1, std::memory_order_release);
		जर (d + 1 == अंक_active_worker_threads_2) {
			UniqueLock lock(workers_to_main_signal_mutex);
			workers_to_main_signal_cv.notify_all();
		}
	}
	शून्य wait_for_workers_done() {
		UniqueLock lock(workers_to_main_signal_mutex);
		workers_to_main_signal_cv.रुको(lock, [this] {
			पूर्णांक अंक_active_worker_threads_2 = अंक_active_worker_threads.load(std::memory_order_relaxed);
			परत done_workers.load(std::memory_order_acquire) == अंक_active_worker_threads_2;
		});
		done_workers.store(0, std::memory_order_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

ढाँचा <class T>
class Concurrent_चळत
{
सार्वजनिक:
	शून्य ढकला_back(T &&t) {
		LockGuard lock(mutex);
		contents.ढकला_back(std::move(t));
	}
	std::optional<T> try_काढा_back() {
		LockGuard lock(mutex);
		जर (contents.empty())
			परत std::nullopt;
		T result = std::move(contents.back());
		contents.काढा_back();
		परत result;
	}
खाजगी:
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
	ShardedVector(स्थिरांक ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.अंक_threads());
	}
	ShardedVector(स्थिरांक ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.अंक_threads());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	शून्य घाला(स्थिरांक ThreadIndex &thread, T मूल्य) {
		buckets[thread.thread_अंक].emplace_back(std::move(मूल्य));
	}

	bool empty() स्थिरांक {
		साठी (स्थिरांक std::vector<T> &bucket : buckets)
			जर (!bucket.empty())
				परत false;
		परत true;
	}

	using Buckets = std::vector<std::vector<T>>;
	class iterator {
	सार्वजनिक:
		iterator(typename Buckets::iterator bucket_it, typename Buckets::iterator bucket_end)
			: bucket_it(std::move(bucket_it)), bucket_end(std::move(bucket_end)) {
			जर (bucket_it != bucket_end)
				inner_it = bucket_it->begin();
			normalize();
		}
		T& चालक*() स्थिरांक { परत *inner_it.मूल्य(); }
		iterator &चालक++() {
			++*inner_it;
			normalize();
			परत *this;
		}
		bool चालक!=(स्थिरांक iterator &other) स्थिरांक {
			परत bucket_it != other.bucket_it || inner_it != other.inner_it;
		}
	खाजगी:
		शून्य normalize() {
			जर (bucket_it == bucket_end)
				परत;
			पर्यंत (inner_it == bucket_it->end()) {
				++bucket_it;
				जर (bucket_it == bucket_end) {
					inner_it.reset();
					परत;
				}
				inner_it = bucket_it->begin();
			}
		}
		std::optional<typename std::vector<T>::iterator> inner_it;
		typename Buckets::iterator bucket_it;
		typename Buckets::iterator bucket_end;
	};
	iterator begin() { परत iterator(buckets.begin(), buckets.end()); }
	iterator end() { परत iterator(buckets.end(), buckets.end()); }
खाजगी:
	शून्य init(पूर्णांक अंक_threads) {
		buckets.resize(अंक_threads);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
ढाँचा <typename V>
संरचना SetCollisionHandler {
	शून्य चालक()(typename V::Accumulated &, typename V::Accumulated &) स्थिरांक {}
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
ढाँचा <typename V, typename किल्ली_Equality, typename CollisionHandler>
class ShardedHashtable {
सार्वजनिक:
	// A combination of a `V` and its hash value.
	संरचना मूल्य_3 {
		मूल्य_3(V मूल्य, अचिन्हित पूर्णांक hash) : मूल्य(std::move(मूल्य)), hash(hash) {}
		मूल्य_3(मूल्य_3 &&) = पूर्वनिर्धारित;
		मूल्य_3(स्थिरांक मूल्य_3 &) = delete;
		मूल्य_3 &चालक=(स्थिरांक मूल्य_3 &) = delete;
		V मूल्य;
		अचिन्हित पूर्णांक hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	संरचना Accumulated_मूल्य {
		Accumulated_मूल्य(typename V::Accumulated मूल्य, अचिन्हित पूर्णांक hash) : मूल्य(std::move(मूल्य)), hash(hash) {}
		Accumulated_मूल्य(Accumulated_मूल्य &&) = पूर्वनिर्धारित;
#जर defined(_MSC_VER)
		Accumulated_मूल्य(स्थिरांक Accumulated_मूल्य &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_मूल्य &चालक=(स्थिरांक Accumulated_मूल्य &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			परत *this;
		}
#अन्यथा
		Accumulated_मूल्य(स्थिरांक Accumulated_मूल्य &) = delete;
		Accumulated_मूल्य &चालक=(स्थिरांक Accumulated_मूल्य &) = delete;
#endif
		typename V::Accumulated मूल्य;
		अचिन्हित पूर्णांक hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मूल्य_Equality {
		किल्ली_Equality inner;
		Accumulated_मूल्य_Equality(स्थिरांक किल्ली_Equality &inner) : inner(inner) {}
		bool चालक()(स्थिरांक Accumulated_मूल्य &v1, स्थिरांक Accumulated_मूल्य &v2) स्थिरांक {
			परत inner(v1.मूल्य, v2.मूल्य);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	संरचना Accumulated_मूल्य_Hash_Op {
		माप_प्रकार चालक()(स्थिरांक Accumulated_मूल्य &v) स्थिरांक {
			परत static_cast<माप_प्रकार>(v.hash);
		}
	};
	using Shard = std::unordered_set<Accumulated_मूल्य, Accumulated_मूल्य_Hash_Op, Accumulated_मूल्य_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	सार्वजनिक:
		Builder(स्थिरांक ParallelDispatchThreadPool &thread_pool, किल्ली_Equality equality = किल्ली_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अंक_threads(), std::move(equality));
		}
		Builder(स्थिरांक ParallelDispatchThreadPool::Subpool &thread_pool, किल्ली_Equality equality = किल्ली_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.अंक_threads(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		शून्य घाला(स्थिरांक ThreadIndex &thread, मूल्य_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::vector<std::vector<मूल्य_3>> &buckets = all_buckets[thread.thread_अंक];
			माप_प्रकार bucket = static_cast<माप_प्रकार>(v.hash) % buckets.size();
			buckets[bucket].emplace_back(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		शून्य process(स्थिरांक ThreadIndex &thread) {
			पूर्णांक size = 0;
			साठी (std::vector<std::vector<मूल्य_3>> &buckets : all_buckets)
				size += GetSize(buckets[thread.thread_अंक]);
			Shard &shard = shards[thread.thread_अंक];
			shard.reserve(size);
			साठी (std::vector<std::vector<मूल्य_3>> &buckets : all_buckets) {
				साठी (मूल्य_3 &मूल्य : buckets[thread.thread_अंक])
					accumulate(मूल्य, shard);
				// Free as much memory as we can during the parallel phase.
				std::vector<मूल्य_3>().swap(buckets[thread.thread_अंक]);
			}
		}
	खाजगी:
		मित्र class ShardedHashtable<V, किल्ली_Equality, CollisionHandler>;
		शून्य accumulate(मूल्य_3 &मूल्य, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_मूल्य accumulated_मूल्य{std::move(मूल्य.मूल्य), मूल्य.hash};
			स्वतः [it, inserted] = shard.घाला(std::move(accumulated_मूल्य));
			जर (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->मूल्य), accumulated_मूल्य.मूल्य);
		}
		शून्य init(पूर्णांक अंक_threads, किल्ली_Equality equality) {
			all_buckets.resize(अंक_threads);
			साठी (std::vector<std::vector<मूल्य_3>> &buckets : all_buckets)
				buckets.resize(अंक_threads);
			साठी (पूर्णांक i = 0; i < अंक_threads; ++i)
				shards.emplace_back(0, Accumulated_मूल्य_Hash_Op(), Accumulated_मूल्य_Equality(equality));
		}
		स्थिरांक CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::vector<std::vector<std::vector<मूल्य_3>>> all_buckets;
		std::vector<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		साठी (std::vector<std::vector<मूल्य_3>> &buckets : builder.all_buckets)
			साठी (std::vector<मूल्य_3> &bucket : buckets)
				log_assert(bucket.empty());
		// Free memory.
		std::vector<std::vector<std::vector<मूल्य_3>>>().swap(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = पूर्वनिर्धारित;
	ShardedHashtable() {}

	ShardedHashtable &चालक=(ShardedHashtable &&other) = पूर्वनिर्धारित;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	स्थिरांक typename V::Accumulated *find(स्थिरांक Accumulated_मूल्य &v) स्थिरांक {
		माप_प्रकार अंक_shards = shards.size();
		जर (अंक_shards == 0)
			परत nullptr;
		माप_प्रकार shard = static_cast<माप_प्रकार>(v.hash) % अंक_shards;
		स्वतः it = shards[shard].find(v);
		जर (it == shards[shard].end())
			परत nullptr;
		परत &it->मूल्य;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	शून्य घाला(Accumulated_मूल्य v) {
		माप_प्रकार अंक_shards = shards.size();
		जर (अंक_shards == 0)
			परत;
		माप_प्रकार shard = static_cast<माप_प्रकार>(v.hash) % अंक_shards;
		shards[shard].घाला(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	शून्य clear(स्थिरांक ThreadIndex &shard) {
		Accumulated_मूल्य_Equality equality = shards[shard.thread_अंक].किल्ली_eq();
		shards[shard.thread_अंक] = Shard(0, Accumulated_मूल्य_Hash_Op(), equality);
	}
खाजगी:
	std::vector<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
ढाँचा <typename T>
class ConcurrentWorkQueue {
सार्वजनिक:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	ConcurrentWorkQueue(पूर्णांक अंक_threads, पूर्णांक batch_size = 100)
		: batch_size(batch_size), thread_states(अंक_threads) {}
	पूर्णांक अंक_threads() स्थिरांक { परत GetSize(thread_states); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	शून्य ढकला(स्थिरांक ThreadIndex &thread, T work) {
		ThreadState &thread_state = thread_states[thread.thread_अंक];
		thread_state.next_batch.emplace_back(std::move(work));
		जर (GetSize(thread_state.next_batch) < batch_size)
			परत;
		bool was_empty;
		{
			UniqueLock lock(thread_state.batches_lock);
			was_empty = thread_state.batches.empty();
			thread_state.batches.ढकला_back(std::move(thread_state.next_batch));
		}
		जर (was_empty) {
			UniqueLock lock(waiters_lock);
			जर (अंक_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::vector<T> काढा_batch(स्थिरांक ThreadIndex &thread) {
		ThreadState &thread_state = thread_states[thread.thread_अंक];
		जर (!thread_state.next_batch.empty())
			परत std::move(thread_state.next_batch);
		// Empty our own work queue first.
		{
			UniqueLock lock(thread_state.batches_lock);
			जर (!thread_state.batches.empty()) {
				std::vector<T> batch = std::move(thread_state.batches.back());
				thread_state.batches.काढा_back();
				परत batch;
			}
		}
		// From here on in this function, our work queue is empty.
		पर्यंत (true) {
			std::vector<T> batch = try_steal(thread);
			जर (!batch.empty()) {
				परत std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			UniqueLock lock(waiters_lock);
			++अंक_waiters;
			जर (अंक_waiters == अंक_threads()) {
				waiters_cv.notify_all();
				परत {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.रुको(lock);
			जर (अंक_waiters == अंक_threads())
				परत {};
			--अंक_waiters;
		}
	}
खाजगी:
	std::vector<T> try_steal(स्थिरांक ThreadIndex &thread) {
		साठी (पूर्णांक i = 1; i < अंक_threads(); i++) {
			पूर्णांक other_thread_अंक = (thread.thread_अंक + i) % अंक_threads();
			ThreadState &other_thread_state = thread_states[other_thread_अंक];
			UniqueLock lock(other_thread_state.batches_lock);
			जर (!other_thread_state.batches.empty()) {
				std::vector<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.काढा_front();
				परत batch;
			}
		}
		परत {};
	}

	पूर्णांक batch_size;

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
	पूर्णांक अंक_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
सार्वजनिक:
	MonotonicFlag() : मूल्य(false) {}
	bool load() स्थिरांक { परत मूल्य.load(std::memory_order_relaxed); }
	शून्य set() { मूल्य.store(true, std::memory_order_relaxed); }
	bool set_and_return_old() {
		परत मूल्य.exchange(true, std::memory_order_relaxed);
	}
खाजगी:
	std::atomic<bool> मूल्य;
};

YOSYS_NAMESPACE_END

#endif // YOSYS_THREADING_H
