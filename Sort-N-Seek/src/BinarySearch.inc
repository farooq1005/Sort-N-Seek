; ------------------------------------------------------------------
; Parameters:
; 4 bytes: [EBP+8]  = First iterator address
; 4 bytes: [EBP+12] = Last iterator address
; 4 bytes; [EBP+16] = Target value address
; 4 bytes: [EBP+20] = Comparator function address
; Return: address of iterator in edi matching the target value otherwise last iterator if not found
; 
; Total size of parameters: 16 Bytes
; 
; Local Variables:
; [EBP - 4]: n = distance 
; [EBP - 16]: Mid iterator 
; [EBP - 20]: Value Type (Size in Bytes)
; Onwards: 1 x Dereferenced Value 
;
; Total size of Known Local Varibales: 20 bytes
; ------------------------------------------------------------------

BinarySearch PROC
	enter 20, 0

	; save the value type
	mov ecx, DWORD PTR [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).value_type
	mov DWORD PTR [ebp - 20], ecx

	; allocate 1 x it.value_type bytes in stack
	sub esp, ecx
	PUSHAD

	; store the function pointers object first hand 
	mov ecx, DWORD PTR [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).function_pointers	

    ; calulate the distance 
	PUSH DWORD PTR [ebp + 12]
	PUSH DWORD PTR [ebp + 8]  
	mov ebx, (Iterator_Functions PTR [ecx])._dist ; distance function
	call ebx
	mov DWORD PTR [ebp - 4], eax
	
	_while:
		
		; check condition whether n > 0
		mov eax, DWORD PTR [ebp - 4]
		cmp eax, 0
		JBE _endwhile

		; mid iterator address
		mov edi, ebp
		sub edi, 16
		
		; dividing n / 2
		mov eax, DWORD PTR [ebp - 4]
		shr eax, 1

		; going to the mid of the sub-array
		PUSH eax
		PUSH DWORD PTR [ebp + 8]
		mov ebx, (Iterator_Functions PTR [ecx])._next
		call ebx

		mov edi, ebp
		sub edi, 20
		sub edi, DWORD PTR [ebp - 20] ; location to save the value
		
		; dereferencing the mid iterator
		mov esi, ebp
		sub esi, 16
		PUSH esi ; esi = mid iterator pointer
		mov ebx, (Iterator_Functions PTR [ecx])._dref
		call ebx

		PUSH DWORD PTR [ebp + 16] ; target value address
		PUSH edi ; edi points to the location where the dereferenced value is saved
		mov ebx, DWORD PTR [ebp + 20] ; predicate function address
		call ebx

		cmp eax, 1
		JNE _else

		; first iterator
		mov edi, [ebp + 8]

		; moving the first iterator to mid + 1
		mov esi, ebp
		sub esi, 16
		PUSH 1
		PUSH esi
		mov ebx, (Iterator_Functions PTR [ecx])._next
		call ebx

		; evaluating n -= n / 2 + 1
		mov eax, DWORD PTR [ebp - 4]
		shr eax, 1
		add eax, 1
		sub DWORD PTR [ebp - 4], eax ; saving back in n
		JMP _next

		_else:
			;evaluating n = n / 2
			mov eax, DWORD PTR [ebp - 4]
			shr eax, 1
			mov DWORD PTR [ebp - 4], eax ; saving back in n
			JMP _next

		_next:
			JMP _while

	_endwhile:

	POPAD

	mov ecx, DWORD PTR [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).function_pointers

	push DWORD PTR [ebp + 8]
	mov ebx, (Iterator_Functions PTR [ecx])._copy
	call ebx

	leave
	ret 16
BinarySearch ENDP