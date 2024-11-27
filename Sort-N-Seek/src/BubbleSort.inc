.CODE
BubbleSort PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = Comparator function address
  ; Return: None (in-place sorting)

  ; Local Variables
  ; 1 Byte: [EBP-1] = Sorted bool flag
  ; 12 Bytes: [EBP-13] = Iterator object
  ; 12 Bytes: [EBP-25] = Next Iterator
  ; 4 Bytes: [EBP-29] = Value Type

  ; Temporary Memory:
  ; 2*value_type Bytes - To store dereferenced values

  ENTER 29, 0

  ; save the value type
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).value_type
  MOV DWORD PTR [EBP-29], ECX

  ; calculate the sizes for 2 values and allocate the temporary space
  MOV EAX, 2
  MOV EDX, 0
  MUL ECX
  SUB ESP, EAX

  ; now, save the flags
  PUSHAD

  ; store the function pointers object first hand
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers

  ; edge case: first_it == last_it
  IF_EDGE_CASE:
    PUSH DWORD PTR [EBP+12]
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._comp

    CALL EDX

    CMP EAX, 0
    JZ FUNCTION_RETURN
  END_IF_EDGE_CASE:

  DO_WHILE:
    MOV BYTE PTR [EBP-1], 0FFh        ; set the sorted flag

    FOR_LOOP_INIT_1:
      ; Point to the iterator variable
      MOV EDI, EBP
      SUB EDI, 13     

      ; Copy the first iterator in the location of EDI
      PUSH DWORD PTR [EBP+8]
      MOV EDX, (Iterator_Functions PTR [ECX])._copy
      CALL EDX

    FOR_LOOP_1:
      ; point to the next iterator variable
      MOV EDI, EBP
      SUB EDI, 25

      ; get the next iterator (next(it, 1))
      MOV EAX, 1
      PUSH EAX
      MOV ESI, EBP
      SUB ESI, 13
      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._next
      CALL EDX

      ; compare next(it) and last_it
      PUSH DWORD PTR [EBP+12]
      PUSH EDI
      MOV EDX, (Iterator_Functions PTR [ECX])._comp
      CALL EDX

      CMP EAX, 0
      JZ END_FOR_LOOP_1

      IF_PREDICATE:
        ; now, dereference both it and next(it)
        ; first dereference it
        MOV EDI, EBP
        SUB EDI, 29
        SUB EDI, [EBP-29]
        SUB EDI, [EBP-29]

        MOV ESI, EBP
        SUB ESI, 13
        PUSH ESI
        MOV EDX, (Iterator_Functions PTR [ECX])._dref
        CALL EDX

        PUSH EDI

        ; now, dereference next(it)
        MOV EDI, EBP
        SUB EDI, 29
        SUB EDI, [EBP-29]

        MOV ESI, EBP
        SUB ESI, 25
        PUSH ESI
        MOV EDX, (Iterator_Functions PTR [ECX])._dref
        CALL EDX

        PUSH EDI

        ; now, call the comparator function
        MOV EDX, [EBP+16]
        CALL EDX

        CMP EAX, 0
        JZ END_IF_PREDICATE

        MOV ESI, EBP
        SUB ESI, 13
        PUSH ESI

        SUB ESI, 12
        PUSH ESI

        MOV EDX, (Iterator_Functions PTR [ECX])._swap
        CALL EDX

        MOV BYTE PTR [EBP-1], 0

      END_IF_PREDICATE:

      MOV EDI, EBP
      SUB EDI, 13

      MOV ESI, EDI
      SUB ESI, 12
      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._copy
      CALL EDX

      JMP FOR_LOOP_1
    END_FOR_LOOP_1:

    MOV AL, [EBP-1]
    CMP Al, 0

    JZ DO_WHILE
  END_DO_WHILE:

  FUNCTION_RETURN:

    POPAD
    LEAVE
    RET 12
BubbleSort ENDP
