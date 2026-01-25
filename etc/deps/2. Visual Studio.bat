@echo off
CHCP 65001 >nul
CLS
setlocal EnableExtensions EnableDelayedExpansion

:: Diretório do script
set "ROOT=%~dp0"
set "VS_DIR=%ROOT%vs"

ECHO #####################################################
ECHO #####################################################
ECHO ##                Gordao Programas                 ##
ECHO ##     Instalador Visual Studio Community 2026     ##
ECHO ##             + MSBuild + SDK Windows             ##
ECHO ##       Desenvolvido por: Bruno Costa - 2026      ##
ECHO #####################################################
ECHO #####################################################

:: Verifica se o instalador existe
if not exist "%VS_DIR%\vs_Community.exe" (
    echo ERRO: vs_Community.exe não encontrado em "%VS_DIR%"!
    pause
    exit /b 1
)

:: Verifica se o vsconfig existe
if not exist "%VS_DIR%\vsconfig.json" (
    echo ERRO: vsconfig.json não encontrado em "%VS_DIR%"!
    pause
    exit /b 1
)

echo Iniciando instalação do Visual Studio...
echo Isso pode levar vários minutos.
echo.

"%VS_DIR%\vs_Community.exe" ^
 --quiet ^
 --wait ^
 --norestart ^
 --config "%VS_DIR%\vsconfig.json" ^
 --includeRecommended

set "VS_ERROR=%ERRORLEVEL%"
if %VS_ERROR% NEQ 0 (
    echo.
    echo ERRO: A instalação falhou. Código: %VS_ERROR%
    pause
    exit /b %VS_ERROR%
)

COLOR 20
ECHO.
ECHO.
ECHO #######################################
ECHO ## Instalacao concluida com sucesso  ##
ECHO #######################################
ECHO.
PAUSE
