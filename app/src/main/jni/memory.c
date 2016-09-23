//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>


jchar my_program[] = {0xA9, 0x52,
                      0x48,
                      0xA9, 0x07,
                      0x20, 0x0A, 0x00,
                      0x68,
                      0x00,
                      0xE9, 0x06,
                      0x60 };

jchar mainMem[65536];

jchar memory_read(int address) {
  return mainMem[address];
}

void memory_write(int address, jchar value) {
  mainMem[address] = value;
}

void
Java_com_johan_emulator_MainActivity_memoryInit(JNIEnv* pEnv, jobject pObj)
{
  memcpy(mainMem, my_program, sizeof(my_program));
}


jcharArray
Java_com_johan_emulator_MainActivity_dump(JNIEnv* pEnv, jobject pObj)
{
  jcharArray result;
  result = (*pEnv)->NewCharArray(pEnv,sizeof(mainMem)/sizeof(mainMem[0]));
  (*pEnv)->SetCharArrayRegion(pEnv,result,0,sizeof(mainMem)/sizeof(mainMem[0]),mainMem);
  return result;
}


