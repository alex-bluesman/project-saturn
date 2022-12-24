class Test
{
public:
	int a;
};

//Test t1 __attribute__((section(".heap")));

int testFunc()
{
	Test test;
	return test.a;
}
