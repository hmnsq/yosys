#ಅನ್ತರ್ಭಾವ <deque>

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/utils.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
#ಅನ್ತರ್ಭಾವ <condition_variable>
#ಅನ್ತರ್ಭಾವ <mutex>
#ಅನ್ತರ್ಭಾವ <thread>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_THREADING_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_THREADING_H

YOSYS_NAMESPACE_BEGIN

// Redirect to no-op to avoid dependence on <mutex>
// and <condition_variable> in single-threaded builds
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
using Mutex = std::mutex;
using CondVar = std::condition_variable;
using Unique_ಬೀಗ = std::unique_ಬೀಗ<Mutex>;
using ಬೀಗ_Guard = std::ಬೀಗ_guard<Mutex>;
#ಇಲ್ಲದಿದ್ದರೆ
ರಚನೆ Mutex {
	ಶೂನ್ಯ ಬೀಗ() {}
	ಶೂನ್ಯ unlock() {}
	bool try_ಬೀಗ() { ಹಿಂತಿರುಗಿ true; }
};
ರಚನೆ CondVar {
	ಮಾದರಿ <class L> ಶೂನ್ಯ wait(L &) {}
	ಮಾದರಿ <class L, class P> ಶೂನ್ಯ wait(L &, P) {}
	ಶೂನ್ಯ notify_one() {}
	ಶೂನ್ಯ notify_all() {}
};
ರಚನೆ Unique_ಬೀಗ {
	Unique_ಬೀಗ(Mutex &) {}
};
ರಚನೆ ಬೀಗ_Guard {
	ಬೀಗ_Guard(Mutex &) {}
};
#ಆದರೆ_ಅಂತ್ಯ

// Concurrent queue implementation. Not fast, but simple.
// Multi-producer, multi-consumer, optionally bounded.
// When YOSYS_ENABLE_THREADS is not defined, this is just a non-thread-safe non-blocking deque.
ಮಾದರಿ <typename T>
class Concurrent_ಸರತಿ
{
ಸಾರ್ವಜನಿಕ:
	Concurrent_ಸರತಿ(ಪೂರ್ಣಾಂಕ ಸಾಮರ್ಥ್ಯ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
		: ಸಾಮರ್ಥ್ಯ(ಸಾಮರ್ಥ್ಯ) {}
	// Push an element into the queue. If it's at capacity, block until there is room.
	ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(T t)
	{
		Unique_ಬೀಗ ಬೀಗ(mutex);
		not_ತುಂಬಿದ_condition.wait(ಬೀಗ, [this] { ಹಿಂತಿರುಗಿ static_cast<ಪೂರ್ಣಾಂಕ>(contents.ಗಾತ್ರ_2()) < ಸಾಮರ್ಥ್ಯ; });
		ಆದರೆ (contents.ಖಾಲಿಗಳು())
			not_ಖಾಲಿಗಳು_condition.notify_one();
		log_assert(!closed);
		contents.ತಳ್ಳು_ಹಿಂಭಾಗ(std::move(t));
		ಆದರೆ (static_cast<ಪೂರ್ಣಾಂಕ>(contents.ಗಾತ್ರ_2()) < ಸಾಮರ್ಥ್ಯ)
			not_ತುಂಬಿದ_condition.notify_one();
	}
	// Signal that no more elements will be produced. `pop_front()` will return nullopt.
	ಶೂನ್ಯ close()
	{
		Unique_ಬೀಗ ಬೀಗ(mutex);
		not_ಖಾಲಿಗಳು_condition.notify_all();
		closed = true;
	}
	// Pop an element from the queue. Blocks until an element is available
	// or the queue is closed and empty.
	std::optional<T> ತೆಗೆ_front()
	{
		ಹಿಂತಿರುಗಿ ತೆಗೆ_front_ಆಂತರಿಕ(true);
	}
	// Pop an element from the queue. Does not block, just returns nullopt if the
	// queue is empty.
	std::optional<T> try_ತೆಗೆ_front()
	{
		ಹಿಂತಿರುಗಿ ತೆಗೆ_front_ಆಂತರಿಕ(false);
	}
ಖಾಸಗಿ:
	std::optional<T> ತೆಗೆ_front_ಆಂತರಿಕ(bool wait)
	{
		Unique_ಬೀಗ ಬೀಗ(mutex);
		ಆದರೆ (wait) {
			not_ಖಾಲಿಗಳು_condition.wait(ಬೀಗ, [this] { ಹಿಂತಿರುಗಿ !contents.ಖಾಲಿಗಳು() || closed; });
		}
		ಆದರೆ (contents.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ std::nullopt;
		ಆದರೆ (static_cast<ಪೂರ್ಣಾಂಕ>(contents.ಗಾತ್ರ_2()) == ಸಾಮರ್ಥ್ಯ)
			not_ತುಂಬಿದ_condition.notify_one();
		T ಫಲಿತಾಂಶ = std::move(contents.front());
		contents.ತೆಗೆ_front();
		ಆದರೆ (!contents.ಖಾಲಿಗಳು())
			not_ಖಾಲಿಗಳು_condition.notify_one();
		ಹಿಂತಿರುಗಿ std::move(ಫಲಿತಾಂಶ);
	}

	Mutex mutex;
	// Signals one waiter thread when the queue changes and is not full.
	CondVar not_ತುಂಬಿದ_condition;
	// Signals one waiter thread when the queue changes and is not empty.
	CondVar not_ಖಾಲಿಗಳು_condition;
	std::deque<T> contents;
	ಪೂರ್ಣಾಂಕ ಸಾಮರ್ಥ್ಯ;
	bool closed = false;
};

class DeferredLogs
{
ಸಾರ್ವಜನಿಕ:
	ಮಾದರಿ <typename... ವಾದಗಳು_3>
	ಶೂನ್ಯ ಲಘುಗಣಕ(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ವಾದಗಳು_3... ವಾದಗಳು_2)
	{
		logs.ತಳ್ಳು_ಹಿಂಭಾಗ({fmt.format(ವಾದಗಳು_2...), false});
	}
	ಮಾದರಿ <typename... ವಾದಗಳು_3>
	ಶೂನ್ಯ log_error(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ವಾದಗಳು_3... ವಾದಗಳು_2)
	{
		logs.ತಳ್ಳು_ಹಿಂಭಾಗ({fmt.format(ವಾದಗಳು_2...), true});
	}
	ಶೂನ್ಯ flush();
ಖಾಸಗಿ:
	ರಚನೆ ಸಂದೇಶ_3
	{
		std::string ಪಠ್ಯ;
		bool error;
	};
	std::ಸದಿಶ<ಸಂದೇಶ_3> logs;
};

class ThreadPool
{
ಸಾರ್ವಜನಿಕ:
	// Computes the number of worker threads to use.
	// `reserved_cores` cores are set aside for other threads (e.g. work on the main thread).
	// `max_worker_threads` --- don't return more workers than this.
	// The result may be 0.
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ reserved_cores, ಪೂರ್ಣಾಂಕ max_ಕೆಲಸಗಾರ_ಎಳೆಗಳು);

	// Computes the number of worker threads to use, by dividing work_units among threads.
	// For testing purposes you can set YOSYS_WORK_UNITS_PER_THREAD to override `work_units_per_thread`.
	// The result may be 0.
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ work_pool_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ reserved_cores, ಪೂರ್ಣಾಂಕ work_units, ಪೂರ್ಣಾಂಕ work_units_ಪ್ರತಿ_thread);

