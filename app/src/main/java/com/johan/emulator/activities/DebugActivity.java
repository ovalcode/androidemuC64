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
import com.johan.emulator.engine.Emu6502;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;

public class DebugActivity extends AppCompatActivity {

    private Emu6502 emuInstance; //Emu6502.getInstance(this.getResources().getAssets());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        emuInstance = Emu6502.getInstance(getResources().getAssets());

        //NB!! Do refresh controls when onResume()
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

        EditText editText = (EditText) findViewById(R.id.inputSearchEditText);
        String editAddress = "0"+editText.getText().toString();

        int intaddress = Integer.parseInt(editAddress,16);

        TextView view = (TextView) findViewById(R.id.memoryDump);
        view.setText(emuInstance.getMemDumpAsString(intaddress));

        TextView viewReg = (TextView) findViewById(R.id.Registers);
        viewReg.setText(emuInstance.getRegisterDump());

        TextView viewFlags = (TextView) findViewById(R.id.Flags);
        viewFlags.setText(emuInstance.getFlagDump());

        TextView viewDiss = (TextView) findViewById(R.id.instruction);
        viewDiss.setText(emuInstance.getDisassembled());

    }



    public void onClick(View v) {
      refreshControls();
    }

    public void onRunClick(View v) {

    }

    public void onStepClick(View v) {
        emuInstance.step();
        refreshControls();
    }


    //TODO: menu switching
    public void onStopClick(View v) {
        //running = false;
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

    @Override
    protected  void onResume() {
        super.onResume();
        refreshControls();
    }



    }
