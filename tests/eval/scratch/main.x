function add(int : x, int : y)
{
	return x + y;
}

function main() -> int
{
	int : x = 20;
	x = x * x;
	print(add(x, 20));
	return 0;
}
