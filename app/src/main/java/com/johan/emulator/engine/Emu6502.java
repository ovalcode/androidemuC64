package com.johan.emulator.engine;

import android.content.res.AssetManager;
import java.nio.ByteBuffer;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.widget.EditText;

import com.johan.emulator.R;
import com.johan.emulator.activities.FrontActivity;

/**
 * Created by johan on 2016/10/15.
 */
public class Emu6502 {
    private static final int ADDRESS_MODE_ACCUMULATOR = 0;
    private static final int ADDRESS_MODE_ABSOLUTE = 1;
    private static final int ADDRESS_MODE_ABSOLUTE_X_INDEXED = 2;
    private static final int ADDRESS_MODE_ABSOLUTE_Y_INDEXED = 3;
    private static final int ADDRESS_MODE_IMMEDIATE = 4;
    private static final int ADDRESS_MODE_IMPLIED = 5;
    private static final int ADDRESS_MODE_INDIRECT = 6;
    private static final int ADDRESS_MODE_X_INDEXED_INDIRECT = 7;
    private static final int ADDRESS_MODE_INDIRECT_Y_INDEXED = 8;
    private static final int ADDRESS_MODE_RELATIVE = 9;
    private static final int ADDRESS_MODE_ZERO_PAGE = 10;
    private static final int ADDRESS_MODE_ZERO_PAGE_X_INDEXED = 11;
    private static final int ADDRESS_MODE_ZERO_PAGE_Y_INDEXED = 12;

    private boolean running = true;
    private static Emu6502 emu6502Instance = null;

    public static final int[] ADDRESS_MODES = {
            5, 7, 0, 0, 0, 10, 10, 0, 5, 4, 0, 0, 0, 1, 1, 0,
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
            9, 8, 0, 0, 0, 11, 11, 0, 5, 3, 0, 0, 0, 2, 2, 0
    };

    public static final int[] INSTRUCTION_LEN = {
            1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,
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
            2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0
    };

    public static final String[] opCodeDesc =
            {"BRK", "ORA", "", "", "", "ORA", "ASL", "", "PHP", "ORA", "ASL", "", "", "ORA", "ASL", "",
                    "BPL", "ORA", "", "", "", "ORA", "ASL", "", "CLC", "ORA", "", "", "", "ORA", "ASL", "",
                    "JSR", "AND", "", "", "BIT", "AND", "ROL", "", "PLP", "AND", "ROL", "", "BIT", "AND", "ROL", "",
                    "BMI", "AND", "", "", "", "AND", "ROL", "", "SEC", "AND", "", "", "", "AND", "ROL", "",
                    "RTI", "EOR", "", "", "", "EOR", "LSR", "", "PHA", "EOR", "LSR", "", "JMP", "EOR", "LSR", "",
                    "BVC", "EOR", "", "", "", "EOR", "LSR", "", "CLI", "EOR", "", "", "", "EOR", "LSR", "",
                    "RTS", "ADC", "", "", "", "ADC", "ROR", "", "PLA", "ADC", "ROR", "", "JMP", "ADC", "ROR", "",
                    "BVC", "ADC", "", "", "", "ADC", "ROR", "", "SEI", "ADC", "", "", "", "ADC", "ROR", "",
                    "", "STA", "", "", "STY", "STA", "STX", "", "DEY", "", "TXA", "", "STY", "STA", "STX", "",
                    "BCC", "STA", "", "", "STY", "STA", "STX", "", "TYA", "STA", "TXS", "", "", "STA", "", "",
                    "LDY", "LDA", "LDX", "", "LDY", "LDA", "LDX", "", "TAY", "LDA", "TAX", "", "LDY", "LDA", "LDX", "",
                    "BCS", "LDA", "", "", "LDY", "LDA", "LDX", "", "CLV", "LDA", "TSX", "", "LDY", "LDA", "LDX", "",
                    "CPY", "CMP", "", "", "CPY", "CMP", "DEC", "", "INY", "CMP", "DEX", "", "CPY", "CMP", "DEC", "",
                    "BNE", "CMP", "", "", "", "CMP", "DEC", "", "CLD", "CMP", "", "", "", "CMP", "DEC", "",
                    "CPX", "SBC", "", "", "CPX", "SBC", "INC", "", "INX", "SBC", "NOP", "", "CPX", "SBC", "INC", "",
                    "BEQ", "SBC", "", "", "", "SBC", "INC", "", "SED", "SBC", "", "", "", "SBC", "INC", ""};

