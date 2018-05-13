@echo cleaning...
@echo del /q /s *.pdb *.aps *.opt *.plg *.ncb *.ilk *.update *.exe
del /q /s *.pdb *.aps *.opt *.plg *.ncb *.ilk *.update *.exe
;--------------------x86------------------------
@echo rd /q /s "AutoUpdateConfig\Debug"
rd /q /s "AutoUpdateConfig\Debug"
@echo rd /q /s "AutoUpdate\Debug"
rd /q /s "AutoUpdate\Debug"
set hh=%time:~0,2%
if /i %hh% LSS 10 (
    set hh=0%hh:~1,1%
)
set filename=AutoUpdate_%date:~0,4%年%date:~5,2%月%date:~8,2%日%hh%%time:~3,2%%time:~6,2%.7z
"D:\Program Files\HaoZip\HaoZipc" a -t7z %filename% -r *.*
xcopy %filename% ..\备份
del %filename%