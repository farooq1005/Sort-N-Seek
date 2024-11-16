; Heapify function - adjusts the heap from a specific index downwards
heapify PROC
  ; Stack Frame:
  ; 4 bytes: [EBP+8] = First iterator address
  ; 4 bytes: [EBP+12] = Last iterator address
  ; 4 bytes: [EBP+16] = n (size of the container)
  ; 4 bytes: [EBP+20] = i (index of the element being heapified)
  ; 4 bytes: [EBP+24] = Comparator function address
  ; Return: None (in-place modification)

  ; Local Variables:
  ; 4 bytes: [EBP-4] = largest
  ; 4 bytes: [EBP-8] = left
  ; 4 bytes: [EBP-12] = right
  ; 12 bytes: [EBP-24] = left_it
  ; 12 bytes: [EBP-36] = right_it
  ; 12 bytes: [EBP-48] = largest_it

  ENTER 48, 0
  PUSHAD

  MOV EDX, [EBP+20]
  MOV DWORD PTR [EBP-4], EDX

  SHL EDX, 1
  ADD EDX, 1
  MOV DWORD PTR [EBP-8], EDX
  
  ADD EDX, 1
  MOV DWORD PTR [EBP-12], EDX

  MOV EBX, (Iterator PTR [EBP+8]).function_pointers
  
  PUSH DWORD PTR [EBP-4]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 48
  MOV EDX, (Iterator_Functions PTR EBX)._next
  CALL EDX

  PUSH DWORD PTR [EBP-8]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 24
  MOV EDX, (Iterator_Functions PTR EBX)._next
  CALL EDX
  
  PUSH DWORD PTR [EBP-12]
  PUSH DWORD PTR [EBP+8]
  MOV EDI, EBP
  SUB EDI, 36
  MOV EDX, (Iterator_Functions PTR EBX)._next
  CALL EDX

  IF_6:
    MOV EAX, [EBP-4]
    MOV EDX, [EBP+16]
    CMP EAX, EDX
    JGE END_IF_6



  END_IF_6:
  
  LEAVE
  RET 20
heapify ENDP

; Heapsort function - sort the container from first to last
heapsort PROC
  ; Stack Frame:
  ; [EBP+8] = First iterator address
  ; [EBP+12] = Last iterator address
  ; [EBP+16] = Comparator function address
  ; Return: None (in-place sorting)

  ENTER 0, 0
  PUSH ESI
  PUSH EDI

  ; Get n = distance(first, last)
  MOV ESI, [EBP+8]              ; ESI = First iterator address
  MOV EDI, [EBP+12]             ; EDI = Last iterator address
  ; You would call the Distance function here to calculate n

  ; Build max-heap
  ; Start loop from n / 2 - 1 to 0
  ; Loop through elements and call heapify
  MOV ECX, n / 2
  FOR_LOOP:
    CALL heapify
    DEC ECX
    CMP ECX, 0
    JGE FOR_LOOP

  ; Extract elements one by one, starting from n-1 to 1
  MOV ECX, n - 1
  EXTRACT_LOOP:
    ; Swap the root with the last element
    ; Call the swap function

    ; Call heapify on the reduced heap
    CALL heapify

    DEC ECX
    CMP ECX, 1
    JG EXTRACT_LOOP

  ; Return from PROC
  POP EDI
  POP ESI
  LEAVE
  RET 8
heapsort ENDP