	// Create a pool of threads running the given closure (parameterized by thread number).
	// `pool_size` must be the result of a `pool_size()` call.
	ThreadPool(ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ, std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಂಕ)> b);
	ThreadPool(ThreadPool &&other) = delete;
	// Waits for all threads to terminate. Make sure those closures return!
	~ThreadPool();

	// Return the number of threads in the pool.
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು() ಸ್ಥಿರಾಂಕ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
		ಹಿಂತಿರುಗಿ ಎಳೆಗಳು.ಗಾತ್ರ_2();
#ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
	}
ಖಾಸಗಿ:
	std::function<ಶೂನ್ಯ(ಪೂರ್ಣಾಂಕ)> body;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ಎಳೆಗಳು
	std::ಸದಿಶ<std::thread> ಎಳೆಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
};

// Divides some number of items into `num_threads` subranges and returns the
// `thread_num`'th subrange. If `num_threads` is zero, returns the whole range.
IntRange ಅಂಶ_range_for_ಕೆಲಸಗಾರ(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ವಸ್ತುಗಳು, ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು);

// A type that encapsulates the index of a thread in some list of threads. Useful for
// stronger typechecking and code readability.
ರಚನೆ Thread_ಸೂಚ್ಯಂಕ {
	ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ;
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
	ParallelDispatchThreadPool(ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ);
	~ParallelDispatchThreadPool();

	// For each thread running a closure, a `RunCtx` is passed to the closure. Currently
	// it contains the thread index and the total number of threads. It can be passed
	// directly to any APIs requiring a `ThreadIndex`.
	ರಚನೆ ಸತತ_Ctx : ಸಾರ್ವಜನಿಕ Thread_ಸೂಚ್ಯಂಕ {
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು;
		IntRange ಅಂಶ_range(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ವಸ್ತುಗಳು) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಅಂಶ_range_for_ಕೆಲಸಗಾರ(ಅಂಕೆ_ವಸ್ತುಗಳು, thread_ಅಂಕೆ, ಅಂಕೆ_ಎಳೆಗಳು);
		}
	};
	// Sometimes we only want to activate a subset of the threads in the pool. This
	// class provides a way to do that. It provides the same `num_threads()`
	// and `run()` APIs as a `ParallelDispatchThreadPool`.
	class Subpool {
	ಸಾರ್ವಜನಿಕ:
		Subpool(ParallelDispatchThreadPool &ಪೋಷಕ, ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು)
				: ಪೋಷಕ(ಪೋಷಕ), max_ಎಳೆಗಳು(max_ಎಳೆಗಳು) {}
		// Returns the number of threads that will be used when calling `run()`.
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು() ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಪೋಷಕ.ಅಂಕೆ_ಎಳೆಗಳು(max_ಎಳೆಗಳು);
		}
		ಶೂನ್ಯ ಸತತ(std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ ಸತತ_Ctx &)> work) {
			ಪೋಷಕ.ಸತತ(std::move(work), max_ಎಳೆಗಳು);
		}
		ParallelDispatchThreadPool &thread_pool() { ಹಿಂತಿರುಗಿ ಪೋಷಕ; }
	ಖಾಸಗಿ:
		ParallelDispatchThreadPool &ಪೋಷಕ;
		ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು;
	};

	// Run the `work` function in parallel on each thread in the pool (parameterized by
	// thread number). Waits for all work functions to complete. Only one `run()` can be
	// active at a time.
	// Uses no more than `max_threads` threads (but at least one).
	ಶೂನ್ಯ ಸತತ(std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ ಸತತ_Ctx &)> work) {
		ಸತತ(std::move(work), ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
	}

	// Returns the number of threads that will be used when calling `run()`.
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಅಂಕೆ_ಎಳೆಗಳು(ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
	}
ಖಾಸಗಿ:
	ಮಿತ್ರ class Subpool;

	ಶೂನ್ಯ ಸತತ(std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ ಸತತ_Ctx &)> work, ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು);
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು(ಪೂರ್ಣಾಂಕ max_ಎಳೆಗಳು) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ std::min(ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು + 1, std::max(1, max_ಎಳೆಗಳು));
	}
	ಶೂನ್ಯ ಸತತ_ಕೆಲಸಗಾರ(ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ);

	std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ ಸತತ_Ctx &)> *current_work = nullptr;
	// Keeps a correct count even when threads are exiting.
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು;
	// The count of active worker threads for the current `run()`.
	// This is only written by the main thread, and only written when
	// no other worker threads are running (i.e. all worker threads have
	// passed the increment of `done_workers` in `signal_worker_done()`
	// and not passed the release of the lock in `worker_wait_for_start()`.
	// Although there can't be any races, we still need to make it atomic
	// to prevent the compiler reordering accesses so the above invariant
	// is maintained.
	std::atomic<ಪೂರ್ಣಾಂಕ> ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು = 0;

	// Not especially efficient for large numbers of threads. Worker wakeup could scale
	// better by conceptually organising workers into a tree and having workers wake
	// up their children.
	Mutex main_ಗಮ್ಯ_workers_signal_mutex;
	CondVar main_ಗಮ್ಯ_workers_signal_cv;
	std::ಸದಿಶ<uint8_t> main_ಗಮ್ಯ_workers_signal;
	ಶೂನ್ಯ signal_workers_ಆರಂಭ() {
		Unique_ಬೀಗ ಬೀಗ(main_ಗಮ್ಯ_workers_signal_mutex);
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 = ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
		std::fill(main_ಗಮ್ಯ_workers_signal.begin(), main_ಗಮ್ಯ_workers_signal.begin() + ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2, 1);
		// When `num_active_worker_threads_` is small compared to `num_worker_threads_`, we have a "thundering herd"
		// problem here. Fixing that would add complexity so don't worry about it for now.
		main_ಗಮ್ಯ_workers_signal_cv.notify_all();
	}
	ಶೂನ್ಯ ಕೆಲಸಗಾರ_wait_for_ಆರಂಭ(ಪೂರ್ಣಾಂಕ thread_ಅಂಕೆ) {
		Unique_ಬೀಗ ಬೀಗ(main_ಗಮ್ಯ_workers_signal_mutex);
		main_ಗಮ್ಯ_workers_signal_cv.wait(ಬೀಗ, [this, thread_ಅಂಕೆ] { ಹಿಂತಿರುಗಿ main_ಗಮ್ಯ_workers_signal[thread_ಅಂಕೆ] > 0; });
		main_ಗಮ್ಯ_workers_signal[thread_ಅಂಕೆ] = 0;
	}

	std::atomic<ಪೂರ್ಣಾಂಕ> ಮುಗಿದ_workers = 0;
	Mutex workers_ಗಮ್ಯ_main_signal_mutex;
	CondVar workers_ಗಮ್ಯ_main_signal_cv;
	ಶೂನ್ಯ signal_ಕೆಲಸಗಾರ_ಮುಗಿದ() {
		// Must read `num_active_worker_threads_` before we increment `d`! Otherwise
		// it is possible we would increment `d`, and then another worker signals the
		// main thread that all workers are done, and the main thread writes to
		// `num_active_worker_threads_` before we check it.
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 = ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
		ಪೂರ್ಣಾಂಕ d = ಮುಗಿದ_workers.fetch_ಕೂಡಿಸು(1, std::ಸ್ಮರಣೆ_ಕ್ರಮ_release);
		ಆದರೆ (d + 1 == ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2) {
			Unique_ಬೀಗ ಬೀಗ(workers_ಗಮ್ಯ_main_signal_mutex);
			workers_ಗಮ್ಯ_main_signal_cv.notify_all();
		}
	}
	ಶೂನ್ಯ wait_for_workers_ಮುಗಿದ() {
		Unique_ಬೀಗ ಬೀಗ(workers_ಗಮ್ಯ_main_signal_mutex);
		workers_ಗಮ್ಯ_main_signal_cv.wait(ಬೀಗ, [this] {
			ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2 = ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
			ಹಿಂತಿರುಗಿ ಮುಗಿದ_workers.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_acquire) == ಅಂಕೆ_ಸಕ್ರಿಯ_ಕೆಲಸಗಾರ_ಎಳೆಗಳು_2;
		});
		ಮುಗಿದ_workers.store(0, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
	}
	// Ensure `thread_pool` is destroyed before any other members,
	// forcing all threads to be joined before destroying the
	// members (e.g. workers_to_main_signal_mutex) they might be using.
	std::unique_ptr<ThreadPool> thread_pool;
};

