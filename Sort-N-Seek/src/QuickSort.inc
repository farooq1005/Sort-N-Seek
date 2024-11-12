; ------------------------------------------------------------------
; No need to specify the model and stack memory. The user will 
; define them before including this file
; ------------------------------------------------------------------
; For now, I won't be using aliases for the parameter variables 
; since it is essential to grasp the structure of stackframe for 
; now. 
; ------------------------------------------------------------------
; The procedures will be using stdcall convention. The user is 
; required to specify that in .MODEL directive
; ------------------------------------------------------------------


; ------------------------------------------------------------------
; Parameters:
; Offset with respect to EBP = Description
; 8 = First Iterator: 4 bytes (object) address
; 12 = Last Iterator: 4 bytes (object) address 
; 16 = Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; 20 = Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; 24 = Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; 28 = Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; 32 = Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; 36 = Next: 4 bytes (procedure with two operand: iterator and integer) (return address of an iterator in EAX) address
; 40 = Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; 
; Total size of parameters: 36 Bytes
; 
; Local Variables:
; Pivot iterator: 4 bytes 
; i-th iterator: 4 bytes
; j-th iterator: 4 bytes
;
; Total size of Local Varibales: 12 bytes
; ------------------------------------------------------------------

.CODE
Partition PROC
  ; Set up stack frame for the procedure
  PUSH EBP
  MOV EBP, ESP
  SUB ESP, 12
  
  ; first save Pivot iterator and i-th Iterator
  
  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP + 12]
  MOV EBX, [EBP + 40]                 ; Load the address of prev() function
  CALL EBX

  MOV DWORD PTR [EBP - 4], EAX        ; pivot = prev(last)

  ; copy first iterator to i-th iterator variable
  PUSH DWORD PTR [EBP + 8]
  MOV EBX, [EBP + 20]
  CALL EBX

  MOV DWORD PTR [EBP - 8], EAX        ; i = copy_it(first)

PRE_LOOP_30:                          ; pre-forloop initializations
  PUSH DWORD PTR [EBP + 8]
  MOV EBX, [EBP + 20]
  CALL EBX

  MOV DWORD PTR [EBP - 12], EAX       ; j = copy_it(first)

LOOP_30:  
  ; Loop Condition: next(j) != last
  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP - 12]
  MOV EBX, [EBP + 36]
  CALL EBX

  PUSH DWORD PTR [EBP + 12]
  PUSH EAX
  MOV EBX, [EBP + 24]
  CALL EBX

  CMP EAX, 0
  JZ END_LOOP_30

IF_30:
  ; Calling pred(*j, *p) 
  PUSH DWORD PTR [EBP - 4]
  MOV EBX, [EBP + 16]                ; *p
  CALL EBX

  PUSH EAX

  PUSH DWORD PTR [EBP - 12]
  MOV EBX, [EBP + 16]                ; *y
  CALL EBX

  PUSH EAX

  MOV EBX, [EBP + 32]
  CALL EBX

  CMP EAX, 0                         ; if pred(*j, *p) == True, proceed to swapping
  JZ END_IF_30

  PUSH DWORD PTR [EBP - 8]           ; i (Local)
  PUSH DWORD PTR [EBP - 12]          ; j (Local)
  MOV EBX, [EBP + 28]                ; iter_swap(i, j)
  CALL EBX     

  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP - 8]
  MOV EBX, [EBP + 36]
  CALL EBX

  MOV DWORD PTR [EBP - 8], EAX       ; After swapping, ++i
END_IF_30:
  
  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP - 12]
  MOV EBX, [EBP + 36]
  CALL EBX

  MOV DWORD PTR [EBP - 12], EAX     ; after each iteration, ++j
  JMP LOOP_30
END_LOOP_30:
  
  ; we reached correct position, perform iter_swap(i, p)
  ; and return i

  PUSH DWORD PTR [EBP - 4]           ; p (Local)
  PUSH DWORD PTR [EBP - 8]           ; i (Local)
  MOV EBX, [EBP + 28]                ; iter_swap(i, p)
  CALL EBX 

  MOV EAX, [EBP - 8]

  MOV ESP, EBP
  POP EBP
  RET 36
Partition ENDP

; ------------------------------------------------------------------
; Parameters:
; Offset with respect to EBP = Description
; 8 = First Iterator: 4 bytes (object) address
; 12 = Last Iterator: 4 bytes (object) address 
; 16 = Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; 20 = Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; 24 = Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; 28 = Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; 32 = Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; 36 = Next: 4 bytes (procedure with two operand: iterator and integer) (return address of an iterator in EAX) address
; 40 = Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; 44 = Distance: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) 
; 
; Total size of parameters: 40 Bytes
; 
; Local Variables:
; Pivot iterator: 4 bytes

; Total size of Local Varibales: 4 bytes
; ------------------------------------------------------------------

QuickSort PROC
  ; Set up stack frame for the procedure
  PUSH EBP
  MOV EBP, ESP
  SUB ESP, 4
  PUSHAD

IF_31:
  ; first check the base case
  ; if first == last OR next(first) == last, return
  PUSH DWORD PTR [EBP + 12]
  PUSH DWORD PTR [EBP + 8]
  MOV EBX, [EBP + 24]
  CALL EBX

  CMP EAX, 0                          ; first == last, return
  JZ END_IF_31

  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP - 8]
  MOV EBX, [EBP + 36]
  CALL EBX

  PUSH DWORD PTR [EBP + 12]
  PUSH EAX
  MOV EBX, [EBP + 24]
  CALL EBX

  CMP EAX, 0
  JZ END_IF_31                        ; next(first) == last, return

  PUSH DWORD PTR [EBP + 40]
  PUSH DWORD PTR [EBP + 36]
  PUSH DWORD PTR [EBP + 32]
  PUSH DWORD PTR [EBP + 28]
  PUSH DWORD PTR [EBP + 24]
  PUSH DWORD PTR [EBP + 20]
  PUSH DWORD PTR [EBP + 16]
  PUSH DWORD PTR [EBP + 12]
  PUSH DWORD PTR [EBP + 8]

  MOV EBX, OFFSET Partition
  CALL Partition                      ; EAX has pivot now

  MOV DWORD PTR [EBP - 4], EAX

  ; two recursive calls
  ; quickSort(first, pivot)
  ; quickSort(next(pivot), last)

  PUSH DWORD PTR [EBP + 44]
  PUSH DWORD PTR [EBP + 40]
  PUSH DWORD PTR [EBP + 36]
  PUSH DWORD PTR [EBP + 32]
  PUSH DWORD PTR [EBP + 28]
  PUSH DWORD PTR [EBP + 24]
  PUSH DWORD PTR [EBP + 20]
  PUSH DWORD PTR [EBP + 16]
  PUSH EAX          
  PUSH DWORD PTR [EBP + 8]

  CALL QuickSort

  ; now, next(pivot)
  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP - 4]
  MOV EBX, [EBP + 36]
  CALL EBX

  PUSH DWORD PTR [EBP + 44]
  PUSH DWORD PTR [EBP + 40]
  PUSH DWORD PTR [EBP + 36]
  PUSH DWORD PTR [EBP + 32]
  PUSH DWORD PTR [EBP + 28]
  PUSH DWORD PTR [EBP + 24]
  PUSH DWORD PTR [EBP + 20]
  PUSH DWORD PTR [EBP + 16]
  PUSH DWORD PTR [EBP + 12]
  PUSH EAX          

  CALL QuickSort

END_IF_31:
  POPAD
  MOV ESP, EBP
  POP EBP
  RET 40
QuickSort ENDP