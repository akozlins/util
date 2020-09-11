
call "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat"

@set FileName=main
rm %FileName%.obj %FileName%.exe
cl /Oi /Gy /GS- /GR- %FileName%.cpp /FAs
link /out:%FileName%.exe /OPT:REF /OPT:ICF %FileName%.obj dbghelp.lib
