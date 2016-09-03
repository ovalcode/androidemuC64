//
// Created by johan on 2016/08/30.
//

#include <memory.h>
#include <jni.h>

  #define ADDRESS_MODE_ACCUMULATOR 0
  #define ADDRESS_MODE_ABSOLUTE 1
  #define ADDRESS_MODE_ABSOLUTE_X_INDEXED 2
  #define ADDRESS_MODE_ABSOLUTE_Y_INDEXED 3
  #define ADDRESS_MODE_IMMEDIATE 4
  #define ADDRESS_MODE_IMPLIED 5
  #define ADDRESS_MODE_INDIRECT 6
  #define ADDRESS_MODE_X_INDEXED_INDIRECT 7
  #define ADDRESS_MODE_INDIRECT_Y_INDEXED 8
  #define ADDRESS_MODE_RELATIVE 9
  #define ADDRESS_MODE_ZERO_PAGE 10
  #define ADDRESS_MODE_ZERO_PAGE_X_INDEXED 11
  #define ADDRESS_MODE_ZERO_PAGE_Y_INDEXED 12

  const unsigned char addressModes[] = {5, 7, 0, 0, 0, 10, 10, 0, 5, 4, 0, 0, 0, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0, 
  1, 7, 0, 0, 10, 10, 10, 0, 5, 4, 0, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0, 
  5, 7, 0, 0, 0, 10, 10, 0, 5, 4, 0, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0, 
  5, 7, 0, 0, 0, 10, 10, 0, 5, 4, 0, 0, 6, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0, 
  0, 7, 0, 0, 10, 10, 10, 0, 5, 0, 5, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 11, 11, 12, 0, 5, 3, 5, 0, 0, 2, 0, 0, 
  4, 7, 4, 0, 10, 10, 10, 0, 5, 4, 5, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 11, 11, 12, 0, 5, 3, 5, 0, 2, 2, 3, 0, 
  4, 7, 0, 0, 10, 10, 10, 0, 5, 4, 5, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0, 
  4, 7, 0, 0, 10, 10, 10, 0, 5, 4, 5, 0, 1, 1, 1, 0, 
  9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0};

  const unsigned char instructionLengths[] = {1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 
  3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 
  1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 
  1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 
  0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0, 
  2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 
  2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0};

  const unsigned char instructionCycles[] = {7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, 
  2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, 
  2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, 
  2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0, 
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 3, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0};

    jchar acc = 0;
    jchar xReg = 0;
    jchar yReg = 0;
    int pc = 0;
    int zeroFlag = 0;
    int negativeFlag = 0;


  int calculateEffevtiveAdd(unsigned char mode, int argbyte1, int argbyte2) {

    int tempAddress = 0;
    switch (mode)
    {
      case ADDRESS_MODE_ACCUMULATOR: return 0; 
      break;

      case ADDRESS_MODE_ABSOLUTE: return ((argbyte2 << 8) + argbyte1);
      break;

      case ADDRESS_MODE_ABSOLUTE_X_INDEXED: tempAddress = ((argbyte2 << 8) + argbyte1);
        tempAddress = tempAddress + xReg;
        return tempAddress;
      break;

      case ADDRESS_MODE_ABSOLUTE_Y_INDEXED: tempAddress = ((argbyte2 << 8) + argbyte1);
        tempAddress = tempAddress + yReg;
        return tempAddress;
      break;

      case ADDRESS_MODE_IMMEDIATE: 
      break;

      case ADDRESS_MODE_IMPLIED:
      break;

      case ADDRESS_MODE_INDIRECT:
        tempAddress = ((argbyte2 << 8) + argbyte1);
        return ((memory_read(tempAddress + 1) << 8) + memory_read(tempAddress));
      break;

      case ADDRESS_MODE_X_INDEXED_INDIRECT:
        tempAddress = (argbyte1 + xReg) & 0xff;
        return ((memory_read(tempAddress + 1) << 8) + memory_read(tempAddress));
      break;

      case ADDRESS_MODE_INDIRECT_Y_INDEXED:
        tempAddress = (memory_read(argbyte1 + 1) << 8) + memory_read(argbyte1) + yReg;
        return tempAddress;
      break;

      case ADDRESS_MODE_RELATIVE:
      break;

      case ADDRESS_MODE_ZERO_PAGE:
         return argbyte1;
      break;

      case ADDRESS_MODE_ZERO_PAGE_X_INDEXED:
        return (argbyte1 + xReg) & 0xff;
      break;

      case ADDRESS_MODE_ZERO_PAGE_Y_INDEXED:
        return (argbyte1 + yReg) & 0xff;
      break;
    }
  }






    void step() {
        int opCode = memory_read(pc);
        int address;
        pc++;
        switch (opCode) {
            case 0xa9:
                acc = memory_read(pc);
                zeroFlag = (acc == 0) ? 1 : 0;
                negativeFlag = ((acc & 0x80) != 0) ? 1 : 0;
                pc++;
            break;
            case 0x85:
                address = memory_read(pc);
                memory_write(address, acc);
                pc++;
            break;
        }
    }

void
Java_com_johan_emulator_MainActivity_step(JNIEnv* pEnv, jobject pObj)
{
  step();
}




