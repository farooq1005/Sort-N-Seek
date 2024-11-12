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
; 4 = Address of an iterator: 4 bytes (used in for loop)
; 8 = Address of next iterator: 4 bytes (used in for loop)
; 12 = Address of last iterator: 4 bytes
; 16 = Address of second last iterator: 4 bytes
; 20 = Address of min Iterator
;
; Total size of Local Varibales: 20 bytes
; ------------------------------------------------------------------

.code

SelectionSort PROC
	enter 20, 0
	PUSHAD
	
	mov ebx, DWORD PTR [ebp + 8]							; First Iterator Function			
	mov DWORD PTR [ebp - 4], ebx			

	LAST_IT:
		mov edx, DWORD PTR [ebp + 12]						; Last Iterator
		mov DWORD PTR [ebp - 12], edx		

		PUSH 1
		PUSH DWORD PTR [ebp - 12]
		mov ebx, DWORD PTR [ebp + 40]
		call ebx
		mov DWORD PTR [ebp - 16], eax						; Second Last Iterator
	
	FOR_OUTER:
		PUSH DWORD PTR [ebp - 4]
		PUSH DWORD PTR [ebp - 16]		
		mov ebx, DWORD PTR [ebp + 24]
		call ebx

		cmp al, 0
		JGE _end_FOR_OUTER		
	
		mov edx, DWORD PTR [ebp - 4]
		mov DWORD PTR [ebp - 20], edx
		
		PUSH 1
		PUSH DWORD PTR [ebp - 4]							; Argument for nextIt 
		mov ebx, DWORD PTR [ebp + 36]
		call ebx							
		mov DWORD PTR [ebp - 8], eax						; set nextIt = i + 1

			FOR_INNER:
				PUSH DWORD PTR [ebp - 8]
				PUSH DWORD PTR [ebp - 12]		
				mov ebx, DWORD PTR [ebp + 24]				; compare iterator
				call ebx

				cmp al, 0
				JGE _end_FOR_INNER		

				_dref:
					PUSH DWORD PTR [ebp - 20]
					MOV ebx, [ebp + 16]                     ; dref (Func)
					call ebx	
					PUSH eax								; arg for dref

					PUSH DWORD PTR [ebp - 8]
					MOV ebx, [ebp + 16]                     ; Deref (Func)
					call ebx
					PUSH eax								; arg for dref

				mov ebx, DWORD PTR [ebp + 32]				; predicate: a[j] < a[minIndex]
				call ebx
				
				cmp al, 0
				JE _INC_IT_J
				mov edx, DWORD PTR [ebp - 8]
				mov DWORD PTR [ebp - 20], edx

				_INC_IT_J:
					PUSH 1
					PUSH DWORD PTR [ebp - 8]
					mov ebx, DWORD PTR [ebp + 36]
					call ebx
					mov DWORD PTR [ebp - 8], eax

				JMP FOR_INNER
			_end_FOR_INNER:

		_Swap:
			PUSH DWORD PTR [ebp - 20]                        ; it (Local)
			PUSH DWORD PTR [ebp - 4]                         ; nx (Local)
			mov ebx, [ebp + 28]								 ; iter_swap(nx, it)
			call ebx                                         

		_INC_IT_I:
			PUSH 1
			PUSH DWORD PTR [ebp - 4]
			mov ebx, DWORD PTR [ebp + 36]
			call ebx
			mov DWORD PTR [ebp - 4], eax

		JMP FOR_OUTER
	_end_FOR_OUTER:

	POPAD
	leave
	ret 40
SelectionSort ENDP