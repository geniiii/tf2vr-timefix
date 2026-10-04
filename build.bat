@echo off
setlocal EnableDelayedExpansion

set compiler=cl.exe
set build_type=release
set build_options=/D_CRT_SECURE_NO_WARNINGS

if /i "%build_type%"=="debug" (
    set MSVC_RUNTIME=/MTd
    set LINK_DEBUG=/Debug:full
) else (
    set MSVC_RUNTIME=/MT /O2
    set LINK_DEBUG=/Debug:none
)

where /Q cl.exe || (
  set "PATH=!PATH!;%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
  for /f "tokens=*" %%i in ('vswhere.exe -latest -products * -prerelease -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath') do set "VS=%%i"
  if not defined VS (
    echo ERROR: Visual Studio installation not found
    exit /b 1
  )
  call "!VS!\VC\Auxiliary\Build\vcvarsall.bat" x64 || exit /b 1
)

if "%VSCMD_ARG_TGT_ARCH%" neq "x64" (
  echo ERROR: Please run this script from the MSVC x64 Native Tools command prompt
  exit /b 1
)

if "%compiler%"=="clang-cl.exe" (
    set external_flag=/imsvc
) else (
    set external_flag=/external:I
)

set compile_flags=%external_flag% ../ext/ /utf-8 /TC /Zc:__STDC__ /std:clatest /nologo /Zi /FC /W4 /wd4100 /wd4701 /GR- /EHsc %MSVC_RUNTIME%
if "%compiler%"=="clang-cl.exe" (
    set compile_flags=%compile_flags% -Wno-missing-braces -Wno-unused-function -Wno-missing-declarations -Wno-unused-parameter -fdiagnostics-absolute-paths -fuse-ld=lld-link
) else (
    set compile_flags=%compile_flags% /experimental:external /external:W0
)

set link_flags=User32.lib -opt:ref %LINK_DEBUG%

if not exist build mkdir build
pushd build

echo Compiling tf2vr_timefix.dll in %build_type% mode...
%compiler% %build_options% %compile_flags% ../build.c /LD /link %link_flags% /out:tf2vr_timefix.dll
popd
