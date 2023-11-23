# BlomblyVM

*A code block assembly language.* 

This means that there are no jump instructions and the assembly itself is easier to learn.
The provided implementation does two things:
1. Compiles programming language files (`.bb`) into assembly instruction files (`.bbvm`).
2. Provides a virtual machine to execute assembly instructions from assembly files.

Author: Emmanouil (Manios) Krasanakis<br/>
Contact: maniospas@hotmail.com<br/>
License: Apache 2.0

:warning: Currently, the compiler does not support expression nesting.

## Quickstart

Run `blombly main.bb` to compile and run the corresponding file. This will create
a compiled assembly instruction file as an intermediate step.



