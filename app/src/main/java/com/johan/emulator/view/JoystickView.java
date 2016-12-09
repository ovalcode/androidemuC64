package com.johan.emulator.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

/**
 * Created by johan on 2016/12/08.
 */

public class JoystickView extends View {

    Bitmap mBitmap;
    Canvas mCanvas;

    public JoystickView(Context c, AttributeSet attrs) {
        super(c, attrs);
    }

    @Override
    protected  void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        mBitmap = Bitmap.createBitmap(w,h, Bitmap.Config.ARGB_8888);
        mCanvas = new Canvas(mBitmap);
    }

    protected void drawArcCanvas(Canvas canvas) {
        int canvasHeight = canvas.getHeight();
        int canvasWidth = canvas.getWidth();
        int arcHeight = canvasHeight;
        int arcWidth = arcHeight;

        int arcLeftPos = (canvasWidth - arcWidth) /2;
        int arcRightPos = arcLeftPos + arcWidth;
//        Path s = new Path();
//        s.moveTo(0,0);
        //public RectF(float left, float top, float right, float bottom) { /* compiled code */ }
        RectF rectf = new RectF(arcLeftPos, //left
                0, //top
                arcRightPos, //right
                arcHeight); //bottom
//        s.addArc(rectf, 0,270);
//        s.close();

        Paint mPaint = new Paint();
        mPaint.setColor(Color.RED);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeWidth(4f);
//        canvas.drawPath(s, mPaint);
        canvas.drawArc(rectf, 270, 180, false, mPaint);
    }

    protected void drawArcPath(Canvas canvas) {
        int canvasHeight = canvas.getHeight();
        int canvasWidth = canvas.getWidth();
        int arcHeight = canvasHeight;
        int arcWidth = arcHeight;
        int arcLeftPos = (canvasWidth - arcWidth) /2;

        Paint mPaint = new Paint();
        mPaint.setColor(Color.RED);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeWidth(4f);


        Path s = new Path();
        //s.moveTo(500,0);

        RectF rectf = new RectF(canvasWidth - arcWidth, //left
                0, //top
                canvasWidth, //right
                arcHeight); //bottom
         s.addArc(rectf, 270, 180);

        s.lineTo(canvasWidth - 200 - (arcWidth / 2), arcHeight);

        rectf = new RectF(canvasWidth - arcWidth -200, //left
                0, //top
                canvasWidth - 200, //right
                arcHeight); //bottom
        s.addArc(rectf, 90, -180);
        s.moveTo(canvasWidth - 200 - (arcWidth / 2), 0);
        s.close();
        canvas.drawPath(s, mPaint);

    }

    void movePathTo(Path path, double x, double y) {
        x = x + 200;
        y = y - 200;

        path.moveTo((float)x, (float)-y);
    }

    void linePathTo(Path path, double x, double y) {
        x = x + 200;
        y = y - 200;

        path.lineTo((float)x, (float)-y);
    }


    void drawArcTo (Path path, double radius, double beginAngle, double sweepAngle) {
        double left = -radius;
        double top = radius;
        double right = radius;
        double bottom = -radius;

        left = left + 200;
        top = top - 200;
        right = right + 200;
        bottom = bottom - 200;

        RectF rectf = new RectF((float) left,
                (float)-top,
                (float)               right,
                (float)                -bottom

        );

        path.arcTo(rectf, (float)beginAngle, (float)sweepAngle);
    }

    void drawAttempt(Canvas canvas) {

        Paint mPaint = new Paint();
        mPaint.setColor(Color.RED);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeWidth(10f);


        Path path = new Path();
        double x = 100 * Math.cos(Math.PI * 90 / 180);
        double y = 100 * Math.sin(Math.PI * 90 / 180);

        movePathTo(path, x, y);
        drawArcTo(path, 100, -90, 90);

        x = 200 * Math.cos(Math.PI * 0 / 180);
        y = 200 * Math.sin(Math.PI * 0 / 180);

        linePathTo(path, x, y);
        drawArcTo(path, 200, 0, -90);

        path.close();

        canvas.drawPath(path, mPaint);
        //move to arc first
        //to arc
        return;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        drawAttempt(canvas);
        //drawArcPath(canvas);
    }

}
