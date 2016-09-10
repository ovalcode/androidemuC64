//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>


jchar my_program[] = {0xa9, 0x0a, 0x69, 0x0f, 0xa9, 0x32, 0x69, 0x32,
                      0xa9, 0xe7, 0x69, 0xce, 0xa9, 0x88, 0x69, 0xec,
                      0xa9, 0x43, 0x69, 0x3c, 0xa9, 0x0f, 0xe9, 0x0a,
                      0xa9, 0x0a, 0xe9, 0x0d, 0xa9, 0x78, 0xe9, 0xf9,
                      0xa9, 0x88, 0xe9, 0xf8, 0xa2, 0xfe, 0xe8, 0xe8,
                      0xca, 0xca, 0xa9, 0xfe, 0x85, 0x64, 0xe6, 0x64,
                      0xe6, 0x64, 0xc6, 0x64, 0xc6, 0x64};

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


