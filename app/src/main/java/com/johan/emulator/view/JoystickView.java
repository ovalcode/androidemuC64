package com.johan.emulator.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.johan.emulator.engine.Emu6502;

import java.util.ArrayList;

/**
 * Created by johan on 2016/12/08.
 */

public class JoystickView extends View {

    private static float OFFSET_X;
    private static float OFFSET_Y;
    private static float INNER_RADIUS;
    private static float OUTER_RADIUS;
    private float CENTER_FIRE_X;
    private float CENTER_FIRE_Y;
    private float FIRE_RADIUS;
    //private Emu6502 emuInstance;

    int imageDownNumber;
    boolean fireDown;
    //boolean imageDown = false;
    Bitmap mBitmap;
    Canvas mCanvas;
    Paint mPaint;



    private long lastProcessed = System.currentTimeMillis();

    private ArrayList<Segment> segmentList = new ArrayList<Segment>();

    public JoystickView(Context c, AttributeSet attrs) {
        super(c, attrs);
        //emuInstance = Emu6502.getInstance();
        imageDownNumber = -1;
        fireDown = false;
        float startAngle = (float) (90+22.5);
        for (int i = 0; i < 8; i++) {
            segmentList.add(new Segment(startAngle + 5, 45 - 5 -5));
            startAngle = (float) startAngle - 45;
        }
    }

    @Override
    protected  void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        mBitmap = Bitmap.createBitmap(w,h, Bitmap.Config.ARGB_8888);
        mCanvas = new Canvas(mBitmap);
        int totalRadius = w / 2;
        OUTER_RADIUS = totalRadius / 2;
        INNER_RADIUS = (int)(OUTER_RADIUS * 0.6);
        OFFSET_X = OUTER_RADIUS;
        OFFSET_Y = OUTER_RADIUS;

        CENTER_FIRE_X = (float) (w * 0.75);
        CENTER_FIRE_Y = OUTER_RADIUS;
        FIRE_RADIUS = OUTER_RADIUS / 2;

        mPaint = new Paint();
        mPaint.setColor(Color.RED);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeJoin(Paint.Join.ROUND);
        mPaint.setStrokeWidth(4f);

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
        x = x + OFFSET_X;
        y = y - OFFSET_Y;

