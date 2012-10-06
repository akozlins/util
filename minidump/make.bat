
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

@set FileName=main
rm %FileName%.obj %FileName%.exe
cl /Oi /Gy /GS- /GR- %FileName%.cpp /FAs
link /out:%FileName%.exe /OPT:REF /OPT:ICF %FileName%.obj dbghelp.lib
