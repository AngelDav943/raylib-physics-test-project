@echo off
setlocal enabledelayedexpansion 

:: Input images
set foreground=android_launcher_foreground.png
set background=android_launcher_background.png

:: Specify the path to the res folder
:: Adjust this path to the actual location of your "res" folder
set res_base_path=..\res

:: Check if input files exist
if not exist %foreground% (
    echo ERROR: File %foreground% not found!
    exit /b
)
if not exist %background% (
    echo ERROR: File %background% not found!
    exit /b
)

if not exist %res_base_path% (
	echo ERROR: res path doesn't exist, generating path
	mkdir %res_base_path%
)

:: List of resolutions and corresponding folders
:: set resolutions=108 162 216 324 432
:: set folders=drawable-mdpi drawable-hdpi drawable-xhdpi drawable-xxhdpi drawable-xxxhdpi

:: Array of resolutions and corresponding folders
set resolutions[0]=108
set resolutions[1]=162
set resolutions[2]=216
set resolutions[3]=324
set resolutions[4]=432
set folders[0]=drawable-mdpi
set folders[1]=drawable-hdpi
set folders[2]=drawable-xhdpi
set folders[3]=drawable-xxhdpi
set folders[4]=drawable-xxxhdpi

:: Iterate through resolutions and create folders
for /l %%n in (0, 1, 4) do (

    :: Create folder in the res directory
    mkdir "%res_base_path%\!folders[%%n]!"

	@echo ^<?xml version="1.0" encoding="utf-8"?^> > "%res_base_path%\!folders[%%n]!\launcher_icon.xml"
	@echo ^<adaptive-icon xmlns:android="http://schemas.android.com/apk/res/android"^> >> "%res_base_path%\!folders[%%n]!\launcher_icon.xml"
	@echo  ^<background android:drawable="@drawable/ic_launcher_background"/^> >> "%res_base_path%\!folders[%%n]!\launcher_icon.xml"
	@echo  ^<foreground android:drawable="@drawable/ic_launcher_foreground"/^> >> "%res_base_path%\!folders[%%n]!\launcher_icon.xml"
	@echo ^</adaptive-icon^> >> "%res_base_path%\!folders[%%n]!\launcher_icon.xml"

	
    :: Resize images with FFmpeg
    ffmpeg -i %foreground% -vf scale=!resolutions[%%n]!:!resolutions[%%n]! "%res_base_path%\!folders[%%n]!\ic_launcher_foreground.png" -y
    ffmpeg -i %background% -vf scale=!resolutions[%%n]!:!resolutions[%%n]! "%res_base_path%\!folders[%%n]!\ic_launcher_background.png" -y
)
endlocal

echo All images resized and saved in the appropriate folders.