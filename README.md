# BlomblyVM

*A code block assembly language.* 

This means that there are no jump instructions and the assembly itself is easier to learn
by both humans and code synthesis environments.
The provided implementation does two things:
1. Compiles programming language files (`.bb`) into assembly instruction files (`.bbvm`).
2. Provides a virtual machine to execute assembly instructions from assembly files.

Author: Emmanouil (Manios) Krasanakis<br/>
Contact: maniospas@hotmail.com<br/>
License: Apache 2.0

## About

:computer: Simple structure<br/>
:duck: Duck typing<br/>
:rocket: Multithreaded<br/>
:goggles: Variable safety<br/>

## Quickstart

Run `blombly main.bb` to compile and run the corresponding file. This will create
a compiled assembly instruction file as an intermediate step. You can use a java
syntax highlighter for `.bb` files.

Your code will typically look like the following example. In this, there are blocks
of code that can be used as methods (that run in parallel threads) or called inline. 
Communication between blocks is made either with final values, which are immutable,
or by passing computations on how to extract local variables from the calling scope.

```java
final bias = 1;
final inc_result = {
    result = add(result, bias);
}
final addinc = {
    result = add(x, y);
    inc_result: // end with colon for inline execution injection of code block
    return(result);
}
print(addinc(x=4;y=5)); // 10 - used code block as a method
```


1. [Language syntax](docs/syntax.md)
2. [Assembly specification](docs/assembly.md)
