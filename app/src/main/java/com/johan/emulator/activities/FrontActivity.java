package com.johan.emulator.activities;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DrawFilter;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import com.johan.emulator.R;
import com.johan.emulator.engine.Emu6502;
import com.johan.emulator.view.C64SurfaceView;
import com.johan.emulator.view.JoystickView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class FrontActivity extends AppCompatActivity {
/*
refactor

emuInstance.setMainActivityObject(this); in create -> move this to creation emuinstance + do definition of method ids
memory change currentacitivity to emuInstance
//do attach to different thread
 */
    Timer timer;
    TimerTask timerTask;
    int xpos = 100;
    int ypos = 100;
    int numSamples = 0;
    long runningTotal = 0;

    private ByteBuffer mByteBuffer;
    private ByteBuffer mTape;
    private ByteBuffer keyBoardMatrix;
    private Keyboard.Key shiftKey;
//    private Bitmap mBitmap;
    private Paint paint;
    private DrawFilter filter;


    int screenWidth;
    float scale;

    private boolean running = false;
    private boolean switchToDebug = false;
    final Handler handler = new Handler();
    private Emu6502 emuInstance; //Emu6502.getInstance(getResources().getAssets());


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_front);

        GLSurfaceView mGLSurfaceView = (GLSurfaceView) findViewById(R.id.Video);
        //C64SurfaceView surfaceView = (GLSurfaceView) findViewById(R.id.Video);
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        screenWidth = displayMetrics.widthPixels;
        mGLSurfaceView.setLayoutParams(new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,(int)(300.0*screenWidth/368.0)));
        mGLSurfaceView.setEGLContextClientVersion(2);
        mGLSurfaceView.setEGLContextClientVersion(2);
        MyGL20Renderer myRenderer = new MyGL20Renderer(this);
        mGLSurfaceView.setRenderer(myRenderer);

//        initAudio(44100, 16, 512);

        scale = (float)(screenWidth / 368.0);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        emuInstance = Emu6502.getInstance(getResources().getAssets());
        mByteBuffer = ByteBuffer.allocateDirect(
                (368 + //foreground sprite
                 368 + //front
                 368 + //backround sprite
                 320 //background

                 ) * (300) * 4);
        //mByteBuffer.order(ByteOrder.nativeOrder());
        keyBoardMatrix = ByteBuffer.allocateDirect(8);
        emuInstance.setKeyboardMatrix(keyBoardMatrix);
        myRenderer.setEmuInstance(emuInstance);
        myRenderer.setByteBuffer(mByteBuffer);
        //mBitmap = Bitmap.createBitmap(368,300, Bitmap.Config.RGB_565);
        filter = new PaintFlagsDrawFilter(Paint.ANTI_ALIAS_FLAG, 0);
        paint = new Paint();
        paint.setAntiAlias(false);
        paint.setDither(false);
        paint.setFilterBitmap(false);

        emuInstance.setFrameBuffer(mByteBuffer);
//        emuInstance.setMainActivityObject(this);

        // Create the Keyboard
        Keyboard mKeyboard= new Keyboard(this,R.xml.kbd);

        // Lookup the KeyboardView
        KeyboardView mKeyboardView= (KeyboardView)findViewById(R.id.keyboardview);

        mKeyboardView.setKeyboard(mKeyboard);

        List<Keyboard.Key> keys = mKeyboard.getKeys();
        for (Keyboard.Key currentKey : keys) {
            if (currentKey.codes[0] == 15)
                shiftKey = currentKey;
        }


        mKeyboardView.setPreviewEnabled(false);
        mKeyboardView.setOnKeyboardActionListener(new KeyboardView.OnKeyboardActionListener() {

            @Override
            public void onPress(int i) {
                if (i == 15)
                    return;
                System.out.println("Kode: "+i);
                byte col = (byte)(i & 7);
                col = (byte) (1 << col);
                byte row = (byte)(i & 0x38);
                row = (byte) (row >> 3);
                row = (byte)(7 - row);
                System.out.println("Row: "+row);
                System.out.println("Col: "+col);
                byte tempKey = keyBoardMatrix.get(row);
                tempKey = (byte)(tempKey | col);
                keyBoardMatrix.put(row, tempKey);
            }

            @Override
            public void onRelease(int i) {
                if (i == 15)
                    return;
                byte col = (byte)(i & 7);
                col = (byte) (1 << col);
                byte row = (byte)(i & 0x38);
                row = (byte) (row >> 3);
                row = (byte)(7 - row);
                byte tempKey = keyBoardMatrix.get(row);
                tempKey = (byte)(tempKey & ~col);
                keyBoardMatrix.put(row, tempKey);
            }

            @Override
            public void onKey(int i, int[] ints) {
                if (i != 15)
                    return;
                if (shiftKey.on) {
                    byte tempKey = keyBoardMatrix.get(6);
                    tempKey = (byte)(tempKey | (1 << 7));
                    keyBoardMatrix.put(6, tempKey);

                } else {
                    byte tempKey = keyBoardMatrix.get(6);
                    tempKey = (byte)(tempKey & ~ (1 << 7));
                    keyBoardMatrix.put(6, tempKey);

                }
            }

            @Override
            public void onText(CharSequence charSequence) {

            }

            @Override
            public void swipeLeft() {

            }

            @Override
            public void swipeRight() {

            }

            @Override
            public void swipeDown() {

            }

            @Override
            public void swipeUp() {

            }
        });

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
            emuInstance.setRunning(false);
            return true;
        } else if (id == R.id.action_attach) {
            Intent i = new Intent(this, FileDialogueActivity.class);
            startActivityForResult(i, 1);

            return true;
        } else if(id == R.id.action_play) {
            emuInstance.togglePlay();
        } else if(id == R.id.action_test) {
            KeyboardView mKeyboardView= (KeyboardView)findViewById(R.id.keyboardview);
            JoystickView mJoystickView = (JoystickView) findViewById(R.id.joystickView);
            if (mKeyboardView.getVisibility() == View.GONE) {
                mJoystickView.setVisibility(View.GONE);
                mKeyboardView.setVisibility(View.VISIBLE);
            } else {
                mJoystickView.setVisibility(View.VISIBLE);
                mKeyboardView.setVisibility(View.GONE);

            }
            //mKeyboardView.setVisibility(View.GONE);
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


    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        // See which child activity is calling us back.
        if (requestCode == 1){
            if (resultCode == RESULT_OK) {
                String curFileName = data.getStringExtra("GetFullPath");
                try {

                    RandomAccessFile file = new RandomAccessFile(curFileName, "r");
                    FileChannel inChannel = file.getChannel();
                    long fileSize = inChannel.size();
                    mTape = ByteBuffer.allocateDirect((int) fileSize);
                    inChannel.read(mTape);
                    mTape.rewind();
                    inChannel.close();
                    file.close();
                    emuInstance.attachNewTape((int)fileSize, mTape);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

}
