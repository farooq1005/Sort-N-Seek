
.code
; ------------------------------------------------------------------
; Parameters:
; 8 = k: 4 byte unsigned integer
;
; Gap Sequence = 4 ^ k + (3 * 2 ^ (k - 1)) + 1
; ------------------------------------------------------------------

GapCalculatorShell PROC
enter 0, 0

PUSH ebx
PUSH ecx
PUSH edx

; load k into ecx
mov ecx, dword ptr[ebp + 8]; k is at[esp + 4], which is[ebp + 8]

; calculate 4 ^ k
mov eax, 4; base 4
mov ebx, ecx; copy k to ebx
cmp ebx, 0; if k == 0
je _poweroffourdone; 4 ^ k = 1 for k == 0
_poweroffourloop:
dec ebx
jz _poweroffourdone
mul eax; multiply eax by itself(eax *= 4)
jmp _poweroffourloop
_poweroffourdone :

; store 4 ^ k in eax
push eax; save result of 4 ^ k on the stack

; calculate 2 ^ (k - 1)
mov eax, 2; base 2
mov ebx, ecx; copy k to ebx
dec ebx; k - 1
cmp ebx, 0; if k - 1 <= 0
je _poweroftwodone; 2 ^ (k - 1) = 1 for k - 1 <= 0
_poweroftwoloop:
dec ebx
jz _poweroftwodone
mul eax; multiply eax by 2 (eax *= 2)
jmp _poweroftwoloop
_poweroftwodone :

; calculate(3 * 2 ^ (k - 1))
mov ebx, 3; load 3
mul ebx; eax = 3 * eax(eax *= 3)

; add this to 4 ^ k
pop ebx; restore 4 ^ k from the stack
add eax, ebx; add 4 ^ k to(3 * 2 ^ (k - 1))

; add 1
add eax, 1; eax = eax + 1

POP edx
POP ecx
POP ebx
leave
ret 4; return to caller
GapCalculatorShell ENDP

; ------------------------------------------------------------------
; Parameters:
; 4 bytes: [EBP + 8] = First iterator address
; 4 bytes: [EBP + 12] = Last iterator address
; 4 bytes: [EBP + 16] = Predicate function address
; Return: None(in - place sorting)
;
; Total size of parameters : 12 Bytes
;
; Local Variables :
; 12 - iterator i : 12 bytes
; 24 - iterator j : 12 bytes
; 36 - iterator gapped : 12 bytes
; 48 - iterator[j - gap]: 12 bytes
; 52 - gap: 4 bytes
; 56 - k: 4 bytes
;
; 60 - it.value_type size : 4 bytes
; Onwards 2 x iterator values
;
; Total size of Known Local Varibales : 60 bytes
; ------------------------------------------------------------------

ShellSort PROC
enter 60, 0

; save the value type
mov ecx, [ebp + 8]
mov ecx, (Iterator PTR[ecx]).value_type
mov DWORD PTR[ebp - 60], ecx

; allocate 2 x it.value_type bytes in stack
mov eax, 2
mov edx, 0
mul ecx
sub esp, eax

; store the function pointers object first hand
mov ecx, [ebp + 8]
mov ecx, (Iterator PTR[ecx]).function_pointers

; calulate the distance
PUSH DWORD PTR[ebp + 12]
PUSH DWORD PTR[ebp + 8]
mov ebx, (Iterator_Functions PTR[ecx])._dist; distance function
call ebx

; set k = n
mov DWORD PTR[ebp - 56], eax

; initialize the gap with GapCalculatorShell(n)
PUSH eax
call GapCalculatorShell
mov DWORD PTR[ebp - 52], eax

_gap_loop :

_check_gap_loop_condition:

; check whether if k has become 0 or less
mov eax, DWORD PTR[ebp - 56]
cmp eax, 0
JL _end_gap_loop

_end_check_gap_loop_condition :

