package com.johan.emulator.activities;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLUtils;

import com.johan.emulator.R;

import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.nio.ByteBuffer;

/**
 * Created by johan on 2016/12/20.
 */

public class Sprite
{
    //Reference to Activity Context
    private final Context mActivityContext;

    //Added for Textures
    private final FloatBuffer mCubeTextureCoordinates;
    private int mTextureUniformHandle;
    private int mTextureCoordinateHandle;
    private final int mTextureCoordinateDataSize = 2;
    private int mTextureDataHandle;

    private static int[] textureHandle;

    //static Bitmap c64bitmap = Bitmap.createBitmap(368,300,Bitmap.Config.RGB_565);

    private final String vertexShaderCode =
//Test
            "attribute vec2 a_TexCoordinate;" +
                    "varying vec2 v_TexCoordinate;" +
//End Test
                    "uniform mat4 uMVPMatrix;" +
                    "attribute vec4 vPosition;" +
                    "void main() {" +
                    "  gl_Position =  uMVPMatrix * vPosition;" +
                    //Test
                    "v_TexCoordinate = a_TexCoordinate;" +
                    //End Test
                    "}";

    private final String fragmentShaderCode =
            "precision mediump float;" +
                    "uniform vec4 v_Color;" +
//Test
                    "uniform sampler2D u_Texture;" +
                    "varying vec2 v_TexCoordinate;" +
//End Test
                    "void main() {" +
//"gl_FragColor = vColor;" +
                    "gl_FragColor = ( texture2D(u_Texture, v_TexCoordinate));" +
                    "}";

    private final int shaderProgram;
    private final FloatBuffer vertexBuffer;
    private final ShortBuffer drawListBuffer;
    private final ShortBuffer drawListBuffer2;
    private final ShortBuffer drawListBuffer3;
    private final ShortBuffer drawListBuffer4;

    private int mPositionHandle;
    private int mColorHandle;
    private int mMVPMatrixHandle;

    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 3;
    static float spriteCoords[] = {
            //Sprite Foreground
            -1.22666f,  1f, 0f,  // top left
            -1.22666f, -1f, 0f,   // bottom left
            1.22666f, -1f, 0f,  // bottom right
            1.22666f,  1f, 0f,

            //Foreground
            -1.22666f,  1f, -1f,  // top left
            -1.22666f, -1f, -1f,   // bottom left
            1.22666f, -1f, -1f,  // bottom right
            1.22666f,  1f, -1f,

            //Sprite background
            -1.22666f,  1f, -2f,  // top left
            -1.22666f, -1f, -2f,   // bottom left
            1.22666f, -1f, -2f,  // bottom right
            1.22666f,  1f, -2f,


            //Background
            -1.06666f,  0.6666f, -4f,  // top left
            -1.06666f, -0.6666f, -4f,   // bottom left
            1.06666f, -0.6666f, -4f,  // bottom right
            1.06666f,  0.6666f, -4f,

            //TODO: Add background coords + change to three value coordinates
    }; //top right

    private short drawOrder[] = { 0, 1, 2, 0, 3, 2, 0 }; //Order to draw vertices
    private short drawOrder2[] = { 0 + 4, 1 + 4, 2 + 4, 0 + 4, 3 + 4, 2 + 4, 0 + 4 }; //Order to draw vertices
    private short drawOrder3[] = { 0 + 8, 1 + 8, 2 + 8, 0 + 8, 3 + 8, 2 + 8, 0 + 8 }; //Order to draw vertices
    private short drawOrder4[] = { 0 + 12, 1 + 12, 2 + 12, 0 + 12, 3 + 12, 2 + 12, 0 + 12 }; //Order to draw vertices

    private final int vertexStride = COORDS_PER_VERTEX * 4; //Bytes per vertex

