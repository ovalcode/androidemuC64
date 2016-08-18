package com.example.johan.myapplication;

/**
 * Created by johan on 2016/08/18.
 */
public class Memory {
    private byte[] mainMem = {(byte) 0xa9, 0x20, (byte)0x85,0x8,0,0,0,0,0,0,0};

    public int readMem(int address) {
      return mainMem[address] & 0xff;
    }

    public void writeMem (int address, int value) {
        mainMem[address] = (byte) (value & 0xff);
    }

}
