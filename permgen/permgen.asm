
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
fmt db `%s\n`, 0

v db '123456789', 0
;v db 'abcdefghijklmnopqrstuvwxyz', 0

section .code use32 class=code

start:
..start:

push dword prompt
call [printf]
add esp, 4
call [getchar] ; '0' + n

cmp eax, '2'
jl .exit
cmp eax, '9'
jg .exit

mov ebx, v
sub eax, '0' ; eax = n
add ebx, eax ; ebx = v + n
mov [ebx], byte 0

std

push dword v
push dword fmt
.while_1:
  call [printf]

  mov esi, ebx
  dec esi ; k = n - 1
  .while_esi: ; find max k such that v[k] < v[k + 1]
    lodsb ; al = [esi--]
    cmp esi, v
    jl .while_1_end ; k < 0 - finish
    mov dl, [esi]
    cmp dl, al
    jg .while_esi ; v[k] > v[k + 1]
  mov edi, esi
  ; dl = [edi] = v[k]

  mov esi, ebx
  dec esi ; l = n - 1
  .while_edi: ; find max l such that v[k] < v[l]
    lodsb ; al = [esi--]
    cmp dl, al
    jg .while_edi ; v[k] > v[l]
  inc esi;
  ; al = [esi] = v[l]

  ; v[k] <-> v[l]
  mov [esi], dl
  mov [edi], al

  mov esi, ebx ; l = n
  .reverse: ; reverse order from k + 1 to n - 1
    inc edi
    dec esi
    cmp edi, esi
    jnl .reverse_end
    mov al, [esi]
    mov dl, [edi]
    mov [esi], dl
    mov [edi], al
    jmp .reverse
  .reverse_end:

  jmp .while_1
.while_1_end:
add esp, 8 ; printf parameters

cld

.exit:
push dword 0 ; exitCode
call [ExitProcess]
