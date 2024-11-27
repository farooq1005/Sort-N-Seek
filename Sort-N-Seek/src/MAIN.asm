INCLUDE Irvine32.inc
INCLUDE ITERATOR.inc
INCLUDE HeapSort.inc
INCLUDE CombSort.inc
INCLUDE QuickSort.inc
INCLUDE ShellSort.inc
INCLUDE MergeSort.inc
INCLUDE BubbleSort.inc
INCLUDE BinarySearch.inc
INCLUDE SelectionSort.inc
INCLUDE InsertionSort.inc

.CODE

Copy PROC
  ; Stack Frame:
  ; [EBP+8] = Iterator address
  ; Copies the iterator to the memory pointed by EDI (preserving EDI) 

  ENTER 0, 0
  PUSHAD
  MOV ESI, [EBP+8]        

  MOV ECX, SIZEOF Iterator
  CLD
  REP MOVSB        

  POPAD
  LEAVE
  RET 4    
Copy ENDP

Assign PROC
  ; Stack Frame:
  ; [EBP+8] = Iterator address
  ; [EBP+12] = Value address 

  ENTER 0, 0
  PUSHAD

  MOV ESI, [EBP+12]
  MOV EBX, [ESI]
  MOV EDI, [EBP+8]
  MOV ECX, (Iterator PTR [EDI]).value_type
  MOV EDI, (Iterator PTR [EDI]).pointer
  CLD
  REP MOVSB

  POPAD
  LEAVE
  RET 8
Assign ENDP

; Procedures for Iterator_Functions
Next PROC
  ; Stack Frame: 
  ; [EBP+8] = Iterator address
  ; [EBP+12] = Offset
  ; Return: Address of the updated iterator in EDI
  
  ENTER 0, 0 
  PUSHAD
  PUSH DWORD PTR [EBP+8]
  CALL Copy

  MOV ESI, [EBP+8]               
  MOV EAX, (Iterator PTR [ESI]).pointer 
  MOV EBX, (Iterator PTR [ESI]).value_type
  
  MOV ECX, [EBP+12]              
  IMUL EBX, ECX                  
  
  ADD EAX, EBX
  MOV DWORD PTR (Iterator PTR [EDI]).pointer, EAX
      
  POPAD
  LEAVE
  RET 8                          
Next ENDP

Previous PROC
  ; Stack Frame: 
  ; [EBP+8] = Iterator address
  ; [EBP+12] = Offset
  ; Return: Address of the updated iterator in EDI
  
  ENTER 0, 0 
  PUSHAD
  PUSH DWORD PTR [EBP+8]
  CALL Copy                 

  MOV ESI, [EBP+8]               
  MOV EAX, (Iterator PTR [ESI]).pointer 
  MOV EBX, (Iterator PTR [ESI]).value_type
  
  MOV ECX, [EBP+12]              
  IMUL EBX, ECX                  
  
  SUB EAX, EBX                   
  MOV DWORD PTR (Iterator PTR [EDI]).pointer, EAX                 
        
  POPAD
  LEAVE
  RET 8                          
Previous ENDP

Compare PROC
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Comparison result (0, <0, >0) in EAX

  ENTER 0, 0
  PUSH ESI
  PUSH EDI
  PUSH EBX

  MOV ESI, [EBP+8]  
  MOV EDI, [EBP+12] 

  MOV EBX, (Iterator PTR [ESI]).pointer
  MOV EAX, (Iterator PTR [EDI]).pointer
  SUB EAX, EBX

  POP EBX
  POP EDI
  POP ESI
  LEAVE
  RET 8 
Compare ENDP

Dereference PROC 
  ; Stack Frame:
  ; [EBP+8] = Iterator address
  ; Return: Value in memory pointed by EDI

  ENTER 0, 0
  PUSHAD

  MOV EAX, [EBP+8]
  MOV EBX, [EAX+0]
  MOV ESI, (Iterator PTR [EAX]).pointer
  MOV ECX, (Iterator PTR [EAX]).value_type
  CLD
  REP MOVSB 

  POPAD
  LEAVE
  RET 4  
Dereference ENDP