ಮಾದರಿ <class T>
class Concurrent_ರಾಶಿ
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(T &&t) {
		ಬೀಗ_Guard ಬೀಗ(mutex);
		contents.ತಳ್ಳು_ಹಿಂಭಾಗ(std::move(t));
	}
	std::optional<T> try_ತೆಗೆ_ಹಿಂಭಾಗ() {
		ಬೀಗ_Guard ಬೀಗ(mutex);
		ಆದರೆ (contents.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ std::nullopt;
		T ಫಲಿತಾಂಶ = std::move(contents.ಹಿಂಭಾಗ());
		contents.ತೆಗೆ_ಹಿಂಭಾಗ();
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}
ಖಾಸಗಿ:
	Mutex mutex;
	std::ಸದಿಶ<T> contents;
};

// A vector that is sharded into buckets, one per thread. This lets multiple threads write
// efficiently to the vector without synchronization overhead. After all writers have
// finished writing, the vector can be iterated over. The iteration order is deterministic:
// all the elements written by thread 0 in the order it inserted them, followed by all elements
// written by thread 1, etc.
ಮಾದರಿ <typename T>
class Sharded_ಸದಿಶ {
ಸಾರ್ವಜನಿಕ:
	Sharded_ಸದಿಶ(ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool &thread_pool) {
		init(thread_pool.ಅಂಕೆ_ಎಳೆಗಳು());
	}
	Sharded_ಸದಿಶ(ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool::Subpool &thread_pool) {
		init(thread_pool.ಅಂಕೆ_ಎಳೆಗಳು());
	}

