del /Q /S /F %2\*.* 
robocopy %1 %2 *.fx *.mgp *.mat *.fxh *.rfx *.bam /S
exit 0