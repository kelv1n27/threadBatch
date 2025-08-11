#include "threadBatchCore.h"
#include <iostream>

ThreadBatch::ThreadBatch(threadDims dims, std::function<void(threadDims)> threadFunc){
	this->dims = dims;
	threadArr = new std::thread[dims.x * dims.y * dims.z * dims.w];
	auto span = std::mdspan(threadArr, dims.x, dims.y, dims.z, dims.w);
	threadDims dim = { 0, 0, 0, 0 };
	for(unsigned int x = 0; x < dims.x; x++){
		dim.x = x;
		for(unsigned int y = 0; y < dims.y; y++){
			dim.y = y;
			for(unsigned int z = 0; z < dims.z; z++){
				dim.z = z;
				for(unsigned int w = 0; w < dims.w; w++){
					dim.w = w;
					std::thread t(threadFunc, dim);
					span[x, y, z, w].swap(t);
				}
			}
		}
	}
}
	
ThreadBatch::ThreadBatch(ThreadBatch&& x){
	this->threadArr = x.threadArr;
	this->dims = x.dims;
	x.threadArr = nullptr;
}
	
ThreadBatch::~ThreadBatch(){
	if (threadArr != nullptr) delete[](threadArr);
}
	
ThreadBatch& ThreadBatch::operator=(ThreadBatch&& rhs){
	this->threadArr = rhs.threadArr;
	this->dims = rhs.dims;
	rhs.threadArr = nullptr;
	return *this;
}
	
std::thread::id ThreadBatch::get_id(unsigned int x, unsigned int y, unsigned int z, unsigned int w){
	auto span = std::mdspan(threadArr, dims.x, dims.y, dims.z, dims.w);
	return span[x, y, z, w].get_id();
}
	
bool ThreadBatch::joinable(){
	if(threadArr == nullptr) return false;
	for(auto i = threadArr; i < threadArr + ((dims.x * dims.y * dims.z * dims.w) * sizeof(std::thread)); i += sizeof(std::thread)){
		if (i->joinable()) return true;
	}
	return false;
}
	
void ThreadBatch::join(){
	std::thread* endptr = threadArr + (dims.x * dims.y * dims.x * dims.w);
	for (std::thread* ptr = threadArr; ptr < endptr; ptr++){
		ptr->join();
	}
	delete [] threadArr;
	threadArr = nullptr;
}
	
void ThreadBatch::detach(){
	auto span = std::mdspan(threadArr, dims.x, dims.y, dims.z, dims.w);
	for(unsigned int x = 0; x < dims.x; x++){
		for(unsigned int y = 0; y < dims.y; y++){
			for(unsigned int z = 0; z < dims.z; z++){
				for(unsigned int w = 0; w < dims.w; w++){
					span[x, y, z, w].detach();
				}
			}
		}
	}
	delete[](threadArr);
	threadArr = nullptr;
}
	
	
void ThreadBatch::swap(ThreadBatch& other){
	threadDims tempDims = other.dims;
	std::thread* tempThreadPtr = other.threadArr;
	other.dims = dims;
	other.threadArr = threadArr;
	dims = tempDims;
	threadArr = tempThreadPtr;
}

std::thread::native_handle_type ThreadBatch::native_handle(unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	auto span = std::mdspan(threadArr, dims.x, dims.y, dims.z, dims.w);
	return span[x, y, z, w].native_handle();
}
	
unsigned int ThreadBatch::hardware_concurrency(){
	return std::thread::hardware_concurrency();
}