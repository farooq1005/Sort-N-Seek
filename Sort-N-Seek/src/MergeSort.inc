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
; Parameters (Merge):
; 4 bytes: [EBP+8] = First iterator address
; 4 bytes: [EBP+12] = Last iterator address
; 4 bytes: [EBP+16] = n (size of the container)
; 4 bytes: [EBP+20] = i (index of the element being heapified)
; 4 bytes: [EBP+24] = Comparator function address (takes reference to the objects)
; Return: None (in-place modification)
;
; Local Variables:
; 4 = iterator #1 (i)
; 8 = iterator #2 (j)
; 12 = index
; 16 = value
; 20 = value
; 24 = extra storage
;
; Total size of Local Variables: 20 bytes
; ------------------------------------------------------------------

.code

Merge PROC
	enter 12, 0
	PUSHAD
	
	; initialize iterator #1 (i)
	mov edx, DWORD PTR [ebp + 8]
	mov DWORD PTR [ebp - 4], edx

	; initialize iterator #2 (j)
	mov edx, DWORD PTR [ebp + 12]
	PUSH 1   ; Number of elements to skip next to
	PUSH edx ; Starting Address
	mov ebx, DWORD PTR [ebp + 40] ; Next Function Addresss
	call ebx
	mov DWORD PTR [ebp - 8], eax ; mov the return address in iterator #2

	_while_outer:
		
		_check_loop_condition:

			PUSH DWORD PTR [ebp - 4]		; Iterator #1 (i)
			PUSH DWORD PTR [ebp + 12]		; Mid Iterator
			mov ebx, DWORD PTR [ebp + 28]   ; Compare Iterator Function Address
			call ebx

			cmp al, 0
			JG _endwhile_outer

			PUSH DWORD PTR [ebp - 8]		; Iterator #2 (j)
			PUSH DWORD PTR [ebp + 16]		; Last Iterator
			mov ebx, DWORD PTR [ebp + 28]   ; Compare Iterator Function Address
			call ebx		
		
			cmp al, 0
			JG _endwhile_outer

		_end_check:

		_compare_iterators_if:

			_dref:
				PUSH DWORD PTR [ebp - 4]
				mov ebx, DWORD PTR [ebp + 20]		 
				call ebx
				PUSH eax						   ; pushed the dereferenced iterator #2 (j) onto the stack

				PUSH DWORD PTR [ebp - 8]       
				mov ebx, DWORD PTR [ebp + 20]      ; dreference function
				call ebx							
				PUSH eax						   ; pushed the dereferenced iterator #1 (i) onto the stack	

			
			mov ebx, [ebp + 36]                    ; calling predicate function
			call ebx
			
			cmp al, 0
			JG _end_compare_iterators_if
		
		   _inc_iterator_1:
				PUSH 1
				PUSH DWORD PTR [ebp - 4]
				mov ebx, DWORD PTR [ebp + 40]          ; next function address
				call ebx
				mov DWORD PTR [ebp - 4], eax
			
			JMP _while_outer
		_end_compare_iterators_if:
	
		_else:

			PUSH DWORD PTR [ebp - 8]			; Iterator #2 (j)
			mov ebx, DWORD PTR [ebp + 20]		; deference function
			call ebx
			mov DWORD PTR [ebp - 20], eax		; Store value in value_var

			mov eax, DWORD PTR [ebp - 8]		; Value at iterator #2 (j)
			mov DWORD PTR [ebp - 16], eax		; Store iterator #2 (j) in index

			_while_inner:
				
				_check_inner_while_conditon:
					PUSH DWORD PTR [ebp - 4]
					PUSH DWORD PTR [ebp - 16]
					mov ebx, DWORD PTR [ebp + 28]    ; compare function address
					call ebx
					
					cmp al, 0
					JE _end_while_inner
			
				_assign_value:
					mov edx, DWORD PTR [ebp - 16]   ; moving index into edx

					PUSH 1
					PUSH edx
					mov ebx, DWORD PTR [ebp + 44]   ; previous iterator function
					call ebx                        ; eax = (index - 1)
					
					PUSH eax
					mov ebx, DWORD PTR [ebp + 20]   ; dereference function
					call ebx
					mov DWORD PTR [ebp - 24], eax	; move into extra storage

					mov eax, ebp
					sub eax, 24
					PUSH eax
					PUSH DWORD PTR [ebp - 16]
					mov ebx, DWORD PTR [ebp + 32]	; swap values in a[index] and extra storage
					call ebx
					
					_dec_iterator_index:
						PUSH 1
						PUSH DWORD PTR [ebp - 16]
						mov ebx, DWORD PTR [ebp + 44]   ; previous iterator function
						call ebx
						mov DWORD PTR [ebp - 16], eax

				JMP _while_inner
			_end_while_inner:

			_assign_iterator_1_value:
				mov eax, ebp
				sub eax, 20
				PUSH eax
				PUSH DWORD PTR [ebp - 4]
				mov ebx, DWORD PTR [ebp + 32]
				call ebx
						
			_increment_all_iterators:
				PUSH 1
				PUSH DWORD PTR [ebp - 4]
				mov ebx, DWORD PTR [ebp + 40]      ; Next function
				call ebx
				mov DWORD PTR [ebp - 4], eax

				PUSH 1
				PUSH DWORD PTR [ebp + 12]
				mov ebx, DWORD PTR [ebp + 40]      ; Next function
				call ebx
				mov DWORD PTR [ebp + 12], eax

				PUSH 1
				PUSH DWORD PTR [ebp - 8]
				mov ebx, DWORD PTR [ebp + 40]      ; Next function
				call ebx
				mov DWORD PTR [ebp - 8], eax
		_end_else:
			
		JMP _while_outer
	_endwhile_outer:

	POPAD
	leave
	ret 44