        path.moveTo((float)x, (float)-y);
    }

    void linePathTo(Path path, double x, double y) {
        x = x + OFFSET_X;
        y = y - OFFSET_Y;

        path.lineTo((float)x, (float)-y);
    }


    void drawArcTo (Path path, double radius, double beginAngle, double sweepAngle) {
        double left = -radius;
        double top = radius;
        double right = radius;
        double bottom = -radius;

        left = left + OFFSET_X;
        top = top - OFFSET_Y;
        right = right + OFFSET_X;
        bottom = bottom - OFFSET_Y;

        RectF rectf = new RectF((float) left,
                (float)-top,
                (float)               right,
                (float)                -bottom

        );

        path.arcTo(rectf, (float)beginAngle, (float)sweepAngle);
    }

    private int getCurrentNumberDown(int x, int y) {
        float xTranslated = x - OUTER_RADIUS;
        float yTranslated = -y + OUTER_RADIUS;

        double calculatedRadius = Math.sqrt(xTranslated * xTranslated + yTranslated * yTranslated);
        if (calculatedRadius > OUTER_RADIUS)
            return -1;
        double angleInRadians = Math.acos(xTranslated / calculatedRadius);
        float angleInDegrees = (float) (angleInRadians * 180 / Math.PI);
        angleInDegrees = Math.abs(angleInDegrees);
        if (angleInDegrees > 90)
            angleInDegrees = 180 - angleInDegrees;
        if (xTranslated < 0 && yTranslated >=0)
            angleInDegrees = 180 - angleInDegrees;
        else if (xTranslated < 0 && yTranslated < 0)
            angleInDegrees = 180 + angleInDegrees;
        else if (xTranslated >= 0 && yTranslated < 0)
            angleInDegrees = 360 - angleInDegrees;

        if (angleInDegrees > segmentList.get(0).startAngle)
            angleInDegrees = angleInDegrees -360;
        int i = 0;
        for (Segment segment : segmentList) {
            float startAngle = segment.startAngle;
//            if (startAngle < 0)
//                startAngle = 360 - startAngle;
            float endAngle = segment.endAngle;
//            if (endAngle < 0)
//                endAngle = 360 - endAngle;
            if (angleInDegrees <= startAngle && angleInDegrees >= endAngle) {
                return i;
            }
            i++;
        }

        return -1;
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

    public boolean isActionUp(int action) {
        return (action == MotionEvent.ACTION_UP) || (action == MotionEvent.ACTION_POINTER_UP) || (action == MotionEvent.ACTION_POINTER_2_UP) || (action == MotionEvent.ACTION_POINTER_1_UP);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
//        event.eve
        //System.out.print("Number of points: " + event.getPointerCount());
        //for (int i = 0; i < event.getPointerCount(); i++) {
        //    System.out.print(" Point: " + i + " X="+event.getX(i) + " Y=" + event.getY(i));
        //    System.out.println();
        //}
        //System.out.println(event.getX() + " " + event.getY() + " " + event.getActionIndex());

        long currentTime = System.currentTimeMillis();
        int action = event.getAction();
        if (action != 2)
          System.out.println(action + " " + event.getActionIndex());
        if (((currentTime - lastProcessed) < 200) && !isActionUp(action))
            return true;
        lastProcessed = currentTime;
        int currentNumDown = -1;
        boolean currentFireDown = false;
        int upActionIndex = -1;
        if (isActionUp(action)) {
            upActionIndex = event.getActionIndex();
        }
        boolean found = false;
        int eventX = 0;
        int eventY = 0;
        for (int i = 0; i < event.getPointerCount(); i++) {
            if (i == upActionIndex)
                continue;
            if ((event.getX(i) <= OUTER_RADIUS * 2 + 10) && (event.getY(i) <= OUTER_RADIUS * 2 + 10)) {
                found = true;
                eventX = (int)event.getX(i);
                eventY = (int)event.getY(i);
            }

        }

        if (found) {
            currentNumDown = getCurrentNumberDown((int)eventX,(int) eventY);
        }

        for (int i = 0; i < event.getPointerCount(); i++) {
            if (i == upActionIndex)
                continue;
            if ((event.getX(i) >= CENTER_FIRE_X - FIRE_RADIUS) && (event.getX(i) <= CENTER_FIRE_X + FIRE_RADIUS)
                    && (event.getY(i) >= CENTER_FIRE_Y - FIRE_RADIUS)
                    && (event.getY(i) <= CENTER_FIRE_Y + FIRE_RADIUS)) {
                currentFireDown = true;
            }

        }

        if ((currentNumDown != imageDownNumber) || (currentFireDown != fireDown)){
            imageDownNumber = currentNumDown;
            fireDown = currentFireDown;
            Emu6502.setFireButton(fireDown ? 1 : 0);
            Emu6502.setJoystickDirectionButton(imageDownNumber);
            invalidate();
        }
        return true;
    }

    void drawSegment(Canvas canvas, double startAngle, double sweepAngle) {

        //Paint mPaint = new Paint();
        //mPaint.setColor(Color.RED);
        //mPaint.setStyle(Paint.Style.STROKE);
        //mPaint.setStrokeJoin(Paint.Join.ROUND);
        //mPaint.setStrokeWidth(10f);


        Path path = new Path();
        double x = INNER_RADIUS * Math.cos(Math.PI * startAngle / 180);
        double y = INNER_RADIUS * Math.sin(Math.PI * startAngle / 180);

        movePathTo(path, x, y);

        drawArcTo(path, INNER_RADIUS, -startAngle, sweepAngle);

        x = OUTER_RADIUS * Math.cos(Math.PI * (startAngle - sweepAngle) / 180);
        y = OUTER_RADIUS * Math.sin(Math.PI * (startAngle - sweepAngle) / 180);

        linePathTo(path, x, y);
        drawArcTo(path, OUTER_RADIUS, -startAngle+sweepAngle, -sweepAngle);

        path.close();

        canvas.drawPath(path, mPaint);
        //move to arc first
        //to arc
        return;
    }


    @Override
    protected void onDraw(Canvas canvas) {
        int i = 0;
        for (Segment segment : segmentList) {
            if (imageDownNumber == i)
                mPaint.setStyle(Paint.Style.FILL);
            else
                mPaint.setStyle(Paint.Style.STROKE);
            drawSegment(canvas, segment.startAngle, segment.sweepAngle);
            i++;
        }
        if (fireDown)
           mPaint.setStyle(Paint.Style.FILL);
        else
            mPaint.setStyle(Paint.Style.STROKE);
        canvas.drawCircle(CENTER_FIRE_X, CENTER_FIRE_Y, FIRE_RADIUS, mPaint);

    }

    private class Segment {
        private float startAngle;
        private float sweepAngle;
        private float endAngle;

        Segment (float startAngle, float sweepAngle) {
            this.startAngle = startAngle;
            this.sweepAngle = sweepAngle;
            this.endAngle = startAngle - sweepAngle;
        }

        public float getStartAngle() {
            return this.startAngle;
        }

        public float getSweepAngle() {
            return this.sweepAngle;
        }

        public float getEndAngle() {
            return this.endAngle;
        }
    }

}
