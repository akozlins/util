
javac Test.java
javah Test

nasm -fobj -o Test.obj Test.asm
"c:/develop/alink" -dll -oPE -o Test.dll Test.obj
java Test