	// Insert a value, passing the `ThreadIndex` of the writer thread.
	// Parallel inserts with different `ThreadIndex` values are fine.
	// Inserts must not run concurrently with any other methods (e.g.
	// iteration or `empty()`.)
	ಶೂನ್ಯ ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread, T ಮೌಲ್ಯ) {
		buckets[thread.thread_ಅಂಕೆ].emplace_ಹಿಂಭಾಗ(std::move(ಮೌಲ್ಯ));
	}

	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &ಪಾತ್ರೆ : buckets)
			ಆದರೆ (!ಪಾತ್ರೆ.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ true;
	}

	using Buckets = std::ಸದಿಶ<std::ಸದಿಶ<T>>;
	class iterator {
	ಸಾರ್ವಜನಿಕ:
		iterator(typename Buckets::iterator ಪಾತ್ರೆ_it, typename Buckets::iterator ಪಾತ್ರೆ_ಕೊನೆ)
			: ಪಾತ್ರೆ_it(std::move(ಪಾತ್ರೆ_it)), ಪಾತ್ರೆ_ಕೊನೆ(std::move(ಪಾತ್ರೆ_ಕೊನೆ)) {
			ಆದರೆ (ಪಾತ್ರೆ_it != ಪಾತ್ರೆ_ಕೊನೆ)
				inner_it = ಪಾತ್ರೆ_it->begin();
			normalize();
		}
		T& ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *inner_it.ಮೌಲ್ಯ(); }
		iterator &ಕ್ರಿಯಾಕಾರಕ++() {
			++*inner_it;
			normalize();
			ಹಿಂತಿರುಗಿ *this;
		}
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಪಾತ್ರೆ_it != other.ಪಾತ್ರೆ_it || inner_it != other.inner_it;
		}
	ಖಾಸಗಿ:
		ಶೂನ್ಯ normalize() {
			ಆದರೆ (ಪಾತ್ರೆ_it == ಪಾತ್ರೆ_ಕೊನೆ)
				ಹಿಂತಿರುಗಿ;
			ಆಗ (inner_it == ಪಾತ್ರೆ_it->ಕೊನೆ()) {
				++ಪಾತ್ರೆ_it;
				ಆದರೆ (ಪಾತ್ರೆ_it == ಪಾತ್ರೆ_ಕೊನೆ) {
					inner_it.reset();
					ಹಿಂತಿರುಗಿ;
				}
				inner_it = ಪಾತ್ರೆ_it->begin();
			}
		}
		std::optional<typename std::ಸದಿಶ<T>::iterator> inner_it;
		typename Buckets::iterator ಪಾತ್ರೆ_it;
		typename Buckets::iterator ಪಾತ್ರೆ_ಕೊನೆ;
	};
	iterator begin() { ಹಿಂತಿರುಗಿ iterator(buckets.begin(), buckets.ಕೊನೆ()); }
	iterator ಕೊನೆ() { ಹಿಂತಿರುಗಿ iterator(buckets.ಕೊನೆ(), buckets.ಕೊನೆ()); }
