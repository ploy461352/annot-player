:: annot-player.update.cmd
:: 11/5/2011
setlocal
cd /d d:/devel/releases/player || exit /b 1

set VERSION=0.1.2.2
set APP=annot-player
set ZIPFILE=%APP%-%VERSION%-win.zip

set MSVC_HOME=/Volumes/win/Windows/System32
set MSVC_DLLS=msvcp100.dll,msvcr100.dll

set QT_HOME=/Volumes/win/qt/current
set QT_DLLS=QtCore4.dll,QtGui4.dll,QtNetwork4.dll,QtSql4.dll,QtWebkit4.dll,QtXml4.dll,phonon4.dll

set ITH_HOME=/Volumes/win/dev/ith
set ITH_DLLS=ITH.dll,ITH_engine.dll

set ZLIB_HOME=/Volumes/win/dev/zlib
set ZLIB_DLL=zlib1.dll

set VLC_HOME=/Volumes/win/Program Files/VideoLAN/VLC
set VLC_DLLS=libvlc.dll,libvlccore.dll
set VLC_PLUGINS=plugins,lua

set BUILD=/Volumes/local/devel/annot-build-desktop/build.win
set SOURCE=/Volumes/local/devel/annot

:: deploy into app dir

rm -Rf "%APP%"
mkdir "%APP%"
cd "%APP%" || exit /b 1

cp -v "%BUILD%/Annot Player.exe" . || exit /b 1

cp -v "%SOURCE%/README" "Read Me.txt" || exit /b 1
unix2dos "Read Me.txt"

cp -v "%SOURCE%/ChangeLog" ChangeLog.txt || exit /b 1
unix2dos ChangeLog.txt

rm -Rf licenses
cp -R "%SOURCE%/licenses" .
cp "%SOURCE%/COPYING" licenses/COPYING.txt || exit /b 1

:: deploy modules
mkdir modules
cd modules || exit /b 1

mkdir imageformats
cp -v "%QT_HOME%"/plugins/imageformats/*4.dll imageformats/ || exit /b 1
rm -f imageformats/*d4.dll

mkdir sqldrivers
cp -v "%QT_HOME%"/plugins/sqldrivers/qsqlite4.dll sqldrivers/ || exit /b 1

cp -v "%QT_HOME%"/bin/{%QT_DLLS%} . || exit /b 1

cp -v "%MSVC_HOME%"/{%MSVC_DLLS%} . || exit /b 1
cp -v "%ITH_HOME%"/bin/{%ITH_DLLS%} . || exit /b 1
cp -v "%ZLIB_HOME%"/bin/%ZLIB_DLL% . || exit /b 1

cp -Rv "%VLC_HOME%"/{%VLC_PLUGINS%} . || exit /b 1
rm -fv plugins/*.dat*

::cp -v "%BUILD%"/*.{exe,dll} . || exit /b 1
cp -v "%BUILD%"/*.{exe,dll} .

rm -f "Annot Player.exe"
rm -fv hook.dll

cd ..

:: repair permissions

chown -R jichi .
chmod -R 755 .

:: archive
::call "Delete Caches.cmd"

cd ..

rm -f "%ZIPFILE%"
zip -9r "%ZIPFILE%" "%APP%" > nul

:: EOF