Swap PROC
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Nothing

  ENTER 0, 0
  PUSHAD

  MOV EDX, [EBP+8]
  MOV EAX, (Iterator PTR [EDX]).value_type
  SUB ESP, EAX

  ; Approach: Move one value to a temporary storage
  ; then, copy one value to another iterator 
  ; then, copy the temporary storage to the iterator
  MOV ESI, (Iterator PTR [EDX]).pointer
  MOV EDI, ESP
  MOV ECX, EAX
  REP MOVSB

  PUSH EDX
  MOV EDX, [EBP+12]
  MOV ESI, (Iterator PTR [EDX]).pointer
  POP EDX
  MOV EDI, (Iterator PTR [EDX]).pointer
  MOV ECX, EAX
  REP MOVSB

  MOV EDX, [EBP+12]
  MOV ESI, ESP
  MOV EDI, (Iterator PTR [EDX]).pointer
  MOV ECX, EAX
  REP MOVSB

  ADD ESP, EAX

  POPAD
  LEAVE
  RET 8
Swap ENDP

Distance PROC
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Distance in EAX (number of elements)

  ENTER 0, 0
  PUSH ESI
  PUSH EDI
  PUSH EBX
  PUSH ECX

  MOV ESI, [EBP+8]
  MOV EDI, [EBP+12]

  MOV EBX, (Iterator PTR [ESI]).pointer 
  MOV EAX, (Iterator PTR [EDI]).pointer
  
  SUB EAX, EBX 
  CDQ

  MOV ECX, [ESI].Iterator.value_type
  IDIV ECX 

  POP ECX
  POP EBX
  POP EDI
  POP ESI
  LEAVE
  RET 8
Distance ENDP

Predicate PROC
  ; Stack Frame: 
  ; [EBP+8] = ADDRESS of first value 
  ; [EBP+12] = ADDRESS of second value
  ; Return: 1 or 0 as predicate result in EAX

  ; Local Variables: None
  ; Temporary memory: None

  ENTER 0, 0
  PUSH EBX

  MOV EAX, [EBP+8]
  MOV EAX, [EAX]

  MOV EBX, [EBP+12]
  MOV EBX, [EBX]

  CMP EAX, EBX
  JL TRUE_RES

  MOV EAX, 0
  JMP RETURN_PROC

  TRUE_RES:
    MOV EAX, 1

  RETURN_PROC:
  
  POP EBX
  LEAVE
  RET 8
Predicate ENDP

PrintRange PROC
  ; Stack Frame:
  ; [EBP+8] = First iterator address
  ; [EBP+12] = Second iterator address
  ; Return: None

  ; Local variables: None
  ; Temporary memory: 
  ; 4 Bytes - for dereferenced value

  ENTER 4, 0 
  PUSHAD 

  ; Load function pointers from the first iterator
  MOV EAX, [EBP+8]            ; Load the address of first iterator
  MOV EDX, (Iterator PTR [EAX]).function_pointers

  ; Prepare function pointers
  MOV EBX, (Iterator_Functions PTR [EDX])._next 
  MOV EBX, (Iterator_Functions PTR [EDX])._next   ; Calculate address for and load _next
  MOV ECX, (Iterator_Functions PTR [EDX])._comp   ; Calculate address for and load _comp
  MOV ESI, (Iterator_Functions PTR [EDX])._dref   ; Calculate address for and load _dref

MAIN_PRINT_LOOP:
  ; Compare first and second iterators
  PUSH DWORD PTR [EBP+12]     ; Push second iterator
  PUSH DWORD PTR [EBP+8]      ; Push first iterator
  CALL ECX                    ; Call _comp function
  CMP EAX, 0                  ; Check if iterators are equal
  JZ END_MAIN_PRINT_LOOP      ; If equal, exit loop

  ; Dereference the current iterator
  MOV EDI, EBP
  SUB EDI, 4                  ; Point to temporary storage
  PUSH DWORD PTR [EBP+8]      ; Push first iterator
  CALL ESI                    ; Call _dref function
  MOV EAX, [EDI]              ; Load dereferenced value
  CALL WriteInt               ; Print the value
  CALL Crlf                   ; Print a newline

  ; Move to the next iterator
  MOV EDI, [EBP+8]
  PUSH 1                      ; Push offset (move by 1 element)
  PUSH DWORD PTR [EBP+8]      ; Push first iterator
  CALL EBX                    ; Call _next function
  JMP MAIN_PRINT_LOOP         ; Repeat loop

