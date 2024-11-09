.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode:DWORD

EXTERN ModuleFunction:PROC

.DATA

.CODE
  MAIN PROC
    CALL ModuleFunction

    INVOKE ExitProcess, 0
  MAIN ENDP

END MAIN