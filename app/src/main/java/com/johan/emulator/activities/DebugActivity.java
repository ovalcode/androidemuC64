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


    private int getBreakAddress () {
        EditText editText = (EditText) findViewById(R.id.breakAddress);
        String editAddress = "0"+editText.getText().toString();

        int intaddress = Integer.parseInt(editAddress,16);
        return intaddress;
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


}
