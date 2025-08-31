@echo off
setlocal ENABLEEXTENSIONS
REM =========================================
REM  ScreenTranslate / QSettings 清理脚本
REM  目标键：HKCU\Software\ScreenTranslate\ScreenTranslate
REM  用法：clear_screentranslate_settings.bat [all|apikey]
REM        all    删除整个设置（默认）
REM        apikey 仅删除 apiKey 值
REM =========================================

set "KEY=HKCU\Software\ScreenTranslate\ScreenTranslate"

echo [信息] 将尝试备份当前配置到桌面: ScreenTranslate_backup.reg
reg query "%KEY%" >nul 2>&1 && (
  reg export "%KEY%" "%USERPROFILE%\Desktop\ScreenTranslate_backup.reg" /y >nul 2>&1
  echo [√] 备份完成（若存在）。
) || (
  echo [!] 未找到该注册表键，可能尚未生成或已被清除。
)

if "%~1"=="" goto DO_ALL
if /I "%~1"=="all" goto DO_ALL
if /I "%~1"=="apikey" goto DO_APIKEY

echo.
echo 用法: %~nx0 [all|apikey]
echo   all    删除整個設置（包含 apiKey、代理、主题等）
echo   apikey 僅刪除 apiKey 值
goto END

:DO_ALL
echo.
echo [操作] 删除整个设置：%KEY%
reg delete "%KEY%" /f >nul 2>&1 && (
  echo [√] 已清除全部设置。
) || (
  echo [!] 删除失败或键不存在。
)
goto END

:DO_APIKEY
echo.
echo [操作] 仅删除 apiKey 值
reg delete "%KEY%" /v apiKey /f >nul 2>&1 && (
  echo [√] 已清除 apiKey。
) || (
  echo [!] 删除失败或未找到该值。
)
goto END

:END
echo.
pause
endlocal
