INCLUDE Irvine32.inc
INCLUDE ITERATOR.inc

.DATA
  myArr DD 3, 2, 5, 6, 1, 7, 8, 9, 0

.CODE

Copy PROC USES ESI EAX EBX ECX
  ; Stack Frame:
  ; [EBP+8] = Iterator address
  ; Copies the iterator to the memory pointed by EDI (preserving EDI)

  ENTER 0, 0

  MOV ESI, [EBP+8]        

  MOV ECX, SIZEOF Iterator
  CLD
  REP MOVSB        

  LEAVE
  RET 4    
Copy ENDP

; Procedures for Iterator_Functions
Next PROC USES ESI EAX EBX ECX
  ; Stack Frame: 
  ; [EBP+8] = Iterator address
  ; [EBP+12] = Offset
  ; Return: Address of the updated iterator in EDI
  
  ENTER 0, 0       
  PUSH DWORD PTR [EBP+8]
  CALL Copy

  MOV ESI, [EBP+8]               
  MOV EAX, (Iterator PTR [ESI]).pointer 
  MOV EBX, (Iterator PTR [ESI]).value_type
  
  MOV ECX, [EBP+12]              
  IMUL EBX, ECX                  
  
  ADD EAX, EBX                   
  MOV DWORD PTR (Iterator PTR [EDI]).pointer, EAX               
                        
  LEAVE
  RET 8                          
Next ENDP

Previous PROC USES ESI EAX EBX ECX
  ; Stack Frame: 
  ; [EBP+8] = Iterator address
  ; [EBP+12] = Offset
  ; Return: Address of the updated iterator in EDI
  
  ENTER 0, 0 
  PUSH DWORD PTR [EBP+8]
  CALL Copy                 

  MOV ESI, [EBP+8]               
  MOV EAX, (Iterator PTR [ESI]).pointer 
  MOV EBX, (Iterator PTR [ESI]).value_type
  
  MOV ECX, [EBP+12]              
  IMUL EBX, ECX                  
  
  SUB EAX, EBX                   
  MOV DWORD PTR (Iterator PTR [EDI]).pointer, EAX                 
                        
  LEAVE
  RET 8                          
Previous ENDP

Compare PROC USES ESI EDI EAX EBX ECX
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Comparison result (0, <0, >0) in EAX

  ENTER 0, 0

  MOV ESI, [EBP+8]  
  MOV EDI, [EBP+12] 

  MOV EAX, (Iterator PTR [ESI]).pointer
  MOV EBX, (Iterator PTR [EDI]).pointer
  SUB EAX, EBX

  LEAVE
  RET 8 
Compare ENDP

Dereference PROC USES ESI EAX EBX ECX
  ; Stack Frame:
  ; [EBP+8] = Iterator address
  ; Return: Value in memory pointed by EDI

  ENTER 0, 0

  MOV EAX, [EBP+8]
  MOV ESI, (Iterator PTR [EAX]).pointer
  MOV ECX, (Iterator PTR [EAX]).value_type
  CLD
  REP MOVSB 

  LEAVE
  RET 4  
Dereference ENDP

Swap PROC USES ESI EDI EAX EBX ECX
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Nothing

  ENTER 0, 0

  MOV EAX, (Iterator PTR [EBP+8]).value_type
  SUB ESP, EAX

  ; Approach: Move one value to a temporary storage
  ; then, copy one value to another iterator 
  ; then, copy the temporary storage to the iterator
  MOV ESI, (Iterator PTR [EBP+8]).pointer
  MOV EDI, ESP
  MOV ECX, EAX
  REP MOVSB

  MOV ESI, (Iterator PTR [EBP+12]).pointer
  MOV EDI, (Iterator PTR [EBP+8]).pointer
  MOV ECX, EAX
  REP MOVSB

  MOV ESI, ESP
  MOV EDI, (Iterator PTR [EBP+12]).pointer
  MOV ECX, EAX
  REP MOVSB

  ADD ESP, EAX

  LEAVE
  RET 8
Swap ENDP

Distance PROC USES ESI EDI EBX ECX
  ; Stack Frame:
  ; [EBP+8] = Iterator 1 address
  ; [EBP+12] = Iterator 2 address
  ; Return: Distance in EAX (number of elements)

  ENTER 0, 0

  MOV ESI, [EBP+8]
  MOV EDI, [EBP+12]

  MOV EBX, (Iterator PTR [ESI]).pointer 
  MOV EAX, (Iterator PTR [EDI]).pointer
  
  SUB EAX, EBX 
  CDQ

  MOV ECX, [ESI].Iterator.value_type
  IDIV ECX 

  LEAVE
  RET 8
Distance ENDP

MAIN PROC
  MOV ESI, OFFSET myArr
  MOV ECX, LENGTHOF myArr
  MOV EBX, TYPE myArr

  call DumpMem

  EXIT
MAIN ENDP

END MAIN