ಖಾಸಗಿ:
	ಶೂನ್ಯ init(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು) {
		buckets.resize(ಅಂಕೆ_ಎಳೆಗಳು);
	}
	Buckets buckets;
};

// This collision handler for `ShardedHashtable` resolves collisions by keeping
// the current value and discarding the other. This is correct when all values with the
// same key are interchangeable, i.e. when the hashtable is being used as a set instead
// of a map.
ಮಾದರಿ <typename V>
ರಚನೆ ಗಣ_Collision_Handler {
	ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(typename V::Accumulated &, typename V::Accumulated &) ಸ್ಥಿರಾಂಕ {}
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
ಮಾದರಿ <typename V, typename ಕೀಲಿ_Equality, typename CollisionHandler>
class ShardedHashtable {
ಸಾರ್ವಜನಿಕ:
	// A combination of a `V` and its hash value.
	ರಚನೆ ಮೌಲ್ಯ_3 {
		ಮೌಲ್ಯ_3(V ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hash) : ಮೌಲ್ಯ(std::move(ಮೌಲ್ಯ)), hash(hash) {}
		ಮೌಲ್ಯ_3(ಮೌಲ್ಯ_3 &&) = ಪೂರ್ವನಿಯೋಜಿತ;
		ಮೌಲ್ಯ_3(ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_3 &) = delete;
		ಮೌಲ್ಯ_3 &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_3 &) = delete;
		V ಮೌಲ್ಯ;
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hash;
	};
	// A combination of a `V::Accumulated` and its hash value.
	ರಚನೆ Accumulated_ಮೌಲ್ಯ {
		Accumulated_ಮೌಲ್ಯ(typename V::Accumulated ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hash) : ಮೌಲ್ಯ(std::move(ಮೌಲ್ಯ)), hash(hash) {}
		Accumulated_ಮೌಲ್ಯ(Accumulated_ಮೌಲ್ಯ &&) = ಪೂರ್ವನಿಯೋಜಿತ;
#ಆದರೆ defined(_MSC_VER)
		Accumulated_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &) {
			log_error("Copy constructor called on AccumulatedValue\n");
		}
		Accumulated_ಮೌಲ್ಯ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &) {
			log_error("Copy assignment called on AccumulatedValue\n");
			ಹಿಂತಿರುಗಿ *this;
		}
#ಇಲ್ಲದಿದ್ದರೆ
		Accumulated_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &) = delete;
		Accumulated_ಮೌಲ್ಯ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &) = delete;
