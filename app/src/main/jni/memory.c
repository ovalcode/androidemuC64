//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>


jchar my_program[] = {0xa9, 0xd0, 0xa2, 0x01, 0x85, 0x96, 0xa9, 0x07, 0x95, 0x96,
                    0x85, 0x99, 0xa9, 0xd1, 0x85, 0x98, 0xa9, 0x55, 0xa2, 0x02, 
                    0x81, 0x96, 0xa0, 0x02, 0xa9, 0x56, 0x91, 0x96, 0xad, 0xd1, 
                    0x07, 0x9d, 0xd1, 0x07};

jchar mainMem[65536];

jchar memory_read(int address) {
  return mainMem[address];
}

void memory_write(int address, jchar value) {
  mainMem[address] = value;
}

void
Java_com_johan_emulator_MainActivity_memory_init(JNIEnv* pEnv, jobject pObj)
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


