import io;

function fib(int n) -> ()
{
	if(n == 0 or n == 1)
		return 1;
	else
	{
		int t0 = fib(n - 1);
		int t1 = fib(n - 2);
		return t0 + t1;
	}
}