    // Set color with red, green, blue and alpha (opacity) values
    //float color[] = { 0.63671875f, 0.76953125f, 0.22265625f, 1.0f };
float color[] = { 1f, 0f, 0f, 1.0f };
    public Sprite(final Context activityContext)
    {
        mActivityContext = activityContext;

        //Initialize Vertex Byte Buffer for Shape Coordinates / # of coordinate values * 4 bytes per float
        ByteBuffer bb = ByteBuffer.allocateDirect(spriteCoords.length * 4);
        //Use the Device's Native Byte Order
        bb.order(ByteOrder.nativeOrder());
        //Create a floating point buffer from the ByteBuffer
        vertexBuffer = bb.asFloatBuffer();
        //Add the coordinates to the FloatBuffer
        vertexBuffer.put(spriteCoords);
        //Set the Buffer to Read the first coordinate
        vertexBuffer.position(0);

        // S, T (or X, Y)
        // Texture coordinate data.
        // Because images have a Y axis pointing downward (values increase as you move down the image) while
        // OpenGL has a Y axis pointing upward, we adjust for that here by flipping the Y axis.
        // What's more is that the texture coordinates are the same for every face.
        final float[] cubeTextureCoordinateData =
                {
                        //Front face
            /*0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f*/

//                        -0.5f,  0.5f,
//                        -0.5f, -0.5f,
//                        0.5f, -0.5f,
//                        0.5f,  0.5f

//                        -1.22666f,  1f, 0f,  // top left
//                        -1.22666f, -1f, 0f,   // bottom left
//                        1.22666f, -1f, 0f,  // bottom right
//                        1.22666f,  1f, 0f,

//368 + 368 + 368 + 320
// 0.258426966 + 0.258426966 + 0.258426966 + 0.224719101
                        //0.534883721 -> x fore
                        //Sprite Foreground
                        0.0f, 0.0f, // top left
                        0.0f, 1.0f, // bottom left
                        0.258426966f, 1.0f, // bottom right
                        0.258426966f, 0.0f, //top right

                        //Foreground
                        0.258426966f, 0.0f, // top left
                        0.258426966f, 1.0f, // bottom left
                        0.516853932f, 1.0f, // bottom right
                        0.516853932f, 0.0f, //top right

                        //Sprite Background
                        0.516853932f, 0.0f, // top left
                        0.516853932f, 1.0f, // bottom left
                        0.775280898f, 1.0f, // bottom right
                        0.775280898f, 0.0f, //top right


                        //Background
                        0.775280898f, 0.166666f, // top left
                        0.775280898f, 0.833333f, // bottom left
                        1.0f, 0.833333f, // bottom right
                        1.0f, 0.166666f //top right

//TODO
                };

        mCubeTextureCoordinates = ByteBuffer.allocateDirect(cubeTextureCoordinateData.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mCubeTextureCoordinates.put(cubeTextureCoordinateData).position(0);

        //Initialize byte buffer for the draw list
        //??? Check this out
        ByteBuffer dlb = ByteBuffer.allocateDirect(spriteCoords.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);

        ByteBuffer dlb2 = ByteBuffer.allocateDirect(spriteCoords.length * 2);
        dlb2.order(ByteOrder.nativeOrder());
        drawListBuffer2 = dlb2.asShortBuffer();
        drawListBuffer2.put(drawOrder2);
        drawListBuffer2.position(0);

        ByteBuffer dlb3 = ByteBuffer.allocateDirect(spriteCoords.length * 2);
        dlb3.order(ByteOrder.nativeOrder());
        drawListBuffer3 = dlb3.asShortBuffer();
        drawListBuffer3.put(drawOrder3);
        drawListBuffer3.position(0);

        ByteBuffer dlb4 = ByteBuffer.allocateDirect(spriteCoords.length * 2);
        dlb4.order(ByteOrder.nativeOrder());
        drawListBuffer4 = dlb4.asShortBuffer();
        drawListBuffer4.put(drawOrder4);
        drawListBuffer4.position(0);


        int vertexShader = MyGL20Renderer.loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = MyGL20Renderer.loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

        shaderProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(shaderProgram, vertexShader);
        GLES20.glAttachShader(shaderProgram, fragmentShader);

        //Texture Code
        GLES20.glBindAttribLocation(shaderProgram, 0, "a_TexCoordinate");

        GLES20.glLinkProgram(shaderProgram);

        //Load the texture
        //mTextureDataHandle = loadTexture(mActivityContext, R.drawable.test        );
    }

    public void Draw(float[] mvpMatrix, ByteBuffer imgBuffer)
    {
        //System.out.println("In SPrite Draw");
        //Add program to OpenGL ES Environment
        GLES20.glUseProgram(shaderProgram);

        //Get handle to vertex shader's vPosition member
        mPositionHandle = GLES20.glGetAttribLocation(shaderProgram, "vPosition");

        //Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);

        //Prepare the triangle coordinate data
        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, true, vertexStride, vertexBuffer);

        //Get Handle to Fragment Shader's vColor member
        mColorHandle = GLES20.glGetUniformLocation(shaderProgram, "v_Color");

        //System.out.println("In SPrite Draw 2");

        //Set the Color for drawing the triangle
        GLES20.glUniform4fv(mColorHandle, 1, color, 0);

        //Set Texture Handles and bind Texture
        mTextureUniformHandle = GLES20.glGetAttribLocation(shaderProgram, "u_Texture");
        mTextureCoordinateHandle = GLES20.glGetAttribLocation(shaderProgram, "a_TexCoordinate");

        //Set the active texture unit to texture unit 0.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);

        mTextureDataHandle = loadTextureFromByteBuffer(imgBuffer);
        //Bind the texture to this unit.
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDataHandle);

        //Tell the texture uniform sampler to use this texture in the shader by binding to texture unit 0.
        GLES20.glUniform1i(mTextureUniformHandle, 0);

        //Pass in the texture coordinate information
        //NB!! move to end call these three lines again
        mCubeTextureCoordinates.position(0);
        GLES20.glVertexAttribPointer(mTextureCoordinateHandle, mTextureCoordinateDataSize, GLES20.GL_FLOAT, true, 0, mCubeTextureCoordinates);
        GLES20.glEnableVertexAttribArray(mTextureCoordinateHandle);


        //Get Handle to Shape's Transformation Matrix
        mMVPMatrixHandle = GLES20.glGetUniformLocation(shaderProgram, "uMVPMatrix");



        //Apply the projection and view transformation
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);

        //Draw the triangle
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder4.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer4);
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder3.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer3);
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder2.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer2);
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer);




        //Draw another triangle



        //Disable Vertex Array
        GLES20.glDisableVertexAttribArray(mPositionHandle);

        //System.out.println("In SPrite Draw 3");

//        try {
//            Thread.sleep(1000);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
    }


    public static int loadTextureFromByteBuffer(ByteBuffer buffer)
    {
        //final int[] textureHandle = new int[1];
        if (textureHandle == null) {
            textureHandle = new int[1];
            GLES20.glGenTextures(1, textureHandle, 0);
        }

        if (textureHandle[0] != 0)
        {

            // Bind to the texture in OpenGL
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            // Set filtering
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
            buffer.rewind();
            Bitmap c64bitmap = Bitmap.createBitmap(368+368+368+320,300,Bitmap.Config.ARGB_8888);
             c64bitmap.copyPixelsFromBuffer(buffer);
            // Load the bitmap into the bound texture.
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, c64bitmap, 0);
            c64bitmap.recycle();

            // Recycle the bitmap, since its data has been loaded into OpenGL.
//            bitmap.recycle();
        }

        if (textureHandle[0] == 0)
        {
            throw new RuntimeException("Error loading texture.");
        }

        return textureHandle[0];
    }
}