Merge ENDP

; ------------------------------------------------------------------
; Parameters (MergeSortDefinition):
; 8 = First Iterator: 4 bytes (object) address
; 12 = Last Iterator: 4 bytes (object) address 
; 16 = Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; 20 = Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; 24 = Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; 28 = Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; 32 = Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; 36 = Next: 4 bytes (procedure with two operands: iterator and integer) (return address of an iterator in EAX) address
; 40 = Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; 44 = Distance: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) 
; Total size of parameters: 40 Bytes
; 
; Local Variables:
; 4 = Mid Iterator: 4 bytes
;
; Total size of Local Variables: 4 bytes
; ------------------------------------------------------------------

MergeSortDefinition PROC
    enter 4, 0
    PUSHAD

    ; Compare first and last iterators
    PUSH DWORD PTR [ebp + 8]        ; First Iterator
    PUSH DWORD PTR [ebp + 12]       ; Last Iterator
    mov ebx, DWORD PTR [ebp + 24]   ; Compare function
    call ebx

    cmp al, 0
    JGE end_function				; if first >= last, return

    ; Calculate mid iterator
    PUSH DWORD PTR [ebp + 12]       ; Last Iterator
    PUSH DWORD PTR [ebp + 8]        ; First Iterator
    mov ebx, DWORD PTR [ebp + 44]   ; Distance function
    call ebx
	
    shr eax, 1						; (first + last) / 2

	PUSH eax
    PUSH DWORD PTR [ebp + 8]        ; First Iterator
    mov ebx, DWORD PTR [ebp + 36]   ; Next function
    call ebx
    mov DWORD PTR [ebp - 4], eax    ; Mid Iterator

	PUSH DWORD PTR [ebp - 4]
	mov ebx, DWORD PTR [ebp + 16]
	call ebx

    ; Recursively call MergeSortDefinition for the left half
    PUSH DWORD PTR [ebp + 44]       
    PUSH DWORD PTR [ebp + 40]       
    PUSH DWORD PTR [ebp + 36]       
    PUSH DWORD PTR [ebp + 32]       
    PUSH DWORD PTR [ebp + 28]       
    PUSH DWORD PTR [ebp + 24]       
    PUSH DWORD PTR [ebp + 20]       
    PUSH DWORD PTR [ebp + 16]       
    PUSH DWORD PTR [ebp - 4]        
    PUSH DWORD PTR [ebp + 8]        
    call MergeSortDefinition

    ; Recursively call MergeSortDefinition for the right half
    PUSH DWORD PTR [ebp + 44]       
    PUSH DWORD PTR [ebp + 40]       
    PUSH DWORD PTR [ebp + 36]       
    PUSH DWORD PTR [ebp + 32]       
    PUSH DWORD PTR [ebp + 28]       
    PUSH DWORD PTR [ebp + 24]       
    PUSH DWORD PTR [ebp + 20]       
    PUSH DWORD PTR [ebp + 16]       
    PUSH DWORD PTR [ebp + 12]       
	_mid_next:
		PUSH 1
		PUSH DWORD PTR [ebp - 4]
		mov ebx, DWORD PTR [ebp + 36]
		call ebx
	PUSH eax
    call MergeSortDefinition

    ; Call Merge to merge the two halves
    PUSH DWORD PTR [ebp + 44]       
    PUSH DWORD PTR [ebp + 40]       
    PUSH DWORD PTR [ebp + 36]       
    PUSH DWORD PTR [ebp + 32]       
    PUSH DWORD PTR [ebp + 28]       
    PUSH DWORD PTR [ebp + 24]       
    PUSH DWORD PTR [ebp + 20]       
    PUSH DWORD PTR [ebp + 16]       
    PUSH DWORD PTR [ebp + 12]       
    PUSH DWORD PTR [ebp - 4]        
    PUSH DWORD PTR [ebp + 8]        
    call Merge

