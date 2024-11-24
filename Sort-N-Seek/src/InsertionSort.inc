.CODE

InsertionSort PROC
  ; Stack Frame:
  ; 4 Byte: [EBP+8] = First iterator address
  ; 4 Byte: [EBP+12] = Last iterator address
  ; 4 Byte: [EBP+16] = Comparator (Predicate) function address

  ; Local Variables:
  ; 12 Bytes: [EBP-12] = i-th iterator
  ; 12 Bytes: [EBP-24] = j-th iterator
  ; 12 Bytes: [EBP-36] = u iterator (j - 1 th iterator)
  ; 4 Bytes: [EBP-40] = value size (value_type)
  ; 1*value_type Bytes: [EBP-[EBP-40]] = k-th value

  ; Temporary memory:
  ; 1*value_type Bytes: For dereferencing iterator

  ENTER 40, 0

  ; save the value type
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).value_type
  MOV DWORD PTR [EBP-40], ECX

  ; calculate the sizes for 2 values and allocate the space for 2*value_type 
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

  FOR_LOOP_INIT_1:
    ; initialize i-th iterator to next(first, 1)
    MOV EDI, EBP
    SUB EDI, 12

    MOV EAX, 1
    PUSH EAX
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._next
    CALL EDX

  FOR_LOOP_1:
    ; loop condition: i_it != last_it
    MOV ESI, EBP
    SUB ESI, 12

    PUSH DWORD PTR [EBP+12]
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._comp
    CALL EDX

    CMP EAX, 0
    JZ END_FOR_LOOP_1

    MOV EDI, EBP
    SUB EDI, 40
    SUB EDI, [EBP-40]
    MOV EBX, EDI                ; save the address of k for quick access

    MOV ESI, EBP
    SUB ESI, 12
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._dref
    CALL EDX

    ; now, initialize j-th iterator to j_it = i_it
    MOV EDI, EBP
    SUB EDI, 24

    MOV ESI, EBP
    SUB ESI, 12
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._copy
    CALL EDX

    WHILE_LOOP_2:
      ; first condition: j_it != first_it
      MOV ESI, EBP
      SUB ESI, 24

      PUSH ESI
      PUSH DWORD PTR [EBP+8]
      MOV EDX, (Iterator_Functions PTR [ECX])._comp
      CALL EDX

      CMP EAX, 0
      JZ END_WHILE_LOOP_2

      ; second condition: pred(k, *u) = true
      ; for that, let's first initialize u
      MOV EDI, EBP
      SUB EDI, 36

      MOV ESI, EBP
      SUB ESI, 24
      MOV EAX, 1
      
      PUSH EAX
      PUSH ESI 
      MOV EDX, (Iterator_Functions PTR [ECX])._prev
      CALL EDX

      ; now, let's dereference the u Iterator
      MOV ESI, EDI

      MOV EDI, EBP
      SUB EDI, 40
      SUB EDI, [EBP-40]
      SUB EDI, [EBP-40]

      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._dref
      CALL EDX

      ; finally, check the second condition
      PUSH EDI              ; save the EDI (debug)
      PUSH EDI              ; push the address of *u
      PUSH EBX              ; push the address of k
      MOV EDX, [EBP+16]     ; Predicate
      CALL EDX
      POP EDI               ; retreive the EDI  

      CMP EAX, 0
      JZ END_WHILE_LOOP_2

      ; if the conditions are true,
      ; then, *j_it = *u
      ; and j_it = u

      MOV ESI, EBP
      SUB ESI, 24

      PUSH EDI
      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._asig
      CALL EDX

      MOV EDI, EBP
      SUB EDI, 24

      MOV ESI, EDI
      SUB ESI, 12

      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._copy
      CALL EDX

      JMP WHILE_LOOP_2

    END_WHILE_LOOP_2:

    ; now, before continuing the loop, 
    ; *j_it = k
    ; and, ++i_it

    MOV ESI, EBP
    SUB ESI, 24

    PUSH EBX
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._asig
    CALL EDX

    MOV EDI, EBP
    SUB EDI, 12
    MOV EAX, 1

    PUSH EAX
    PUSH EDI
    MOV EDX, (Iterator_Functions PTR [ECX])._next
    CALL EDX

    JMP FOR_LOOP_1
  END_FOR_LOOP_1:

  FUNCTION_RETURN:
    POPAD
    LEAVE
    RET 12
InsertionSort ENDP
