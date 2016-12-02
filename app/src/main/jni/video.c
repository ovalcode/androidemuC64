//
// Created by johan on 2016/12/02.
//

#include <stdbool.h>
#include <jni.h>
int line_count = 0;
int posInBuffer = 0;
jchar* g_buffer;

jchar colors_RGB_888[16][3] = {
{0, 0, 0},
                  {255, 255, 255},
                  {136, 0, 0},
                  {170, 255, 238},
                  {204, 68, 204},
                  {0, 204, 85},
                  {0, 0, 170},
                  {238, 238, 119},
                  {221, 136, 85},
                  {102, 68, 0},
                  {255, 119, 119},
                  {51, 51, 51},
                  {119, 119, 119},
                  {170, 255, 102},
                  {0, 136, 255},
                  {187, 187, 187}
};

jchar colors_RGB_565[16];

int inBorderArea(int line_num, int charPos) {
  //if
  //return false;
  //if
}

void initialise() {
  int i;
  for (i=0; i < 16; i++) {
    int red = colors_RGB_888[i][0] >> 3;
    int green = colors_RGB_888[i][1] >> 2;
    int blue = colors_RGB_888[i][2] >> 3;
    colors_RGB_565[i] =  red << 11;
    colors_RGB_565[i] =  green << 5;
    colors_RGB_565[i] =  blue << 0;
  }
}

inline void fillColor(int count, int colorEntryNumber) {
  int currentPos;
  for (currentPos = 0; currentPos < count; currentPos++) {
    g_buffer[posInBuffer] = colors_RGB_565[colorEntryNumber];
    posInBuffer++;
  }
}


void video_line_expired(struct timer_struct *tdev) {
  if (line_count > 299)
    return;

  int currentCharPos;
  for (currentCharPos = 0; currentCharPos < 3; currentCharPos++) {
  //do border
  }


  line_count++;
  if (line_count > 310)
    line_count = 0;
  //300 visible lines -> 311 total -> 11 lines invisible
}
