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

    public String getMemDump() {
        String result = "";
        byte[] temp = new byte[48];
        for (int i=0; i < mainMem.length; i++) {
            temp[i] = mainMem[i];
        }

        for (int i = 0; i < temp.length; i++) {
            if ((i % 16) == 0) {
                String numberStr = Integer.toHexString(i);
                numberStr = "0000" + numberStr;
                result = result + "\n" + numberStr.substring(numberStr.length() - 4);
            }
            String number = "0" + Integer.toHexString(temp[i] & 0xff);
            number = number.substring(number.length() - 2);
            result = result + " " + number;
        }
        return result;
    }

}
