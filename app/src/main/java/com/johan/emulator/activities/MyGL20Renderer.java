package com.johan.emulator.activities;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import com.johan.emulator.engine.Emu6502;

import java.nio.ByteBuffer;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

/**
 * Created by johan on 2016/12/20.
 */

public class MyGL20Renderer implements GLSurfaceView.Renderer
{
    private final Context mActivityContext;

    //Matrix Initializations
    private final float[] mMVPMatrix = new float[16];
    private final float[] mProjMatrix = new float[16];
    private final float[] mVMatrix = new float[16];
    private float[] mRotationMatrix = new float[16];
    private Emu6502 emuInstance;
    private ByteBuffer byteBuffer;

    //Declare as volatile because we are updating it from another thread
    public volatile float mAngle;

    //private Triangle triangle;
    private Sprite sprite;

    public MyGL20Renderer(final Context activityContext)
    {
        mActivityContext = activityContext;
    }

    public void setEmuInstance(Emu6502 emuInstance) {
        this.emuInstance = emuInstance;
    }

    public void setByteBuffer (ByteBuffer byteBuffer) {
        this.byteBuffer = byteBuffer;

    }

    public void onSurfaceCreated(GL10 unused, EGLConfig config)
    {
        //Set the background frame color
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //Initialize Shapes
        //triangle = new Triangle();
        sprite = new Sprite(mActivityContext);
    }

    public void onDrawFrame(GL10 unused)
    {
//        try {
//            Thread.sleep(20);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }

//        try {
//            Thread.sleep(30);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }

//        System.out.println("Draw frame hit");
//        System.out.println("Draw frame hit");
        //Redraw background color
        // Draw objects back to front
        GLES20.glDisable(GLES20.GL_DEPTH_TEST);

        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        GLES20.glDepthMask(false);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        //Set the camera position (View Matrix)
        Matrix.setLookAtM(mVMatrix, 0, 0, 0, 3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

        //Calculate the projection and view transformation
        Matrix.multiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mVMatrix, 0);

        //Create a rotation transformation for the triangle
        Matrix.setRotateM(mRotationMatrix, 0, mAngle, 0, 0, -1.0f);

        //Combine the rotation matrix with the projection and camera view
        Matrix.multiplyMM(mMVPMatrix, 0, mRotationMatrix, 0, mMVPMatrix, 0);
        GLES20.glEnable(GLES20.GL_BLEND);

        //Draw Shape
        //triangle.Draw(mMVPMatrix);
//        System.out.println("progress");
        byteBuffer.rewind();
        for (int i = 0; i < byteBuffer.limit(); i++) {
            byteBuffer.put((byte) 0);
        }
        byteBuffer.rewind();
        emuInstance.runBatch(0);
//        System.out.println("after batch");
        sprite.Draw(mMVPMatrix, byteBuffer);
        //GLES20.glFinish();
//        try {
//            Thread.sleep(1000);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
    }

    public void onSurfaceChanged(GL10 unused, int width, int height)
    {
        GLES20.glViewport(0, 0, width, height);

        float ratio = (float) width / height;

        //This Projection Matrix is applied to object coordinates in the onDrawFrame() method
//        Matrix.frustumM(mProjMatrix, 0, -ratio, ratio, -1, 1, 3, 7);
        Matrix.orthoM(mProjMatrix,0, -ratio, ratio, -1, 1, 3, 7);
    }

    public static int loadShader(int type, String shaderCode)
    {
        //Create a Vertex Shader Type Or a Fragment Shader Type (GLES20.GL_VERTEX_SHADER OR GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);

        //Add The Source Code and Compile it
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }
}