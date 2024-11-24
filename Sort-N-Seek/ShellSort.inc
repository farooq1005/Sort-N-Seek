
.code
; ------------------------------------------------------------------
; Parameters:
; 8 = k: 4 byte unsigned integer
;
; Gap Sequence = 4^k + (3 * 2^(k-1)) + 1 
; ------------------------------------------------------------------

GapCalculatorShell PROC
    ; Function prologue
    enter 8, 0                    ; Create stack frame, 8 bytes for local vars
    
    PUSH ecx

    ; Get parameter k from stack [ebp+8]
    mov eax, [ebp+8]             
    
    ; Check if k == 0
    cmp eax, 0
    jne calculate_gap
    mov eax, 1                    ; Return 1 if k == 0
    jmp done

calculate_gap:
    ; Calculate 4^k
    mov ecx, eax                  ; Save k in ecx
    mov eax, 1                    ; Initialize result
    mov ebx, 4                    ; Base = 4
power_loop1:
    test ecx, ecx                 ; Check if exponent is 0
    jz power_done1
    mul ebx                       ; eax *= 4
    dec ecx
    jmp power_loop1
power_done1:
    mov [ebp-4], eax             ; Store 4^k

    ; Calculate 2^(k-1)
    mov eax, [ebp+8]             ; Get k again
    dec eax                       ; k-1
    mov ecx, eax                  ; Store k-1 in ecx
    mov eax, 1                    ; Initialize result
    mov ebx, 2                    ; Base = 2
power_loop2:
    test ecx, ecx                 ; Check if exponent is 0
    jz power_done2
    mul ebx                       ; eax *= 2
    dec ecx
    jmp power_loop2
power_done2:
    ; Multiply by 3
    mov ebx, 3
    mul ebx                       ; eax *= 3
    
    ; Add 4^k and 1
    add eax, [ebp-4]             ; Add 4^k
    inc eax                       ; Add 1

done:
    POP ecx
    leave                         ; Restore stack frame
    ret 4                        ; Return and clean up 4 bytes (stdcall convention)
GapCalculatorShell ENDP

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
; 12 - iterator i: 12 bytes
; 24 - iterator j: 12 bytes
; 36 - iterator gapped: 12 bytes
; 48 - iterator [j - gap]: 12 bytes
; 52 - gap: 4 bytes
; 56 - k: 4 bytes
; 60 - size of container
;
; 64 - it.value_type size: 4 bytes
; Onwards 2 x iterator values
;
; Total size of Known Local Varibales: 64 bytes
; ------------------------------------------------------------------

