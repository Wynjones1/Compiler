function main()
{
	       int a;
	static int b;
	const  int c;
	static const int d;
	       float e;
	static float f;
	const  float g;
	static const float h;
	file f = io.open("filename.txt", io.read);
}

generic<T>
function f(T a) -> T
{
	return a;
}


namespace io
{
	function printf(string format, args arg)
	{
	}
}

namespace concurrent
{
	generic<T>
	function start(function f, args a) -> thread
	{
		magic to start thread;
	}

	function start() piped
	{
	
	}

	thread a = start(a)
	thread b = start(b)

	pipe(a, b)
	pipe(b, a)
}
