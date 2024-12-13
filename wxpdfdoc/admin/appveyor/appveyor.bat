set MSBUILD_LOGGER=/logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
goto %TOOLSET%

:msbuild
cd build
msbuild /m:2 /v:n /p:Platform=%ARCH% /p:Configuration="%CONFIGURATION%" wxpdfdoc_vc17.sln %MSBUILD_LOGGER%
goto :eof

:error
echo.
echo --- Build failed !
echo.
