//
// Created by johan on 2016/08/30.
//

#include <string.h>
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <alarm.h>
#include <timer.h>
#include <interrupts.h>
#include <tape.h>
#include <video.h>

#define BASIC_VISIBLE 1
#define KERNAL_VISIBLE 2
#define CHAR_ROM_VISIBLE 4
#define IO_VISIBLE 8

#define JOYSTICK_UP 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 4
#define JOYSTICK_RIGHT 8

int bank_visibility[8] =
{
  0,//000
  CHAR_ROM_VISIBLE,//001
  CHAR_ROM_VISIBLE | KERNAL_VISIBLE,//010
  BASIC_VISIBLE | KERNAL_VISIBLE | CHAR_ROM_VISIBLE,//011
  0,//100
  IO_VISIBLE,//101
  IO_VISIBLE | KERNAL_VISIBLE,//110
  BASIC_VISIBLE | KERNAL_VISIBLE | IO_VISIBLE//111
};

jchar my_program[] = {
};

int joystickStatus = 0;

jchar mainMem[65536];
jchar charRom[4096];
jchar basicROM[8192];
jchar kernalROM[8192];
jchar IOUnclaimed[4096];
jchar* g_buffer;
jbyte* keyboardMatrix;
extern int line_count;

struct timer_struct timerA;
struct timer_struct timerB;
struct timer_struct tape_timer;
struct timer_struct video_timer;

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
      result = 0xff;
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

jchar read_port_1() {
  jchar result = mainMem[1] & 0xcf;
  result = result | (getMotorOnBit(&tape_timer) << 5);
  result = result | (isPlayDownBit() << 4);
  return result;
}

void write_port_1(jchar value) {
  mainMem[1] = value;

  int motorStatus = (value & (1 << 5)) >> 5;
  setMotorOn(&tape_timer, motorStatus);
}

inline int kernalROMEnabled() {
  int bankBits = mainMem[1] & 7;
  return (bank_visibility[bankBits] & KERNAL_VISIBLE) ? 1 : 0;
}

inline int basicROMEnabled() {
  int bankBits = mainMem[1] & 7;
  return (bank_visibility[bankBits] & BASIC_VISIBLE) ? 1 : 0;
}

inline int IOEnabled() {
  int bankBits = mainMem[1] & 7;
  return (bank_visibility[bankBits] & IO_VISIBLE) ? 1 : 0;
}

jchar memory_unclaimed_io_read(int address) {
  address = address & 0xfff;
  return IOUnclaimed[address];
}

jchar memory_read(int address) {
  if ((address >=0xa000) && (address < 0xc000) && basicROMEnabled())
    return basicROM[address & 0x1fff];
  else if ((address >=0xe000) && (address < 0x10000) && kernalROMEnabled())
    return kernalROM[address & 0x1fff];
  else if (address == 1)
    return read_port_1();
  else if ((address >=0xd000) && (address < 0xe000) && IOEnabled()) {
    if ((address >=0xdc00) && (address < 0xdc10))
      return cia1_read(address);
    else if (address == 0xd012)
      return line_count & 0xff;
    else
      return IOUnclaimed[address & 0xfff];
  }
  else
    return mainMem[address];
}

void memory_read_batch(int *batch, int address, int count) {
  int i;
  for (i = 0; i < count; i++) {
    batch[i] = mainMem[address + i];
  }
}



void memory_read_batch_io_unclaimed(int *batch, int address, int count) {
  int i;
  address = address & 0xfff;
  for (i = 0; i < count; i++) {
    batch[i] = IOUnclaimed[address + i];
  }
}


void memory_write(int address, jchar value) {
  //if (((address >= 0xa000) && (address < 0xc000)) |
  //     ((address >= 0xe000) && (address < 0x10000)))
  //  return;

  if (address == 1)
    write_port_1(value);
  else if ((address >=0xd000) && (address < 0xe000) && IOEnabled()) {
    if((address >=0xdc00) & (address < 0xdc10))
      cia1_write(address, value);
    else
      IOUnclaimed[address & 0xfff] = value;
  }
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
  tape_timer = getTapeInstance();
  add_timer_to_list(&tape_timer);
  video_timer = getVideoInstance();
  add_timer_to_list(&video_timer);
  initialise_video();
  mainMem[1] = 7;
}


void Java_com_johan_emulator_engine_Emu6502_setKeyboardMatrix(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  keyboardMatrix = (jbyte *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);
}


void Java_com_johan_emulator_engine_Emu6502_setFrameBuffer(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  g_buffer = (jchar *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);

}

void Java_com_johan_emulator_engine_Emu6502_setFireButton(JNIEnv* pEnv, jobject pObj, jint fireButtonStatus) {
  if (fireButtonStatus)
    joystickStatus = joystickStatus | 16;
  else
    joystickStatus = joystickStatus & 0xef;
}

void Java_com_johan_emulator_engine_Emu6502_setJoystickDirectionButton(JNIEnv* pEnv, jobject pObj, jint fireButtonStatus) {
  //start at north, then goes clockwise till again at north
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
  for (i = 0x0; i < 0x2000; i++) {
    basicROM[i] = buffer[i];
  }
  AAsset_close(assetF);

  assetF = AAssetManager_open(assetManager, "kernal.bin", AASSET_MODE_UNKNOWN);
  AAsset_read(assetF, buffer, 8192);

  for (i = 0x0; i < 0x2000; i++) {
    kernalROM[i] = buffer[i];
  }
  AAsset_close(assetF);

  assetF = AAssetManager_open(assetManager, "characters.bin", AASSET_MODE_UNKNOWN);
  AAsset_read(assetF, buffer, 4096);

  for (i = 0x0; i < 0x1000; i++) {
    charRom[i] = buffer[i];
  }
  AAsset_close(assetF);


}

void Java_com_johan_emulator_engine_Emu6502_attachNewTape(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  jbyte * tape_image = (jbyte *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);
  attachNewTape(tape_image, &tape_timer);
}


jcharArray
Java_com_johan_emulator_engine_Emu6502_dump(JNIEnv* pEnv, jobject pObj)
{
  jcharArray result;
  result = (*pEnv)->NewCharArray(pEnv,sizeof(mainMem)/sizeof(mainMem[0]));
  (*pEnv)->SetCharArrayRegion(pEnv,result,0,sizeof(mainMem)/sizeof(mainMem[0]),mainMem);
  return result;
}


