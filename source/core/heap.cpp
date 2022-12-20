#include <list>
#include <sys.h>

#include "Heap.hpp"

template <int size>
struct Data_Block
{
	int Id;
	char Data[size];
	list_head List;
};

static const int _heap_size = 10;

class Heap : public HeapInterface
{
public:
	void	Init(void);
	void*	Alloc(int size);
	void	Free(void *base);

public:
	void	Debug(void);

private:
	Data_Block<16>	Data16[_heap_size];
	list_head_t	List16_Available;
	list_head_t	List16_Allocated;
};

int __heap_start_marker __section("heap");

// Our Heap, we should keep is as safe as possible
Heap SaturnHeap  __section("heap");

int __heap_end_marker __section("heap");



template<typename T>
void Heap_List_Init(list_head_t *avail, list_head_t *alloc, T *data)
{
	List_Init(avail);
	List_Init(alloc);

	for (int i = 0; i < _heap_size; ++i)
	{
		data[i].Id = 1;
		List_Add(&data[i].List, avail);
	}
}

void Heap::Init(void)
{
	//TBD
	Heap_List_Init<Data_Block<16>>(&List16_Available, &List16_Allocated, Data16);
}

void* Heap::Alloc(int size)
{
	return 0;
}

void Heap::Free(void *base)
{
	//TBD
}

extern "C" void console_msg(const char *fmt, ...);

void heap_debug()
{
	SaturnHeap.Debug();
}

void Heap::Debug(void)
{
	list_head_t *head = List16_Available.next;

	for (int i = 0; i < _heap_size; ++i)
	{
		Data_Block<16> *data = list_entry(head, Data_Block<16>, List);

		console_msg("-> %d\r\n", data->Id);
		head = head->next;
	}
}
