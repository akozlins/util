
DLL_PROCESS_DETACH equ 0
DLL_PROCESS_ATTACH equ 1
DLL_THREAD_ATTACH equ 2
DLL_THREAD_DETACH equ 3

WH_KEYBOARD equ 2
WH_GETMESSAGE equ 3

%macro API 1-*
  %define dll %1
  %rep %0 - 1
    extern %2
    import %2 dll
    %rotate 1
  %endrep
%endmacro

API kernel32.dll, DisableThreadLibraryCalls
API user32.dll, SetWindowsHookExA, UnhookWindowsHookEx, CallNextHookEx
API msvcrt.dll, fopen, fclose, fprintf

section .data use32

file_mode db "a+", 0
file_name db "d:/out.txt", 0
file_fmt db `%08X : %08X\n`, 0

msg_attach db `DLL_PROCESS_ATTACH\n`, 0
msg_detach db `DLL_PROCESS_DETACH\n`, 0

fd dd 0
hook dd 0
module dd 0

section .code use32 class=code

export hookFunc
global hookFunc
hookFunc: ; code, wParam, lParam
  push dword file_mode
  push dword file_name
  call [fopen]
  add esp, 8

  push dword [esp + 12] ; lParam
  push dword [esp + 12] ; wParam
  cmp eax, 0 ; fd == 0
  je .nfd
  push dword file_fmt
  push dword eax ; fd
  call [fprintf]
  call [fclose]
  add esp, 8
  .nfd:

  push dword [ebp + 12] ; code
  push dword [hook]
  call [CallNextHookEx]

  ret 12

hookFunc_:
  push dword ebp
  mov ebp, esp

  push dword file_mode
  push dword file_name
  call [fopen]
  add esp, 8
  mov [fd], eax

  push dword [ebp + 16] ; lParam
  push dword [ebp + 12] ; wParam
  push dword file_fmt
  push dword [fd]
  call [fprintf]
  add esp, 16

  push dword [fd]
  call [fclose]
  add esp, 4

  xor eax, eax

  push dword [ebp + 16] ; lParam
  push dword [ebp + 12] ; wParam
  push dword [ebp + 8] ; code
  push dword [hook]
  call [CallNextHookEx]

  pop ebp
  ret 12

export install
global install
install:
  push dword 0 ; thread id
  push dword [module]
  push dword hookFunc ; hook proc
;  push dword WH_KEYBOARD ; hook id
  push dword WH_GETMESSAGE ; hook id
  call [SetWindowsHookExA]
  mov [hook], eax
  ret 0

export uninstall
global uninstall
uninstall:
  xor eax, eax
  xchg dword [hook], eax ; hook <-> 0
  cmp eax, 0
  je .end ; hook == 0

  push eax
  call [UnhookWindowsHookEx]

  .end:
  ret 0

start:
..start: ; module, reason, reserved
  mov eax, [esp + 4]
  mov [module], eax

  cmp dword [esp + 8], DLL_PROCESS_ATTACH
  jne .end
    ; reason == DLL_PROCESS_ATTACH
    push dword [module]
    call [DisableThreadLibraryCalls]

    push dword file_mode
    push dword file_name
    call [fopen]
    add esp, 8

    cmp eax, 0 ; fd == 0
    je .nfd1
    push dword msg_attach
    push dword eax ; fd
    call [fprintf]
    call [fclose]
    add esp, 8
    .nfd1:

    jmp .end

  cmp dword [esp + 8], DLL_PROCESS_DETACH
  jne .end
    ; reason == DLL_PROCESS_DETACH
    call [uninstall]

    push dword file_mode
    push dword file_name
    call [fopen]
    add esp, 8

    cmp eax, 0 ; fd == 0
    je .nfd2
    push dword msg_detach
    push dword eax ; fd
    call [fprintf]
    call [fclose]
    add esp, 8
    .nfd2:

  .end:
  mov eax, 1 ; return TRUE
  ret 12
