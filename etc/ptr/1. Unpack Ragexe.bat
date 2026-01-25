@ECHO OFF
COLOR 80
CHCP 65001
CLS
SETLOCAL EnableExtensions EnableDelayedExpansion

SET SCRIPT_DIR=%~dp0

SET TARGET_FOLDER=C:\Gravity\Ragnarok
SET TARGET_EXE=Ragexe.exe

SET UNPACKED_EXE=%TARGET_FOLDER%\unpacked_%TARGET_EXE%

ECHO #####################################################
ECHO #####################################################
ECHO ##                Gordao Programas                 ##
ECHO ##              unpacker Ragexe.exe                ##
ECHO ##       Desenvolvido por: ergrelet - 2022         ##
ECHO #####################################################
ECHO #####################################################

if not exist "%TARGET_FOLDER%\%TARGET_EXE%" (
    ECHO.
    ECHO [ERRO] Arquivo nao encontrado:
    ECHO %TARGET_EXE%
    PAUSE
    exit /b 1
)

REM -------------------------------------------------
REM Executa unlicense
REM -------------------------------------------------
ECHO.
ECHO Executando unlicense...
ECHO.

cd %TARGET_FOLDER%
python -m unlicense "%TARGET_EXE%"

if %ERRORLEVEL% NEQ 0 (
    ECHO.
    ECHO [ERRO] Falha ao executar unlicense.
    PAUSE
    exit /b 1
)

REM -------------------------------------------------
REM Verifica se o unpacked foi criado
REM -------------------------------------------------
if not exist "%UNPACKED_EXE%" (
    ECHO.
    ECHO [ERRO] Unpacked nao encontrado:
    ECHO %UNPACKED_EXE%
    PAUSE
    exit /b 1
)

REM -------------------------------------------------
REM Move o unpacked para a pasta do script
REM -------------------------------------------------
ECHO.
ECHO Movendo executavel unpacked para a pasta do script...
ECHO.

move /Y "%UNPACKED_EXE%" "%SCRIPT_DIR%" >nul

if not exist "%SCRIPT_DIR%\unpacked_%TARGET_EXE%" (
    ECHO.
    ECHO [ERRO] Falha ao mover o executavel unpacked.
    PAUSE
    exit /b 1
)

COLOR 20
ECHO.
ECHO ==========================================
ECHO  Processo finalizado
ECHO ==========================================
PAUSE
