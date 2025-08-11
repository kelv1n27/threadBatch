#include "threadPoolBatchCore.h"

void ThreadBatchPool::start(){
	const uint32_t num_threads = std::thread::hardware_concurrency();
	for(uint32_t i = 0; i < num_threads; i++){
		threads.emplace_back(&ThreadBatchPool::threadLoop, this);
	}
}

void ThreadBatchPool::stop(){
	{
		std::unique_lock<std::mutex> lock(mutex);
		terminate = true;
	}
	condition.notify_all();
	for (std::thread& t : threads){
		t.join();
	}
	threads.clear();
}

void ThreadBatchPool::batch(std::function<void(threadDims)> job, threadDims dims){
	{
		std::unique_lock<std::mutex> lock(mutex);
		storedJob = job;
		maxDims = dims;
		currentPos = {0, 0, 0, 0};
	}
	condition.notify_one();
}

ThreadBatchPool::threadDims& ThreadBatchPool::progress(){
	return currentPos;
}

bool ThreadBatchPool::complete(){
	return currentPos == maxDims;
}

void ThreadBatchPool::finishBatch(){
	while(!(currentPos == maxDims)){}
}

void ThreadBatchPool::threadLoop(){
	while (true){
		std::function<void(threadDims)> job;
		threadDims pos;
		{
			std::unique_lock<std::mutex> lock(mutex);
			condition.wait(lock, [this] {
				return !(currentPos == maxDims) || terminate;
			});
			if(terminate) {
				return;
			}
			job = storedJob;
			pos = currentPos;
			currentPos.w += 1;
			if (currentPos.w > maxDims.w){
				currentPos.w -= maxDims.w;
				currentPos.z += 1;
				if(currentPos.z > maxDims.z){
					currentPos.z -= maxDims.z;
					currentPos.y += 1;
					if(currentPos.y > maxDims.y){
						currentPos.y -= maxDims.y;
						currentPos.x += 1;
					}
				}
			}
		}
		job(pos);
	}
}