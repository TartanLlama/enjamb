# Enjamb

An esoteric programming language where it's not what's in your lines that matters: it's where you break them.

## Outline

Enjamb is a stack-based language where instructions are encoded by the number of characters in each line of text. For example, a line with `14` characters in it means "take the top two values off of the stack, add them, and push the result." 

### I/O instructions
1. print character
2. print number
3. read character
4. read number

### Control flow instructions
5. label
6. call
7. jump
8. jump if zero
9. jump if neg
10. return
11. terminate

### Heap instructions
12. store
13. load

### Arithmetic instructions
14. add
15. sub
16. mul
17. div
18. mod

### Stack instructions
19. push  
20. dup
21. swap
22. pop