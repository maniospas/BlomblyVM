final fib = {
    res = Vector(n+1);
    res[0] = 1;
    res[1] = 1;
    i = 2;
    while(i<=n,
        res[i] = res[i-2]+res[i-1];
        i = i+1;
    );
    return res[n];
}

print(fib(n=9)); // 55