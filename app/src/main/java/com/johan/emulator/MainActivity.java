package com.johan.emulator;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
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

    public String getMemDumpAsString(char[] memContents) {
        String result = "";
        byte[] temp = new byte[48];
        for (int i=0; i < memContents.length; i++) {
            temp[i] = (byte) memContents[i];
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

    private void refreshControls() {
        TextView view = (TextView) findViewById(R.id.memoryDump);
        view.setText(getMemDumpAsString(dump()));
    }

    public native char[] dump();
    public native void step();


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
    }

}
