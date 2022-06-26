# BrainScript - BrainFuck to JavaScript transpiler
This is an extremely over-engineered transpiler that converts an input
BrainFuck file into a JavaScript file that produces the same results
as you'd get running the BrainFuck code with a dedicated interpreter.

This is my first time using C++ for a project of mine,
so the code is ~~probably~~ terrible.

## Why
I wanted to get some experience with C++ so I thought of making something
small and kind of funny so I made this thing.

## Notes
I know I could've done a better job at generating the javascript
(like creating nested data structures that we can later run), but it's much
cooler to have tons of nested while statements.

## Build & Execution
This was only tested on my Ubuntu 20.04 machine, but would probably work
on other linux distributions.

### Prerequisites

* A C++ compiler with C++11 support

* `make`

* [NodeJS](https://nodejs.org/)

* The npm package `prompt-sync`

To download on Ubuntu:
```
sudo apt install g++ make nodejs
npm install prompt-sync
```


### Compiling
To compile, just run:
```
make
```

### Running
To run, you need to give two parameters: the name of the brainfuck file
and the name of the output JavaScript file.

E.g:
```
build/bf input.bf output.js
```

There's a third optional parameter which specifies the size of the tape.
