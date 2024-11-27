.CODE
Partition PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = Comparator function address
  ; Return: Pivot iterator at the address pointed by EDI

  ; Local Variables:
  ; 12 Bytes: [EBP-12] = Pivot Iterator
  ; 12 Bytes: [EBP-24] = i-th Iterator
  ; 12 Bytes: [EBP-36] = j-th Iterator
  ; 12 Bytes: [EBP-48] = next(j_it, 1) iterator
  ; 4 Bytes: [EBP-52] = Value type

  ; Temporary Memory:
  ; 2*value_type: 2x Dereferenced values

  ENTER 52, 0

  ; save the value type
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).value_type
  MOV DWORD PTR [EBP-52], ECX

  ; calculate the sizes for 2 values and allocate the temporary space
  MOV EAX, 2
  MOV EDX, 0
  MUL ECX
  SUB ESP, EAX

  ; finally, save the flags
  PUSHAD

  ; store the function pointers object first 
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers

  ; initialize the pivot iterator
  MOV EDI, EBP
  SUB EDI, 12

  MOV EAX, 1
  PUSH EAX
  PUSH DWORD PTR [EBP+12]
  MOV EDX, (Iterator_Functions PTR [ECX])._prev
  CALL EDX

  ; initialize the i-th iterator
  MOV EDI, EBP
  SUB EDI, 24

  PUSH DWORD PTR [EBP+8]
  MOV EDX, (Iterator_Functions PTR [ECX])._copy
  CALL EDX

  ; dereference the pivot iterator (we gonna need it)
  MOV EDI, EBP
  SUB EDI, 52
  SUB EDI, [EBP-52]

  MOV ESI, EBP
  SUB ESI, 12
  PUSH ESI
  MOV EDX, (Iterator_Functions PTR [ECX])._dref
  CALL EDX

  MOV EBX, EDI           ; save the value reference of *p_it

  FOR_LOOP_INIT_1:
    ; initialize the j-th iterator
    MOV EDI, EBP
    SUB EDI, 36

    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._copy
    CALL EDX
  
  FOR_LOOP_1:
    ; initialize the next(j_it, 1) iterator
    MOV EDI, EBP
    SUB EDI, 48
    MOV ESI, EBP
    SUB ESI, 36

    MOV EAX, 1
    PUSH EAX
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._next
    CALL EDX
    
    ; compare next(j_it) and last_it
    PUSH DWORD PTR [EBP+12]
    PUSH EDI
    MOV EDX, (Iterator_Functions PTR [ECX])._comp
    CALL EDX

    CMP EAX, 0
    JZ END_FOR_LOOP_1

    IF_PREDICATE:
      ; now, dereference j-th iterator (we already derefed the p_it)
      MOV EDI, EBP
      SUB EDI, 52
      SUB EDI, [EBP-52]
      SUB EDI, [EBP-52]

      MOV ESI, EBP
      SUB ESI, 36
      PUSH ESI
      MOV EDX, (Iterator_Functions PTR [ECX])._dref
      CALL EDX

      ; now, compare the values using the predicate (comparator)
      PUSH EBX
      PUSH EDI
      MOV EDX, [EBP+16]
      CALL EDX

      CMP EAX, 0
      JZ END_IF_PREDICATE

      ; if pred(*j_it, *p_it), iter_swap(i, j)
      MOV ESI, EBP
      SUB ESI, 24
      PUSH ESI

      SUB ESI, 12
      PUSH ESI

      MOV EDX, (Iterator_Functions PTR [ECX])._swap
      CALL EDX

      ; increment i_it
      MOV EDI, EBP
      SUB EDI, 24

      MOV EAX, 1
      PUSH EAX
      PUSH EDI
      MOV EDX, (Iterator_Functions PTR [ECX])._next
      CALL EDX
    END_IF_PREDICATE:

    ; increment j_it (in other words, j_it = next(j_it) which we already saved)
    MOV EDI, EBP
    SUB EDI, 36

    MOV ESI, EDI
    SUB ESI, 12
    PUSH ESI
    MOV EDX, (Iterator_Functions PTR [ECX])._copy
    CALL EDX
  
    JMP FOR_LOOP_1
  END_FOR_LOOP_1:

  MOV ESI, EBP
  SUB ESI, 12
  PUSH ESI
  SUB ESI, 12
  PUSH ESI
  MOV EDX, (Iterator_Functions PTR [ECX])._swap
  CALL EDX

  POPAD
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers
  MOV ESI, EBP
  SUB ESI, 24
  PUSH ESI
  MOV EDX, (Iterator_Functions PTR [ECX])._copy
  CALL EDX

  LEAVE
  RET 12
Partition ENDP

; QuickSort function: Performs in-place sorting
QuickSort PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = Comparator function address
  ; Return: None (in-place sorting)

  ; Local Variables: 
  ; 12 Bytes: [EBP-12] = Pivot Iterator

  ; Temporary memory:
  ; 12 Bytes: Temporary iterator variable

  ENTER 24, 0
  PUSHAD

  ; store the function pointers object first hand
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers

  IF_EDGE_CASE:
    PUSH DWORD PTR [EBP+12]
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._comp
    CALL EDX

    CMP EAX, 0
    JZ FUNCTION_RETURN

    MOV EDI, EBP
    SUB EDI, 24

    MOV EAX, 1
    PUSH EAX
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._next
    CALL EDX

    PUSH DWORD PTR [EBP+12]
    PUSH EDI
    MOV EDX, (Iterator_Functions PTR [ECX])._comp
    CALL EDX

    CMP EAX, 0
    JZ FUNCTION_RETURN
  END_IF_EDGE_CASE:

  MOV EDI, EBP
  SUB EDI, 12

  PUSH DWORD PTR [EBP+16]
  PUSH DWORD PTR [EBP+12]
  PUSH DWORD PTR [EBP+8]
  CALL Partition

  PUSH DWORD PTR [EBP+16]
  PUSH EDI
  PUSH DWORD PTR [EBP+8]
  CALL QuickSort

  MOV ESI, EDI
  SUB EDI, 12
  MOV EAX, 1
  PUSH EAX
  PUSH ESI
  MOV EDX, (Iterator_Functions PTR [ECX])._next
  CALL EDX

  PUSH DWORD PTR [EBP+16]
  PUSH DWORD PTR [EBP+12]
  PUSH EDI
  CALL QuickSort

  FUNCTION_RETURN:
    POPAD
    LEAVE
    RET 12
QuickSort ENDP