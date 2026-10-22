#ਅਨ੍ਤਰ੍ਭਾਵ <deque>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/utils.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
#ਅਨ੍ਤਰ੍ਭਾਵ <condition_variable>
#ਅਨ੍ਤਰ੍ਭਾਵ <mutex>
#ਅਨ੍ਤਰ੍ਭਾਵ <thread>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_THREADING_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_ਤਾਲਾ = std::unique_ਤਾਲਾ<Mutex>;
using ਤਾਲਾ_Guard = std::ਤਾਲਾ_guard<Mutex>;
#ਨਹੀਂਤਾਂ
ਬਣਤਰ Mutex {
	ਖਾਲੀ ਤਾਲਾ() {}
	ਖਾਲੀ unlock() {}
	bool try_ਤਾਲਾ() { ਵਾਪਸ true; }
};
ਬਣਤਰ CondVar {
	ਸਾਂਚਾ <class L> ਖਾਲੀ wait(L &) {}
	ਸਾਂਚਾ <class L, class P> ਖਾਲੀ wait(L &, P) {}
	ਖਾਲੀ notify_one() {}
	ਖਾਲੀ notify_all() {}
};
ਬਣਤਰ Unique_ਤਾਲਾ {
	Unique_ਤਾਲਾ(Mutex &) {}
};
ਬਣਤਰ ਤਾਲਾ_Guard {
	ਤਾਲਾ_Guard(Mutex &) {}
};
#ਜੇ_ਅੰਤ

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
ਸਾਂਚਾ <typename T>
class Concurrent_ਕਤਾਰ
{
ਜਨਤਕ:
	Concurrent_ਕਤਾਰ(ਪੂਰਨ ਸਮਰੱਥਾ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
		: ਸਮਰੱਥਾ(ਸਮਰੱਥਾ) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(T t)
	{
		Unique_ਤਾਲਾ ਤਾਲਾ(mutex);
		not_ਭਰਿਆ_condition.wait(ਤਾਲਾ, [this] { ਵਾਪਸ static_cast<ਪੂਰਨ>(contents.ਮਾਪ()) < ਸਮਰੱਥਾ; });
		ਜੇ (contents.ਖਾਲੀ_ਥਾਵਾਂ())
			not_ਖਾਲੀ_ਥਾਵਾਂ_condition.notify_one();
		log_assert(!closed);
		contents.ਧੱਕੋ_ਪਿੱਠ(std::move(t));
		ਜੇ (static_cast<ਪੂਰਨ>(contents.ਮਾਪ()) < ਸਮਰੱਥਾ)
			not_ਭਰਿਆ_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	ਖਾਲੀ close()
	{
		Unique_ਤਾਲਾ ਤਾਲਾ(mutex);
		not_ਖਾਲੀ_ਥਾਵਾਂ_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> ਕੱਢੋ_front()
	{
		ਵਾਪਸ ਕੱਢੋ_front_ਅੰਦਰੂਨੀ(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_ਕੱਢੋ_front()
	{
		ਵਾਪਸ ਕੱਢੋ_front_ਅੰਦਰੂਨੀ(false);
	}
ਨਿਜੀ:
	std::optional<T> ਕੱਢੋ_front_ਅੰਦਰੂਨੀ(bool wait)
	{
		Unique_ਤਾਲਾ ਤਾਲਾ(mutex);
		ਜੇ (wait) {
			not_ਖਾਲੀ_ਥਾਵਾਂ_condition.wait(ਤਾਲਾ, [this] { ਵਾਪਸ !contents.ਖਾਲੀ_ਥਾਵਾਂ() || closed; });
		}
		ਜੇ (contents.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ std::nullopt;
		ਜੇ (static_cast<ਪੂਰਨ>(contents.ਮਾਪ()) == ਸਮਰੱਥਾ)
			not_ਭਰਿਆ_condition.notify_one();
		T ਨਤੀਜਾ = std::move(contents.front());
		contents.ਕੱਢੋ_front();
		ਜੇ (!contents.ਖਾਲੀ_ਥਾਵਾਂ())
			not_ਖਾਲੀ_ਥਾਵਾਂ_condition.notify_one();
		ਵਾਪਸ std::move(ਨਤੀਜਾ);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_ਭਰਿਆ_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_ਖਾਲੀ_ਥਾਵਾਂ_condition;
	std::deque<T> contents;
	ਪੂਰਨ ਸਮਰੱਥਾ;
	bool closed = false;
};

class DeferredLogs
{
ਜਨਤਕ:
	ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
	ਖਾਲੀ ਲਘੂਗਣਕ(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਦਲੀਲਾਂ_3... ਦਲੀਲਾਂ_2)
	{
		logs.ਧੱਕੋ_ਪਿੱਠ({fmt.format(ਦਲੀਲਾਂ_2...), false});
	}
	ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
	ਖਾਲੀ log_error(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਦਲੀਲਾਂ_3... ਦਲੀਲਾਂ_2)
	{
		logs.ਧੱਕੋ_ਪਿੱਠ({fmt.format(ਦਲੀਲਾਂ_2...), true});
	}
	ਖਾਲੀ flush();
ਨਿਜੀ:
	ਬਣਤਰ ਸੁਨੇਹਾ_3
	{
		std::string ਪਾਠ;
		bool error;
	};
	std::ਸਦਿਸ਼<ਸੁਨੇਹਾ_3> logs;
};

class ThreadPool
{
ਜਨਤਕ:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ਸਥਿਰ ਪੂਰਨ pool_ਮਾਪ(ਪੂਰਨ reserved_cores, ਪੂਰਨ max_ਕਾਮਾ_ਧਾਗੇ);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ਸਥਿਰ ਪੂਰਨ work_pool_ਮਾਪ(ਪੂਰਨ reserved_cores, ਪੂਰਨ work_units, ਪੂਰਨ work_units_ਪ੍ਰਤੀ_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(ਪੂਰਨ pool_ਮਾਪ, std::function<ਖਾਲੀ(ਪੂਰਨ)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	ਪੂਰਨ ਅੰਕ_ਧਾਗੇ() ਅਚਲ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
		ਵਾਪਸ ਧਾਗੇ.ਮਾਪ();
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
	}
ਨਿਜੀ:
	std::function<ਖਾਲੀ(ਪੂਰਨ)> body;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ਧਾਗੇ
	std::ਸਦਿਸ਼<std::thread> ਧਾਗੇ;
#ਜੇ_ਅੰਤ
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange ਵਸਤੂ_range_for_ਕਾਮਾ(ਪੂਰਨ ਅੰਕ_ਚੀਜ਼ਾਂ, ਪੂਰਨ thread_ਅੰਕ, ਪੂਰਨ ਅੰਕ_ਧਾਗੇ);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ਬਣਤਰ Thread_ਸੂਚਕਾਂਕ {
	ਪੂਰਨ thread_ਅੰਕ;
};

// A set of threads with a `run()` API that runs a closure on all of the threads
// and wait for all those closures to complete. This is a convenient way to implement
// parallel algorithms that use barrier synchronization.
class ParallelDispatchThreadPool
{
ਜਨਤਕ:
	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	// `pool_size` can be zero, which we treat as 1.
	ParallelDispatchThreadPool(ਪੂਰਨ pool_ਮਾਪ);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ਬਣਤਰ ਲੜੀਵਾਰ_Ctx : ਜਨਤਕ Thread_ਸੂਚਕਾਂਕ {
		ਪੂਰਨ ਅੰਕ_ਧਾਗੇ;
		IntRange ਵਸਤੂ_range(ਪੂਰਨ ਅੰਕ_ਚੀਜ਼ਾਂ) ਅਚਲ {
			ਵਾਪਸ ਵਸਤੂ_range_for_ਕਾਮਾ(ਅੰਕ_ਚੀਜ਼ਾਂ, thread_ਅੰਕ, ਅੰਕ_ਧਾਗੇ);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	ਜਨਤਕ:
		Subpool(ParallelDispatchThreadPool &ਮਾਪਾ, ਪੂਰਨ max_ਧਾਗੇ)
				: ਮਾਪਾ(ਮਾਪਾ), max_ਧਾਗੇ(max_ਧਾਗੇ) {}
		// Returns the number of threads that will be used when calling `run()`.
		ਪੂਰਨ ਅੰਕ_ਧਾਗੇ() ਅਚਲ {
			ਵਾਪਸ ਮਾਪਾ.ਅੰਕ_ਧਾਗੇ(max_ਧਾਗੇ);
		}
		ਖਾਲੀ ਲੜੀਵਾਰ(std::function<ਖਾਲੀ(ਅਚਲ ਲੜੀਵਾਰ_Ctx &)> work) {
			ਮਾਪਾ.ਲੜੀਵਾਰ(std::move(work), max_ਧਾਗੇ);
		}
		ParallelDispatchThreadPool &thread_pool() { ਵਾਪਸ ਮਾਪਾ; }
	ਨਿਜੀ:
		ParallelDispatchThreadPool &ਮਾਪਾ;
		ਪੂਰਨ max_ਧਾਗੇ;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	ਖਾਲੀ ਲੜੀਵਾਰ(std::function<ਖਾਲੀ(ਅਚਲ ਲੜੀਵਾਰ_Ctx &)> work) {
		ਲੜੀਵਾਰ(std::move(work), ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
	}

	// Returns the number of threads that will be used when calling `run()`.
	ਪੂਰਨ ਅੰਕ_ਧਾਗੇ() ਅਚਲ {
		ਵਾਪਸ ਅੰਕ_ਧਾਗੇ(ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
	}
ਨਿਜੀ:
	ਮਿੱਤਰ class Subpool;

	ਖਾਲੀ ਲੜੀਵਾਰ(std::function<ਖਾਲੀ(ਅਚਲ ਲੜੀਵਾਰ_Ctx &)> work, ਪੂਰਨ max_ਧਾਗੇ);
	ਪੂਰਨ ਅੰਕ_ਧਾਗੇ(ਪੂਰਨ max_ਧਾਗੇ) ਅਚਲ {
		ਵਾਪਸ std::min(ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ + 1, std::max(1, max_ਧਾਗੇ));
	}
	ਖਾਲੀ ਲੜੀਵਾਰ_ਕਾਮਾ(ਪੂਰਨ thread_ਅੰਕ);

	std::function<ਖਾਲੀ(ਅਚਲ ਲੜੀਵਾਰ_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	ਪੂਰਨ ਅੰਕ_ਕਾਮਾ_ਧਾਗੇ;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<ਪੂਰਨ> ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_ਮੰਜ਼ਿਲ_workers_signal_mutex;
	CondVar main_ਮੰਜ਼ਿਲ_workers_signal_cv;
	std::ਸਦਿਸ਼<uint8_t> main_ਮੰਜ਼ਿਲ_workers_signal;
	ਖਾਲੀ signal_workers_ਸ਼ੁਰੂ() {
		Unique_ਤਾਲਾ ਤਾਲਾ(main_ਮੰਜ਼ਿਲ_workers_signal_mutex);
		ਪੂਰਨ ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 = ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
		std::fill(main_ਮੰਜ਼ਿਲ_workers_signal.begin(), main_ਮੰਜ਼ਿਲ_workers_signal.begin() + ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_ਮੰਜ਼ਿਲ_workers_signal_cv.notify_all();
	}
	ਖਾਲੀ ਕਾਮਾ_wait_for_ਸ਼ੁਰੂ(ਪੂਰਨ thread_ਅੰਕ) {
		Unique_ਤਾਲਾ ਤਾਲਾ(main_ਮੰਜ਼ਿਲ_workers_signal_mutex);
		main_ਮੰਜ਼ਿਲ_workers_signal_cv.wait(ਤਾਲਾ, [this, thread_ਅੰਕ] { ਵਾਪਸ main_ਮੰਜ਼ਿਲ_workers_signal[thread_ਅੰਕ] > 0; });
		main_ਮੰਜ਼ਿਲ_workers_signal[thread_ਅੰਕ] = 0;
	}

	std::atomic<ਪੂਰਨ> ਪੂਰਾ_workers = 0;
	Mutex workers_ਮੰਜ਼ਿਲ_main_signal_mutex;
	CondVar workers_ਮੰਜ਼ਿਲ_main_signal_cv;
	ਖਾਲੀ signal_ਕਾਮਾ_ਪੂਰਾ() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		ਪੂਰਨ ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 = ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
		ਪੂਰਨ d = ਪੂਰਾ_workers.fetch_ਜੋੜੋ(1, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_release);
		ਜੇ (d + 1 == ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2) {
			Unique_ਤਾਲਾ ਤਾਲਾ(workers_ਮੰਜ਼ਿਲ_main_signal_mutex);
			workers_ਮੰਜ਼ਿਲ_main_signal_cv.notify_all();
		}
	}
	ਖਾਲੀ wait_for_workers_ਪੂਰਾ() {
		Unique_ਤਾਲਾ ਤਾਲਾ(workers_ਮੰਜ਼ਿਲ_main_signal_mutex);
		workers_ਮੰਜ਼ਿਲ_main_signal_cv.wait(ਤਾਲਾ, [this] {
			ਪੂਰਨ ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2 = ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
			ਵਾਪਸ ਪੂਰਾ_workers.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire) == ਅੰਕ_ਸਰਗਰਮ_ਕਾਮਾ_ਧਾਗੇ_2;
		});
		ਪੂਰਾ_workers.store(0, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

ਸਾਂਚਾ <class T>
class Concurrent_ਢੇਰ
{
ਜਨਤਕ:
	ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(T &&t) {
		ਤਾਲਾ_Guard ਤਾਲਾ(mutex);
		contents.ਧੱਕੋ_ਪਿੱਠ(std::move(t));
	}
	std::optional<T> try_ਕੱਢੋ_ਪਿੱਠ() {
		ਤਾਲਾ_Guard ਤਾਲਾ(mutex);
		ਜੇ (contents.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ std::nullopt;
		T ਨਤੀਜਾ = std::move(contents.ਪਿੱਠ());
		contents.ਕੱਢੋ_ਪਿੱਠ();
		ਵਾਪਸ ਨਤੀਜਾ;
	}
ਨਿਜੀ:
	Mutex mutex;
	std::ਸਦਿਸ਼<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
ਸਾਂਚਾ <typename T>
class Sharded_ਸਦਿਸ਼ {
ਜਨਤਕ:
	Sharded_ਸਦਿਸ਼(ਅਚਲ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.ਅੰਕ_ਧਾਗੇ());
	}
	Sharded_ਸਦਿਸ਼(ਅਚਲ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.ਅੰਕ_ਧਾਗੇ());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	ਖਾਲੀ ਪਾਓ(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread, T ਮੁੱਲ) {
		buckets[thread.thread_ਅੰਕ].emplace_ਪਿੱਠ(std::move(ਮੁੱਲ));
	}

	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
		ਲਈ (ਅਚਲ std::ਸਦਿਸ਼<T> &ਬਾਲਟੀ : buckets)
			ਜੇ (!ਬਾਲਟੀ.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ false;
		ਵਾਪਸ true;
	}

	using Buckets = std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<T>>;
	class iterator {
	ਜਨਤਕ:
		iterator(typename Buckets::iterator ਬਾਲਟੀ_it, typename Buckets::iterator ਬਾਲਟੀ_ਸਿਰਾ)
			: ਬਾਲਟੀ_it(std::move(ਬਾਲਟੀ_it)), ਬਾਲਟੀ_ਸਿਰਾ(std::move(ਬਾਲਟੀ_ਸਿਰਾ)) {
			ਜੇ (ਬਾਲਟੀ_it != ਬਾਲਟੀ_ਸਿਰਾ)
				inner_it = ਬਾਲਟੀ_it->begin();
			normalize();
		}
		T& ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ *inner_it.ਮੁੱਲ(); }
		iterator &ਸੰਚਾਲਕ++() {
			++*inner_it;
			normalize();
			ਵਾਪਸ *this;
		}
		bool ਸੰਚਾਲਕ!=(ਅਚਲ iterator &other) ਅਚਲ {
			ਵਾਪਸ ਬਾਲਟੀ_it != other.ਬਾਲਟੀ_it || inner_it != other.inner_it;
		}
	ਨਿਜੀ:
		ਖਾਲੀ normalize() {
			ਜੇ (ਬਾਲਟੀ_it == ਬਾਲਟੀ_ਸਿਰਾ)
				ਵਾਪਸ;
			ਜਦੋਂ (inner_it == ਬਾਲਟੀ_it->ਸਿਰਾ()) {
				++ਬਾਲਟੀ_it;
				ਜੇ (ਬਾਲਟੀ_it == ਬਾਲਟੀ_ਸਿਰਾ) {
					inner_it.reset();
					ਵਾਪਸ;
				}
				inner_it = ਬਾਲਟੀ_it->begin();
			}
		}
		std::optional<typename std::ਸਦਿਸ਼<T>::iterator> inner_it;
		typename Buckets::iterator ਬਾਲਟੀ_it;
		typename Buckets::iterator ਬਾਲਟੀ_ਸਿਰਾ;
	};
	iterator begin() { ਵਾਪਸ iterator(buckets.begin(), buckets.ਸਿਰਾ()); }
	iterator ਸਿਰਾ() { ਵਾਪਸ iterator(buckets.ਸਿਰਾ(), buckets.ਸਿਰਾ()); }
ਨਿਜੀ:
	ਖਾਲੀ init(ਪੂਰਨ ਅੰਕ_ਧਾਗੇ) {
		buckets.resize(ਅੰਕ_ਧਾਗੇ);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
ਸਾਂਚਾ <typename V>
ਬਣਤਰ ਸੈੱਟ_Collision_Handler {
	ਖਾਲੀ ਸੰਚਾਲਕ()(typename V::Accumulated &, typename V::Accumulated &) ਅਚਲ {}
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
ਸਾਂਚਾ <typename V, typename ਕੁੰਜੀ_Equality, typename CollisionHandler>
class ShardedHashtable {
ਜਨਤਕ:
	// A combination of a `V` and its hash value.
	ਬਣਤਰ ਮੁੱਲ_3 {
		ਮੁੱਲ_3(V ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hash) : ਮੁੱਲ(std::move(ਮੁੱਲ)), hash(hash) {}
		ਮੁੱਲ_3(ਮੁੱਲ_3 &&) = ਮੂਲ;
		ਮੁੱਲ_3(ਅਚਲ ਮੁੱਲ_3 &) = delete;
		ਮੁੱਲ_3 &ਸੰਚਾਲਕ=(ਅਚਲ ਮੁੱਲ_3 &) = delete;
		V ਮੁੱਲ;
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ਬਣਤਰ Accumulated_ਮੁੱਲ {
		Accumulated_ਮੁੱਲ(typename V::Accumulated ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hash) : ਮੁੱਲ(std::move(ਮੁੱਲ)), hash(hash) {}
		Accumulated_ਮੁੱਲ(Accumulated_ਮੁੱਲ &&) = ਮੂਲ;
#ਜੇ defined(_MSC_VER)
		Accumulated_ਮੁੱਲ(ਅਚਲ Accumulated_ਮੁੱਲ &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_ਮੁੱਲ &ਸੰਚਾਲਕ=(ਅਚਲ Accumulated_ਮੁੱਲ &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			ਵਾਪਸ *this;
		}
#ਨਹੀਂਤਾਂ
		Accumulated_ਮੁੱਲ(ਅਚਲ Accumulated_ਮੁੱਲ &) = delete;
		Accumulated_ਮੁੱਲ &ਸੰਚਾਲਕ=(ਅਚਲ Accumulated_ਮੁੱਲ &) = delete;
#ਜੇ_ਅੰਤ
		typename V::Accumulated ਮੁੱਲ;
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ਬਣਤਰ Accumulated_ਮੁੱਲ_Equality {
		ਕੁੰਜੀ_Equality inner;
		Accumulated_ਮੁੱਲ_Equality(ਅਚਲ ਕੁੰਜੀ_Equality &inner) : inner(inner) {}
		bool ਸੰਚਾਲਕ()(ਅਚਲ Accumulated_ਮੁੱਲ &v1, ਅਚਲ Accumulated_ਮੁੱਲ &v2) ਅਚਲ {
			ਵਾਪਸ inner(v1.ਮੁੱਲ, v2.ਮੁੱਲ);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ਬਣਤਰ Accumulated_ਮੁੱਲ_Hash_Op {
		ਮਾਪ_t ਸੰਚਾਲਕ()(ਅਚਲ Accumulated_ਮੁੱਲ &v) ਅਚਲ {
			ਵਾਪਸ static_cast<ਮਾਪ_t>(v.hash);
		}
	};
	using Shard = std::unordered_ਸੈੱਟ<Accumulated_ਮੁੱਲ, Accumulated_ਮੁੱਲ_Hash_Op, Accumulated_ਮੁੱਲ_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	ਜਨਤਕ:
		Builder(ਅਚਲ ParallelDispatchThreadPool &thread_pool, ਕੁੰਜੀ_Equality equality = ਕੁੰਜੀ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ਅੰਕ_ਧਾਗੇ(), std::move(equality));
		}
		Builder(ਅਚਲ ParallelDispatchThreadPool::Subpool &thread_pool, ਕੁੰਜੀ_Equality equality = ਕੁੰਜੀ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ਅੰਕ_ਧਾਗੇ(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		ਖਾਲੀ ਪਾਓ(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread, ਮੁੱਲ_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>> &buckets = all_buckets[thread.thread_ਅੰਕ];
			ਮਾਪ_t ਬਾਲਟੀ = static_cast<ਮਾਪ_t>(v.hash) % buckets.ਮਾਪ();
			buckets[ਬਾਲਟੀ].emplace_ਪਿੱਠ(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		ਖਾਲੀ process(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread) {
			ਪੂਰਨ ਮਾਪ = 0;
			ਲਈ (std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>> &buckets : all_buckets)
				ਮਾਪ += Get_ਮਾਪ(buckets[thread.thread_ਅੰਕ]);
			Shard &shard = shards[thread.thread_ਅੰਕ];
			shard.reserve(ਮਾਪ);
			ਲਈ (std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>> &buckets : all_buckets) {
				ਲਈ (ਮੁੱਲ_3 &ਮੁੱਲ : buckets[thread.thread_ਅੰਕ])
					accumulate(ਮੁੱਲ, shard);
				// Free as much memory as we can during the parallel phase.
				std::ਸਦਿਸ਼<ਮੁੱਲ_3>().ਅਦਲਾ_ਬਦਲੀ(buckets[thread.thread_ਅੰਕ]);
			}
		}
	ਨਿਜੀ:
		ਮਿੱਤਰ class ShardedHashtable<V, ਕੁੰਜੀ_Equality, CollisionHandler>;
		ਖਾਲੀ accumulate(ਮੁੱਲ_3 &ਮੁੱਲ, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_ਮੁੱਲ accumulated_ਮੁੱਲ{std::move(ਮੁੱਲ.ਮੁੱਲ), ਮੁੱਲ.hash};
			ਸਵੈਚਾਲਿਤ [it, inserted] = shard.ਪਾਓ(std::move(accumulated_ਮੁੱਲ));
			ਜੇ (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->ਮੁੱਲ), accumulated_ਮੁੱਲ.ਮੁੱਲ);
		}
		ਖਾਲੀ init(ਪੂਰਨ ਅੰਕ_ਧਾਗੇ, ਕੁੰਜੀ_Equality equality) {
			all_buckets.resize(ਅੰਕ_ਧਾਗੇ);
			ਲਈ (std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>> &buckets : all_buckets)
				buckets.resize(ਅੰਕ_ਧਾਗੇ);
			ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਧਾਗੇ; ++i)
				shards.emplace_ਪਿੱਠ(0, Accumulated_ਮੁੱਲ_Hash_Op(), Accumulated_ਮੁੱਲ_Equality(equality));
		}
		ਅਚਲ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>>> all_buckets;
		std::ਸਦਿਸ਼<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		ਲਈ (std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>> &buckets : builder.all_buckets)
			ਲਈ (std::ਸਦਿਸ਼<ਮੁੱਲ_3> &ਬਾਲਟੀ : buckets)
				log_assert(ਬਾਲਟੀ.ਖਾਲੀ_ਥਾਵਾਂ());
		// Free memory.
		std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<std::ਸਦਿਸ਼<ਮੁੱਲ_3>>>().ਅਦਲਾ_ਬਦਲੀ(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = ਮੂਲ;
	ShardedHashtable() {}

	ShardedHashtable &ਸੰਚਾਲਕ=(ShardedHashtable &&other) = ਮੂਲ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ਅਚਲ typename V::Accumulated *ਲੱਭੋ(ਅਚਲ Accumulated_ਮੁੱਲ &v) ਅਚਲ {
		ਮਾਪ_t ਅੰਕ_shards = shards.ਮਾਪ();
		ਜੇ (ਅੰਕ_shards == 0)
			ਵਾਪਸ nullptr;
		ਮਾਪ_t shard = static_cast<ਮਾਪ_t>(v.hash) % ਅੰਕ_shards;
		ਸਵੈਚਾਲਿਤ it = shards[shard].ਲੱਭੋ(v);
		ਜੇ (it == shards[shard].ਸਿਰਾ())
			ਵਾਪਸ nullptr;
		ਵਾਪਸ &it->ਮੁੱਲ;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	ਖਾਲੀ ਪਾਓ(Accumulated_ਮੁੱਲ v) {
		ਮਾਪ_t ਅੰਕ_shards = shards.ਮਾਪ();
		ਜੇ (ਅੰਕ_shards == 0)
			ਵਾਪਸ;
		ਮਾਪ_t shard = static_cast<ਮਾਪ_t>(v.hash) % ਅੰਕ_shards;
		shards[shard].ਪਾਓ(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	ਖਾਲੀ clear(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &shard) {
		Accumulated_ਮੁੱਲ_Equality equality = shards[shard.thread_ਅੰਕ].ਕੁੰਜੀ_eq();
		shards[shard.thread_ਅੰਕ] = Shard(0, Accumulated_ਮੁੱਲ_Hash_Op(), equality);
	}
ਨਿਜੀ:
	std::ਸਦਿਸ਼<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
ਸਾਂਚਾ <typename T>
class Concurrent_Work_ਕਤਾਰ {
ਜਨਤਕ:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_ਕਤਾਰ(ਪੂਰਨ ਅੰਕ_ਧਾਗੇ, ਪੂਰਨ batch_ਮਾਪ = 100)
		: batch_ਮਾਪ(batch_ਮਾਪ), thread_ਹਾਲਤਾਂ(ਅੰਕ_ਧਾਗੇ) {}
	ਪੂਰਨ ਅੰਕ_ਧਾਗੇ() ਅਚਲ { ਵਾਪਸ Get_ਮਾਪ(thread_ਹਾਲਤਾਂ); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	ਖਾਲੀ ਧੱਕੋ(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread, T work) {
		ThreadState &thread_state = thread_ਹਾਲਤਾਂ[thread.thread_ਅੰਕ];
		thread_state.ਅਗਲਾ_batch.emplace_ਪਿੱਠ(std::move(work));
		ਜੇ (Get_ਮਾਪ(thread_state.ਅਗਲਾ_batch) < batch_ਮਾਪ)
			ਵਾਪਸ;
		bool was_ਖਾਲੀ_ਥਾਵਾਂ;
		{
			Unique_ਤਾਲਾ ਤਾਲਾ(thread_state.batches_ਤਾਲਾ);
			was_ਖਾਲੀ_ਥਾਵਾਂ = thread_state.batches.ਖਾਲੀ_ਥਾਵਾਂ();
			thread_state.batches.ਧੱਕੋ_ਪਿੱਠ(std::move(thread_state.ਅਗਲਾ_batch));
		}
		ਜੇ (was_ਖਾਲੀ_ਥਾਵਾਂ) {
			Unique_ਤਾਲਾ ਤਾਲਾ(waiters_ਤਾਲਾ);
			ਜੇ (ਅੰਕ_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::ਸਦਿਸ਼<T> ਕੱਢੋ_batch(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread) {
		ThreadState &thread_state = thread_ਹਾਲਤਾਂ[thread.thread_ਅੰਕ];
		ਜੇ (!thread_state.ਅਗਲਾ_batch.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ std::move(thread_state.ਅਗਲਾ_batch);
		// Empty our own work queue first.
		{
			Unique_ਤਾਲਾ ਤਾਲਾ(thread_state.batches_ਤਾਲਾ);
			ਜੇ (!thread_state.batches.ਖਾਲੀ_ਥਾਵਾਂ()) {
				std::ਸਦਿਸ਼<T> batch = std::move(thread_state.batches.ਪਿੱਠ());
				thread_state.batches.ਕੱਢੋ_ਪਿੱਠ();
				ਵਾਪਸ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		ਜਦੋਂ (true) {
			std::ਸਦਿਸ਼<T> batch = try_steal(thread);
			ਜੇ (!batch.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਵਾਪਸ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_ਤਾਲਾ ਤਾਲਾ(waiters_ਤਾਲਾ);
			++ਅੰਕ_waiters;
			ਜੇ (ਅੰਕ_waiters == ਅੰਕ_ਧਾਗੇ()) {
				waiters_cv.notify_all();
				ਵਾਪਸ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(ਤਾਲਾ);
			ਜੇ (ਅੰਕ_waiters == ਅੰਕ_ਧਾਗੇ())
				ਵਾਪਸ {};
			--ਅੰਕ_waiters;
		}
	}
ਨਿਜੀ:
	std::ਸਦਿਸ਼<T> try_steal(ਅਚਲ Thread_ਸੂਚਕਾਂਕ &thread) {
		ਲਈ (ਪੂਰਨ i = 1; i < ਅੰਕ_ਧਾਗੇ(); i++) {
			ਪੂਰਨ other_thread_ਅੰਕ = (thread.thread_ਅੰਕ + i) % ਅੰਕ_ਧਾਗੇ();
			ThreadState &other_thread_state = thread_ਹਾਲਤਾਂ[other_thread_ਅੰਕ];
			Unique_ਤਾਲਾ ਤਾਲਾ(other_thread_state.batches_ਤਾਲਾ);
			ਜੇ (!other_thread_state.batches.ਖਾਲੀ_ਥਾਵਾਂ()) {
				std::ਸਦਿਸ਼<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.ਕੱਢੋ_front();
				ਵਾਪਸ batch;
			}
		}
		ਵਾਪਸ {};
	}

	ਪੂਰਨ batch_ਮਾਪ;

	ਬਣਤਰ ThreadState {
		// Entirely thread-local.
		std::ਸਦਿਸ਼<T> ਅਗਲਾ_batch;

		Mutex batches_ਤਾਲਾ;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::ਸਦਿਸ਼<T>> batches;
	};
	std::ਸਦਿਸ਼<ThreadState> thread_ਹਾਲਤਾਂ;

	Mutex waiters_ਤਾਲਾ;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	ਪੂਰਨ ਅੰਕ_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
ਜਨਤਕ:
	MonotonicFlag() : ਮੁੱਲ(false) {}
	bool ਲੱਦੋ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed); }
	ਖਾਲੀ ਸੈੱਟ() { ਮੁੱਲ.store(true, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed); }
	bool ਸੈੱਟ_and_return_old() {
		ਵਾਪਸ ਮੁੱਲ.exchange(true, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
	}
ਨਿਜੀ:
	std::atomic<bool> ਮੁੱਲ;
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // YOSYS_THREADING_H
