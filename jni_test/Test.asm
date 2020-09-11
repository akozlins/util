
section .text

export Java_Test_sum
global Java_Test_sum

segment code public use32

Java_Test_sum:
  mov eax, [esp + 12]
  add eax, [esp + 16]
  ret 16

..start:
  mov eax, 1
  ret 12
