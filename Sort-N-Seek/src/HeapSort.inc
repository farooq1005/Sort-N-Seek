; Heapify function - adjusts the heap from a specific index downwards

.CODE

heapify PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = n (size of the container)
  ; 4 bytes: [EBP+20] = i (index of the element being heapified)
  ; 4 bytes: [EBP+24] = Comparator function address (takes reference to the objects)
  ; Return: None (in-place modification)

  ; Local Variables:
  ; 4 bytes: [EBP-4] = largest
  ; 4 bytes: [EBP-8] = left
  ; 4 bytes: [EBP-12] = right
  ; 12 bytes: [EBP-24] = left_it
  ; 12 bytes: [EBP-36] = right_it
  ; 12 bytes: [EBP-48] = largest_it
  
  ; Temporary Storage:
  ; 4 bytes - to store it.value_type
  ; (it.value_type * 2) bytes - to store dereferenced values

  ENTER 52, 0

  ; save the value type
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).value_type
  MOV DWORD PTR [EBP - 52], ECX

  ; allocate it.value_type * 3 bytes in stack
  MOV EAX, 2
  MOV EDX, 0
  MUL ECX
  SUB ESP, EAX

  ; save the registers after allocating enough space for the function
  PUSHAD

  ; store the function pointers object first hand
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers

  MOV EDX, [EBP+20]                                            ; get the current index
  MOV DWORD PTR [EBP-4], EDX                                   ; largest = i

  SHL EDX, 1                                                   ; i * 2
  ADD EDX, 1                                                   ; left = i * 2 + 1
  MOV DWORD PTR [EBP-8], EDX
  
  ADD EDX, 1                                                   ; right = i * 2 + 2
  MOV DWORD PTR [EBP-12], EDX
  
  ; saving iterators for largest, left, and right
  PUSH DWORD PTR [EBP-4]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 48
  MOV EDX, (Iterator_Functions PTR [ECX])._next
  CALL EDX

  PUSH DWORD PTR [EBP-8]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 24
  MOV EDX, (Iterator_Functions PTR [ECX])._next
  CALL EDX
  
  PUSH DWORD PTR [EBP-12]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 36
  MOV EDX, (Iterator_Functions PTR [ECX])._next
  CALL EDX

  ; temporarily save the address of largest iterator
  MOV EBX, EBP
  SUB EBX, 48

  IF_LEFT:                                                    ; if comp(*largest, *left) is true
    ; first condition, left < n (do not do anything if it is false)
    MOV EAX, [EBP-8]
    CMP EAX, DWORD PTR [EBP+16]
    JGE END_LEFT    
    
    ; adjusting pointer to copy dereferenced value
    MOV EDI, EBP
    SUB EDI, 52
    SUB EDI, [EBP-52]
    SUB EDI, [EBP-52]                                         ; EDI = EBP - 52 - (value_type*2)
    
    MOV ESI, EBP
    SUB ESI, 24
    PUSH ESI                                                  ; push left_it
    MOV EDX, (Iterator_Functions PTR [ECX])._dref
    CALL EDX

    PUSH EDI                                                  ; save the value of EDI

    ; adjusting pointer to copy dereferenced value
    MOV EDI, EBP
    SUB EDI, 52
    SUB EDI, [EBP-52]                                         ; EDI = EBP - 52 - value_type
    
    PUSH EBX                                                  ; push largest_it
    MOV EDX, (Iterator_Functions PTR [ECX])._dref
    CALL EDX

    PUSH EDI

    MOV EDX, [EBP+24]
    CALL EDX                                                  ; call predicate(*largest_it, *left_it)

    CMP EAX, 0
    JE END_LEFT                                               ; if predicate is false, do not do anything

    ; if everything is true, modify the largest index and pointed largest iterator
    MOV EBX, EBP
    SUB EBX, 24
    MOV EAX, [EBP-8] 
    MOV DWORD PTR [EBP-4], EAX                  
  END_LEFT:

  IF_RIGTH:
    ; first condition, right < n (do not do anything if it is false)
    MOV EAX, [EBP-12]
    CMP EAX, DWORD PTR [EBP+16]
    JGE END_RIGHT    
    
    ; adjusting pointer to copy dereferenced value
    MOV EDI, EBP
    SUB EDI, 52
    SUB EDI, [EBP-52]
    SUB EDI, [EBP-52]                                         ; EDI = EBP - 52 - (value_type*2)
    
    MOV ESI, EBP
    SUB ESI, 36
    PUSH ESI                                                  ; push right_it
    MOV EDX, (Iterator_Functions PTR [ECX])._dref
    CALL EDX

    PUSH EDI                                                  ; save the value of EDI

    ; adjusting pointer to copy dereferenced value
    MOV EDI, EBP
    SUB EDI, 52
    SUB EDI, [EBP-52]                                         ; EDI = EBP - 52 - value_type
    
    PUSH EBX                                                  ; push largest_it
    MOV EDX, (Iterator_Functions PTR [ECX])._dref
    CALL EDX

    PUSH EDI

    MOV EDX, [EBP+24]
    CALL EDX                                                  ; call predicate(*largest_it, *left_it)

    CMP EAX, 0
    JE END_RIGHT                                               ; if predicate is false, do not do anything

    ; if everything is true, modify the largest index and pointed largest iterator
    MOV EBX, EBP
    SUB EBX, 36
    MOV EAX, [EBP-12]
    MOV DWORD PTR [EBP-4], EAX        
  END_RIGHT:

  HEAPIFY_CONDITION:
    MOV EAX, [EBP-4]
    MOV EDX, [EBP+20]
    CMP EAX, EDX

    JE END_HEAPIFY

    PUSH EBX                                                  ; Push modified largest
    MOV EBX, EBP
    SUB EBX, 48
    PUSH EBX                                                  ; Push original largest (first + i)
    
    MOV EDX, (Iterator_Functions PTR [ECX])._swap
    CALL EDX

    PUSH DWORD PTR [EBP+24]
    PUSH EAX
    PUSH DWORD PTR [EBP+16]
    PUSH DWORD PTR [EBP+12]
    PUSH DWORD PTR [EBP+8]

    CALL heapify

  END_HEAPIFY:
  
  LEAVE
  RET 20
