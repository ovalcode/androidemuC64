package com.johan.emulator;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    //private Memory mem = new Memory();
    //private Cpu myCpu = new Cpu(mem);
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
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

    public String getFlagDump() {
        String result ="";
        result = result + ((getNegativeFlag() == 1) ? "N" : "-");
        result = result + ((getZeroFlag() == 1) ? "Z" : "-");
        result = result + ((getCarryFlag() == 1) ? "C" : "-");
        result = result + ((getInterruptFlag() == 1) ? "I" : "-");
        result = result + ((getDecimalFlag() == 1) ? "N" : "-");
        result = result + ((getOverflowFlag() == 1) ? "V" : "-");

        return result;
    }

    public String getRegisterDump(char acc, char xReg, char yReg) {
        String accStr = "00"+Integer.toHexString(acc);
        accStr = accStr.substring(accStr.length() - 2);

        String xRegStr = "00"+Integer.toHexString(xReg);
        xRegStr = xRegStr.substring(xRegStr.length() - 2);

        String yRegStr = "00"+Integer.toHexString(yReg);
        yRegStr = yRegStr.substring(yRegStr.length() - 2);

        return accStr + xRegStr + yRegStr;

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
        TextView view = (TextView) findViewById(R.id.memoryDump);
        view.setText(getMemDumpAsString(dump()));

        TextView viewReg = (TextView) findViewById(R.id.Registers);
        viewReg.setText(getRegisterDump(getAcc(), getXreg(), getYreg()));

        TextView viewFlags = (TextView) findViewById(R.id.Flags);
        viewFlags.setText(getFlagDump());

    }

    public native char[] dump();
    public native void step();
    public static native void memoryInit();

    public native char getAcc();
    public native char getXreg();
    public native char getYreg();

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
