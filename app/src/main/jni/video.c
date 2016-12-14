//
// Created by johan on 2016/12/02.
//

#include <stdbool.h>
#include <jni.h>
#include <alarm.h>
#include <memory.h>

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

jchar vic_interrupt = 0;

int line_in_visible;

void initialise_video() {
  int i;
  for (i=0; i < 16; i++) {
    int red = colors_RGB_888[i][0] >> 3;
    int green = colors_RGB_888[i][1] >> 2;
    int blue = colors_RGB_888[i][2] >> 3;
    colors_RGB_565[i] =  (red << 11) | (green << 5) | (blue << 0);
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

static inline void drawScreenLineNormalText() {
  int i;
  int batchCharMem[40];
  int batchColorMem[40];
  int memPointer = memory_unclaimed_io_read(0xd018);
  int videoMemoryBase = memPointer & 0xf0;
  videoMemoryBase = videoMemoryBase << 6;
  int charROMBase = memPointer & 0xe;
  charROMBase = charROMBase << 10;
  int backgroundColor = memory_unclaimed_io_read(0xd021) & 0xf;
  memory_read_batch(batchCharMem, videoMemoryBase + posInCharMem, 40);
  memory_read_batch_io_unclaimed(batchColorMem, 0xd800 + posInCharMem, 40);
  for (i = 0; i < 40; i++) {
    jchar charcode = batchCharMem[i];//memory_read(1024 + i + posInCharMem);
    int bitmapDataRow = memory_read_vic_model(((charcode << 3) | (line_in_visible & 7)) + charROMBase);
    int j;
    int foregroundColor = batchColorMem[i] & 0xf;//memory_read(0xd800 + i + posInCharMem) & 0xf;

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

static inline void drawScreenLineMultiColorText() {
  int i;
  int batchCharMem[40];
  int batchColorMem[40];
  int color_tablet[4];
  int memPointer = memory_unclaimed_io_read(0xd018);
  int videoMemoryBase = memPointer & 0xf0;
  videoMemoryBase = videoMemoryBase << 6;
  int charROMBase = memPointer & 0xe;
  charROMBase = charROMBase << 10;

  int backgroundColor = memory_unclaimed_io_read(0xd021) & 0xf;
  memory_read_batch(batchCharMem, videoMemoryBase + posInCharMem, 40);
  memory_read_batch_io_unclaimed(batchColorMem, 0xd800 + posInCharMem, 40);
  for (i = 0; i < 40; i++) {
    jchar charcode = batchCharMem[i];//memory_read(1024 + i + posInCharMem);
    int bitmapDataRow = memory_read_vic_model(((charcode << 3) | (line_in_visible & 7)) + charROMBase);
    int j;
    int foregroundColor = batchColorMem[i] & 0xf;//memory_read(0xd800 + i + posInCharMem) & 0xf;
    if (foregroundColor & 8) {
      foregroundColor = foregroundColor & 7;
      color_tablet[0] = backgroundColor;
      color_tablet[1] = memory_unclaimed_io_read(0xd022) & 0xf;
      color_tablet[2] = memory_unclaimed_io_read(0xd023) & 0xf;
      color_tablet[3] = foregroundColor;
          for (j = 0; j < 4; j++) {
            int pixelSet = bitmapDataRow & 0xc0;
            pixelSet = pixelSet >> 6;

            g_buffer[posInBuffer] = colors_RGB_565[color_tablet[pixelSet]];
            posInBuffer++;

            g_buffer[posInBuffer] = colors_RGB_565[color_tablet[pixelSet]];
            posInBuffer++;

            bitmapDataRow = bitmapDataRow << 2;
          }

    } else {
      for (j = 0; j < 8; j++) {
        foregroundColor = foregroundColor & 7;
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
}

static inline void drawScreenLineMultiColorBitmap() {
  int i;
  int batchCharMem[40];
  int batchColorMem[40];
  int memPointer = memory_unclaimed_io_read(0xd018);
  int videoMemoryBase = memPointer & 0xf0;
  videoMemoryBase = videoMemoryBase << 6;
  int charROMBase = memPointer & 0xe;
  charROMBase = charROMBase << 10;
  int color_tablet[4];
  color_tablet[0] = memory_unclaimed_io_read(0xd021) & 0xf;
  //int backgroundColor = memory_unclaimed_io_read(0xd021) & 0xf;
  memory_read_batch(batchCharMem, videoMemoryBase + posInCharMem, 40);
  memory_read_batch_io_unclaimed(batchColorMem, 0xd800 + posInCharMem, 40);
  for (i = 0; i < 40; i++) {
    jchar charcode = batchCharMem[i];//memory_read(1024 + i + posInCharMem);
    color_tablet[1] = charcode >> 4;
    color_tablet[2] = charcode & 0xf;
    color_tablet[3] = batchColorMem[i] & 0xf;
    int bitmapDataRow = memory_read_vic_model((((posInCharMem + i) << 3) | (line_in_visible & 7)) + charROMBase);
    int j;
    //int foregroundColor = batchColorMem[i] & 0xf;//memory_read(0xd800 + i + posInCharMem) & 0xf;

    for (j = 0; j < 4; j++) {
      int pixelSet = bitmapDataRow & 0xc0;
      pixelSet = pixelSet >> 6;

      g_buffer[posInBuffer] = colors_RGB_565[color_tablet[pixelSet]];
      posInBuffer++;

      g_buffer[posInBuffer] = colors_RGB_565[color_tablet[pixelSet]];
      posInBuffer++;

      bitmapDataRow = bitmapDataRow << 2;
    }
  }
}


static inline void processLine() {
  if (line_count > 299)
    return;

  updatelineCharPos();
  fillColor(24, memory_unclaimed_io_read(0xd020) & 0xf);
  int screenEnabled = (memory_unclaimed_io_read(0xd011) & 0x10) ? 1 : 0;
  if (screenLineRegion && screenEnabled) {
    jchar bitmapMode = (memory_unclaimed_io_read(0xd011) & 0x20) ? 1 : 0;
    jchar multiColorMode = (memory_unclaimed_io_read(0xd016) & 0x10) ? 1 : 0;
    jchar screenMode = (bitmapMode << 1) | (multiColorMode);
    switch (screenMode) {
      case 0: //Normal texmode
        drawScreenLineNormalText();
      break;

      case 1: //Multi color text mode
        drawScreenLineMultiColorText();
      break;

      case 2: //Standard bitmap mode
      break;

      case 3: //Multicolor bitmap
        drawScreenLineMultiColorBitmap();
      break;
    }

  } else {
    fillColor(320, memory_unclaimed_io_read(0xd020) & 0xf);
  }
  fillColor(24, memory_unclaimed_io_read(0xd020) & 0xf);
}

int raster_int_enabled() {
  return (memory_unclaimed_io_read(0xd01a) & 1) ? 1 : 0;
}


void video_line_expired(struct timer_struct *tdev) {
  tdev->remainingCycles = 63;
  processLine();
  line_count++;
  jchar RST_0_7 = memory_unclaimed_io_read(0xd012);
  jchar RST_8 = (memory_unclaimed_io_read(0xd011) & 0x80) << 1;
  int targetRasterLine = RST_8 | RST_0_7;
  if ((targetRasterLine == line_count) && raster_int_enabled())
    vic_interrupt = vic_interrupt | 1 | 128;
  if (line_count > 310) {
    line_count = 0;
    frameFinished = 1;
    posInBuffer = 0;
  }
}

int vic_raster_int_occured() {
  return (vic_interrupt > 128) ? 1 : 0;
}

int read_vic_int_reg () {
  return vic_interrupt;
}

void write_vic_int_reg(jchar value) {
  value = ~value & 0x7f;
  vic_interrupt = vic_interrupt & value;
  if (vic_interrupt > 0)
    vic_interrupt = vic_interrupt | 128;
}

struct timer_struct getVideoInstance() {
  struct timer_struct myVideo;
  myVideo.expiredevent = &video_line_expired;
  myVideo.remainingCycles = 63;
  myVideo.started = 1;
  //myVideo.interrupt = &interrupt_timer_A;
  return myVideo;
}

