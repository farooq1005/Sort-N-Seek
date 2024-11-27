.CODE
Merge PROC 
	; Stack Frame:
	; 4 Bytes: [EBP+8] = First iterator address
	; 4 Bytes: [EBP+12] = Mid iterator address
	; 4 Bytes: [EBP+16] = Last iterator address
	; 4 Bytes: [EBP+20] = Predicate address

	; Local Variables
	; 12 Bytes: [EBP-12] = i-th iterator
	; 12 Bytes: [EBP-24] = j-th iterator
	; 4 Bytes: [EBP-28] = value type

	; Temporary memory:
	; 2*value_type: To dereference iterators
	; distance(first, last)*value_type: auxiliary range to store value pointers

	ENTER 28, 0
	
	; first, let's calculate the temporary memory required for this function
	MOV ECX, [EBP+8]
	MOV ECX, (Iterator PTR [ECX]).value_type
	MOV DWORD PTR [EBP-28], ECX

	MOV EAX, 2
	MOV EDX, 0
	MUL ECX 

	SUB ESP, EAX							; allocate space for 2*value_type 

	; save the function pointers object in ECX first
	MOV ECX, [EBP+8]
	MOV ECX, (Iterator PTR [ECX]).function_pointers

	; now, allocating memory for distance(first, last)
	PUSH DWORD PTR [EBP+16]
	PUSH DWORD PTR [EBP+8]
	MOV EDX, (Iterator_Functions PTR [ECX])._dist
	CALL EDX

	MOV EBX, [EBP-28]
	MOV EDX, 0
	MUL EBX

	SUB ESP, EAX							; allocate space for distance(first, last) * value_type

	; Now, we are good to proceed

	; Initialize i-th iterator
	MOV EDI, EBP
	SUB EDI, 12
	PUSH DWORD PTR [EBP+8]
	MOV EDX, (Iterator_Functions PTR [ECX])._copy
	CALL EDX

	; Initialize j-th iterator
	MOV EDI, EBP
	SUB EDI, 24
	PUSH DWORD PTR [EBP+12]
	MOV EDX, (Iterator_Functions PTR [ECX])._copy
	CALL EDX

	; EBX will be pointing to the starting address of AUX range
	MOV EBX, EBP
	SUB EBX, 28
	SUB EBX, [EBP-28]
	SUB EBX, [EBP-28]
	SUB EBX, [EBP-28]

	WHILE_LOOP_1:
		; Loop Condition: i_it != mid_it && j_it != last_it
		MOV EDI, EBP
		SUB EDI, 12
		PUSH DWORD PTR [EBP+12]
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._comp
		CALL EDX

		CMP EAX, 0
		JZ END_WHILE_LOOP_1

		MOV EDI, EBP
		SUB EDI, 24
		PUSH DWORD PTR [EBP+16]
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._comp
		CALL EDX
		
		CMP EAX, 0
		JZ END_WHILE_LOOP_1

		IF_PREDICATE:
			MOV EDI, EBP
			SUB EDI, 28
			SUB EDI, [EBP-28]
			SUB EDI, [EBP-28]

			MOV ESI, EBP
			SUB ESI, 12
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._dref
			CALL EDX

			PUSH EDI

			MOV EDI, EBP
			SUB EDI, 28
			SUB EDI, [EBP-28]

			MOV ESI, EBP
			SUB ESI, 24
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._dref
			CALL EDX

			PUSH EDI

			MOV EDX, [EBP+20]
			CALL EDX

			CMP EAX, 0
			JZ ELSE_IF_PREDICATE

			MOV ESI, EBP
			SUB ESI, 24
			MOV EDI, EBX
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._dref
			CALL EDX

			MOV EDI, ESI
			MOV EAX, 1
			PUSH EAX
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._next
			CALL EDX

			JMP END_IF_PREDICATE

		ELSE_IF_PREDICATE:
			MOV ESI, EBP
			SUB ESI, 12
			MOV EDI, EBX
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._dref
			CALL EDX
			
			MOV EDI, ESI
			MOV EAX, 1
			PUSH EAX
			PUSH ESI
			MOV EDX, (Iterator_Functions PTR [ECX])._next
			CALL EDX

		END_IF_PREDICATE:
			SUB EBX, [EBP-28]

		JMP WHILE_LOOP_1

	END_WHILE_LOOP_1:

	WHILE_LOOP_2:
		; loop condition: i_it != mid
		MOV EDI, EBP
		SUB EDI, 12
		PUSH DWORD PTR [EBP+12]
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._comp
		CALL EDX

		CMP EAX, 0
		JZ END_WHILE_LOOP_2

		MOV ESI, EBP
		SUB ESI, 12
		MOV EDI, EBX
		PUSH ESI
		MOV EDX, (Iterator_Functions PTR [ECX])._dref
		CALL EDX

		MOV EDI, ESI
		MOV EAX, 1
		PUSH EAX
		PUSH ESI
		MOV EDX, (Iterator_Functions PTR [ECX])._next
		CALL EDX

		SUB EBX, [EBP-28]
		JMP WHILE_LOOP_2

	END_WHILE_LOOP_2:

	WHILE_LOOP_3:
		; loop condition: j_it != last
		MOV EDI, EBP
		SUB EDI, 24
		PUSH DWORD PTR [EBP+16]
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._comp
		CALL EDX

		CMP EAX, 0
		JZ END_WHILE_LOOP_3

		MOV ESI, EBP
		SUB ESI, 24
		MOV EDI, EBX
		PUSH ESI
		MOV EDX, (Iterator_Functions PTR [ECX])._dref
		CALL EDX

		MOV EDI, ESI
		MOV EAX, 1
		PUSH EAX
		PUSH ESI
		MOV EDX, (Iterator_Functions PTR [ECX])._next
		CALL EDX

		SUB EBX, [EBP-28]
		JMP WHILE_LOOP_3

	END_WHILE_LOOP_3:

	; now, time for copy back the values to the original range
	; for that, recalculate the address of AUX range in EBX
	MOV EBX, EBP
	SUB EBX, 28
	SUB EBX, [EBP-28]
	SUB EBX, [EBP-28]
	SUB EBX, [EBP-28]

	; Re-initialize i-th iterator (we are going to use this)
	MOV EDI, EBP
	SUB EDI, 12
	PUSH DWORD PTR [EBP+8]
	MOV EDX, (Iterator_Functions PTR [ECX])._copy
	CALL EDX

	COPY_WHILE_LOOP_4:
		; Loop condition: i_it != last_it
		MOV EDI, EBP
		SUB EDI, 12
		PUSH DWORD PTR [EBP+16]
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._comp
		CALL EDX

		CMP EAX, 0
		JZ END_COPY_WHILE_LOOP_4

		PUSH EBX
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._asig
		CALL EDX

		MOV EAX, 1
		PUSH EAX
		PUSH EDI
		MOV EDX, (Iterator_Functions PTR [ECX])._next
		CALL EDX

		SUB EBX, [EBP-28]
		JMP COPY_WHILE_LOOP_4
	END_COPY_WHILE_LOOP_4:

	LEAVE
	RET 16
