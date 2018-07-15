# Tinyscript

A small programming language I wrote as a way to procrastinate, and experiment. Tinyscript is
written in C++, is not optimised, is full of hacks, and was pretty fun to work on. I plan on
maybe cleaning it enough that I could use it as a light scripting system in a game. Please don't
attempt to use this for any productive purpose, bad things will most likely happen.

The VM is dynamically typed for the moment but that's likely to go away. The language is strongly
typed (enforced by the compiler) and uses type inference. You can get an example of almost
everything it does in [`test.tiny`](test.tiny)

## Building

There are two versions of tinyscript at this point. The `master` branch uses an Objective-C-style
call syntax (`[functioname arg1, arg2]`), the `c-call-syntax` branch uses the C-style syntax you
most likely know (`functionname(arg1, arg2)`).

Tinyscript uses cmake. Building goes:
    
    $ git checkout master # or git checkout c-call-syntax
    $ mkdir build && cd build
    $ cmake ..
    $ make

## Embedding

Tinyscript is built with embedding in mind. All functions are foreign, there are no ways to define
functions in a script (at the moment). I plan on writing a bit more about embedding it soon.
