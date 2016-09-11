function add(int : a, int : b) -> int
{
    return a + b;
}

function main() -> int
{
    int : x = add(10, 10);
    int : y = add(x, x);
    return 0;
}
