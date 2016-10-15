package com.johan.emulator.activities;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.TextView;

import com.johan.emulator.R;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;

public class DebugActivity extends AppCompatActivity {

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

    Timer timer;
    TimerTask timerTask;

    private boolean running = false;
    private int breakAddress = 0;

    final Handler handler = new Handler();
    //private Memory mem = new Memory();
    //private Cpu myCpu = new Cpu(mem);
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        AssetManager mgr = getResources().getAssets();
        loadROMS(mgr);
        resetCpu();

        refreshControls();
        //TextView view = (TextView) findViewById(R.id.memoryDump);
        //view.setText(mem.getMemDump());
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
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


    protected String getDisassembled(char[] memContents, int pc) {
        int opCode = memContents[pc];
        int mode = ADDRESS_MODES[opCode];
        int numArgs = INSTRUCTION_LEN[opCode] - 1;
        int argbyte1 = 0;
        int argbyte2 = 0;
        if (numArgs > 0) {
            argbyte1 = memContents[pc + 1];
        }

        if (numArgs > 1) {
            argbyte2 = memContents[pc + 2];
        }

        String addrStr = "";
        String result = getAsFourDigit(pc);
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
                addrStr = getAsFourDigit(((argbyte1 > 127) ? (argbyte1 - 256) : argbyte1) + pc + 2);
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

    public String getRegisterDump(char acc, char xReg, char yReg, char SP) {
        String accStr = "00"+Integer.toHexString(acc);
        accStr = accStr.substring(accStr.length() - 2);

        String xRegStr = "00"+Integer.toHexString(xReg);
        xRegStr = xRegStr.substring(xRegStr.length() - 2);

        String yRegStr = "00"+Integer.toHexString(yReg);
        yRegStr = yRegStr.substring(yRegStr.length() - 2);

        String spStr = "00"+Integer.toHexString(SP);
        spStr = spStr.substring(spStr.length() - 2);

        return accStr + xRegStr + yRegStr + spStr;

    }

    private int getBreakAddress () {
        EditText editText = (EditText) findViewById(R.id.breakAddress);
        String editAddress = "0"+editText.getText().toString();

        int intaddress = Integer.parseInt(editAddress,16);
        return intaddress;
    }

    public String getMemDumpAsString(char[] memContents) {
        EditText editText = (EditText) findViewById(R.id.inputSearchEditText);
        String editAddress = "0"+editText.getText().toString();

        int intaddress = Integer.parseInt(editAddress,16);
        String result = "";
        byte[] temp = new byte[48];
        for (int i=0; i < temp.length; i++) {
            temp[i] = (byte) memContents[i+intaddress];
        }

        for (int i = 0; i < temp.length; i++) {
            if ((i % 16) == 0) {
                String numberStr = Integer.toHexString(i+intaddress);
                numberStr = "0000" + numberStr;
                result = result + "\n" + numberStr.substring(numberStr.length() - 4);
            }
            String number = "0" + Integer.toHexString(temp[i] & 0xff);
            number = number.substring(number.length() - 2);
            result = result + " " + number;
        }
        return result;
    }

    private void refreshControls() {

        char[] memDump = dump();

        TextView view = (TextView) findViewById(R.id.memoryDump);
        view.setText(getMemDumpAsString(memDump));

        TextView viewReg = (TextView) findViewById(R.id.Registers);
        viewReg.setText(getRegisterDump(getAcc(), getXreg(), getYreg(), getSP()));

        TextView viewFlags = (TextView) findViewById(R.id.Flags);
        viewFlags.setText(getFlagDump());

        TextView viewDiss = (TextView) findViewById(R.id.instruction);
        viewDiss.setText(getDisassembled(memDump, getPc()));

    }

    public native char[] dump();
    public native void step();
    public native int runBatch(int address);
    public static native void memoryInit();
    private native void loadROMS (AssetManager pAssetManager);

    public native char getAcc();
    public native char getXreg();
    public native char getYreg();
    public native char getSP();
    public native char getPc();
    public native void resetCpu();

    public native char getZeroFlag();
    public native char getNegativeFlag();
    public native char getCarryFlag();
    public native char getInterruptFlag();
    public native char getDecimalFlag();
    public native char getOverflowFlag();


    public void onClick(View v) {
      refreshControls();
    }

    public void onStepClick(View v) {
        step();
        refreshControls();
    }

    public void onRunClick(View v) {
        running = true;
        timer = new Timer();
        breakAddress = getBreakAddress();

        timerTask = new TimerTask() {
            @Override
            public void run() {
              //System.out.println("In Beginning: " + System.currentTimeMillis());
              final int result = runBatch(breakAddress);
              //System.out.println("In End: " + System.currentTimeMillis());
              if (result > 0) {
                  handler.post(new Runnable() {
                      @Override
                      public void run() {
                          timer.cancel();
                          doAlert(result);
                      }
                  });
              } else if (!running | (result < 0)) {
                  handler.post(new Runnable() {
                      @Override
                      public void run() {
                          timer.cancel();
                          refreshControls();
                      }
                  });

              }
            }
        };

        timer.schedule(timerTask, 20, 20);
    }

    public void doAlert(int result) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder
                .setTitle("Error")
                .setMessage(result+"")
                .setPositiveButton("Yes",null)
                .show();
    }

    public void onStopClick(View v) {
        running = false;
        //timer.cancel();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    static {
        System.loadLibrary("native_emu");
        memoryInit();
    }

}