Merge ENDP

; Merge Sort function

MergeSort PROC
	; Stack frame: 
	; 4 Bytes: [EBP+8] = First Iterator
	; 4 Bytes: [EBP+12] = Last Iterator
	; 4 Bytes: [EBP+16] = Comparator
	; Return: None

	; Local Variables
	; 12 Bytes: [EBP-12] = Mid Iterator

  ENTER 12, 0
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
    SUB EDI, 12

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

	; calculate the distance between first and last
	PUSH DWORD PTR [EBP+12]
	PUSH DWORD PTR [EBP+8]
	MOV EDX, (Iterator_Functions PTR [ECX])._dist
	CALL EDX

	MOV EBX, 2
	CDQ 
	IDIV EBX

	; now, set the EDI to mid iterator
	MOV EDI, EBP
  SUB EDI, 12

	; set the mid iterator to the mid of the range
	PUSH EAX
	PUSH DWORD PTR [EBP+8]
	MOV EDX, (Iterator_Functions PTR [ECX])._next
	CALL EDX

	PUSH DWORD PTR [EBP+16]
	PUSH EDI
	PUSH DWORD PTR [EBP+8]
	CALL MergeSort 

	PUSH DWORD PTR [EBP+16]
	PUSH DWORD PTR [EBP+12]
	PUSH EDI
	CALL MergeSort
	
	PUSH DWORD PTR [EBP+16]
	PUSH DWORD PTR [EBP+12]
	PUSH EDI
	PUSH DWORD PTR [EBP+8]
	CALL Merge
	
	FUNCTION_RETURN:
	POPAD
	LEAVE
	RET 12
MergeSort ENDP