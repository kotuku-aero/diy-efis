#!/bin/sh

if [ -f diy-efis.reg ]
 then
   rm diy-efis.reg
 fi
../../tools/regimport/Debug/regimport -o diy-efis.reg test.ini

