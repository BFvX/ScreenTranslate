@echo off
setlocal ENABLEEXTENSIONS
REM =========================================
REM  ScreenTranslate / QSettings ����ű�
REM  Ŀ�����HKCU\Software\ScreenTranslate\ScreenTranslate
REM  �÷���clear_screentranslate_settings.bat [all|apikey]
REM        all    ɾ���������ã�Ĭ�ϣ�
REM        apikey ��ɾ�� apiKey ֵ
REM =========================================

set "KEY=HKCU\Software\ScreenTranslate\ScreenTranslate"

echo [��Ϣ] �����Ա��ݵ�ǰ���õ�����: ScreenTranslate_backup.reg
reg query "%KEY%" >nul 2>&1 && (
  reg export "%KEY%" "%USERPROFILE%\Desktop\ScreenTranslate_backup.reg" /y >nul 2>&1
  echo [��] ������ɣ������ڣ���
) || (
  echo [!] δ�ҵ���ע������������δ���ɻ��ѱ������
)

if "%~1"=="" goto DO_ALL
if /I "%~1"=="all" goto DO_ALL
if /I "%~1"=="apikey" goto DO_APIKEY

echo.
echo �÷�: %~nx0 [all|apikey]
echo   all    ɾ�������O�ã����� apiKey����������ȣ�
echo   apikey �H�h�� apiKey ֵ
goto END

:DO_ALL
echo.
echo [����] ɾ���������ã�%KEY%
reg delete "%KEY%" /f >nul 2>&1 && (
  echo [��] �����ȫ�����á�
) || (
  echo [!] ɾ��ʧ�ܻ�������ڡ�
)
goto END

:DO_APIKEY
echo.
echo [����] ��ɾ�� apiKey ֵ
reg delete "%KEY%" /v apiKey /f >nul 2>&1 && (
  echo [��] ����� apiKey��
) || (
  echo [!] ɾ��ʧ�ܻ�δ�ҵ���ֵ��
)
goto END

:END
echo.
pause
endlocal
