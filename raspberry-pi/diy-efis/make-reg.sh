#!/bin/sh

if [ -f diy-efis.reg ]
 then
   rm diy-efis.reg
 fi
../regimport/Debug/regimport -o diy-efis.reg ../../../configs/PiPFD.ini

