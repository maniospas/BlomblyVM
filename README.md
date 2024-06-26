# Blombly

*A code block assembly language.* 

There are no jump instructions other than the return statement. The assembly itself is easier to learn by humans and code synthesis environments. The provided implementation:
1. Compiles the language's *.bb* files into instruction sets *.bbvm*.
2. Is a virtual machine to execute compilation outcomes.

Author: Emmanouil (Manios) Krasanakis<br/>
Contact: maniospas@hotmail.com<br/>
License: Apache 2.0

*Build command: g++ -std=c++20 -pthread -I./include src/\*.cpp src/data/\*.cpp src/interpreter/\*.cpp blombly.cpp -o blombly -O3*

## About

:computer: Simple structure<br/>
:duck: Duck typing<br/>
:rocket: Automatic parallelization<br/>
:goggles: Memory safety<br/>

## Quickstart

Run `blombly main.bb` to compile and run a file called `main.bb`. This will create
a compiled *.bbvm* version as an intermediate step. You can use a java
syntax highlighter for *.bb* files and the script `bbreader.py` to convert back compilation outcomes.

<details>
<summary>Control flow: semicolon, if, while</summary>

```java
i = 1;
while(i<=20,
    if(i%2==0 && i%3==0, 
        message = str(i)+" is divided by 2 and 3";
        print(message);
    );  
    i = i+1;
);
```
</details>


<details>
<summary>Inlining</summary>

```java
message = "original"
defs = {
    message="overwritten";
}  // this is a code block (not executed yet)
print(message);  // original
defs:  // effectively copy-paste the code here
print(message);  // overwritten
```
</details>


<details>
<summary>Calling</summary>

```java
add = {
    return x+y;
}
z = add(x=1;y=2);  // all calls of non-native methods use keyword arguments (forced clarity)
print(z);  // 3
```

```java
add = {
    return x+y;
}
x = 0;
values = {
    x=x+1;
    y=2;
}
z = add(values:);  // can run more code within arguments (arguments are a temporary scope)
print(x);  // 0
print(z);  // 3
```

```java
// can remove the block's last semicolon.
// if no return is encountered, the last value is obtained
add = {x+y} 
z = add(x=1;y=2);
print(z);  // 3
```
</details>


<details>
<summary>Final visibility</summary>

```java
createInc = {
    final bias = 1;
    inc = {
        // can see all finals of its declaring scope
        return x+bias;
    }
    return inc;
}
inc = createInc();
print(inc(1));  // 2
// cannot run print(bias); 
```

```java
final fib = {  // prefer making block declarations final
    if(n<2, return 1); 
    return fib(n=n-1)+fib(n=n-2);  // can only see fib from its calling scopy if its final
}

print(fib(n=9));  // 55
```
</details>


<details>
<summary>Recursive inlining</summary>

```java
final search = {
    default start = 0; // set only if not found
    default end = len(list)-1; // set only if not found
    if(start>end, return new(found=false));
    middle = int((start + end)/2);
    if(list[middle]==query, return new(found=true;pos=middle));
    if(list[middle]<query, start=middle+1, end=middle-1);
    search:  // inline recursion (no need for passing variables around)
}

z = List(1,2,3,5,6,7);
res = search(list=z;query=7);
// ommited semicolons below for last commands of each if block
if(res.found, 
    print("Query found at index", res.pos), 
    print("Query not found") 
); // semicolon mandatory after if and while
```

```java
// this is similar to recursive calls but inlining and new run sequentially (is faster than generating a ton of threads)
final fib = {
    // returning from a new just returns a different value
    if(n<2, return 1); 
    // using new to keep the original value of n and catch the return value of fib:
    return new(n = n-1;fib:) + new(n = n-2;fib:);
}

tic = time();
print("Result", new(n=21;fib:));
print("Time", time()-tic);
```


</details>


## Tips

1. Either call code blocks (these run in parallel) or execute them inline (faster for sequential code). 
2. Called blocks can access only final values of their declaration scope. Sequential code is automatically parallelized.
3. `new(...)` creates a new scope with a `final this` object that stores new assignments there. This is returned and detached from the scope after *new* finishes.
4. You can return custom values from *new* to replace the generated object.

## Links

1. [Language syntax](docs/syntax.md)
2. [Assembly specification](docs/assembly.md)
3. [Code block execution](docs/executions.md)
4. [Keywords](docs/keywords.md)
