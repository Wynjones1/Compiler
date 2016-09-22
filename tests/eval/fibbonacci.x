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
    return fib(3);
}
$
2