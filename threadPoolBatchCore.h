#pragma once
#include <thread>
#include <functional>
#include <vector>
#include <mutex>

class ThreadBatchPool {
public:
	struct threadDims {
		unsigned int x;
		unsigned int y;
		unsigned int z;
		unsigned int w;
		bool operator==(const threadDims& other) const {
			return other.x == x &&
			other.y == y &&
			other.z == z &&
			other.w == w;
		}	
	};
	//Creates new threads to pull from the job queue
	void start();
	//Stops threads from picking up new jobs. Threads are destroyed when they have finished their currently running job.
	void stop();
	//changes threads to be created with the new function for the new dimensions. If the current batch has not completed then it will be overwritten.
	void batch(std::function<void(threadDims)> job, threadDims dims);
	//returns the current position in the job dims
	threadDims& progress();
	//returns if the current batch has dispatched all jobs
	bool complete();
	//waits for the current batch to dispatch all jobs
	void finishBatch();
private:
	void threadLoop();
	
	bool terminate = false;
	threadDims maxDims;
	threadDims currentPos;
	std::function<void(threadDims)> storedJob;
	std::mutex mutex;
	std::condition_variable condition;
	std::vector<std::thread> threads;
};