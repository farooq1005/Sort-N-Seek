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
; First Iterator: 4 bytes (object) address
; Last Iterator: 4 bytes (object) address 
; Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; Next: 4 bytes (procedure with two operand: iterator and integer) (return address of an iterator in EAX) address
; Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; Distance: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) 
; 
; Total size of parameters: 40 Bytes
; 
; Local Variables:
; Sorted flag: 1 byte 
; Address of an iterator: 4 bytes (used in for loop)
; Address of next iterator: 4 bytes (used in for loop)
;
; Total size of Local Varibales: 9 bytes
; ------------------------------------------------------------------

.CODE
BubbleSort PROC
  ; Set up stack frame for the procedure
  PUSH EBP
  MOV EBP, ESP
  SUB ESP, 9
  PUSHAD                                          ; store the register values

  MOV BYTE PTR [EBP + 1], 0FFh                    ; Sorted flag

  LOOP1:                                            ; Do-while loop
    MOV BYTE PTR [EBP - 1], 0FFh                    ; set Sorted flag to true
  
    LOOP2_PRE:                                        ; pre-forloop initialization
      ; Calling copy_iter
      PUSH DWORD PTR [EBP + 8]
      MOV EBX, [EBP + 24]                             ; Address of copy_iter function
      CALL EBX
      MOV DWORD PTR [EBP - 5], EAX                    ; it (Local)

    LOOP2:                                            ; For loop
  
      ; Checking (next(it) != Last)
      MOV EAX, 1
      PUSH EAX
      PUSH DWORD PTR [EBP - 5]
      MOV EBX, [EBP + 36]                             ; Address of next function
      CALL EBX
      MOV DWORD PTR [EBP - 9], EAX                    ; nx (Local)

      PUSH DWORD PTR [EBP + 12]                       ; last iterator
      PUSH DWORD PTR [EBP - 9]                        ; nx (Local)
      MOV EBX, [EBP + 24]                             ; compare iterator function
      CALL EBX

      CMP EAX, 0
      JZ END_LOOP2

      IF3:
        ; Calling pred(*nx, *it)
        PUSH DWORD PTR [EBP - 5]                        ; it (Local)
        MOV EBX, [EBP + 16]                             ; Deref (Func)
        CALL EBX
  
        PUSH EAX                                        ; *it pushed to args of pred
  
        PUSH DWORD PTR [EBP - 9]                        ; nx (Local)
        MOV EBX, [EBP + 16]                             ; Deref (Func)
        CALL EBX

        PUSH EAX                                        ; *nx pushed to args of pred

        MOV EAX, [EBP + 32]                             ; pred(*nx, *it)
        CALL EAX
        CMP EAX, 0
        JZ END_IF3                                      ; !pred(*nx, *it)

        PUSH DWORD PTR [EBP - 5]                        ; it (Local)
        PUSH DWORD PTR [EBP - 9]                        ; nx (Local)
        MOV EBX, [EBP + 28]                             
        CALL EBX                                        ; iter_swap(nx, it)

        MOV BYTE PTR [EBP - 1], 00h                     ; range is possibily unsorted

      END_IF3:

      MOV EAX, [EBP - 9]
      MOV DWORD PTR [EBP - 5], EAX                    ; ++it (in other words, it = nx)

      JMP LOOP2
    END_LOOP2:

    MOVSX EAX, BYTE PTR [EBP - 1]
    CMP EAX, 0FFFFFFFFh
  
    JZ END_LOOP1
    JMP LOOP1
  END_LOOP1:
  
  POPAD                                           ; Restore flags
  MOV ESP, EBP                                    ; Restore ESP
  POP EBP                                         ; Restore EBP

  RET 40
BubbleSort ENDP