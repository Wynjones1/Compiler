function fib(int : value) -> int
{
    if(value < 2)
    {
        return value;
    }

    return fib(value - 1) + fib(value - 2);
}

function main() -> int
{
    print(fib(0));
    print(fib(1));
    print(fib(2));
    print(fib(3));
    print(fib(4));
    print(fib(5));
    return 0;
}

