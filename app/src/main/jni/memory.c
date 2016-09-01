//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>

jchar mainMem[] = {0xa9, 0x20, 0x85,0x8,0,0,0,0,0,0,0};

jchar memory_read(int address) {
  return mainMem[address];
}

void memory_write(int address, jchar value) {
  mainMem[address] = value;
}

jcharArray
Java_com_johan_emulator_MainActivity_dump(JNIEnv* pEnv, jobject pObj)
{
  jcharArray result;
  result = (*pEnv)->NewCharArray(pEnv,sizeof(mainMem)/sizeof(mainMem[0]));
  (*pEnv)->SetCharArrayRegion(pEnv,result,0,sizeof(mainMem)/sizeof(mainMem[0]),mainMem);
  return result;
}


