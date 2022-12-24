class HeapInterface
{
public:
	virtual void	Init(void) = 0;
	virtual void*	Alloc(int size) = 0;
	virtual void	Free(void *base) = 0;
};
