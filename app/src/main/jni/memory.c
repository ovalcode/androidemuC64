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
jint* g_buffer;
jbyte* keyboardMatrix;
jobject currentActivity;
jmethodID initAudio;
jmethodID sendAudio;
JNIEnv* global_env = NULL;
JavaVM* gJavaVM = NULL;
extern int line_count;

void WriteRegister(uint16_t adr, uint8_t byte);
void init_sound();

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

void init_sid();
void Reset();

jchar cia1_read(int address) {
  jchar result = 0;
  switch (address) {
    case 0xdc00:
      result =  ~joystickStatus & 0xff ;
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
    else if (address == 0xd011) {
      int tempValue = IOUnclaimed[address & 0xfff] & 0x7f;
      tempValue = tempValue | ((line_count & 0x100) >> 1);
      return tempValue;
    }
    else if (address == 0xd012)
      return line_count & 0xff;
    else if (address == 0xd019)
      return read_vic_int_reg();
    else
      return IOUnclaimed[address & 0xfff];
  }
  else
    return mainMem[address];
}

void memory_read_batch(int *batch, int address, int count) {
  address = ((~IOUnclaimed[0xd00] & 3) << 14) | address;
  int i;
  for (i = 0; i < count; i++) {
    if ((address >= 0x1000 && address < 0x2000) || (address >= 0x9000 && address < 0xa000))
      batch[i] = charRom[address & 0xfff];
    else
      batch[i] = mainMem[address + i];
  }
}

jchar memory_read_vic_model(int address) {
  address = ((~IOUnclaimed[0xd00] & 3) << 14) | address;
  if ((address >= 0x1000 && address < 0x2000) || (address >= 0x9000 && address < 0xa000))
    return charRom[address & 0xfff];
  else
    return mainMem[address];

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
    else if (address == 0xd019)
      write_vic_int_reg(value);
    else if ((address >=0xd400) & (address < 0xd800)) {
      IOUnclaimed[address & 0xfff] = value;
      WriteRegister(address & 0x1f, value & 0xff);
    }
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
  init_sid();
  Reset();
  init_sound();
}


void Java_com_johan_emulator_engine_Emu6502_setKeyboardMatrix(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  keyboardMatrix = (jbyte *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);
}


void Java_com_johan_emulator_engine_Emu6502_setFrameBuffer(JNIEnv* pEnv, jobject pObj, jobject oBuf) {
  g_buffer = (jint *) (*pEnv)->GetDirectBufferAddress(pEnv, oBuf);

}

void Java_com_johan_emulator_engine_Emu6502_setFireButton(JNIEnv* pEnv, jobject pObj, jint fireButtonStatus) {
  if (fireButtonStatus)
    joystickStatus = joystickStatus | 16;
  else
    joystickStatus = joystickStatus & 0xef;
}

