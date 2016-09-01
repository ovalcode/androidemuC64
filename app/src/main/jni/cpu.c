//
// Created by johan on 2016/08/30.
//

#include <memory.h>
#include <jni.h>

    jchar acc = 0;
    jchar xReg = 0;
    jchar yReg = 0;
    int pc = 0;
    int zeroFlag = 0;
    int negativeFlag = 0;



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




