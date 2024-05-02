# BlomblyVM

*A code block assembly language.* 

There are no jump instructions other than the return statement, 
and the assembly itself is easier to learn by humans and code synthesis environments.
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

Your code will look like the following example. Call code blocks either as methods
(these run in parallel) or inline. Final variables are immutable in their declaring
scope and can be seen from all threads.

```java
final inc = {
    default bias = 1;
    result = result + bias;
}
final addinc = {
    result = x+y;
    inc: // end with colon for inline execution injection of code block
    return result;
}
print(addinc(x=4, y=5)); // 10 - call code block using another to fill arguments (, is understood as ;)
```


1. [Language syntax](docs/syntax.md)
2. [Assembly specification](docs/assembly.md)
3. [Three ways to execute blocks](docs/executions.md)
