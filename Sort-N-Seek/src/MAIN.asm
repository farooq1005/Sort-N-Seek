INCLUDE Irvine32.inc
INCLUDE BubbleSort.inc

.DATA
  myArr DD 3, 2, 5, 6, 1, 7, 8, 9, 0

.CODE
Deref PROC
  PUSH EBP
  MOV EBP, ESP

  MOV EAX, [EBP + 8]
  MOV ECX, [EAX]
  MOV EAX, [EAX]
  
  MOV ESP, EBP
  POP EBP
  RET 4
Deref ENDP

CopyIter PROC
  PUSH EBP
  MOV EBP, ESP

  MOV EAX, [EBP + 8]

  MOV ESP, EBP
  POP EBP
  RET 4
CopyIter ENDP

Compare PROC
  PUSH EBP
  MOV EBP, ESP

  MOV EDX, [EBP + 8]                        ; First iterator
  MOV EAX, [EBP + 12]                       ; Second iterator

  SUB EAX, EDX

  MOV ESP, EBP
  POP EBP
  RET 8
Compare ENDP

IterSwap PROC
  PUSH EBP
  MOV EBP, ESP

  MOV ESI, [EBP + 8]                        ; First iterator
  MOV EDI, [EBP + 12]                       ; Second iterator

  MOV EAX, [ESI]
  MOV EDX, [EDI]

  MOV DWORD PTR [ESI], EDX
  MOV DWORD PTR [EDI], EAX

  MOV ESP, EBP
  POP EBP
  RET 8
IterSwap ENDP

Predicate PROC                              ; simulating std::less<T> function (EAX < EDX)
  PUSH EBP
  MOV EBP, ESP

  MOV EAX, [EBP + 8]                        ; First iterator
  MOV EDX, [EBP + 12]                       ; Second iterator

  CMP EAX, EDX

If1000: 
  JL If1001

  MOV EAX, 0
  JMP Endif_1000

If1001:
  MOV EAX, 1

Endif_1000:

  MOV ESP, EBP
  POP EBP
  RET 8
Predicate ENDP

Next PROC
  PUSH EBP
  MOV EBP, ESP

  MOV ESI, [EBP + 8]
  MOV EAX, 4
  MUL DWORD PTR [EBP + 12]

  ADD ESI, EAX
  XCHG EAX, ESI

  MOV ESP, EBP
  POP EBP
  RET 8
Next ENDP

Prev PROC
  PUSH EBP
  MOV EBP, ESP

  MOV ESI, [EBP + 8]
  MOV EAX, 4
  MUL DWORD PTR [EBP + 12]

  SUB ESI, EAX
  XCHG EAX, ESI

  MOV ESP, EBP
  POP ESP
  RET 8
Prev ENDP

Distance PROC
  PUSH EBP
  MOV EBP, ESP

  MOV EDX, [EBP + 8]
  MOV EAX, [EBP + 12]

  SUB EAX, EDX
  
  MOV EDX, 0
  MOV ECX, 4
  DIV ECX

  MOV ESP, EBP
  POP ESP
  RET 8
Distance ENDP

MAIN PROC
  PUSH OFFSET Distance
  PUSH OFFSET Prev
  PUSH OFFSET Next
  PUSH OFFSET Predicate
  PUSH OFFSET IterSwap
  PUSH OFFSET Compare
  PUSH OFFSET CopyIter
  PUSH OFFSET Deref
  PUSH OFFSET myArr
  ADD DWORD PTR [ESP], SIZEOF myArr
  PUSH OFFSET myArr

  CALL BubbleSort

  MOV ESI, OFFSET myArr
  MOV ECX, LENGTHOF myArr

LOOP1003:
  MOV EAX, [ESI]
  CALL WriteDec
  CALL Crlf

  ADD ESI, 4
  LOOP LOOP1003
END_LOOP3:

  EXIT
MAIN ENDP

END MAIN