; set i = gap
mov edi, ebp
sub edi, 12
PUSH DWORD PTR[ebp - 52]
PUSH DWORD PTR[ebp + 8]
mov ebx, (Iterator_Functions PTR[ecx])._next
call ebx

_outer_loop :

_check_outer_loop_condition:

; compare i with last iterator
mov edi, ebp
sub edi, 12
PUSH edi; i
PUSH DWORD PTR[ebp + 12]; last iterator
mov ebx, (Iterator_Functions PTR[ecx])._comp
call ebx

cmp eax, 0
JGE _end_outer_loop

_end_check_outer_loop_condition :

; pointer to j
mov edi, ebp
sub edi, 24

; pointer to i
mov esi, ebp
sub esi, 12

; set j = i
PUSH esi
mov ebx, (Iterator_Functions PTR[ecx])._copy
call ebx

; set gapped iterator
mov edi, ebp
sub edi, 36

PUSH DWORD PTR[ebp - 52]; gap
PUSH DWORD PTR[ebp + 8]; first iterator address
mov ebx, (Iterator_Functions PTR[ecx])._next
call ebx

_inner_loop :

; initializing[j - gap] iterator
mov edi, ebp
sub edi, 48

mov esi, ebp
sub esi, 24

PUSH DWORD PTR[ebp - 52]; gap
PUSH esi; iterator j
mov ebx, (Iterator_Functions PTR[ecx])._prev
call ebx

_check_inner_loop_condition :

; gapped iterator
mov edi, ebp
sub edi, 36

; iterator j
mov esi, ebp
sub esi, 24

PUSH esi; iterator j
PUSH edi; gapped iterator
mov ebx, (Iterator_Functions PTR[ecx])._comp
call ebx

cmp eax, 0
JL _end_inner_loop

_dreference :

mov edi, ebp
sub edi, 60
sub edi, DWORD PTR[ebp - 60]

; dereference[j - gap] iterator
mov esi, ebp
sub esi, 48
PUSH esi
mov ebx, (Iterator_Functions PTR[ecx])._dref
call ebx
mov eax, edi
; PUSH edi
mov eax, edi; [edi] = 3

mov edi, ebp
sub edi, 60
sub edi, DWORD PTR[ebp - 60]
sub edi, DWORD PTR[ebp - 60]

; dereference iterator j
mov esi, ebp
sub esi, 24
PUSH esi
mov ebx, (Iterator_Functions PTR[ecx])._dref
call ebx; pushed onto the stack as argument
; PUSH edi

; ------------------------------------------------------ -
mov edx, edi; [edi] = 2
mov esi, [edx]; correct value, esi = 2
PUSH 5; pushed eax = [3]
PUSH[edx]; pushed edx = [2]

POP edx; edx = [2]
mov eax, [edx]; expected eax = [3]
; ------------------------------------------------------ -

POP eax
PUSH eax
mov ebx, DWORD PTR[ebp + 16]; predicate function
call ebx

_end_check_inner_loop_condition :

; swap iterators j and [j - gap]
_swap:

mov edi, ebp
sub edi, 24

mov esi, ebp
sub esi, 48

PUSH edi
PUSH esi
mov ebx, (Iterator_Functions PTR[ecx])._swap
call ebx

_end_swap :

_decrement_j:

mov edi, ebp
sub edi, 24

PUSH DWORD PTR[ebp - 52]; gap
PUSH edi; iterator j
mov ebx, (Iterator_Functions PTR[ecx])._prev
call ebx

JMP _inner_loop
_end_inner_loop :

_increment_i:
mov edi, ebp
sub edi, 12

PUSH 1
PUSH edi
mov ebx, (Iterator_Functions PTR[ecx])._next

JMP _outer_loop
_end_outer_loop :

_calculate_new_gap:
dec DWORD PTR[ebp - 56]
PUSH DWORD PTR[ebp - 56]
call GapCalculatorShell
mov DWORD PTR[ebp - 56], eax

JMP _gap_loop
_end_gap_loop :

leave
ret 56
ShellSort ENDP