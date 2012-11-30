
call "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat"

cl /O2 /Gy /Zl /EHsc /I"C:/develop/include" *.cpp
link /OPT:REF /OPT:ICF /FIXED /NODEFAULTLIB kernel32.lib libcmt.lib libcpmt.lib ../boost_thread_1_44_0/bin/boost_thread_win32.lib *.obj
