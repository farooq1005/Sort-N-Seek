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
; 4 = First Iterator: 4 bytes (object) address
; 8 = Last Iterator: 4 bytes (object) address 
; 12 = Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; 16 = Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; 20 = Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; 24 = Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; 26 = Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; 30 = Next: 4 bytes (procedure with two operand: iterator and integer) (return address of an iterator in EAX) address
; 34 = Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; 38 = Distance: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) 
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

.code

SelectionSort PROC
	enter 9, 0
	PUSHAD
	
	mov ebx, DWORD PTR [ebp + 4]			; First Iterator Function
	call ebx				
	mov DWORD PTR [ebp - 5], eax			

	LAST_IT:
		mov ebx, DWORD PTR [ebp + 8]		; Last Iterator
		call ebx
		mov edi, eax						; edi = Last Iterator

		PUSH eax
		mov ebx, DWORD PTR [ebp + 34]
		call ebx
		mov edx, eax						; edx = Second Last Iterator

	FOR_OUTER:
		PUSH edx		
		PUSH DWORD PTR [ebp - 5]
		mov ebx, DWORD PTR [ebp + 20]
		call ebx

		cmp al, 0FFh
		JNE _end_FOR_OUTER		

		mov esi, eax						; esi = minIndex
		
		mov ebx, DWORD PTR [ebp + 30]
		PUSH DWORD PTR [ebp - 5]		    ; Argument for nextIt 
		call ebx							
		mov DWORD PTR [ebp - 9], eax		; set nextIt = i + 1

			FOR_INNER:
				PUSH edi		
				PUSH DWORD PTR [ebp - 9]
				mov ebx, DWORD PTR [ebp + 20]
				call ebx

				cmp al, 0FFh
				JNE _end_FOR_INNER		

				PUSH esi
				PUSH DWORD PTR [ebp - 9]	
				mov ebx, DWORD PTR [ebp + 26]	; compare a[j] < minIndex
				call ebx
				
				cmp al, 0FFh
				JNE _INC_IT_J
				mov esi, DWORD PTR [ebp - 9]

				_INC_IT_J:
					PUSH 1
					PUSH DWORD PTR [ebp - 9]
					mov ebx, DWORD PTR [ebp + 30]
					call ebx
					mov DWORD PTR [ebp - 9], eax

				JMP FOR_INNER
			_end_FOR_INNER:

		_Swap:
			PUSH DWORD PTR [ebp - 5]                        ; it (Local)
			PUSH DWORD PTR [ebp - 9]                        ; nx (Local)
			mov ebx, [ebp + 28]                             
			call ebx                                        ; iter_swap(nx, it)

		_INC_IT_I:
			PUSH 1
			PUSH DWORD PTR [ebp - 5]
			mov ebx, DWORD PTR [ebp + 34]
			call ebx
			mov DWORD PTR [ebp - 5], eax

		JMP FOR_OUTER
	_end_FOR_OUTER:

	POPAD
	leave
	ret 40
SelectionSort ENDP