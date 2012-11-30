
rem bash -exec 'gcc -O2 permgen.c -S -masm=intel -o permgen.c.asm'
rem bash -exec 'gcc -O2 permgen.c -o permgen.c.exe'

nasm -fobj -o permgen.asm.obj permgen.asm
"c:/develop/alink.exe" -oPE -subsys con -o permgen.asm.exe permgen.asm.obj

