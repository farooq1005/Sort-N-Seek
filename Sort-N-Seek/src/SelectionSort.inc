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
; 12 = iterator: 12 bytes 
; 24 = next iterator: 12 bytes 
; 36 = last iterator: 12 bytes
; 48 = second last iterator: 12 bytes
; 60 = min Iterator: 12 bytes
;
; Temporary Storage:
; 64 = 4 bytes - to store it.value_type
; 64 onwards = (it.value_type * 2 bytes - to store dereferenced values
;
; Total known size of Local Varibales: 64 bytes
; ------------------------------------------------------------------

.code

SelectionSort PROC
	enter 64, 0

	; save the value type
	mov ecx, [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).value_type
	mov DWORD PTR [ebp - 64], ecx

	; allocate it.value_type bytes in stack
	MOV EAX, 2
	MOV EDX, 0
	MUL ECX
	SUB ESP, EAX

	PUSHAD
	
	; store the function pointers object first hand
	mov ecx, [ebp + 8]
	mov ecx, (Iterator PTR [ECX]).function_pointers		

	PUSH DWORD PTR [ebp + 8]										
	mov edi, ebp
	sub edi, 12
	mov ebx, (Iterator_Functions PTR [ecx])._copy
	call ebx														; first iterator saved in [ebp - 12]

	LAST_IT:
		PUSH DWORD PTR [ebp + 12]									
		mov edi, ebp
		sub edi, 36
		mov ebx, (Iterator_Functions PTR [ecx])._copy
		call ebx													; last iterator saved in [ebp - 36]

		PUSH 1
		PUSH edi
		mov edi, ebp
		sub edi, 48
		mov ebx, (Iterator_Functions PTR [ecx])._prev
		call ebx													; Second Last Iterator saved in [ebp - 48]
	
	FOR_OUTER:
		mov edi, ebp
		sub edi, 12													; i - iterator
		PUSH edi
		mov edi, ebp
		sub edi, 48													; second last iterator
		PUSH edi	
		mov ebx, (Iterator_Functions PTR [ecx])._comp				; comparing i and second last iterator
		call ebx	

		cmp eax, 0
		JGE _end_FOR_OUTER		
	
		mov esi, ebp
		sub esi, 12
		PUSH esi			
		
		mov edi, ebp
		sub edi, 60
		mov ebx, (Iterator_Functions PTR [ecx])._copy
		call ebx	

		mov edi, ebp
		sub edi, 24
		
		PUSH 1
		mov esi, ebp
		sub esi, 12													; Argument for nextIt 
		PUSH esi
		mov ebx, (Iterator_Functions PTR [ecx])._next
		call ebx													; next iterator saved in [ebp - 24]

			FOR_INNER:
				mov edi, ebp
				sub edi, 24
				PUSH edi
				mov edi, ebp
				sub edi, 36
				PUSH edi		
				mov ebx, (Iterator_Functions PTR [ecx])._comp		; compare iterator
				call ebx

				cmp eax, 0
				JGE _end_FOR_INNER		

				_dref:
					mov edi, ebp
					sub edi, 64
					sub edi, [ebp - 64]

					mov esi, ebp
					sub esi, 60
					PUSH esi
					MOV ebx, (Iterator_Functions PTR [ecx])._dref   ; dref (Func)
					call ebx	
					PUSH edi										; arg for predicate

					sub edi, [ebp - 64]
					mov esi, ebp
					sub esi, 24
					PUSH esi
					MOV ebx, (Iterator_Functions PTR [ecx])._dref		; Deref (Func)
					call ebx
					PUSH edi

				mov ebx, DWORD PTR [ebp + 16]						; predicate: a[j] < a[minIndex]
				call ebx
				
				cmp eax, 0
				JE _INC_IT_J
				mov esi, ebp
				sub esi, 24
				PUSH esi
				mov edi, ebp
				sub edi, 60
				mov ebx, (Iterator_Functions PTR [ecx])._copy
				call ebx

					_INC_IT_J:
						mov edi, ebp
						sub edi, 24

						PUSH 1
						PUSH edi
						mov ebx, (Iterator_Functions PTR [ecx])._next
						call ebx

				JMP FOR_INNER
			_end_FOR_INNER:

		_Swap:
			mov edi, ebp
			sub edi, 60
			PUSH edi
			mov edi, ebp
			sub edi, 12
			PUSH edi
			mov ebx, (Iterator_Functions PTR [ecx])._swap	 
			call ebx                                         

		_INC_IT_I:
			mov edi, ebp
			sub edi, 12

			PUSH 1
			PUSH edi
			mov ebx, (Iterator_Functions PTR [ecx])._next
			call ebx

		JMP FOR_OUTER
	_end_FOR_OUTER:

	POPAD
	leave
	ret 12
SelectionSort ENDP