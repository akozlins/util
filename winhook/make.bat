
nasm -fobj -o hook.obj hook.asm
"c:/develop/alink" -dll -oPE -o hook.dll hook.obj

nasm -fobj -o test.obj test.asm
"c:/develop/alink" -oPE -o test.exe test.obj
