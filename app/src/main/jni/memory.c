//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>
#include <android/asset_manager_jni.h>


jchar my_program[] = {
};

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
  //memcpy(mainMem, my_program, sizeof(my_program));
}


void Java_com_johan_emulator_engine_Emu6502_loadROMS(JNIEnv* env, jobject pObj, jobject pAssetManager) {
  AAssetManager* assetManager = AAssetManager_fromJava(env, pAssetManager);
  AAsset* assetF = AAssetManager_open(assetManager, "basic.bin", AASSET_MODE_UNKNOWN);
  uint8_t buffer[8192];
  AAsset_read(assetF, buffer, 8192);
  int i;
  for (i = 0xa000; i < 0xc000; i++) {
    mainMem[i] = buffer[i & 0x1fff];
  }
  AAsset_close(assetF);

  assetF = AAssetManager_open(assetManager, "kernal.bin", AASSET_MODE_UNKNOWN);
  AAsset_read(assetF, buffer, 8192);

  for (i = 0xe000; i < 0x10000; i++) {
    mainMem[i] = buffer[i & 0x1fff];
  }
  AAsset_close(assetF);

}


jcharArray
Java_com_johan_emulator_engine_Emu6502_dump(JNIEnv* pEnv, jobject pObj)
{
  jcharArray result;
  result = (*pEnv)->NewCharArray(pEnv,sizeof(mainMem)/sizeof(mainMem[0]));
  (*pEnv)->SetCharArrayRegion(pEnv,result,0,sizeof(mainMem)/sizeof(mainMem[0]),mainMem);
  return result;
}


