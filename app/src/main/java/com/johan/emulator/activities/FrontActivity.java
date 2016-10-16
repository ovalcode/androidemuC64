package com.johan.emulator.activities;

import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
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
    private Emu6502 emuInstance = Emu6502.getInstance(getResources().getAssets());


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_front);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
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
                final int result = emuInstance.runBatch(1);
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

}