    private AudioTrack audio;

    public void initAudio(int freq, int bits, int sound_packet_length) {
        if (audio == null) {
            audio = new AudioTrack(AudioManager.STREAM_MUSIC, freq, AudioFormat.CHANNEL_CONFIGURATION_MONO, bits == 8?AudioFormat.ENCODING_PCM_8BIT: AudioFormat.ENCODING_PCM_16BIT, 2048, AudioTrack.MODE_STREAM);
            //soundThread = new SoundThread(freq);
            //sound_copy = new byte [sound_packet_length*((bits==8)?1:2)];
            audio.play();
        }
    }

    public void sendAudio(short data []) {
        if (audio != null) {
            //Log.i("frodoc64", ">>>");
            long startTime = System.currentTimeMillis();
            audio.write(data, 0, data.length);
            long endTime = System.currentTimeMillis();
//            System.out.println(endTime - startTime);

        }
    }

    private String getAsTwoDigit(int number) {
        String numStr = "00"+Integer.toHexString(number);
        numStr = numStr.substring(numStr.length() - 2);
        return numStr;
    }

    private String getAsFourDigit(int number) {
        String numStr = "0000"+Integer.toHexString(number);
        numStr = numStr.substring(numStr.length() - 4);
        return numStr;
    }

    public boolean getRunning() {
        return running;
    }

    public void setRunning(boolean running) {
        this.running = running;
    }

    public String getDisassembled() {
        char[] memContents = assembleDump();
        int opCode = memContents[getPc()];
        int mode = ADDRESS_MODES[opCode];
        int numArgs = INSTRUCTION_LEN[opCode] - 1;
        int argbyte1 = 0;
        int argbyte2 = 0;
        if (numArgs > 0) {
            argbyte1 = memContents[getPc() + 1];
        }

        if (numArgs > 1) {
            argbyte2 = memContents[getPc() + 2];
        }

        String addrStr = "";
        String result = getAsFourDigit(getPc());
        result = result + " " + opCodeDesc[opCode] + " ";
        switch (mode) {
            case ADDRESS_MODE_ACCUMULATOR: result = result + " A";
                break;

            case ADDRESS_MODE_ABSOLUTE: addrStr = getAsFourDigit(argbyte2 * 256 + argbyte1);
                result = result + "$" + addrStr;
                break;

            case ADDRESS_MODE_ABSOLUTE_X_INDEXED: addrStr = getAsFourDigit(argbyte2 * 256 + argbyte1);
                result = result + "$" + addrStr + ",X";

                break;

            case ADDRESS_MODE_ABSOLUTE_Y_INDEXED: addrStr = getAsFourDigit(argbyte2 * 256 + argbyte1);
                result = result + "$" + addrStr + ",Y";

                break;

            case ADDRESS_MODE_IMMEDIATE: addrStr = getAsTwoDigit(argbyte1);
                result = result + "#$" + addrStr;

                break;

            case ADDRESS_MODE_IMPLIED:
                //return result;
                break;

            //case ADDRESS_MODE_INDIRECT:
            //    tempAddress = (argbyte2 * 256 + argbyte1);
            //    return (localMem.readMem(tempAddress + 1) * 256 + localMem.readMem(tempAddress));
            //break;

            case ADDRESS_MODE_X_INDEXED_INDIRECT:
                addrStr = getAsTwoDigit(argbyte2 * 256 + argbyte1);
                result = result + "($" + addrStr + ",X)";
                break;

            case ADDRESS_MODE_INDIRECT_Y_INDEXED:
                addrStr = getAsTwoDigit(argbyte1);
                result = result + "($" + addrStr + "),Y";

                break;

            case ADDRESS_MODE_RELATIVE:
                addrStr = getAsFourDigit(((argbyte1 > 127) ? (argbyte1 - 256) : argbyte1) + getPc() + 2);
                result = result + "$" + addrStr;
                break;

            case ADDRESS_MODE_ZERO_PAGE:
                addrStr = getAsTwoDigit(argbyte1);
                result = result + "$" + addrStr;
                //return result;
                break;

            case ADDRESS_MODE_ZERO_PAGE_X_INDEXED:
                addrStr = getAsTwoDigit(argbyte1);
                result = result + "$" + addrStr + ",X";
                //return result;
                break;

            case ADDRESS_MODE_ZERO_PAGE_Y_INDEXED:
                addrStr = getAsTwoDigit(argbyte1);
                result = result + "$" + addrStr + ",Y";
                // return result;
                break;

        }
        return result;
    }

