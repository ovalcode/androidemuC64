package com.johan.emulator.activities;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import com.johan.emulator.R;
import com.johan.emulator.engine.Emu6502;
import com.johan.emulator.view.C64SurfaceView;

import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;

public class FrontActivity extends AppCompatActivity {
    Timer timer;
    TimerTask timerTask;
    int xpos = 100;
    int ypos = 100;
    private ByteBuffer mByteBuffer;
    private Bitmap mBitmap;

    private boolean running = false;
    private boolean switchToDebug = false;
    final Handler handler = new Handler();
    private Emu6502 emuInstance; //Emu6502.getInstance(getResources().getAssets());


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_front);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        emuInstance = Emu6502.getInstance(getResources().getAssets());
        mByteBuffer = ByteBuffer.allocateDirect(320*200*2*4);
        mBitmap = Bitmap.createBitmap(320,200, Bitmap.Config.RGB_565);
        emuInstance.setFrameBuffer(mByteBuffer);

        // Create the Keyboard
        Keyboard mKeyboard= new Keyboard(this,R.xml.kbd);

        // Lookup the KeyboardView
        KeyboardView mKeyboardView= (KeyboardView)findViewById(R.id.keyboardview);

        mKeyboardView.setKeyboard(mKeyboard);

        mKeyboardView.setPreviewEnabled(false);
        mKeyboardView.setOnKeyboardActionListener(new MyListener());

        //FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        //fab.setOnClickListener(new View.OnClickListener() {
        //    @Override
        //    public void onClick(View view) {
        //        Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
        //                .setAction("Action", null).show();
        //    }
        //});
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
            switchToDebug = true;
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onPause() {
        super.onPause();
        running = false;
    }

    @Override
    protected  void onResume() {
        super.onResume();
        running = true;
        switchToDebug = false;
        timer = new Timer();
        //breakAddress = getBreakAddress();

        timerTask = new TimerTask() {
            @Override
            public void run() {
                final int result = emuInstance.runBatch(0);
                emuInstance.interruptCpu();
                C64SurfaceView surfaceView = (C64SurfaceView) findViewById(R.id.Video);
                SurfaceHolder holder = surfaceView.getCreatedHolder();
                Canvas canvas = null;
                if (holder != null)
                    canvas = holder.lockCanvas();
                if (canvas != null) {
                    //Paint paint = new Paint();
                    //paint.setStyle(Paint.Style.FILL);
                    //paint.setColor(Color.RED);
                    emuInstance.populateFrame();
                    mByteBuffer.rewind();
                    mBitmap.copyPixelsFromBuffer(mByteBuffer);
                    canvas.drawBitmap(mBitmap,0,0, null);
                    holder.unlockCanvasAndPost(canvas);
                }
                if (result > 0) {
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            timer.cancel();
                            doAlert(result);
                        }
                    });
                } else if (!running | (result < 0) | switchToDebug) {
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            timer.cancel();
                            running = false;
                            //Intent i = new Intent(FrontActivity.this, DebugActivity.class);
                            //FrontActivity.this.startActivity(i);

                            //NB!! jump to debug activity
                        }
                    });

                }
                if (switchToDebug) {
                    Intent i = new Intent(FrontActivity.this, DebugActivity.class);
                    FrontActivity.this.startActivity(i);
                }
            }
        };

        timer.schedule(timerTask, 20, 20);

    }

}
