#ifndef Y86_64_HEAP_H
#define Y86_64_HEAP_H
#include "ISA.h"
#include "ExecutionState.h"
#include <vector>
#include <string>
class Heap
{
public:
	Heap();
	~Heap();
	void Free(ADDRESS addr);
	ADDRESS Malloc(size_t size);
	std::vector<std::string> getBlockDesc() const;

private:
	ADDRESS _brk;
	ADDRESS _heapListp;

	ADDRESS sbrk(int incr);
	ADDRESS extend_heap(size_t size);
	ADDRESS coalesce(ADDRESS blockp);
	ADDRESS find_fit(size_t size);
	void place(ADDRESS bp, size_t size);
};
#endif
