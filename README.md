# BmsBel+ Library & related simple utilities

### BmsBel+
- C++ Library for *.bmx(*.bms, *.bme, *.bml) file read
- have dependency with ```libiconv```
- original code by CHILD

### Bmx2Ogg
- Simple utility which converts *.bmx file to audio file. (but has less function)

### SimpleBMXPlayer
- Just shows and plays *.bmx file with various options.

### BmsTest
- Simple test solution for code coverage. (don't need to compile)

##### Included Libraries are: ```libiconv 1.14```, ```libpng```, ```libogg```, ```libvorbis```. I don't own these files and all copyrights are reserved to them.

##### You can visit each folder for more detailed information.

##### Project is compiled with VC++ 2013.
- since I don't have Mac, I hadn't tested with that.
- I'll test this module in minGW soon.

##### This Project doesn't include ```libSDL 2.0```, ```FFmpeg```
- download ```Dev Lib / Runtime Binaries``` directly from [SDL](https://www.libsdl.org/download-2.0.php), and extract file to ```include/SDL, lib/x86, Release```.
- download ffmpeg from [Zeranoe FFmpeg](http://ffmpeg.zeranoe.com/builds/), and extract to ```include/ffmpeg, lib/ffmpeg, Release```