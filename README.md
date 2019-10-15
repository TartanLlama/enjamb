# Enjamb

An esoteric programming language where it's not what's in your lines that matters — it's where you break them.

## Outline

> _enjambment (/ɛnˈdʒæmbmənt/): the running on of the thought from one line, couplet, or stanza to the next without a syntactical break._

Enjamb is a stack-based language where instructions are encoded by the number of characters in each line of text. For example, a line with `14` characters in it means "take the top two values off of the stack, add them, and push the result." 

## Abstract Machine

Enjamb runs on an abstract machine with:

- A program counter which marks the currently-executing instruction.
- A linear sequence of instructions, stored in memory inaccesible to the program.
- A stack of signed 32-bit integers (called "the stack").
- An indexable array of 4096 signed 32-bit integers (called "the heap").
- A stack of program counters facilitating calling and returning from functions (called "the call stack").

## Instructions

There are 22 instructions in Enjamb. Each opcode takes a single line of text, where the opcode is selected by the number of characters in that line. Some instructions take a single operand, which is encoded in the next line down.

Blank lines are no-ops, unless they come after an instruction which takes an operand, in which case they form the operand (see the specific instructions for details).

### I/O instructions
1. print character: Pop the top of the stack and print it as an ASCII character.
2. print number: Pop the top of the stack and print it as a 32bit signed integer.
3. read character: Read an ASCII character from stdin and push it on to the stack.
4. read number: Read a signed integer from stdin and pust it on to the stack.

### Control flow instructions
Some of these instructions take a label name as an operand. Labels are named by the text on the line after the opcode, and can contain any characters you like. For example, "Exit, pursued by a bear." is a valid Enjamb label.

5. label <label>: Marks this part of the code with the label opcode
6. call <label>: Jumps to the instruction marked by the given label, and pushes the old program counter to the call stack.
7. jump <label>: Unconditionally jumps to the instruction marked by the given label.
8. jump if zero <label>: Jumps to the instruction marked by the given label if the value at the top of the stack is `0`.
9. jump if neg <label>: Jumps to the instruction marked by the given label if the value at the top of the stack is negative.
10. return: Returns to the program counter at the top of the call stack.
11. exit: Exits the program, returning the value at the top of the stack.

### Heap instructions
12. store: Pops the first two values from the stack and stores the first into `heap[second]`.
13. load: Pops the top value from the stack and pushes `heap[value]`.

### Arithmetic instructions
These instructions all pop the first two values from the stack and performs the specified arithmetic operation on them, pushing the result.

14. add
15. sub
16. mul
17. div: Integral division.
18. mod

### Stack instructions
19. push <value>: Pushes the value given as an operand onto the top of the stack. The operand is specified on the line following the opcode, and is encoded by the number of characters on that line.
20. dup: Duplicates the value on the top of the stack. 
21. swap: Swaps the two values on top of the stack.
22. pop: Pops the top value from the stack, discarding it.
  
 
## "Characters"

Enjamb source is UTF-8 encoded, and a "character" in Enjamb is a user-perceived character, approximated by Unicode as a [grapheme cluster](http://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundaries). As such, the German 'ß' is counted as a single character, even though it's encoded in two UTF-8 code units. 'g̈' — constucted from the latin 'g' and the combining diaeresis — is also a single character.

If your system locale changes grapheme cluster boundary rules (for example, if your system locale is Slovak, 'ch' will be one character instead of two) then you may get different results than intended by a program's author.
  
## Implementation

The implementation of Emjamb provided in this repository is an interpreter written in C++.

### Building `enjamb`

_Requirements:_
- [ICU](http://site.icu-project.org/download/), which is available on [vcpkg](https://github.com/Microsoft/vcpkg/tree/master/ports/icu).
- CMake 3.15.
- A C++ compiler with C++17 support.

[Use CMake](https://cmake.org/runningcmake/) for building.

### Running `enjamb`

```
enjamb <source file> [--debug]
```

The `--debug` flag will print out the instructions which your program encodes, which is useful for debugging.

## Examples

Hello World:

```
Hello, world, a far
cry of albatross smoothed
by rocks, by mudded
wave
I am here, world
beyond laws, held by
silences and shouts
and tear
drops darkened
by an unclear dome,

above us all
our heads turned up,
our lids shut and —
icicle

falls from old
skies shattered with
bone tools and soft
eye
bleeding under,
we can save nothing
I
can save you

from the sharp break
of the dome, of the
waves on
that rust sea,
full of glass, sail
half-mast in memory of the Earth
as it once was, but
I
forgot a long
time ago, as did we

destroy wood,
a home to albatross,
or snake or vulture,
or bright teeth, or
a lover
with boots high
above blaze-scorched
grass, glass shield
inevitable, fracturing, head-
bang, belt welt
smash of life under
closed eyes

world
alone we aren't

enough, not you nor I
I
can't build another
I
seep into soil,

waitwait
another earth
comes,
alone we aren't

world
another earth
```
