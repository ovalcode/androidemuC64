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
    int carryFlag =0;
    int overflowFlag =0;

void updateFlags(jchar value) {
    zeroFlag = (value == 0) ? 1 : 0;
    negativeFlag = ((value & 0x80) != 0) ? 1 : 0;
  }

  //TODO:
  //Copy address mode test program to 64KB array
  //do dump for random places in memory
  //show status of registers and flags


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

  char ADC(char operand1, char operand2) {
    int temp = operand1 + operand2 + carryFlag;
    carryFlag = ((temp & 0x100) == 0x100) ? 1 : 0;
    overflowFlag = (((operand1^temp) & (operand2^temp) & 0x80) == 0x80) ? 1 : 0;
    temp = temp & 0xff;
    return temp;
  }

  char SBC(char operand1, char operand2) {
    operand2 = ~operand2 & 0xff;
    operand2 = operand2 + (1 - carryFlag);
    int temp = operand1 + operand2;
    carryFlag = ((temp & 0x100) == 0x100) ? 1 : 0;
    overflowFlag = (((operand1^temp) & (operand2^temp) & 0x80) == 0x80) ? 1 : 0;
    temp = temp & 0xff;
    return temp;
  }

    void step() {
      int opcode = memory_read(pc);
      pc = pc + 1;
      int iLen = instructionLengths[opcode];
      jchar arg1 = 0;
      jchar arg2 = 0;
      int effectiveAdrress = 0;
      if (iLen > 1) {
        arg1 = memory_read(pc);
        pc = pc + 1;
      }    

      if (iLen > 2) {
        arg2 = memory_read(pc);
        pc = pc + 1;
      }    

      effectiveAdrress = calculateEffevtiveAdd(addressModes[opcode], arg1, arg2);
      int tempVal;

      switch (opcode)
      {
/*LDA  Load Accumulator with Memory
     M -> A                           N Z C I D V
                                      + + - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immediate     LDA #oper     A9    2     2
     zeropage      LDA oper      A5    2     3
     zeropage,X    LDA oper,X    B5    2     4
     absolute      LDA oper      AD    3     4
     absolute,X    LDA oper,X    BD    3     4*
     absolute,Y    LDA oper,Y    B9    3     4*
     (indirect,X)  LDA (oper,X)  A1    2     6
     (indirect),Y  LDA (oper),Y  B1    2     5* */

        case 0xa9:
          acc = arg1;
          updateFlags(acc);
        break;
        case 0xA5:
        case 0xB5:
        case 0xAD:
        case 0xBD:
        case 0xB9:
        case 0xA1:
        case 0xB1:
          acc = memory_read(effectiveAdrress);
          updateFlags(acc);
        break;

/*LDX  Load Index X with Memory
     M -> X                           N Z C I D V
                                      + + - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immediate     LDX #oper     A2    2     2
     zeropage      LDX oper      A6    2     3
     zeropage,Y    LDX oper,Y    B6    2     4
     absolute      LDX oper      AE    3     4
     absolute,Y    LDX oper,Y    BE    3     4**/

        case 0xA2:
          xReg = arg1;
          updateFlags(xReg);
        break;

        case 0xA6:
        case 0xB6:
        case 0xAE:
        case 0xBE:
          xReg = memory_read(effectiveAdrress);
          updateFlags(xReg);
        break;



/*LDY  Load Index Y with Memory
     M -> Y                           N Z C I D V
                                      + + - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immediate     LDY #oper     A0    2     2
     zeropage      LDY oper      A4    2     3
     zeropage,X    LDY oper,X    B4    2     4
     absolute      LDY oper      AC    3     4
     absolute,X    LDY oper,X    BC    3     4*/


        case 0xA0:
          yReg = arg1;
          updateFlags(yReg);
        break;

        case 0xA4:
        case 0xB4:
        case 0xAC:
        case 0xBC:
          yReg = memory_read(effectiveAdrress);
          updateFlags(yReg);
        break;
 
/*STA  Store Accumulator in Memory
     A -> M                           N Z C I D V
                                      - - - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STA oper      85    2     3
     zeropage,X    STA oper,X    95    2     4
     absolute      STA oper      8D    3     4
     absolute,X    STA oper,X    9D    3     5
     absolute,Y    STA oper,Y    99    3     5
     (indirect,X)  STA (oper,X)  81    2     6
     (indirect),Y  STA (oper),Y  91    2     6  */

        case 0x85:
        case 0x95:
        case 0x8D:
        case 0x9D:
        case 0x99:
        case 0x81:
        case 0x91:
          memory_write(effectiveAdrress, acc);
        break;


/*STX  Store Index X in Memory
     X -> M                           N Z C I D V
                                      - - - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STX oper      86    2     3
     zeropage,Y    STX oper,Y    96    2     4
     absolute      STX oper      8E    3     4  */

        case 0x86:
        case 0x96:
        case 0x8E:
          memory_write(effectiveAdrress, xReg);
        break;
/*STY  Sore Index Y in Memory
     Y -> M                           N Z C I D V
                                      - - - - - -
     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STY oper      84    2     3
     zeropage,X    STY oper,X    94    2     4
     absolute      STY oper      8C    3     4  */

        case 0x84:
        case 0x94:
        case 0x8C:
          memory_write(effectiveAdrress, yReg);
        break;
/*ADC  Add Memory to Accumulator with Carry

     A + M + C -> A, C                N Z C I D V
                                      + + + - - +

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immediate     ADC #oper     69    2     2
     zeropage      ADC oper      65    2     3
     zeropage,X    ADC oper,X    75    2     4
     absolute      ADC oper      6D    3     4
     absolute,X    ADC oper,X    7D    3     4*
     absolute,Y    ADC oper,Y    79    3     4*
     (indirect,X)  ADC (oper,X)  61    2     6
     (indirect),Y  ADC (oper),Y  71    2     5* */

          case 0x69:
              acc = ADC (acc, arg1);
              updateFlags(acc);
              break;
          case 0x65:
          case 0x75:
          case 0x6D:
          case 0x7D:
          case 0x79:
          case 0x61:
          case 0x71:
              acc = ADC (acc, memory_read(effectiveAdrress));
              updateFlags(acc);
              break;

/*SBC  Subtract Memory from Accumulator with Borrow

     A - M - C -> A                   N Z C I D V
                                      + + + - - +

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immediate     SBC #oper     E9    2     2
     zeropage      SBC oper      E5    2     3
     zeropage,X    SBC oper,X    F5    2     4
     absolute      SBC oper      ED    3     4
     absolute,X    SBC oper,X    FD    3     4*
     absolute,Y    SBC oper,Y    F9    3     4*
     (indirect,X)  SBC (oper,X)  E1    2     6
     (indirect),Y  SBC (oper),Y  F1    2     5*  */

          case 0xE9:
              acc = SBC (acc, arg1);
              updateFlags(acc);
              break;
          case 0xE5:
          case 0xF5:
          case 0xED:
          case 0xFD:
          case 0xF9:
          case 0xE1:
          case 0xF1:
              acc = SBC (acc, memory_read(effectiveAdrress));
              updateFlags(acc);
              break;
/*INC  Increment Memory by One

     M + 1 -> M                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      INC oper      E6    2     5
     zeropage,X    INC oper,X    F6    2     6
     absolut      INC oper      EE    3     6
     absolute,X    INC oper,X    FE    3     7 */

          case 0xE6:
          case 0xF6:
          case 0xEE:
          case 0xFE:
              tempVal = memory_read(effectiveAdrress);
              tempVal++; tempVal = tempVal & 0xff;
              memory_write(effectiveAdrress, tempVal);
              updateFlags(tempVal);
              break;

/*INX  Increment Index X by One

     X + 1 -> X                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INX           E8    1     2*/

          case 0xE8:

              xReg++; xReg = xReg & 0xff;
              updateFlags(xReg);
              break;

/*INY  Increment Index Y by One

     Y + 1 -> Y                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INY           C8    1     2*/

          case 0xC8:

              yReg++; yReg = yReg & 0xff;
              updateFlags(yReg);
              break;

/*DEC  Decrement Memory by One

     M - 1 -> M                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      DEC oper      C6    2     5
     zeropage,X    DEC oper,X    D6    2     6
     absolute      DEC oper      CE    3     3
     absolute,X    DEC oper,X    DE    3     7 */

          case 0xC6:
          case 0xD6:
          case 0xCE:
          case 0xDE:
              tempVal = memory_read(effectiveAdrress);
              tempVal--; tempVal = tempVal & 0xff;
              memory_write(effectiveAdrress, tempVal);
              updateFlags(tempVal);
              break;

/*DEX  Decrement Index X by One

     X - 1 -> X                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       DEC           CA    1     2*/

          case 0xCA:

              xReg--; xReg = xReg & 0xff;
              updateFlags(xReg);
              break;

/*DEY  Decrement Index Y by One

     Y - 1 -> Y                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       DEC           88    1     2*/

          case 0x88:

              yReg--; yReg = yReg & 0xff;
              updateFlags(yReg);
              break;
      }
    }

