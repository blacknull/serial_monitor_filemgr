#include <Arduino.h>

/* flash file system ---------------------------------------------------------*/
#include "FS.h"
#if defined(FS_SPIFFS)
    #ifdef ESP32
        #include "SPIFFS.h"
    #else
    #endif

    #define MyFs SPIFFS
    const String FS_PREFIX = "/spiffs/";
#elif defined(FS_LITTLEFS)
    #include <LittleFS.h>

    #define MyFs LittleFS
    const String FS_PREFIX = "/littlefs/";
#else    
#endif


String storage_file_prefix(const char* file_name) {
    if (!file_name)
        return "";

    String strFile = file_name;
    if (!strFile.startsWith(FS_PREFIX))
        strFile = String(FS_PREFIX) + strFile;

    strFile.replace("//", "/");
    return strFile;
}

int storage_listfiles(const char* pszFileExt) {
    File root = MyFs.open("/");
    if (!root) {
        Serial.println("- failed to open root directory");
        return -1;
    }

    int count = 0;
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            continue;
        } 
        else {
            String strName = file.name();
            if (!pszFileExt || strName.endsWith(String(pszFileExt))) {
                Serial.println(String(file.name()) + " --- " + String(file.size()) + " bytes.");
                count++;
            }            
        }

        file = root.openNextFile();
    }
    root.close();
    return count;
}

void download_file_with_serial(String strFile)
{
    strFile = storage_file_prefix(strFile.c_str());

    FILE *fp = fopen(strFile.c_str(), "r");
    if (!fp)
    {
        Serial.printf("failed to open %s.\n", strFile.c_str());
        return;
    }

    // begin send file data
    Serial.print("[save]");

    // file name section
    String file_name = strFile;
    file_name.replace(FS_PREFIX, "");
    Serial.printf("[file]%s[/file]", file_name.c_str());

    // data section
    unsigned int count = 0;
    int c = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        Serial.printf("%02x,", c);
        count++;
    }

    fclose(fp);
    Serial.print("[/save]");
    Serial.flush();
    Serial.printf("%d bytes sended.\n", count);
}

void list_files_with_serial()
{
    storage_listfiles(NULL);
}

void del_file_with_serial(String strFile)
{
    strFile = storage_file_prefix(strFile.c_str());
    std::remove(strFile.c_str());
}

void rename_file_with_serial(String old_name, String new_name)
{
    old_name = storage_file_prefix(old_name.c_str());
    new_name = storage_file_prefix(new_name.c_str());

    std::rename(old_name.c_str(), new_name.c_str());
}

void serial_process()
{
    if (!Serial.available())
        return;

    String the_cmd = Serial.readString();
    Serial.printf("received cmd: %s", the_cmd.c_str());
    the_cmd.replace("\n", "");
    the_cmd.replace("\r", "");

    int idx_space = the_cmd.indexOf(' ');
    if (-1 != idx_space)
    {
        String cmd_type = the_cmd.substring(0, (unsigned int)idx_space);
        String file_name = the_cmd.substring((unsigned int)(idx_space + 1));
        if (cmd_type == "download" || cmd_type == "get")
        {            
            download_file_with_serial(file_name);
        }
        else if (cmd_type == "del" || cmd_type == "remove" || cmd_type == "rm")
        {
            del_file_with_serial(file_name);
            list_files_with_serial();
        }
        else if (cmd_type == "rename" || cmd_type == "rn")        
        {
            String params = the_cmd.substring((unsigned int)(idx_space + 1));
            idx_space = params.indexOf(' ');
            String old_name = params.substring(0, (unsigned int)idx_space);
            String new_name = params.substring((unsigned int)(idx_space + 1));

            rename_file_with_serial(old_name, new_name);
            list_files_with_serial();
        }
        else
        {
            Serial.printf("unknow cmd type: %s\n", cmd_type.c_str());
        }
    }
    else {
        if (the_cmd == "list" || the_cmd == "dir" || the_cmd == "ls") {
            list_files_with_serial();
        }
        else {
            Serial.printf("unknow cmd: %s\n", the_cmd.c_str());
        }
    }
}

void setup() {
    // Serial port initialization
    Serial.begin(115200);
    delay(10);

    if (!MyFs.begin()) {
        Serial.println(" file system init failed!");
        return;
    }
    else {
        Serial.println(" - OK!");
    }
}

void loop() {

  serial_process();

  delay(10);
}