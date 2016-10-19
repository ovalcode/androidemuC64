package com.johan.emulator.activities;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.johan.emulator.R;
import com.johan.emulator.engine.Emu6502;

import java.util.Timer;
import java.util.TimerTask;

public class FrontActivity extends AppCompatActivity {
    Timer timer;
    TimerTask timerTask;

    private boolean running = false;
    final Handler handler = new Handler();
    private Emu6502 emuInstance; //Emu6502.getInstance(getResources().getAssets());


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_front);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        emuInstance = Emu6502.getInstance(getResources().getAssets());

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }

    public void doAlert(int result) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder
                .setTitle("Error")
                .setMessage(result+"")
                .setPositiveButton("Yes",null)
                .show();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_front, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_stop) {
            running = false;
            return true;
        }

        return super.onOptionsItemSelected(item);
    }




    @Override
    protected  void onResume() {
        super.onResume();
        running = true;
        timer = new Timer();
        //breakAddress = getBreakAddress();

        timerTask = new TimerTask() {
            @Override
            public void run() {
                //System.out.println("In Beginning: " + System.currentTimeMillis());
                final int result = emuInstance.runBatch(0);
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
                            System.out.print("Execution stopped");
                            timer.cancel();
                            Intent i = new Intent(FrontActivity.this, DebugActivity.class);
                            //NB!! jump to debug activity
                        }
                    });

                }
            }
        };

        timer.schedule(timerTask, 20, 20);

    }

}
