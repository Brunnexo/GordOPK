@ECHO OFF
COLOR 80
CHCP 65001
CLS
SETLOCAL EnableExtensions EnableDelayedExpansion

ECHO #####################################################
ECHO #####################################################
ECHO ##                Gordao Programas                 ##
ECHO ## Instalador Python 3.11 x86 + pefile + unlicense ##
ECHO ##       Desenvolvido por: Bruno Costa - 2026      ##
ECHO #####################################################
ECHO #####################################################

SET PYTHON_VERSION=3.11.9
SET PYTHON_EXE=python-3.11.9.exe
SET PYTHON_URL=https://www.python.org/ftp/python/3.11.9/python-3.11.9.exe

python --version >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    ECHO Python ja instalado.
    goto INSTALL_DEPS
)


ECHO Baixando Python %PYTHON_VERSION% (32 bits)...
ECHO.
powershell -Command ^
    "Try { Invoke-WebRequest -Uri '%PYTHON_URL%' -OutFile '%PYTHON_EXE%' -UseBasicParsing } Catch { Exit 1 }"

if not exist "%PYTHON_EXE%" (
    ECHO.
    COLOR 40
    ECHO [ERRO] Falha ao baixar o Python 32 bits.
    PAUSE
    exit /b 1
)

ECHO Instalando Python %PYTHON_VERSION% (32 bits)...
ECHO.
"%PYTHON_EXE%" /quiet ^
    InstallAllUsers=1 ^
    PrependPath=1 ^
    Include_test=0

if %ERRORLEVEL% NEQ 0 (
    ECHO.
    COLOR 40
    ECHO [ERRO] Falha na instalacao do Python.
    PAUSE
    exit /b 1
)

REM Atualiza PATH da sessao atual (padrao x86)
SET PATH=%PATH%;C:\Program Files (x86)\Python311\;C:\Program Files (x86)\Python311\Scripts\

:INSTALL_DEPS
ECHO.
ECHO Instalando biblioteca pefile...
ECHO.

python -m pip install --upgrade pip
python -m pip install pefile

if %ERRORLEVEL% NEQ 0 (
    ECHO.
    COLOR 40
    ECHO [ERRO] Falha ao instalar pefile.
    PAUSE
    exit /b 1
)

ECHO.
ECHO Instalando biblioteca unlicense...
ECHO.

pip install git+https://github.com/ergrelet/unlicense.git

if %ERRORLEVEL% NEQ 0 (
    ECHO.
    COLOR 40
    ECHO [ERRO] Falha ao instalar unlicense.
    PAUSE
    exit /b 1
)


ECHO.
CLS
python --version
python -c "import pefile; print('pefile instalado com sucesso')"
python -c "import unlicense; print('unlicense instalado com sucesso')"

COLOR 20
ECHO.
ECHO.
ECHO #######################################
ECHO ## Instalacao concluida com sucesso  ##
ECHO #######################################
ECHO.
PAUSE