    public String getFlagDump() {
        String result ="";
        result = result + ((getNegativeFlag() == 1) ? "N" : "-");
        result = result + ((getOverflowFlag() == 1) ? "V" : "-");
        result = result + "-";
        result = result + "B";
        result = result + ((getDecimalFlag() == 1) ? "D" : "-");
        result = result + ((getInterruptFlag() == 1) ? "I" : "-");
        result = result + ((getZeroFlag() == 1) ? "Z" : "-");
        result = result + ((getCarryFlag() == 1) ? "C" : "-");

        return result;
    }

    public String getRegisterDump() {
        String accStr = "00"+Integer.toHexString(getAcc());
        accStr = accStr.substring(accStr.length() - 2);

        String xRegStr = "00"+Integer.toHexString(getXreg());
        xRegStr = xRegStr.substring(xRegStr.length() - 2);

        String yRegStr = "00"+Integer.toHexString(getYreg());
        yRegStr = yRegStr.substring(yRegStr.length() - 2);

        String spStr = "00"+Integer.toHexString(getSP());
        spStr = spStr.substring(spStr.length() - 2);

        return accStr + xRegStr + yRegStr + spStr;

    }

    char[] assembleDump() {
        char[] memoryDump = new char[64*1024];
        for (int i = 0; i < memoryDump.length; i++) {
            memoryDump[i] = memoryReadLocation(i);
        }

        return  memoryDump;
    }

    public String getMemDumpAsString(int address) {
        char[] memContents = assembleDump();//dump();

        String result = "";
        byte[] temp = new byte[48];
        for (int i=0; i < temp.length; i++) {
            temp[i] = (byte) memContents[i+address];
        }

        for (int i = 0; i < temp.length; i++) {
            if ((i % 16) == 0) {
                String numberStr = Integer.toHexString(i+address);
                numberStr = "0000" + numberStr;
                result = result + "\n" + numberStr.substring(numberStr.length() - 4);
            }
            String number = "0" + Integer.toHexString(temp[i] & 0xff);
            number = number.substring(number.length() - 2);
            result = result + " " + number;
        }
        return result;
    }

    private native char[] dump();
    private native char memoryReadLocation(int address);
    public native void step();
    public native int runBatch(int address);
    public static native void memoryInit();
    private static native void loadROMS (AssetManager pAssetManager);
    public native void attachNewTape(int len, ByteBuffer buf);
    public native void togglePlay();
    public native void setEmuInstance(Emu6502 emuInstance);

    private native char getAcc();
    private native char getXreg();
    private native char getYreg();
    private native char getSP();
    private native char getPc();
    public static native void resetCpu();
    public static native void setFireButton(int fireButtonStatus);
    public static native void setJoystickDirectionButton(int directionStatus);

    private native char getZeroFlag();
    private native char getNegativeFlag();
    private native char getCarryFlag();
    private native char getInterruptFlag();
    private native char getDecimalFlag();
    private native char getOverflowFlag();
    public native void setFrameBuffer(ByteBuffer buf);
    public native void populateFrame();
    public native void interruptCpu();
    public native void setKeyboardMatrix(ByteBuffer keyBoardMatrix);
    public native void clearDisplayBuffer();

    protected Emu6502() {

    }

    static public Emu6502 getInstance(AssetManager mgr) {
      if (emu6502Instance != null)
          return emu6502Instance;
        loadROMS(mgr);
        resetCpu();
        memoryInit();
        emu6502Instance = new Emu6502();
        emu6502Instance.setEmuInstance(emu6502Instance);
        emu6502Instance.initAudio(44100, 16, 512);
       return emu6502Instance;

    }

    static {
        System.loadLibrary("native_emu");
        memoryInit();
    }


}
