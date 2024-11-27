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
; 4 bytes: [EBP+8]  = First iterator address
; 4 bytes: [EBP+12] = Last iterator address
; 4 bytes: [EBP+16] = Predicate function address
; Return: None (in-place sorting) 
; 
; Total size of parameters: 12 Bytes
; 
; Local Variables:
; 4 = Sorted flag: 4 bytes 
; 16 = iterator: 12 bytes (used in for loop)
; 28 = check iterator: 12 bytes (used in for loop)
; 32 = gap variable: 4 bytes
; 36 = container size: 4 bytes
; 48 = next gapped iterator: 12 bytes
;
; 52 = it.value_type: 4 bytes
; Onwards 2 dereferenced value
;
; Total size of Local Varibales: 48 bytes
; ------------------------------------------------------------------

CombSort PROC
	enter 48, 0

	; save the value type
	MOV ecx, [ebp + 8]
	MOV ecx, (Iterator PTR [ecx]).value_type
	MOV DWORD PTR [ebp - 52], ecx

	; allocate 2 x it.value_type bytes in stack
	MOV eax, 2
	MOV edx, 0
	MUL ecx
	SUB esp, eax
	
	PUSHAD

	; store the function pointers object first hand 
	mov ecx, [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).function_pointers	

	PUSH DWORD PTR [ebp + 12]
	PUSH DWORD PTR [ebp + 8]  
	mov ebx, (Iterator_Functions PTR [ecx])._dist ; distance function
	call ebx

	mov DWORD PTR [ebp - 36], eax ; store the distance
	mov DWORD PTR [ebp - 32], eax ; gap = size of container
	mov DWORD PTR [ebp - 4], 1	  ; swap flag

	_while_outer:
		
		_check_loop_outer_condition:

			cmp DWORD PTR [ebp - 4], 1	; check if no swapping has been done
			JE _end_check_outer
			
			cmp DWORD PTR [ebp - 32], 1	; check if gap is not equal to 1
			JNE _end_check_outer
			
			JMP _end_while_outer ; end loop if both conditions are false

		_end_check_outer:
	
		_set_gap_and_flag:
			PUSH DWORD PTR [ebp - 32]
			call GapCalculator
			mov DWORD PTR [ebp - 32], eax
			
			mov DWORD PTR [ebp - 4], 0

		; initializing iterator to first element
		mov edi, ebp
		sub edi, 16
		PUSH DWORD PTR [ebp + 8]
		mov ebx, (Iterator_Functions PTR [ecx])._copy ; set the it to first element
		call ebx
		

		; going to the previous gapped element from size of container (n - gap)
		mov edi, ebp
		sub edi, 28
		PUSH DWORD PTR [ebp - 32]    
		PUSH DWORD PTR [ebp + 12]
		mov ebx, (Iterator_Functions PTR [ecx])._prev ; prev function
		call ebx									  ; set the check iterator

		_while_inner:
			
			_check_loop_inner_condition:
				
				; check if iterator has exceeded the final gap
				mov edi, ebp
				sub edi, 16
				mov esi, ebp
				sub esi, 28
				PUSH edi
				PUSH esi ; check iterator
				mov ebx,(Iterator_Functions PTR [ecx])._comp ; compare function
				call ebx 
				cmp eax, 0
				JGE _end_while_inner
				
			_end_check_inner:

			; calculate a[i + gap]
			mov edi, ebp
			sub edi, 48
			mov esi, ebp
			sub esi, 16
			PUSH DWORD PTR [ebp - 32]
			PUSH esi
			mov ebx, (Iterator_Functions PTR [ecx])._next
			call ebx

			mov edi, ebp
			sub edi, 52
			sub edi, DWORD PTR [ebp - 52]

			mov esi, ebp
			sub esi, 16
			PUSH esi
			mov ebx, (Iterator_Functions PTR [ecx])._dref 
			call ebx
			PUSH edi

			mov edi, ebp
			sub edi, 52
			sub edi, DWORD PTR [ebp - 52]
			sub edi, DWORD PTR [ebp - 52]

			mov esi, ebp
			sub esi, 48
			PUSH esi
			mov ebx, (Iterator_Functions PTR [ecx])._dref  ; dref
			call ebx
			PUSH edi

			mov ebx, DWORD PTR [ebp + 16]
			call ebx
			
			cmp eax, 0
			JE _increment_iterator

			_swap:
				mov esi, ebp
				sub esi, 16
				mov edi, ebp
				sub edi, 48
				PUSH esi
				PUSH edi
				mov ebx, (Iterator_Functions PTR [ecx])._swap
				call ebx
				mov DWORD PTR [ebp - 4], 1

			_increment_iterator:
				mov edi, ebp
				sub edi, 16
				PUSH DWORD PTR [ebp - 32]
				PUSH edi
				mov ebx, (Iterator_Functions PTR [ecx])._next
				call ebx

			JMP _while_inner
		_end_while_inner:

		JMP _while_outer
	_end_while_outer:

	POPAD
	leave
	ret 12
CombSort ENDP