INCLUDE Irvine32.inc
INCLUDE BubbleSort.inc

.DATA

.CODE
  MAIN PROC
    MOV EAX, 3h
    CALL DumpRegs

    EXIT
  MAIN ENDP

END MAIN