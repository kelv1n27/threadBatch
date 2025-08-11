#pragma once
#include <thread>
#include <functional>
#include <mdspan>

class ThreadBatch {
public:
	struct threadDims {
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int z = 0;
		unsigned int w = 0;
	};
	ThreadBatch() = default;
	ThreadBatch(threadDims dims, std::function<void(threadDims)> threadFunc);
	ThreadBatch(const ThreadBatch&) = delete;
	ThreadBatch(ThreadBatch&& x);
	~ThreadBatch();
	//transfers ownership of all resources to this thread.
	ThreadBatch& operator=(ThreadBatch&& rhs);
	ThreadBatch& operator=(const ThreadBatch&) = delete;
	//get the execution id of a particular thread
	std::thread::id get_id(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, unsigned int w = 0);
	//returns whether any of the threads are "joinable", or are still running. Already joined and detached threads are not joinable.
	bool joinable();
	//blocks the calling thread until all the batched threads have joined
	void join();
	//detaches the batched threads from the current thread, when all batched threads are completed the resources will be deleted. calling this thread makes the batch non-joinable
	void detach();
	//swaps state of threadBatches
	void swap(ThreadBatch& other);
	//gets the native handle of the specified thread. only present if the implementation supports it
	std::thread::native_handle_type native_handle(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, unsigned int w = 0);
	//returns the approximate number of hardware thread contexts. This function just calls the implementation in std::thread
	static unsigned int hardware_concurrency();
private:
	threadDims dims = { 0, 0, 0, 0 };
	std::thread* threadArr = nullptr;
};