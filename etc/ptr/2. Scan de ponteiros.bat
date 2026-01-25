@echo off
setlocal

cd /d "%~dp0"

where python >nul 2>&1
if errorlevel 1 (
    echo [ERRO] Python nao encontrado no PATH.
    echo Instale o Python ou adicione ao PATH.
    pause
    exit /b 1
)

if not exist "scanner.py" (
    echo [ERRO] scanner.py nao encontrado.
    pause
    exit /b 1
)

if not exist "unpacked_Ragexe.exe" (
    echo [ERRO] unpacked_Ragexe.exe nao encontrado.
    pause
    exit /b 1
)

python scanner.py unpacked_Ragexe.exe
if errorlevel 1 (
    echo.
    echo [ERRO] Scanner retornou erro.
    pause
    exit /b 1
)

COLOR 20
echo.
echo [OK] Scanner executado com sucesso.
pause
