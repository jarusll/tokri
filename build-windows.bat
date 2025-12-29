@echo off
setlocal enabledelayedexpansion

rem Product version from first argument
set PRODUCT_VERSION=%1

rem Paths
set EXE=build\Release\Tokri.exe
set OUTDIR=build\Windows
set ICON=resources\net.surajyadav.Tokri.ico
set PRODUCT_NAME=Tokri

rem Deploy Qt
windeployqt --dir "%OUTDIR%" "%EXE%"

rem Copy exe to release (overwrite)
copy /Y "%EXE%" "%OUTDIR%\Tokri.exe"

rem Set properties
rcedit "%OUTDIR%\Tokri.exe" ^
  --set-icon "%ICON%" ^
  --set-version-string "ProductName" "%PRODUCT_NAME%" ^
  --set-product-version "%PRODUCT_VERSION%"

endlocal