void Java_com_johan_emulator_engine_Emu6502_setJoystickDirectionButton(JNIEnv* pEnv, jobject pObj, jint fireButtonStatus) {
  //start at north, then goes clockwise till again at north
  joystickStatus = joystickStatus & 0xf0;
  switch (fireButtonStatus) {
    case 0: //North
      joystickStatus = joystickStatus | JOYSTICK_UP;
    break;

    case 1: //North East
      joystickStatus = joystickStatus | JOYSTICK_UP | JOYSTICK_RIGHT;
    break;

    case 2: //East
      joystickStatus = joystickStatus | JOYSTICK_RIGHT;
    break;

    case 3: //South East
      joystickStatus = joystickStatus | JOYSTICK_DOWN | JOYSTICK_RIGHT;
    break;

    case 4: //South
      joystickStatus = joystickStatus | JOYSTICK_DOWN;
    break;

    case 5: //South West
      joystickStatus = joystickStatus | JOYSTICK_DOWN | JOYSTICK_LEFT;
    break;

    case 6: //West
      joystickStatus = joystickStatus | JOYSTICK_LEFT;
    break;

    case 7: //North West
      joystickStatus = joystickStatus | JOYSTICK_LEFT | JOYSTICK_UP;
    break;

    default:
    break;

  }
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

jint JNI_OnLoad(JavaVM* aVm, void* aReserved) {
  gJavaVM = aVm;

  //JNIEnv* env;
  //aVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  return JNI_VERSION_1_6;
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

void Java_com_johan_emulator_engine_Emu6502_clearDisplayBuffer(JNIEnv* env, jobject pObj) {
  int i;
  for (i = 0; i < 427200; i++) {
    g_buffer[i] = 0;
  }
}

void Java_com_johan_emulator_engine_Emu6502_setMainActivityObject(JNIEnv* env, jobject pObj, jobject activity) {

  currentActivity = (*env)->NewGlobalRef(env,activity);

  jclass thisClass = (*env)->GetObjectClass(env,currentActivity);

  initAudio = (*env)->GetMethodID(env, thisClass, "initAudio", "(III)V");
  sendAudio = (*env)->GetMethodID(env, thisClass, "sendAudio", "([S)V");


}

jcharArray
Java_com_johan_emulator_engine_Emu6502_dump(JNIEnv* pEnv, jobject pObj)
{
  jcharArray result;
  result = (*pEnv)->NewCharArray(pEnv,sizeof(mainMem)/sizeof(mainMem[0]));
  (*pEnv)->SetCharArrayRegion(pEnv,result,0,sizeof(mainMem)/sizeof(mainMem[0]),mainMem);
  return result;
}

int processSprite(int spriteNum, int lineNumber, struct sprite_data_struct * sprite_data) {
  if (!(IOUnclaimed[0x15] & (1 << spriteNum)))
    return 0;

  int spriteY = IOUnclaimed[(spriteNum << 1) | 1];
  int yExpanded = IOUnclaimed[0x17] & (1 << spriteNum);
  int ySpriteDimension = yExpanded ? 42 : 21;
  int spriteYMax = spriteY + ySpriteDimension;

  if (!((lineNumber >= spriteY) && (lineNumber < spriteYMax)))
    return 0;

  int spriteX = (IOUnclaimed[spriteNum << 1] & 0xff);
  if (IOUnclaimed[0x10] & (1 << spriteNum))
    spriteX = 256 | spriteX;

  if (spriteX > 367)
    return 0;

  int xExpanded = IOUnclaimed[0x1d] & (1 << spriteNum);
  int xSpriteDimension = xExpanded ? 48 : 24;
  int spriteXMax = spriteX + xSpriteDimension;

  if (spriteXMax > 367)
    xSpriteDimension = 368 - spriteX;

  sprite_data->sprite_x_pos = spriteX;
  sprite_data->number_pixels_to_draw = xSpriteDimension;
  sprite_data->sprite_type = 0;
  if (xExpanded)
    sprite_data->sprite_type = sprite_data->sprite_type | 2;

  if (IOUnclaimed[0x1c] & (1 << spriteNum)) {
    sprite_data->sprite_type = sprite_data->sprite_type | 1;
    sprite_data->color_tablet[1] = IOUnclaimed[0x25] & 0xf;
    sprite_data->color_tablet[2] = IOUnclaimed[0x27 + spriteNum] & 0xf;
    sprite_data->color_tablet[3] = IOUnclaimed[0x26] & 0xf;
  } else {
    sprite_data->color_tablet[1] = IOUnclaimed[0x27 + spriteNum] & 0xf;
  }

  if (IOUnclaimed[0x1b] & (1 << spriteNum))
    sprite_data->isForegroundSprite = IOUnclaimed[0x1b] & (1 << spriteNum) ? 0 : 1;

  int memPointer = IOUnclaimed[0x18];
  int spritePointerAddress = memPointer & 0xf0;
  spritePointerAddress = spritePointerAddress << 6;

  spritePointerAddress = ((~IOUnclaimed[0xd00] & 3) << 14) | spritePointerAddress;
  spritePointerAddress = spritePointerAddress + 0x400 -8 + spriteNum;
  int spriteBaseAddress = mainMem[spritePointerAddress] << 6;


  int spriteLineNumber = lineNumber - spriteY;

  if (yExpanded)
    spriteLineNumber = spriteLineNumber >> 1;

  int posInSpriteData = (spriteLineNumber << 1) + (spriteLineNumber) + spriteBaseAddress;
  sprite_data->sprite_data = (mainMem[posInSpriteData + 0] << 16) | (mainMem[posInSpriteData + 1] << 8)
          | (mainMem[posInSpriteData + 2] << 0);

  return 1;
  //d027 -> color sprite 0
  //multi color
  //00 transparent
  //01 use color in d025
  //10 use sprite color
  //11 use color in d026

  //return three byte sprite data
  //return 2 bit number -> first bit whether x expanded
   //       -> second bit whether sprite multicolor
  //whether foreground or background sprite
  //return four color tablet
  //return range to populate
  //NB!! for multicolor check even vs uneven
  //  multicolor -> align two pixels
  // multi color expanded -> align four pixels
}


