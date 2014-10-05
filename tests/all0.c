/*
 *	Test for all functionality.
 */
import io;
from io import print;

function a() -> int
{
	return 10;
}

function c(int a, int b) -> (int, int)
{
	return (a, b);
}

function main()
{
	if(a) b = 10;

	if(a)
	{
		b = 10;
	}

	if(a)
	{
		b = 10;
	}
	else
	{
		b = 10;
	}
}

function b()
{}

function c() -> ()
{}

function d()
{
	while(a == 1)
	{}

	if(a)
	{}
	else if(a)
	{}
	else
	{}
}


