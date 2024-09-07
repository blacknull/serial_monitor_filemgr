## Serial Monitor File Manager
### first of all 
---
It's always hard to download files from the flash memeory of esp32 board.

You can use a kind of web service to download file with wifi access, but have to upload a specific "server firmware" first, and then open a web browser to visit a certian of url to acess file manager web page... too many steps, too complicated.

Most of all, this web service may have nothing common with your code, and take too much memeory of your esp32 to integrated into your project.

So you have to switch from file service and your project time to time, it's very annoyed.

And this file manager provide a light weight and very easy way to manage esp32 file system like spiffs or littlefs, just with serial monitor.

### how to use
---
* code add to your project
  - copy sm_filemgr.h and sm_filemgr.cpp to your project dir or lib dir.
  - #include "sm_filemgr.h"
  - sm_filemgr_init(); add in setup(). 
  - sm_filemgr_process(); add in loop().
* copy file_mgr.py to C:\Users\\<font color=red>**NAME**</font>\\\.platformio\platforms\espressif32\monitor,  please replace NAME with your login name in windows.
* modify your platformio.ini, add following lines into the section of your config
  - board_build.filesystem = littlefs ;spiffs
  - build_flags = -D FS_LITTLEFS ;FS_SPIFFS
  - monitor_filters = default, file_mgr
* ok, that's all. now your can compile and upload the firmware to esp32, then open Serial Monitor.
  * click termianl window and you can input file manage command.
      * dir/list/ls --- list all files in your spiffs/littlefs
      * del/rm/remove filename --- remove a specific file
      * rename old_name new_name --- rename a file
      * download/get filename -- download a specific file to local 'data' dir

### to do list
---
* at this version, you can't see the command inputed, that's embarrased.
* upload local file in computer to esp32, it's not easy to me, but should have.
* support SD card?
  


