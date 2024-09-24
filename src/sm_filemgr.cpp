#include <Arduino.h>
#include "sm_filemgr.h"
#include <vector>
std::vector<String> vctListFile;

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

    vctListFile.clear();

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
                vctListFile.push_back(strName);
                count++;
            }            
        }

        file = root.openNextFile();
    }

    root.close();
    return count;
}

void list_files_with_serial()
{
    storage_listfiles(NULL);
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

void download_files_with_serial(String strFile) {
    if (strFile.startsWith("*.")) {
        if (strFile.endsWith(".*"))
            storage_listfiles(NULL);
        else {
            strFile.remove(0, 1);
            storage_listfiles(strFile.c_str());
        }

        for (const String& file_name : vctListFile) {
            delay(100);
            Serial.println("download file: " + file_name);
            download_file_with_serial(file_name);
        }
    }
    else {
        download_file_with_serial(strFile);
    }
}

void del_file_with_serial(String strFile)
{
    if (strFile.startsWith("*.")) {
        if (strFile.endsWith(".*"))
            storage_listfiles(NULL);
        else {
            strFile.remove(0, 1);
            storage_listfiles(strFile.c_str());
        }

        for (const String& file_name : vctListFile) {
            strFile = storage_file_prefix(file_name.c_str());
            std::remove(strFile.c_str());
            Serial.println("remove file " + strFile);
        }
    }
    else {
        strFile = storage_file_prefix(strFile.c_str());        
        std::remove(strFile.c_str());
        Serial.println("remove file " + strFile);
    }
}

void rename_file_with_serial(String old_name, String new_name)
{
    old_name = storage_file_prefix(old_name.c_str());
    new_name = storage_file_prefix(new_name.c_str());

    std::rename(old_name.c_str(), new_name.c_str());
}

void sm_filemgr_init() {
    if (!MyFs.begin()) {
        Serial.println(" file system init failed!");
    }
    else {
        Serial.println(" file system init succeed!");
    }    
}

void sm_filemgr_process()
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
            download_files_with_serial(file_name);
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
