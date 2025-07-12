#ifndef LOCAL_STORAGE_LIB
#define LOCAL_STORAGE_LIB
#include <Arduino.h>
#include <Preferences.h>
bool IsMemoryHaveBeenOpened();
void saveString(const char* key,String value);
void saveInt_32(const char* key,int value);
void saveFloat(const char* key, float value);
String getString(const char* key);
float getFloat(const char* key);
int32_t getInt(const char* key);
void saveBool(const char* key, bool value);
bool getBool(const char* key);
#endif