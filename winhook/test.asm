
MB_OK equ 0

%macro API 1-*
  %define dll %1
  %rep %0 - 1
    extern %2
    import %2 dll
    %rotate 1
  %endrep
%endmacro

API kernel32.dll, ExitProcess
API user32.dll, MessageBoxA

API hook.dll, hookFunc, install, uninstall

section .data use32

mb_title db "hello", 0
mb_msg db "hello world", 0

i dd 0xFFFFFFFF
fmt db "%d", 0

section .code use32 class=code

..start:
  call [install]

  push dword MB_OK
  push dword mb_title
  push dword mb_msg
  push dword 0
  call [MessageBoxA]

  call [uninstall]

  push 0
  call [ExitProcess]
