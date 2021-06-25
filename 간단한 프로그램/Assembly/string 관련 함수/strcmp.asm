.code

_strcmp PROC


compare:
movsx r8, byte ptr [rcx]
movsx r9, byte ptr [rdx]
cmp r8, r9
ja frontBig
cmp r8, r9
jb rearBig
add rcx, 1
add rdx, 1

cmp r8, 0
jne continue
cmp r9, 0
jne continue
jmp sameStr

continue:
jmp compare


frontBig:
 mov rax, 1
 jmp endProc

rearBig:
 mov rax, -1
 jmp endPRoc

sameStr:
 xor eax, eax

endProc:

ret
_strcmp ENDP

END
