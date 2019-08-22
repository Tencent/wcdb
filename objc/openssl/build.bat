call vcvars32.bat
cd ..\..\src

perl Configure VC-WIN32 --prefix=..\win\x86
call ms\do_nasm.bat
nmake -f ms\ntdll.mak clean
nmake -f ms\ntdll.mak
nmake -f ms\ntdll.mak install

cd ..\win\x86