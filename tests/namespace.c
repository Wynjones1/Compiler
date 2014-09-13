from io import *

namespace ns
{
	constant a = 10;

	function do(int a) -> (int)
	{
		return a;
	}
}

function do(int a) -> (int)
{
	return 1;
}

function main()
{
	io.printf("%d\n", do(10));
	io.printf("%d\n", ns::do(10));

	int a = 10;
	io.printf("%d\n", a);
	io.printf("%d\n", ns::a);
}