ShellSort PROC
    enter 64, 0

	; save the value type
	mov ecx, [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).value_type
	mov DWORD PTR [ebp - 64], ecx

	; allocate 2 x it.value_type bytes in stack
	mov eax, 2
	mov edx, 0
	mul ecx
	sub esp, eax

	; store the function pointers object first hand 
	mov ecx, [ebp + 8]
	mov ecx, (Iterator PTR [ecx]).function_pointers	

    ; calulate the distance 
	PUSH DWORD PTR [ebp + 12]
	PUSH DWORD PTR [ebp + 8]  
	mov ebx, (Iterator_Functions PTR [ecx])._dist ; distance function
	call ebx

    ; set k = n
    mov DWORD PTR [ebp - 56], eax 
    mov DWORD PTR [ebp - 60], eax
    
    ; initialize the gap with GapCalculatorShell(n)
    PUSH eax
    call GapCalculatorShell
    mov DWORD PTR [ebp - 52], eax

    _gap_loop:

        _check_gap_loop_condition:
            
            mov eax, DWORD PTR [ebp - 52]
            cmp eax, DWORD PTR [ebp - 60]
            JGE _calculate_new_gap
            
            ; check whether if k has become less than zero
            mov eax, DWORD PTR [ebp - 56]
            cmp eax, 0
            JL _end_gap_loop

        _end_check_gap_loop_condition:
     
        ; set i = gap
        mov edi, ebp
        sub edi, 12
        PUSH DWORD PTR [ebp - 52]                  
        PUSH DWORD PTR [ebp + 8]
        mov ebx, (Iterator_Functions PTR [ecx])._next
        call ebx

        _outer_loop:
            
            _check_outer_loop_condition:
               
               ; compare i with last iterator
               mov edi, ebp
               sub edi, 12
               PUSH DWORD PTR [ebp + 12]                        ; last iterator
               PUSH edi                                         ; i 
               mov ebx, (Iterator_Functions PTR [ecx])._comp
               call ebx

               cmp eax, 0                       
               JLE _end_outer_loop 

            _end_check_outer_loop_condition:
               
            ; pointer to j
            mov edi, ebp
            sub edi, 24

            ; pointer to i
            mov esi, ebp
            sub esi, 12

            ; set j = i
            PUSH esi
            mov ebx, (Iterator_Functions PTR [ecx])._copy
            call ebx

            _inner_loop:
                
                ; initializing [j - gap] iterator
                mov edi, ebp
                sub edi, 48

                mov esi, ebp
                sub esi, 24

                PUSH DWORD PTR [ebp - 52]   ; gap
                PUSH esi                    ; iterator j
                mov ebx, (Iterator_Functions PTR [ecx])._prev
                call ebx

                _check_inner_loop_condition:
                    ; iterator j
                    mov esi, ebp
                    sub esi, 24

                    PUSH esi
                    PUSH [ebp + 8]
                    mov ebx, (Iterator_Functions PTR [ecx])._dist
                    call ebx

                    cmp eax, DWORD PTR [ebp - 52]
                    JL _end_inner_loop

                    _dreference:

                        mov edi, ebp
                        sub edi, 64
                        sub edi, DWORD PTR [ebp - 64]
                        
                        ; dereference [j - gap] iterator
                        mov esi, ebp
                        sub esi, 48
                        PUSH esi
                        mov ebx, (Iterator_Functions PTR [ecx])._dref
                        call ebx  
                        mov eax, edi
                        PUSH edi               

                        mov edi, ebp
                        sub edi, 64
                        sub edi, DWORD PTR [ebp - 64]
                        sub edi, DWORD PTR [ebp - 64]

                        ; dereference iterator j
                        mov esi, ebp
                        sub esi, 24
                        PUSH esi
                        mov ebx, (Iterator_Functions PTR [ecx])._dref
                        call ebx                       ; pushed onto the stack as argument
                        PUSH edi

                        mov ebx, DWORD PTR [ebp + 16]   ; predicate function
                        call ebx
                        
                        cmp eax, 0
                        JE _end_swap

                _end_check_inner_loop_condition:

                ; swap iterators j and [j - gap]
                _swap:
                    
                    mov edi, ebp
                    sub edi, 24

                    mov esi, ebp
                    sub esi, 48

                    PUSH edi
                    PUSH esi
                    mov ebx, (Iterator_Functions PTR [ecx])._swap
                    call ebx

                _end_swap:

                _decrement_j:
                    
                    mov edi, ebp
                    sub edi, 24

                    PUSH DWORD PTR [ebp - 52]   ; gap
                    PUSH edi                    ; iterator j
                    mov ebx, (Iterator_Functions PTR [ecx])._prev
                    call ebx

                JMP _inner_loop
            _end_inner_loop:

            _increment_i:
            mov edi, ebp
            sub edi, 12

            PUSH 1
            PUSH edi
            mov ebx, (Iterator_Functions PTR [ecx])._next
            call ebx
                      
            JMP _outer_loop
        _end_outer_loop:

        _calculate_new_gap:
            cmp DWORD PTR [ebp - 52], 1
            JBE _end_gap_loop
            dec DWORD PTR [ebp - 56]
            PUSH DWORD PTR [ebp - 56]
            call GapCalculatorShell
            mov DWORD PTR [ebp - 52], eax

        JMP _gap_loop
    _end_gap_loop:

    leave
    ret 12
ShellSort ENDP