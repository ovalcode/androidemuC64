//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>


jchar my_program[] = {0xA9, 0xFB, 0xC9, 0x05, 0xA9, 0x05, 0xC9, 0xFB, 0xA9,
                      0xE2, 0xC9, 0x64, 0xA2, 0x40, 0xCA, 0xE0, 0x3A, 0xD0,
                      0xFB, 0xA0, 0x10, 0xC0, 0x23, 0xB0, 0x04, 0xC8, 0x4C,
                      0x15, 0x00, 0x00 };

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


