$Filename = "C:\tmp\diy-efis.app"

Compress-Archive -Path framework\CanFly.DiyEfis\bin\Debug\*.pe \tmp\diy-efis.zip

if(Test-Path $Filename)
 {
 Remove-Item $Filename 
 }
 
 Rename-Item -Path \tmp\diy-efis.zip $Filename
 