END_MAIN_PRINT_LOOP:
  POPAD                       ; Restore all general-purpose registers
  LEAVE                       ; Clean up the stack frame
  RET 8                       ; Return, clean up arguments (2 x 4 bytes)
PrintRange ENDP

.DATA
  myArr SDWORD -3, 2, -5, 6, 1, 7, 8, 9, 0, 4  ; Array to iterate
  iterator_start Iterator <>          ; Starting iterator
  iterator_end Iterator <>            ; Ending iterator
  iter_functions Iterator_Functions <> ; Iterator functions
  
  target DD -3

  str1 DB "The array is: ", 10, 13, 0
  str2 DB "The sorted array is: ", 10, 13, 0
  str3 DB "The target is: ", 0
  str4 DB "The target is found at index: ", 0

.CODE

Main PROC
  ; Set up the starting iterator
  LEA EAX, myArr
  MOV iterator_start.pointer, EAX 
  MOV iterator_start.value_type, TYPE myArr
  MOV iterator_start.function_pointers, OFFSET iter_functions

  ; Set up the ending iterator
  LEA EAX, myArr
  ADD EAX, LENGTHOF myArr * TYPE myArr
  MOV iterator_end.pointer, EAX
  MOV iterator_end.value_type, TYPE myArr
  MOV iterator_end.function_pointers, OFFSET iter_functions

  ; Set up iterator functions
  MOV iter_functions._next, OFFSET Next
  MOV iter_functions._prev, OFFSET Previous
  MOV iter_functions._copy, OFFSET Copy
  MOV iter_functions._dist, OFFSET Distance
  MOV iter_functions._swap, OFFSET Swap
  MOV iter_functions._comp, OFFSET Compare
  MOV iter_functions._dref, OFFSET Dereference
  MOV iter_functions._asig, OFFSET Assign

  MOV EDX, OFFSET str1
  CALL WriteString

  ; Call PrintRange
  LEA EAX, iterator_start          ; Load address of starting iterator
  LEA EBX, iterator_end            ; Load address of ending iterator
  PUSH EBX                         ; Push ending iterator
  PUSH EAX                         ; Push starting iterator
  CALL PrintRange                  ; Call the PrintRange procedure

  ; since we arent making copies of iterator for functions, we would need to reassign the address
  LEA EAX, myArr
  MOV iterator_start.pointer, EAX

  PUSHAD
  MOV EAX, OFFSET Predicate
  PUSH EAX
  LEA EAX, iterator_end
  PUSH EAX
  LEA EAX, iterator_start
  PUSH EAX
  CALL SelectionSort
  POPAD

  LEA EAX, myArr
  MOV iterator_start.pointer, EAX 

  CALL Crlf
  MOV EDX, OFFSET str2
  CALL WriteString

  ; Call PrintRange
  LEA EAX, iterator_start          ; Load address of starting iterator
  LEA EBX, iterator_end            ; Load address of ending iterator
  PUSH EBX                         ; Push ending iterator
  PUSH EAX                         ; Push starting iterator
  CALL PrintRange                  ; Call the PrintRange procedure

  LEA EAX, myArr
  MOV iterator_start.pointer, EAX 

  SUB ESP, 12
  MOV EDI, ESP

  PUSHAD
  MOV EAX, OFFSET Predicate
  PUSH EAX
  MOV EAX, OFFSET target
  PUSH EAX
  LEA EAX, iterator_end
  PUSH EAX
  LEA EAX, iterator_start
  PUSH EAX
  CALL BinarySearch

  CALL Crlf

  MOV EDX, OFFSET str3
  CALL WriteString
  MOV EAX, target
  CALL WriteInt

  LEA EAX, myArr
  MOV iterator_start.pointer, EAX 

  CALL Crlf

  PUSH EDI
  PUSH OFFSET iterator_start
  CALL Distance

  MOV EDX, OFFSET str4
  CALL WriteString
  CALL WriteInt

  ADD ESP, 12

  EXIT
Main ENDP

END Main