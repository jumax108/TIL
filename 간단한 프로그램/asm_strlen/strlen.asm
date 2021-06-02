.code

_strlen PROC
xor rax, rax
checkNull:
movzx edx, byte ptr[rcx + rax]
cmp edx, 0
je endProc
add rax, 1
jmp checkNull
endProc:
ret
_strlen ENDP

END