end_function:
    POPAD
    leave
    ret 40
MergeSortDefinition ENDP

; ------------------------------------------------------------------
; Parameters (MergeSort):
; 8 = First Iterator: 4 bytes (object) address
; 12 = Last Iterator: 4 bytes (object) address 
; 16 = Dereference: 4 bytes (procedure with one operand: iterator address) (returns address of an object in EAX) address
; 20 = Copy_iter: 4 bytes (procedure with one operand: iterator address) (returns address of new iterator in EAX) address
; 24 = Compare: 4 bytes (procedure with two operands: two iterators (address ofc)) (returns the compare result in EAX) address
; 28 = Iter_swap: 4 bytes (procedure with two operands: two iterators (address ofc)) (void return) address
; 32 = Predicate: 4 bytes (procedure with two operands: addresses of two objects) (returns bool in AL) address
; 36 = Next: 4 bytes (procedure with two operands: iterator and integer) (return address of an iterator in EAX) address
; 40 = Previous: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) address
; 44 = Distance: 4 bytes (procedure with two operands: iterator and integer) (returns address of an iterator EAX) 
; Total size of parameters: 40 Bytes
; ------------------------------------------------------------------

MergeSort PROC
	enter 0, 0

	PUSH 1
	PUSH DWORD PTR [ebp + 12] ; last iterator	
	mov ebx, [ebp + 40]       ; prev function
	call ebx			
	mov DWORD PTR [ebp + 12], eax

    PUSH DWORD PTR [ebp + 44]       ; Distance function
    PUSH DWORD PTR [ebp + 40]       ; Previous function
    PUSH DWORD PTR [ebp + 36]       ; Next function
    PUSH DWORD PTR [ebp + 32]       ; Predicate function
    PUSH DWORD PTR [ebp + 28]       ; Iter_swap function
    PUSH DWORD PTR [ebp + 24]       ; Compare function
    PUSH DWORD PTR [ebp + 20]       ; Copy_iter function
    PUSH DWORD PTR [ebp + 16]       ; Dereference function
	PUSH DWORD PTR [ebp + 12]		; Last Iterator
    PUSH DWORD PTR [ebp + 8]        ; First Iterator
    call MergeSortDefinition	

	leave
	ret 40
MergeSort ENDP