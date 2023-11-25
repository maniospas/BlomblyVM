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

:warning: The language is in alpha development; features and syntax may change.

## Quickstart

Run `blombly main.bb` to compile and run the corresponding file. This will create
a compiled assembly instruction file as an intermediate step. You can use a javascript
syntax highlighter for `.bb` files.

## Syntax

The main structure in blombly are code blocks. These represent a series of computations
to be executed on-demand (like methods) by enclosing them in brackets. Blocks can be
called like methods, as shown bellow.

```javascript
block = { 
    // does not run on declaration (like a method)
    print("Hello world!");
}
block(); // prints
block(); // prints again
```

When called, blocks will pull the last variable values from the current scope,
but any changes will only occur on a derived local scope. In short: blocks are
executed in a variable context that sees the parent if no local values are set.
These concepts are demonstrated in the following example.

```javascript
block = {
    x = add(x, 1);
    return(add(x,y));
}
x = 1;
y = 2;
print(block()); // 4
print(x); // still 1
x = 5;
print(block()); // 8
```

You can sequentially execute two blocks of code within the same context. This
looks like a method call that sets keyword arguments.

```javascript
block = {
    x = add(x, 1);
    return(add(x,y));
}
x = 0;
kwargs = {x=1;y=2};
sum = block(kwargs); // runs kwargs and then block
print(x); // still 0 because
print(sum); // 4
```

Take advantage of the same mechanism to apply cross-cutting concerns on
blocks/methods, for example creating appropriate variations with a specific
set of context alterations.


```javascript
sum = {return(add(x,y));}
inc = {x=add(x,1);}
incsum = {return(sum(inc));}
print(incsum({x=1;y=2})); // 4
```
