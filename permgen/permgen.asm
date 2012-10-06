
%macro API 1-*
  %define dll %1
  %rep %0 - 1
    extern %2
    import %2 dll
    %rotate 1
  %endrep
%endmacro

API kernel32.dll, ExitProcess
API msvcrt.dll, getchar, printf

section .data use32

prompt db `n = `, 0
println db `%s\n`, 0

v db '123456789', 0
;v db 'abcdefghijklmnopqrstuvwxyz', 0

section .code use32 class=code

start:
..start:
push ebx

push dword prompt
call [printf]
add esp, 4
call [getchar] ; '0' + n

cmp eax, '2'
jl .exit
cmp eax, '9'
jg .exit

sub eax, '0' ; eax = n
mov ebx, v
add ebx, eax ; ebx = v + n
mov [ebx], byte 0

push dword v
push dword println
.while_1:
  call [printf] ; can't use eax - printf 'trashes' eax

  mov esi, ebx
  dec esi ; k = n - 1
  .while_esi: ; find max k such that v[k] < v[k + 1]
    dec esi
    cmp esi, v
    jl .while_1_end ; k < 0 - finish
    mov cl, [esi]
    mov dl, [esi + 1]
    cmp cl, dl
    jg .while_esi ; v[k] > v[k + 1]

  mov edi, ebx ; l = n
;  mov cl, [esi] ; redundant - cl = [esi] from prev loop
  .while_edi: ; find max l such that v[k] < v[l]
    dec edi
    mov dl, [edi]
    cmp cl, dl
    jg .while_edi ; v[k] > v[l]

  ; v[k] <-> v[l]
  mov cl, [esi]
  mov dl, [edi]
  mov [esi], dl
  mov [edi], cl

  mov edi, ebx ; l = n
  .reverse: ; reverse order from k + 1 to n - 1
    inc esi
    dec edi
    cmp esi, edi
    jnl .reverse_end
    mov cl, [esi]
    mov dl, [edi]
    mov [esi], dl
    mov [edi], cl
    jl .reverse
  .reverse_end:

  jmp .while_1
.while_1_end:
add esp, 8 ; printf parameters

.exit:
pop ebx
call [ExitProcess]
