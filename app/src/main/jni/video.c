//
// Created by johan on 2016/12/02.
//

#include <stdbool.h>
#include <jni.h>
#include <alarm.h>

int line_count = 0;
int posInBuffer = 0;
int posInCharMem = 0;
int screenLineRegion = 0;
extern jchar charRom[4096];
extern jchar* g_buffer;
extern int frameFinished;

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

int line_in_visible;

int inBorderArea(int line_num, int charPos) {
  //if
  //return false;
  //if
}

void initialise_video() {
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

inline void updatelineCharPos() {
  if ((line_count < 50) || (line_count > (49 + 200))) {
    screenLineRegion = false;
    return;
  }

  screenLineRegion = true;

  line_in_visible = line_count - 50;

  if (line_in_visible == 0) {
    posInCharMem = 0;
    return;
  }

  if (!(line_in_visible & 7))
    posInCharMem = posInCharMem + 40;
}

inline void drawScreenLine() {
  int i;
  for (i = 0; i < 40; i++) {
    jchar charcode = memory_read(1024 + i + posInCharMem);
    int bitmapDataRow = charRom[(charcode << 3) | (line_in_visible & 7)];
    int j;
    int foregroundColor = memory_read(0xd800 + i + posInCharMem) & 0xf;
    int backgroundColor = memory_read(0xd021) & 0xf;
    for (j = 0; j < 8; j++) {
      int pixelSet = bitmapDataRow & 0x80;
      if (pixelSet) {
        g_buffer[posInBuffer] = colors_RGB_565[foregroundColor];
      } else {
        g_buffer[posInBuffer] = colors_RGB_565[backgroundColor];
      }
      posInBuffer++;
      bitmapDataRow = bitmapDataRow << 1;
    }
  }
}

inline void processLine() {
  if (line_count > 299)
    return;

  updatelineCharPos();
  fillColor(24, memory_read(0xd020) & 0xf);
  int screenEnabled = (memory_read(0xd011) & 0x10) ? 1 : 0;
  if (screenLineRegion && screenEnabled) {
    drawScreenLine();
  } else {
    fillColor(320, memory_read(0xd020) & 0xf);
  }
  fillColor(24, memory_read(0xd020) & 0xf);
}


void video_line_expired(struct timer_struct *tdev) {
  tdev->remainingCycles = 63;
  processLine();
  line_count++;
  if (line_count > 310) {
    line_count = 0;
    frameFinished = 1;
    posInBuffer = 0;
  }
}

struct timer_struct getVideoInstance() {
  struct timer_struct myVideo;
  myVideo.expiredevent = &video_line_expired;
  myVideo.remainingCycles = 63;
  myVideo.started = 1;
  //myVideo.interrupt = &interrupt_timer_A;
  return myVideo;
}

