#include "local_storage_lib.h"
#define NAME_SPACE "S_P"
Preferences localStorage;

bool IsMemoryHaveBeenOpened(){
    localStorage.begin(NAME_SPACE,false);
    bool value = localStorage.isKey("mem");
    if(value)localStorage.putBool("mem",true);
    localStorage.end();
    return value;
}

void saveString(const char* key,String value){
    localStorage.begin(NAME_SPACE,false);
    localStorage.putString(key,value);
    localStorage.end();
}

void saveInt_32(const char* key,int value){
    localStorage.begin(NAME_SPACE,false);
    localStorage.putInt(key,value);
    localStorage.end();
}

void saveFloat(const char* key, float value){
    localStorage.begin(NAME_SPACE,false);
    localStorage.putInt(key,value);
    localStorage.end();
}

String getString(const char* key){
    localStorage.begin(NAME_SPACE,false);
    String value = localStorage.getString(key);
    localStorage.end();
    return value;
}

float getFloat(const char* key){
    localStorage.begin(NAME_SPACE,false);
    float value = localStorage.getFloat(key);
    localStorage.end();
    return value;
}

int32_t getInt(const char* key){
    localStorage.begin(NAME_SPACE,false);
    int32_t value = localStorage.getInt(key);
    localStorage.end();
    return value;
}

void saveBool(const char* key, bool value){
    localStorage.begin(NAME_SPACE,false);
    localStorage.putBool(key,value);
    localStorage.end();
}

bool getBool(const char* key){
    localStorage.begin(NAME_SPACE,false);
    bool value = localStorage.getBool(key);
    localStorage.end();
    return value;
}