#include "stdafx.h"
#include "Heap.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

const int ALIGN_SIZE = 8;

inline void memsetInt(ADDRESS address, int i) {
	memcpy(Memory + address, &i, sizeof(int));
}

inline int packInt(size_t size, bool alloc) {
	return (size | alloc);
}


inline bool isAllocated(ADDRESS address) {
	int i;
	memcpy(&i, Memory + address - 4, sizeof(int));
	return i & 1;
}

inline size_t getSize(ADDRESS address) {
	int i;
	memcpy(&i, Memory + address - 4, sizeof(int));
	return i & (~0x7);
}

ADDRESS nextBlock(ADDRESS blockp) {
	int i;
	memcpy(&i, Memory + blockp - 4, sizeof(int));
	size_t size = (i & (~0x7));
	return blockp + size;
}

ADDRESS prevBlock(ADDRESS blockp) {
	int i;
	memcpy(&i, Memory + blockp - 8, sizeof(int));
	size_t size = (i & (~0x7));
	return blockp - size;
}

Heap::Heap()
{
	_brk = INIT_HEAP_POS;
	if ((_heapListp = sbrk(16)) == 0) {
		throw;
	}
	/* Padding */
	memsetInt(_heapListp, 0);

	/* Prologue Header */
	memsetInt(_heapListp + 4, packInt(8, true));

	/* Prologue Footer */
	memsetInt(_heapListp + 8, packInt(8, true));

	/* Epilogue Header */
	memsetInt(_heapListp + 12, packInt(0, true));

	/* 起始堆地址 */
	_heapListp += 8;

	if (extend_heap(HEAP_CHUNK_SIZE) == 0)
		throw;
}


Heap::~Heap()
{
}

void Heap::Free(ADDRESS addr)
{
	size_t size = getSize(addr);
	memsetInt(addr - 4, packInt(size, 0));
	memsetInt(addr + size - 8, packInt(size, 0));
	coalesce(addr);
}

ADDRESS Heap::Malloc(size_t size)
{
	size_t asize;

	if (size == 0) return 0;
	if (size < ALIGN_SIZE)
		asize = ALIGN_SIZE * 2;
	else
		asize = ALIGN_SIZE * ((size + ALIGN_SIZE + ALIGN_SIZE - 1) / ALIGN_SIZE);
	ADDRESS blockp;
	if ((blockp = find_fit(asize)) != 0) {
		place(blockp, asize);
		return blockp;
	}
	size_t extended = MAX(asize, HEAP_CHUNK_SIZE);
	if ((blockp = extend_heap(extended)) == 0)
		return 0;
	place(blockp, asize);
	return blockp;
}

std::vector<std::string> Heap::getBlockDesc() const
{
	std::vector<std::string> result;
	ADDRESS bp;
	int index = 1;
	for (bp = _heapListp; getSize(bp) > 0 && bp < _brk; bp = nextBlock(bp)) { 
		char buf[200];
		sprintf_s(buf, "区块#%d - 地址：0x%.8llX 是否分配：%s 大小：%d",
			index, bp, isAllocated(bp) ? "是" : "否", getSize(bp));
		std::string str(buf);
		result.push_back(str);
		index++;
	}
	return result;
}

ADDRESS Heap::sbrk(int incr)
{
	ADDRESS oldbrk = _brk;
	if (_brk + incr > INIT_HEAP_POS + MAX_HEAP_SIZE) {
		fprintf(stderr, "%s\n", "Out of Memory!");
		return 0;
	}
	_brk += incr;
	return oldbrk;
}

ADDRESS Heap::extend_heap(size_t size)
{
	ADDRESS blockp;
	if ((blockp = sbrk(size)) == -1) {
		return 0;
	}
	memsetInt(blockp - 4, packInt(size, 0));
	/* 新的 Prologue Footer */
	memsetInt(blockp + size - 8, packInt(size, 0));
	/* Epilogue Header */
	memsetInt(blockp + size - 4, packInt(0, 1));

	return coalesce(blockp);
}

ADDRESS Heap::coalesce(ADDRESS blockp)
{
	bool nextAlloc = isAllocated(nextBlock(blockp));
	bool prevAlloc = isAllocated(prevBlock(blockp));
	size_t size = getSize(blockp);

	if (prevAlloc && nextAlloc)
		return blockp;
	else if (prevAlloc && !nextAlloc) {
		size += getSize(nextBlock(blockp));
		/* 首部 */
		memsetInt(blockp - 4, packInt(size, 0));
		/* 下一区块尾部 */
		memsetInt(blockp + size - 8, packInt(size, 0));
	}
	else if (!prevAlloc && nextAlloc) {
		size += getSize(prevBlock(blockp));
		/* 此区块尾部 */
		memsetInt(blockp + size - 8, packInt(size, 0));
		/* 首部 */
		ADDRESS haddr = prevBlock(blockp) - 4;
		memsetInt(haddr, packInt(size, 0));
		return haddr;
	}
	else {
		size += getSize(prevBlock(blockp)) + getSize(nextBlock(blockp));
		ADDRESS haddr = prevBlock(blockp) - 4;
		memsetInt(haddr, packInt(size, 0));
		memsetInt(haddr + size - 8, packInt(size, 0));
		return haddr;
	}
	return blockp;
}

ADDRESS Heap::find_fit(size_t size)
{
	ADDRESS bp;
	for (bp = _heapListp; getSize(bp) > 0 && bp < _brk; bp = nextBlock(bp)) {
		if (!isAllocated(bp) && size <= getSize(bp))
			return bp;
	}
	return 0;
}

void Heap::place(ADDRESS bp, size_t size)
{
	size_t oldSize = getSize(bp);
	if (oldSize - size >= 2 * ALIGN_SIZE) {
		memsetInt(bp - 4, packInt(size, 1));
		memsetInt(bp + size - 8, packInt(size, 1));
		bp = nextBlock(bp);
		memsetInt(bp - 4, packInt(oldSize - size, 0));
		memsetInt(bp + size - 8, packInt(oldSize - size, 0));
	}
	else {
		memsetInt(bp - 4, packInt(oldSize, 1));
		memsetInt(bp + size - 8, packInt(oldSize, 1));
	}
}
