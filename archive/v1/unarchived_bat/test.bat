for /l %%i in (1,1,10) do (
set str=%%i
echo %str%
echo %%%i:~0,1%
)