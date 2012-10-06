
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

cl /O2 /Gy /Zl /EHsc /I"C:/develop/include" *.cpp
link /OPT:REF /OPT:ICF /FIXED /NODEFAULTLIB kernel32.lib libcmt.lib libcpmt.lib ../boost_thread_1_44_0/bin/boost_thread_win32.lib *.obj
