//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <alarm.h>
#include <timer.h>
#include <interrupts.h>

jchar my_program[] = {
};

jchar mainMem[65536];
jchar charRom[4096];
jchar* g_buffer;
jbyte* keyboardMatrix;

struct timer_struct timerA;
struct timer_struct timerB;

jchar getKeyPortByte(int outPortBits) {
  int temp = 0;
  int i;
  for (i = 0; i < 8; i++) {
    if ((outPortBits & 0x80) == 0)
      temp = temp | keyboardMatrix[i];
    outPortBits = outPortBits << 1;
  }
  return ~temp & 0xff;
}


jchar cia1_read(int address) {
  jchar result = 0;
  switch (address) {
    case 0xdc00:
    break;

    case 0xdc01:
      result = getKeyPortByte(mainMem[0xdc00]);
    break;

    case 0xdc02:      
    break;

    case 0xdc03:
    break;

    case 0xdc04: //timer A low
      result = get_time_low(&timerA);
    break;

    case 0xdc05: //timer A high
      result = get_time_high(&timerA);
    break;

    case 0xdc06: //timer B low
      result = get_time_low(&timerB);
    break;

    case 0xdc07: //timer B high
      result = get_time_high(&timerB);
    break;

    case 0xdc0d: // interrupt control
      result = read_interrupts_register();
    break;

    case 0xdc0e: // control reg a
      result = get_control_reg(&timerA);
    break;

    case 0xdc0f: // control reg b
      result = get_control_reg(&timerB);
    break;

  }
  return result;
}

void cia1_write(int address, int value) {

  switch (address) {
    case 0xdc00:
      mainMem[address] = value;
    break;

    case 0xdc01:
      mainMem[address] = value;
    break;

    case 0xdc02:
    break;

    case 0xdc03:
    break;

    case 0xdc04: //timer A low
      set_timer_low(&timerA, value);
    break;

    case 0xdc05: //timer A high
      set_timer_high(&timerA, value);
    break;

    case 0xdc06: //timer B low
      set_timer_low(&timerB, value);
    break;

    case 0xdc07: //timer B high
      set_timer_high(&timerB, value);
    break;

    case 0xdc0d: // interrupt control
      set_mask(value);
    break;

    case 0xdc0e: // control reg a
      set_control_reg(&timerA, value);
    break;

    case 0xdc0f: // control reg b
      set_control_reg(&timerB,value);
    break;

  }

}


jchar memory_read(int address) {
  if ((address >=0xdc00) & (address < 0xdc10))
    return cia1_read(address);
  else
    return mainMem[address];
}

void memory_write(int address, jchar value) {
  if (((address >= 0xa000) && (address < 0xc000)) |
       ((address >= 0xe000) && (address < 0x10000)))
    return;

  if ((address >=0xdc00) & (address < 0xdc10))
    cia1_write(address, value);
  else
    mainMem[address] = value;
}

void
Java_com_johan_emulator_engine_Emu6502_memoryInit(JNIEnv* pEnv, jobject pObj)
{
  timerA = getTimerInstanceA();
  add_timer_to_list(&timerA);
  timerB = getTimerInstanceB();
  add_timer_to_list(&timerB);
}


void Java_com_johan_emulator_engine_Emu6502_setKeyboardMatrix(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  keyboardMatrix = (jbyte *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);
}


void Java_com_johan_emulator_engine_Emu6502_setFrameBuffer(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  g_buffer = (jchar *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);

}

void Java_com_johan_emulator_engine_Emu6502_populateFrame(JNIEnv* pEnv, jobject pObj) {
  int currentLine;
  int currentCharInLine;
  int currentPixel;
  int currentPosInCharMem = 1024;
  int posInBuffer = 0;
  for (currentLine = 0; currentLine < 200; currentLine++) {
    int currentLineInChar = currentLine & 7;
    if ((currentLine != 0) && ((currentLineInChar) == 0))
      currentPosInCharMem = currentPosInCharMem + 40;
    for (currentCharInLine = 0; currentCharInLine < 40; currentCharInLine++) {
      jchar currentChar = mainMem[currentPosInCharMem + currentCharInLine];
      jchar dataLine = charRom[(currentChar << 3) | currentLineInChar];
      int posToWriteBegin = posInBuffer + (currentCharInLine << 3);
      int posToWrite;
      for (posToWrite = posToWriteBegin; posToWrite < (posToWriteBegin + 8); posToWrite++) {
        if ((dataLine & 0x80) != 0)
          g_buffer[posToWrite] = 0xffff;
        else
          g_buffer[posToWrite] = 0x0;
        dataLine = dataLine << 1;
      }
    }
    posInBuffer = posInBuffer + 320;
  }
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

  assetF = AAssetManager_open(assetManager, "characters.bin", AASSET_MODE_UNKNOWN);
  AAsset_read(assetF, buffer, 4096);

  for (i = 0x0; i < 0x1000; i++) {
    charRom[i] = buffer[i];
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


