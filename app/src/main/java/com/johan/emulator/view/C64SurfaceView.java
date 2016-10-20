package com.johan.emulator.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.johan.emulator.R;

/**
 * Created by johan on 2016/10/20.
 */

public class C64SurfaceView extends SurfaceView {

    private SurfaceHolder surfaceHolder;
    public C64SurfaceView(Context context) {
        super(context);
        init();
    }

    public C64SurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public C64SurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        surfaceHolder= getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Canvas canvas = holder.lockCanvas();
                Paint paint = new Paint();
                paint.setStyle(Paint.Style.FILL);
                paint.setColor(Color.RED);
                canvas.drawCircle(100,100, 50, paint);
                holder.unlockCanvasAndPost(canvas);

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder,
                                       int format, int width,int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }



}
