@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: ===============================
:: Configurações
:: ===============================
set SOLUTION=GordOPK.slnx
set CONFIG=Release
set PLATFORM=x86
set OUTPUT=Release\GordOPK.asi
set FINAL=GordOPK.asi

echo ========================================
echo   recv.asi Build Script - Visual Studio
echo ========================================
echo.

:: ===============================
:: Localizar vswhere
:: ===============================
set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe

if not exist "%VSWHERE%" (
    echo ERRO: vswhere.exe nao encontrado.
    echo Instale o Visual Studio 2017 ou superior.
    goto :fail
)

:: ===============================
:: Encontrar VS mais recente com MSBuild
:: ===============================
for /f "usebackq delims=" %%I in (`
    "%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
`) do set VSINSTALL=%%I

if not defined VSINSTALL (
    echo ERRO: Nenhuma instalacao valida do Visual Studio foi encontrada.
    goto :fail
)

set VSDEV="%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

if not exist %VSDEV% (
    echo ERRO: VsDevCmd.bat nao encontrado em:
    echo %VSDEV%
    goto :fail
)

echo Visual Studio encontrado em:
echo %VSINSTALL%
echo.

:: ===============================
:: Inicializar ambiente VS
:: ===============================
call %VSDEV% >nul 2>&1
if errorlevel 1 (
    echo ERRO ao configurar ambiente do Visual Studio.
    goto :fail
)

:: ===============================
:: Limpeza
:: ===============================
echo Limpando arquivos anteriores...
del /f /q %FINAL% >nul 2>&1
del /f /q %OUTPUT% >nul 2>&1

:: ===============================
:: Build
:: ===============================
echo Compilando com MSBuild...
msbuild %SOLUTION% /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /verbosity:minimal
if errorlevel 1 goto :fail

:: ===============================
:: Verificação final
:: ===============================
if not exist %OUTPUT% (
    echo Build finalizado, mas recv.asi nao foi encontrado.
    goto :fail
)

copy /y %OUTPUT% %FINAL% >nul

if exist "GordOPK" rmdir /s /q "GordOPK"
if exist "Release" rmdir /s /q "Release"

echo.
echo ========================================
echo   COMPILACAO CONCLUIDA COM SUCESSO!
echo ========================================
dir %FINAL%
goto :end

:fail
echo.
echo ========================================
echo   ERRO NA COMPILACAO!
echo ========================================
echo Verifique as mensagens acima.

:end
echo.
pause
endlocal