#ಆದರೆ_ಅಂತ್ಯ
		typename V::Accumulated ಮೌಲ್ಯ;
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hash;
	};
	// A class containing an `operator()` that returns true of two `AccumulatedValue`
	// elements have equal keys.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ರಚನೆ Accumulated_ಮೌಲ್ಯ_Equality {
		ಕೀಲಿ_Equality inner;
		Accumulated_ಮೌಲ್ಯ_Equality(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_Equality &inner) : inner(inner) {}
		bool ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &v1, ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &v2) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ inner(v1.ಮೌಲ್ಯ, v2.ಮೌಲ್ಯ);
		}
	};
	// A class containing an `operator()` that returns the hash value of an `AccumulatedValue`.
	// Required to insert `AccumulatedValue`s into an `std::unordered_set`.
	ರಚನೆ Accumulated_ಮೌಲ್ಯ_Hash_Op {
		ಗಾತ್ರ_t ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &v) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ static_cast<ಗಾತ್ರ_t>(v.hash);
		}
	};
	using Shard = std::unordered_ಗಣ<Accumulated_ಮೌಲ್ಯ, Accumulated_ಮೌಲ್ಯ_Hash_Op, Accumulated_ಮೌಲ್ಯ_Equality>;

	// First construct one of these. Then populate it in parallel by calling `insert()` from many threads.
	// Then do another parallel phase calling `process()` from many threads.
	class Builder {
	ಸಾರ್ವಜನಿಕ:
		Builder(ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool &thread_pool, ಕೀಲಿ_Equality equality = ಕೀಲಿ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ಅಂಕೆ_ಎಳೆಗಳು(), std::move(equality));
		}
		Builder(ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool::Subpool &thread_pool, ಕೀಲಿ_Equality equality = ಕೀಲಿ_Equality(), CollisionHandler collision_handler = CollisionHandler())
				: collision_handler(std::move(collision_handler)) {
			init(thread_pool.ಅಂಕೆ_ಎಳೆಗಳು(), std::move(equality));
		}
		// First call `insert` to insert all elements. All inserts must finish
		// before calling any `process()`.
		ಶೂನ್ಯ ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread, ಮೌಲ್ಯ_3 v) {
			// You might think that for the single-threaded case, we can optimize by
			// inserting directly into the `std::unordered_set` here. But that slows things down
			// a lot and I never got around to figuring out why.
			std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>> &buckets = all_buckets[thread.thread_ಅಂಕೆ];
			ಗಾತ್ರ_t ಪಾತ್ರೆ = static_cast<ಗಾತ್ರ_t>(v.hash) % buckets.ಗಾತ್ರ_2();
			buckets[ಪಾತ್ರೆ].emplace_ಹಿಂಭಾಗ(std::move(v));
		}
		// Then call `process` for each thread. All `process()`s must finish before using
		// the `Builder` to construct a `ShardedHashtable`.
		ಶೂನ್ಯ process(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread) {
			ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = 0;
			ಗಾಗಿ (std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>> &buckets : all_buckets)
				ಗಾತ್ರ_2 += Get_ಗಾತ್ರ(buckets[thread.thread_ಅಂಕೆ]);
			Shard &shard = shards[thread.thread_ಅಂಕೆ];
			shard.reserve(ಗಾತ್ರ_2);
			ಗಾಗಿ (std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>> &buckets : all_buckets) {
				ಗಾಗಿ (ಮೌಲ್ಯ_3 &ಮೌಲ್ಯ : buckets[thread.thread_ಅಂಕೆ])
					accumulate(ಮೌಲ್ಯ, shard);
				// Free as much memory as we can during the parallel phase.
				std::ಸದಿಶ<ಮೌಲ್ಯ_3>().ಅದಲುಬದಲು(buckets[thread.thread_ಅಂಕೆ]);
			}
		}
	ಖಾಸಗಿ:
		ಮಿತ್ರ class ShardedHashtable<V, ಕೀಲಿ_Equality, CollisionHandler>;
		ಶೂನ್ಯ accumulate(ಮೌಲ್ಯ_3 &ಮೌಲ್ಯ, Shard &shard) {
			// With C++20 we could make this more efficient using heterogenous lookup
			Accumulated_ಮೌಲ್ಯ accumulated_ಮೌಲ್ಯ{std::move(ಮೌಲ್ಯ.ಮೌಲ್ಯ), ಮೌಲ್ಯ.hash};
			ಸ್ವಯಂಚಾಲಿತ [it, inserted] = shard.ಸೇರಿಸು(std::move(accumulated_ಮೌಲ್ಯ));
			ಆದರೆ (!inserted)
				collision_handler(const_cast<typename V::Accumulated &>(it->ಮೌಲ್ಯ), accumulated_ಮೌಲ್ಯ.ಮೌಲ್ಯ);
		}
		ಶೂನ್ಯ init(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು, ಕೀಲಿ_Equality equality) {
			all_buckets.resize(ಅಂಕೆ_ಎಳೆಗಳು);
			ಗಾಗಿ (std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>> &buckets : all_buckets)
				buckets.resize(ಅಂಕೆ_ಎಳೆಗಳು);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಎಳೆಗಳು; ++i)
				shards.emplace_ಹಿಂಭಾಗ(0, Accumulated_ಮೌಲ್ಯ_Hash_Op(), Accumulated_ಮೌಲ್ಯ_Equality(equality));
		}
		ಸ್ಥಿರಾಂಕ CollisionHandler collision_handler;
		// A num_threads x num_threads matrix of buckets.
		// In the first phase, each thread i gemerates elements and writes them to
		// bucket [i][j] where j = hash(element) % num_threads.
		// In the second phase, thread i reads from bucket [j][i] for all j, collecting
		// all elements where i = hash(element) % num_threads.
		std::ಸದಿಶ<std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>>> all_buckets;
		std::ಸದಿಶ<Shard> shards;
	};

	// Then finally construct the hashtable:
	ShardedHashtable(Builder &builder) : shards(std::move(builder.shards)) {
		// Check that all necessary 'process()' calls were made.
		ಗಾಗಿ (std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>> &buckets : builder.all_buckets)
			ಗಾಗಿ (std::ಸದಿಶ<ಮೌಲ್ಯ_3> &ಪಾತ್ರೆ : buckets)
				log_assert(ಪಾತ್ರೆ.ಖಾಲಿಗಳು());
		// Free memory.
		std::ಸದಿಶ<std::ಸದಿಶ<std::ಸದಿಶ<ಮೌಲ್ಯ_3>>>().ಅದಲುಬದಲು(builder.all_buckets);
	}
	ShardedHashtable(ShardedHashtable &&other) = ಪೂರ್ವನಿಯೋಜಿತ;
	ShardedHashtable() {}

	ShardedHashtable &ಕ್ರಿಯಾಕಾರಕ=(ShardedHashtable &&other) = ಪೂರ್ವನಿಯೋಜಿತ;

	// Look up by `AccumulatedValue`. If we switch to C++20 then we could use
	// heterogenous lookup to support looking up by `Value` here. Returns nullptr
	// if the key is not found.
	ಸ್ಥಿರಾಂಕ typename V::Accumulated *ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ Accumulated_ಮೌಲ್ಯ &v) ಸ್ಥಿರಾಂಕ {
		ಗಾತ್ರ_t ಅಂಕೆ_shards = shards.ಗಾತ್ರ_2();
		ಆದರೆ (ಅಂಕೆ_shards == 0)
			ಹಿಂತಿರುಗಿ nullptr;
		ಗಾತ್ರ_t shard = static_cast<ಗಾತ್ರ_t>(v.hash) % ಅಂಕೆ_shards;
		ಸ್ವಯಂಚಾಲಿತ it = shards[shard].ಕಂಡುಹಿಡಿ(v);
		ಆದರೆ (it == shards[shard].ಕೊನೆ())
			ಹಿಂತಿರುಗಿ nullptr;
		ಹಿಂತಿರುಗಿ &it->ಮೌಲ್ಯ;
	}

	// Insert an element into the table. The caller is responsible for ensuring this does not
	// happen concurrently with any other method calls.
	ಶೂನ್ಯ ಸೇರಿಸು(Accumulated_ಮೌಲ್ಯ v) {
		ಗಾತ್ರ_t ಅಂಕೆ_shards = shards.ಗಾತ್ರ_2();
		ಆದರೆ (ಅಂಕೆ_shards == 0)
			ಹಿಂತಿರುಗಿ;
		ಗಾತ್ರ_t shard = static_cast<ಗಾತ್ರ_t>(v.hash) % ಅಂಕೆ_shards;
		shards[shard].ಸೇರಿಸು(v);
	}

	// Call this for each shard to implement parallel destruction. For very large `ShardedHashtable`s,
	// deleting all elements of all shards on a single thread can be a performance bottleneck.
	ಶೂನ್ಯ clear(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &shard) {
		Accumulated_ಮೌಲ್ಯ_Equality equality = shards[shard.thread_ಅಂಕೆ].ಕೀಲಿ_eq();
		shards[shard.thread_ಅಂಕೆ] = Shard(0, Accumulated_ಮೌಲ್ಯ_Hash_Op(), equality);
	}