heapify ENDP

; Heapsort function - sort the container from first to last
HeapSort PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = Comparator function address
  ; Return: None (in-place sorting)

  ; Local variables:
  ; 4 bytes: [EBP-4] = n
  ; 4 bytes: [EBP-8] = i

  ; Temporary memory:
  ; 12 bytes - to store the return value of next(it, m)

  ENTER 20, 0
  PUSHAD

  ; store the function pointers object first hand
  MOV ECX, [EBP+8]
  MOV ECX, (Iterator PTR [ECX]).function_pointers

  ; calculate n = distance(first, last)
  PUSH DWORD PTR [EBP+12]
  PUSH DWORD PTR [EBP+8]
  MOV EDX, (Iterator_Functions PTR [ECX])._dist
  CALL EDX

  MOV DWORD PTR [EBP-4], EAX
  
  PRE_LOOP_10:
    ; initialize i = n / 2 - 1 (division is floor which is the default behaviour of ASM div)
    MOV EAX, [EBP-4]
    CDQ
    MOV EBX, 2
    DIV EBX
    SUB EAX, 1

    MOV DWORD PTR [EBP-8], EAX

  HEAPIFY_LOOP:
    ; Loop condition: 
    ; i >= 0 (signed comparison)
    CMP DWORD PTR [EBP-8], 0
    JL END_HEAPIFY_LOOP

    PUSH DWORD PTR [EBP+16]
    PUSH DWORD PTR [EBP-8]
    PUSH DWORD PTR [EBP-4]
    PUSH DWORD PTR [EBP+12]
    PUSH DWORD PTR [EBP+8]
    CALL heapify

    SUB DWORD PTR [EBP-8], 1
    JMP HEAPIFY_LOOP
  END_HEAPIFY_LOOP:
    
  PRE_LOOP_11:
    MOV EAX, [EBP-4]
    SUB EAX, 1

    MOV DWORD PTR [EBP-8], EAX

  EXTRACTION_LOOP:
    ; Loop condition: 
    ; i > 0 (signed comparison)
    CMP DWORD PTR [EBP-8], 0
    JLE END_EXTRACTION_LOOP

    ; adjust the pointer for return value
    MOV EDI, EBP
    SUB EDI, 20

    ; call next(first, i)
    PUSH DWORD PTR [EBP-8]
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._next
    CALL EDX

    ; call swap(first, next(first, i))
    PUSH EDI
    PUSH DWORD PTR [EBP+8]
    MOV EDX, (Iterator_Functions PTR [ECX])._swap
    CALL EDX

    ; call heapify(first, last, i, 0, predicate)
    MOV EAX, 0
    PUSH DWORD PTR [EBP+16]
    PUSH EAX
    PUSH DWORD PTR [EBP-8]
    PUSH DWORD PTR [EBP+12]
    PUSH DWORD PTR [EBP+8]
    CALL heapify

    SUB DWORD PTR [EBP-8], 1
    JMP EXTRACTION_LOOP
  END_EXTRACTION_LOOP:

  POPAD
  LEAVE
  RET 12
HeapSort ENDP