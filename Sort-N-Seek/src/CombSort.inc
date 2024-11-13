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
; 8 = Previous Gap: 4 byte unsigned integer
;
; Gap Sequence = gap / 1.3
; ------------------------------------------------------------------

.code
GapCalculator PROC
	enter 0, 0
	PUSH edx
	PUSH ebx
	
	mov eax, [ebp + 8]
	mov ebx, 10
	MUL ebx

	mov edx, 0
	mov ebx, 13
	DIV ebx

	cmp eax, 1
	JGE _end
	mov eax, 1

	_end:

	POP ebx
	POP edx
	leave
	ret 4
GapCalculator ENDP

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
; 4 = Sorted flag: 4 bytes 
; 8 = Address of an iterator: 4 bytes (used in for loop)
; 12 = Address of check iterator: 4 bytes (used in for loop)
; 16 = gap variable: 4 bytes
; 20 = container size: 4 bytes
; 24 = Address of next gapped iterator: 4 bytes
;
; Total size of Local Varibales: 24 bytes
; ------------------------------------------------------------------

CombSort PROC
	enter 24, 0
	PUSHAD

	PUSH DWORD PTR [ebp + 12]
	PUSH DWORD PTR [ebp + 8]  
	mov ebx, DWORD PTR [ebp + 44] ; distance function
	call ebx

	mov DWORD PTR [ebp - 20], eax ; store the distance
	mov DWORD PTR [ebp - 16], eax ; gap = size of container
	mov DWORD PTR [ebp - 4], 1	  ; swap flag

	_while_outer:
		
		_check_loop_outer_condition:

			cmp DWORD PTR [ebp - 4], 1	; check if no swapping has been done
			JE _end_check_outer
			
			cmp DWORD PTR [ebp - 16], 1	; check if gap is not equal to 1
			JNE _end_check_outer
			
			JMP _end_while_outer ; end loop if both conditions are false

		_end_check_outer:
	
		_set_gap_and_flag:
			PUSH DWORD PTR [ebp - 16]
			call GapCalculator
			mov DWORD PTR [ebp - 16], eax
			
			mov DWORD PTR [ebp - 4], 0

		mov eax, DWORD PTR [ebp + 8]
		mov DWORD PTR [ebp - 8], eax ; set the it to first element

		; going to the previous gapped element from size of container (n - gap)
		mov eax, DWORD PTR [ebp + 12]
		PUSH DWORD PTR [ebp - 16]    
		PUSH eax
		mov ebx, DWORD PTR [ebp + 40] ; prev function
		call ebx
		mov DWORD PTR [ebp - 12], eax ; set the check iterator

		_while_inner:
			
			_check_loop_inner_condition:
				
				; check if it has exceeded the final gap
				PUSH DWORD PTR [ebp - 8]
				PUSH DWORD PTR [ebp - 12] ; check iterator
				mov ebx, DWORD PTR [ebp + 24] ; compare function
				call ebx 
				cmp al, 0
				JGE _end_while_inner
				
			_end_check_inner:

			; calculate a[i + gap]
			PUSH DWORD PTR [ebp - 16]
			PUSH DWORD PTR [ebp - 8]
			mov ebx, DWORD PTR [ebp + 36]
			call ebx
			mov DWORD PTR [ebp - 24], eax

			PUSH DWORD PTR [ebp - 8]
			mov ebx, DWORD PTR [ebp + 16] ; dref
			call ebx
			PUSH eax

			PUSH DWORD PTR [ebp - 24]
			mov ebx, DWORD PTR [ebp + 16] ; dref
			call ebx
			PUSH eax

			mov ebx, DWORD PTR [ebp + 32]
			call ebx
			
			cmp al, 0
			JE _increment_iterator

			_swap:
				PUSH DWORD PTR [ebp - 8]
				PUSH DWORD PTR [ebp - 24]
				mov ebx, DWORD PTR [ebp + 28]
				call ebx
				mov DWORD PTR [ebp - 4], 1

			_increment_iterator:
				PUSH DWORD PTR [ebp - 16]
				PUSH DWORD PTR [ebp - 8]
				mov ebx, DWORD PTR [ebp + 36]
				call ebx
				mov DWORD PTR [ebp - 8], eax

			JMP _while_inner
		_end_while_inner:

		JMP _while_outer
	_end_while_outer:

	POPAD
	leave
	ret 40
CombSort ENDP