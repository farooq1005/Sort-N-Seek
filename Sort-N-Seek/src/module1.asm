INCLUDE Irvine32.inc

PUBLIC _ModuleFunction

.DATA 
	__module1__message_1 DB "This is Module 1", 0

.CODE
	_ModuleFunction PROC
		MOV EDX, OFFSET __module1__message_1
		CALL WriteString

		RET
	_ModuleFunction ENDP
END
