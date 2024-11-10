# Sort-N-Seek

The repository is still underdevelopment so the details are bound to be changed. 

## Overview
Sort-N-Seek is a collection of files that implement various sorting and searching algorithms in Assembly Language for Win-32. The code demonstrates the low level generic implementation of algorithms, offering an educational perspective on algorithm design and assembly language programming. Sort-N-Seek is for only educational purpose and free for everyone to use and change. 

## Getting Started
Download or clone the repository.
Clone the repository using Git or download the `.zip` archive of the source code.
```
git clone https://github.com/farooq1005/Sort-N-Seek.git
```
You will see various files with names representing the algorithms implemented in them. 
### Before using the files
1. Switch to Win-32 configuration
2. Make sure that you are using Microsoft Macro Assembler (MASM) as an assembler.
3. Use `stdcall` convention for the procedure calls
4. Use `flat` model

## Usage
The procedures implementing the algorithm only depend on iterators, not the containers. So you can implement your own iterable container and provide the function with iterators and it will do the steps for you. 

However, iterators themselves aren't enough. The procedures must be provided with few functions that it needs to perform the required operations. 
The sorting procedures take the following parameters:
| Param # | Parameter Name         | Type              | Usage Example                            | Size (Bytes) | Parameters (for procedures)                        | Parameters' Size (Bytes)                |
|---------|------------------------|-------------------|-------------------------------------------|--------------|---------------------------------------------------|------------------------------------------|
| 1       | First Iterator          | Iterator          | `it1`                                     | 4            | None                                              | N/A                                      |
| 2       | Last Iterator           | Iterator          | `it2`                                     | 4            | None                                              | N/A                                      |
| 3       | Dereference Procedure   | Function Pointer  | `dref(it)`                          | 4            | 1x Iterator references                            | 4                             |
| 4       | Copy Iterator Procedure | Function Pointer  | `copy_iter(it)`                           | 4            | 1x Iterator reference                             | 4                                        |
| 5       | Compare Iterator Proc.  | Function Pointer  | `compare(it1, it2)`                       | 4            | 2x Iterator references                            | 4 + 4 = 8                                |
| 6       | Iterator's Value Swap   | Function Pointer  | `iter_swap(it1, it2)`                     | 4            | 2x Iterator references                            | 4 + 4 = 8                                |
| 7       | Predicate Procedure     | Function Pointer  | `predicate(it1.v, it2.v)`                 | 4            | 2x Iterator values references                     | 4 + 4 = 8                                |
| 8       | Next Iterator Procedure | Function Pointer  | `next(it, c)`                             | 4            | 1x Iterator reference, 1x Unsigned Integer offset | 4 + 4 = 8                                |
| 9       | Previous Iterator Proc. | Function Pointer  | `prev(it, c)`                             | 4            | 1x Iterator reference, 1x Unsigned Integer offset | 4 + 4 = 8                                |
| 10      | Distance Procedure      | Function Pointer  | `distance(it1, it2)`                      | 4            | 2x Iterator references                            | 4 + 4 = 8                                |

Example: Sorting functions
```asm
.DATA
  myArr DD 9, 8, 7, 6, 5, 4, 3, 2, 1

.CODE
Main PROC
  PUSH OFFSET Distance
  PUSH OFFSET Prev
  PUSH OFFSET Next
  PUSH OFFSET Predicate
  PUSH OFFSET IterSwap
  PUSH OFFSET Compare
  PUSH OFFSET CopyIter
  PUSH OFFSET Deref
  PUSH OFFSET myArr
  ADD DWORD PTR [ESP], SIZEOF myArr
  PUSH OFFSET myArr

  CALL BubbleSort
Main ENDP
END MAIN
```

The searching procedures take the following parameters:
| Param # | Parameter Name         | Type              | Usage Example                            | Size (Bytes) | Parameters (for procedures)                        | Parameters' Size (Bytes)                |
|---------|------------------------|-------------------|-------------------------------------------|--------------|---------------------------------------------------|------------------------------------------|
| 1       | First Iterator          | Iterator          | `it1`                                     | 4            | None                                              | N/A                                      |
| 2       | Last Iterator           | Iterator          | `it2`                                     | 4            | None                                              | N/A                                      |
| 3       | Target Value Reference  | Value Reference   | `target`                                  | 4            | 1x Value reference                                | 4                                        |
| 4       | Dereference Procedure   | Function Pointer  | `dref(it1)`                               | 4            | 1x Iterator reference                             | 4                                        |
| 5       | Copy Iterator Procedure | Function Pointer  | `copy_iter(it)`                           | 4            | 1x Iterator reference                             | 4                                        |
| 6       | Compare Iterator Proc.  | Function Pointer  | `compare(it1, it2)`                       | 4            | 2x Iterator references                            | 4 + 4 = 8                                |
| 7       | Iterator's Value Swap   | Function Pointer  | `iter_swap(it1, it2)`                     | 4            | 2x Iterator references                            | 4 + 4 = 8                                |
| 8       | Predicate Procedure     | Function Pointer  | `predicate(it1.v, it2.v)`                 | 4            | 2x Iterator values references                     | 4 + 4 = 8                                |
| 9       | Next Iterator Procedure | Function Pointer  | `next(it, c)`                             | 4            | 1x Iterator reference, 1x Unsigned Integer offset | 4 + 4 = 8                                |
| 10      | Previous Iterator Proc. | Function Pointer  | `prev(it, c)`                             | 4            | 1x Iterator reference, 1x Unsigned Integer offset | 4 + 4 = 8                                |
| 11      | Distance Procedure      | Function Pointer  | `distance(it1, it2)`                      | 4            | 2x Iterator references                            | 4 + 4 = 8                                |

---
## Supported Algorithms
Since repository is underdevelopment, not all algorithms are implemented yet.

### Sorting
  - Bubble Sort
  - Shell Sort
  - Comb Sort
  - Selection Sort
  - Insertion Sort
  - Merge Sort
  - Quick Sort
  - Heap Sort
  - Count Sort
  - Bucket Sort
  - Radix Sort

### Searching
  - Linear Search
  - Binary Search
  - Interpolation Search

## License
This project is licensed under the Creative Commons 1.0 Universal (CC0 1.0) Public Domain Dedication.

You can copy, modify, distribute, and perform the work, even for commercial purposes, without asking for permission.
No copyright or related rights are attached to the work, and there is no requirement for attribution.