void
Java_com_johan_emulator_MainActivity_step(JNIEnv* pEnv, jobject pObj)
{
  step();
}

jchar Java_com_johan_emulator_MainActivity_getAcc(JNIEnv* pEnv, jobject pObj)
{
  return acc;
}

jchar Java_com_johan_emulator_MainActivity_getXreg(JNIEnv* pEnv, jobject pObj)
{
  return xReg;
}

jchar Java_com_johan_emulator_MainActivity_getYreg(JNIEnv* pEnv, jobject pObj)
{
  return yReg;
}

jchar Java_com_johan_emulator_MainActivity_getPc(JNIEnv* pEnv, jobject pObj)
{
  return pc;
}


jchar Java_com_johan_emulator_MainActivity_getZeroFlag(JNIEnv* pEnv, jobject pObj)
{
  return zeroFlag;
}

jchar Java_com_johan_emulator_MainActivity_getNegativeFlag(JNIEnv* pEnv, jobject pObj)
{
  return negativeFlag;
}

jchar Java_com_johan_emulator_MainActivity_getCarryFlag(JNIEnv* pEnv, jobject pObj)
{
  return 0;
}

jchar Java_com_johan_emulator_MainActivity_getInterruptFlag(JNIEnv* pEnv, jobject pObj)
{
  return 0;
}

jchar Java_com_johan_emulator_MainActivity_getDecimalFlag(JNIEnv* pEnv, jobject pObj)
{
  return 0;
}

jchar Java_com_johan_emulator_MainActivity_getOverflowFlag(JNIEnv* pEnv, jobject pObj)
{
  return 0;
}
