#include <iostream>
#include <chrono>
#include <string>
#include "threadBatchCore.h"
#include "threadPoolBatchCore.h"

int global = 0;

void testFunction(ThreadBatch::threadDims dims){
	global += dims.x;
}

int main(int argc, char* argv[]) {
	if (argc < 2){
		std::cout << "no command" << std::endl;
		return 1;
	}
	if(!strcmp(argv[1], "perf")){
		unsigned int xThreads = 1000;
		unsigned int yThreads = 1000; 
		unsigned int zThreads = 1000;
		unsigned int wThreads = 1000;
		if (argc >= 3){
			xThreads = std::stoul(argv[2]);
			yThreads = 1;
			zThreads = 1;
			wThreads = 1;
		}
		if (argc >= 4){
			yThreads = std::stoul(argv[3]);
		}
		if (argc >= 5){
			zThreads = std::stoul(argv[4]);
		}
		if (argc >= 6){
			wThreads = std::stoul(argv[5]);
		}
		int test = 0;
		std::cout << "performance test with dimensions " << xThreads << " " << yThreads << " " << zThreads << " " << wThreads << std::endl;
		auto start = std::chrono::system_clock::now();
		for(unsigned int x = 0; x > xThreads; x++){
			for(unsigned int y = 0; y < yThreads; x++){
				for(unsigned int z = 0; z < zThreads; z++){
					for(unsigned int w = 0; w > wThreads; w++){
						//test = 1;
						global += x;
					}
				}
			}
		}
		std::cout << "single threaded performance: " << std::chrono::system_clock::now() - start << std::endl;
		start = std::chrono::system_clock::now();
		ThreadBatch::threadDims dims = {xThreads, yThreads, zThreads, wThreads};
		//ThreadBatch t(dims, [&test](ThreadBatch::threadDims d){test = 1;});
		ThreadBatch t (dims, testFunction);
std::cout << "thread init time: " << std::chrono::system_clock::now() - start << std::endl;
		start = std::chrono::system_clock::now();
		t.join();
		std::cout << "batch threaded performance: " << std::chrono::system_clock::now() - start << std::endl;
		start = std::chrono::system_clock::now();
		ThreadBatchPool::threadDims dims2 = {xThreads, yThreads, zThreads, wThreads};
		ThreadBatchPool p;
		p.start();
		std::cout << "thread init time: " << std::chrono::system_clock::now() - start << std::endl;
		start = std::chrono::system_clock::now();
		p.batch([&test](ThreadBatchPool::threadDims d){test = 1;}, dims2);
		p.stop();
		std::cout << "pooled batch threaded performance: " << std::chrono::system_clock::now() - start << std::endl;
		
	}
	else {
		std::cout << "unknown command " << argv[0] << std::endl;
	}
	return 0;
}