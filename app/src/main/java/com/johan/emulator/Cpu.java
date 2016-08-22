package com.johan.emulator;

/**
 * Created by johan on 2016/08/18.
 */
public class Cpu {
    private Memory mem;
    private int acc = 0;
    private int xReg = 0;
    private int yReg = 0;
    private int pc = 0;
    private int zeroFlag;
    private int negativeFlag;

    public Cpu(Memory mem) {
        this.mem = mem;
    }

    public void step() {
        int opCode = mem.readMem(pc) & 0xff;
        int address;
        pc++;
        switch (opCode) {
            case 0xa9:
                acc = mem.readMem(pc);
                zeroFlag = (acc == 0) ? 1 : 0;
                negativeFlag = ((acc & 0x80) != 0) ? 1 : 0;
                pc++;
            break;
            case 0x85:
                address = mem.readMem(pc);
                mem.writeMem(address, acc);
                pc++;
            break;
        }
    }
}