ಖಾಸಗಿ:
	std::ಸದಿಶ<Shard> shards;
};

// A concurrent work-queue that can share batches of work across threads.
// Uses a naive implementation of work-stealing.
ಮಾದರಿ <typename T>
class Concurrent_Work_ಸರತಿ {
ಸಾರ್ವಜನಿಕ:
	// Create a queue that supports the given number of threads and
	// groups work into `batch_size` units.
	Concurrent_Work_ಸರತಿ(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು, ಪೂರ್ಣಾಂಕ batch_ಗಾತ್ರ = 100)
		: batch_ಗಾತ್ರ(batch_ಗಾತ್ರ), thread_ಸ್ಥಿತಿಗಳು(ಅಂಕೆ_ಎಳೆಗಳು) {}
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಎಳೆಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ Get_ಗಾತ್ರ(thread_ಸ್ಥಿತಿಗಳು); }
	// Push some work to do. Pushes and pops with the same `thread` must
	// not happen concurrently.
	ಶೂನ್ಯ ತಳ್ಳು(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread, T work) {
		ThreadState &thread_state = thread_ಸ್ಥಿತಿಗಳು[thread.thread_ಅಂಕೆ];
		thread_state.ಮುಂದಿನ_batch.emplace_ಹಿಂಭಾಗ(std::move(work));
		ಆದರೆ (Get_ಗಾತ್ರ(thread_state.ಮುಂದಿನ_batch) < batch_ಗಾತ್ರ)
			ಹಿಂತಿರುಗಿ;
		bool was_ಖಾಲಿಗಳು;
		{
			Unique_ಬೀಗ ಬೀಗ(thread_state.batches_ಬೀಗ);
			was_ಖಾಲಿಗಳು = thread_state.batches.ಖಾಲಿಗಳು();
			thread_state.batches.ತಳ್ಳು_ಹಿಂಭಾಗ(std::move(thread_state.ಮುಂದಿನ_batch));
		}
		ಆದರೆ (was_ಖಾಲಿಗಳು) {
			Unique_ಬೀಗ ಬೀಗ(waiters_ಬೀಗ);
			ಆದರೆ (ಅಂಕೆ_waiters > 0) {
				waiters_cv.notify_one();
			}
		}
	}
	// Grab some work to do.
	// If all threads enter `pop_batch()`, then instead of deadlocking the
	// queue will return no work. That is the only case in which it will
	// return no work.
	std::ಸದಿಶ<T> ತೆಗೆ_batch(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread) {
		ThreadState &thread_state = thread_ಸ್ಥಿತಿಗಳು[thread.thread_ಅಂಕೆ];
		ಆದರೆ (!thread_state.ಮುಂದಿನ_batch.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ std::move(thread_state.ಮುಂದಿನ_batch);
		// Empty our own work queue first.
		{
			Unique_ಬೀಗ ಬೀಗ(thread_state.batches_ಬೀಗ);
			ಆದರೆ (!thread_state.batches.ಖಾಲಿಗಳು()) {
				std::ಸದಿಶ<T> batch = std::move(thread_state.batches.ಹಿಂಭಾಗ());
				thread_state.batches.ತೆಗೆ_ಹಿಂಭಾಗ();
				ಹಿಂತಿರುಗಿ batch;
			}
		}
		// From here on in this function, our work queue is empty.
		ಆಗ (true) {
			std::ಸದಿಶ<T> batch = try_steal(thread);
			ಆದರೆ (!batch.ಖಾಲಿಗಳು()) {
				ಹಿಂತಿರುಗಿ std::move(batch);
			}
			// Termination: if all threads run out of work, then all of
			// them will eventually enter this loop and there will be no further
			// notifications on waiters_cv, so all will eventually increment
			// num_waiters and wait, so num_waiters == num_threads()
			// will become true. In single-threaded builds, num_threads() is 1,
			// so we always terminate on the first iteration.
			Unique_ಬೀಗ ಬೀಗ(waiters_ಬೀಗ);
			++ಅಂಕೆ_waiters;
			ಆದರೆ (ಅಂಕೆ_waiters == ಅಂಕೆ_ಎಳೆಗಳು()) {
				waiters_cv.notify_all();
				ಹಿಂತಿರುಗಿ {};
			}
			// As above, it's possible that we'll wait here even when there
			// are work batches posted by other threads. That's OK.
			waiters_cv.wait(ಬೀಗ);
			ಆದರೆ (ಅಂಕೆ_waiters == ಅಂಕೆ_ಎಳೆಗಳು())
				ಹಿಂತಿರುಗಿ {};
			--ಅಂಕೆ_waiters;
		}
	}
ಖಾಸಗಿ:
	std::ಸದಿಶ<T> try_steal(ಸ್ಥಿರಾಂಕ Thread_ಸೂಚ್ಯಂಕ &thread) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಅಂಕೆ_ಎಳೆಗಳು(); i++) {
			ಪೂರ್ಣಾಂಕ other_thread_ಅಂಕೆ = (thread.thread_ಅಂಕೆ + i) % ಅಂಕೆ_ಎಳೆಗಳು();
			ThreadState &other_thread_state = thread_ಸ್ಥಿತಿಗಳು[other_thread_ಅಂಕೆ];
			Unique_ಬೀಗ ಬೀಗ(other_thread_state.batches_ಬೀಗ);
			ಆದರೆ (!other_thread_state.batches.ಖಾಲಿಗಳು()) {
				std::ಸದಿಶ<T> batch = std::move(other_thread_state.batches.front());
				other_thread_state.batches.ತೆಗೆ_front();
				ಹಿಂತಿರುಗಿ batch;
			}
		}
		ಹಿಂತಿರುಗಿ {};
	}

	ಪೂರ್ಣಾಂಕ batch_ಗಾತ್ರ;

	ರಚನೆ ThreadState {
		// Entirely thread-local.
		std::ಸದಿಶ<T> ಮುಂದಿನ_batch;

		Mutex batches_ಬೀಗ;
		// Only the associated thread ever adds to this, and only at the back.
		// Other threads can remove elements from the front.
		std::deque<std::ಸದಿಶ<T>> batches;
	};
	std::ಸದಿಶ<ThreadState> thread_ಸ್ಥಿತಿಗಳು;

	Mutex waiters_ಬೀಗ;
	CondVar waiters_cv;
	// Number of threads waiting for work. Their queues are empty.
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_waiters = 0;
};

// A monotonic flag. Starts false, and can be set to true in a thread-safe way.
// Once `load()` returns true, it will always return true.
// Uses relaxed atomics so there are no memory ordering guarantees. Do not use this
// to guard access to shared memory.
class MonotonicFlag {
ಸಾರ್ವಜನಿಕ:
	MonotonicFlag() : ಮೌಲ್ಯ(false) {}
	bool ತುಂಬು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed); }
	ಶೂನ್ಯ ಗಣ() { ಮೌಲ್ಯ.store(true, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed); }
	bool ಗಣ_and_return_old() {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ.exchange(true, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
	}
ಖಾಸಗಿ:
	std::atomic<bool> ಮೌಲ್ಯ;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // YOSYS_THREADING_H
