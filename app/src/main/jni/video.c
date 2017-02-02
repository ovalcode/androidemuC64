//
// Created by johan on 2016/12/02.
//

#include <stdbool.h>
#include <jni.h>
#include <alarm.h>
#include <video.h>
#include <memory.h>

void EmulateLine();

int line_count = 0;
int startOfLineTxtBuffer = 0;
int startOfFrontSpriteBuffer = 0;
int startOfBackgroundSpriteBuffer = 0;
int posInFrontBuffer = 0;
int posInBackgroundBuffer = 0;
int posInCharMem = 0;
int screenLineRegion = 0;
extern jchar charRom[4096];
extern jint* g_buffer;
extern int frameFinished;

void (*spriteFunctions[4]) (struct sprite_data_struct spriteData);

void drawExpandedMulticolorSpriteLine(struct sprite_data_struct spriteData);
void drawUnExpandedMulticolorSpriteLine(struct sprite_data_struct spriteData);
void drawExpandedNormalSpriteLine(struct sprite_data_struct spriteData);
void drawUnExpandedNormalSpriteLine(struct sprite_data_struct spriteData);

#define STRIDE 368 + 368 + 368 + 320


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

jint colors_RGB_8888[16];

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

  for (i=0; i < 16; i++) {
    colors_RGB_8888[i] = (255 << 24) | (colors_RGB_888[i][2] << 16) | (colors_RGB_888[i][1] << 8) | (colors_RGB_888[i][0] << 0);
    //colors_RGB_8888[i] = (255);
  }

  spriteFunctions[0] = &drawUnExpandedNormalSpriteLine;
  spriteFunctions[1] = &drawUnExpandedMulticolorSpriteLine;
  spriteFunctions[2] = &drawExpandedNormalSpriteLine;
  spriteFunctions[3] = &drawExpandedMulticolorSpriteLine;
}

static inline void fillColor(int count, int colorEntryNumber) {
  int currentPos;
  for (currentPos = 0; currentPos < count; currentPos++) {
    g_buffer[posInFrontBuffer] = colors_RGB_8888[colorEntryNumber];
    posInFrontBuffer++;
  }
}

static inline void updatelineCharPos() {
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
        g_buffer[posInFrontBuffer] = colors_RGB_8888[foregroundColor];
      } else {
        g_buffer[posInBackgroundBuffer] = colors_RGB_8888[backgroundColor];
      }
      posInFrontBuffer++;
      posInBackgroundBuffer++;
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

            if (pixelSet > 1) {
              g_buffer[posInFrontBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
              posInFrontBuffer++;
              posInBackgroundBuffer++;

              g_buffer[posInFrontBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
              posInFrontBuffer++;
              posInBackgroundBuffer++;
            } else {
              g_buffer[posInBackgroundBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
              posInFrontBuffer++;
              posInBackgroundBuffer++;

              g_buffer[posInBackgroundBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
              posInFrontBuffer++;
              posInBackgroundBuffer++;
            }

            bitmapDataRow = bitmapDataRow << 2;
          }

    } else {
      for (j = 0; j < 8; j++) {
        foregroundColor = foregroundColor & 7;
        int pixelSet = bitmapDataRow & 0x80;
        if (pixelSet) {
          g_buffer[posInFrontBuffer] = colors_RGB_8888[foregroundColor];
        } else {
          g_buffer[posInBackgroundBuffer] = colors_RGB_8888[backgroundColor];
        }
            posInFrontBuffer++;
            posInBackgroundBuffer++;

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

      g_buffer[posInFrontBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
      posInFrontBuffer++;
      posInBackgroundBuffer++;

      g_buffer[posInFrontBuffer] = colors_RGB_8888[color_tablet[pixelSet]];
      posInFrontBuffer++;
      posInBackgroundBuffer++;


      bitmapDataRow = bitmapDataRow << 2;
    }
  }
}

void drawUnExpandedNormalSpriteLine(struct sprite_data_struct currentSpriteData) {
      int currentPosInSpriteBuffer;
      if (currentSpriteData.isForegroundSprite)
        currentPosInSpriteBuffer = startOfFrontSpriteBuffer;
      else
        currentPosInSpriteBuffer = startOfBackgroundSpriteBuffer;
      currentPosInSpriteBuffer = currentPosInSpriteBuffer + currentSpriteData.sprite_x_pos;
      int j;
      int spriteData = currentSpriteData.sprite_data;
      int upperLimit = currentPosInSpriteBuffer + currentSpriteData.number_pixels_to_draw;
      for (j = currentPosInSpriteBuffer; j < (upperLimit); j++) {
        if (spriteData & 0x800000) {
          g_buffer[currentPosInSpriteBuffer] = colors_RGB_8888[currentSpriteData.color_tablet[1]];
        }
        spriteData = (spriteData << 1) & 0xffffff;
        currentPosInSpriteBuffer++;
      }

}

void drawExpandedNormalSpriteLine(struct sprite_data_struct currentSpriteData) {
      int currentPosInSpriteBuffer;
      if (currentSpriteData.isForegroundSprite)
        currentPosInSpriteBuffer = startOfFrontSpriteBuffer;
      else
        currentPosInSpriteBuffer = startOfBackgroundSpriteBuffer;
      currentPosInSpriteBuffer = currentPosInSpriteBuffer + currentSpriteData.sprite_x_pos;
      int j;
      int spriteData = currentSpriteData.sprite_data;
      for (j = 0; j < (currentSpriteData.number_pixels_to_draw >> 1); j++) {
        if (spriteData & 0x800000) {
          g_buffer[currentPosInSpriteBuffer + 0] = colors_RGB_8888[currentSpriteData.color_tablet[1]];
          g_buffer[currentPosInSpriteBuffer + 1] = colors_RGB_8888[currentSpriteData.color_tablet[1]];
        }
        currentPosInSpriteBuffer = currentPosInSpriteBuffer + 2;
        spriteData = (spriteData << 1) & 0xffffff;
      }

}

void drawUnExpandedMulticolorSpriteLine(struct sprite_data_struct currentSpriteData) {
      int currentPosInSpriteBuffer;
      if (currentSpriteData.isForegroundSprite)
        currentPosInSpriteBuffer = startOfFrontSpriteBuffer;
      else
        currentPosInSpriteBuffer = startOfBackgroundSpriteBuffer;
      currentPosInSpriteBuffer = currentPosInSpriteBuffer + currentSpriteData.sprite_x_pos;
      int j;
      int spriteData = currentSpriteData.sprite_data;
      for (j = 0; j < (currentSpriteData.number_pixels_to_draw >> 1); j++) {
        int pixels = (spriteData & 0xC00000) >> 22;
        if (pixels > 0) {
          g_buffer[currentPosInSpriteBuffer + 0] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
          g_buffer[currentPosInSpriteBuffer + 1] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
        }
        currentPosInSpriteBuffer = currentPosInSpriteBuffer + 2;
        spriteData = (spriteData << 2) & 0xffffff;
      }

}

void drawExpandedMulticolorSpriteLine(struct sprite_data_struct currentSpriteData) {
      int currentPosInSpriteBuffer;
      if (currentSpriteData.isForegroundSprite)
        currentPosInSpriteBuffer = startOfFrontSpriteBuffer;
      else
        currentPosInSpriteBuffer = startOfBackgroundSpriteBuffer;
      currentPosInSpriteBuffer = currentPosInSpriteBuffer + currentSpriteData.sprite_x_pos;
      int j;
      int spriteData = currentSpriteData.sprite_data;
      for (j = 0; j < (currentSpriteData.number_pixels_to_draw >> 2); j++) {
        int pixels = (spriteData & 0xC00000) >> 22;
        if (pixels > 0) {
          g_buffer[currentPosInSpriteBuffer + 0] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
          g_buffer[currentPosInSpriteBuffer + 1] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
          g_buffer[currentPosInSpriteBuffer + 2] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
          g_buffer[currentPosInSpriteBuffer + 3] = colors_RGB_8888[currentSpriteData.color_tablet[pixels]];
        }
        currentPosInSpriteBuffer = currentPosInSpriteBuffer + 4;
        spriteData = (spriteData << 2) & 0xffffff;
      }

}

void processSprites() {
  int i;
  struct sprite_data_struct currentSpriteData;
  for (i = 0; i < 8; i++) {
    int currentSpriteNum = 7 - i;
    if (processSprite(currentSpriteNum, line_count, &currentSpriteData)) {
      spriteFunctions[currentSpriteData.sprite_type] (currentSpriteData);
    }
  }
}


static inline void processLine() {
  EmulateLine();
  if (line_count > 299)
    return;

  posInFrontBuffer = startOfLineTxtBuffer + 368;
  posInBackgroundBuffer = startOfLineTxtBuffer + 368 + 368 + 368;

  startOfFrontSpriteBuffer = startOfLineTxtBuffer;
  startOfBackgroundSpriteBuffer = startOfLineTxtBuffer + 368 + 368;

  updatelineCharPos();
  fillColor(24, memory_unclaimed_io_read(0xd020) & 0xf);
  int screenEnabled = (memory_unclaimed_io_read(0xd011) & 0x10) ? 1 : 0;
  if (screenLineRegion && screenEnabled) {
    processSprites();
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

  startOfLineTxtBuffer = startOfLineTxtBuffer + STRIDE;
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
  if (line_count > 310) {
    line_count = 0;
    frameFinished = 1;
    startOfLineTxtBuffer = 0;
  }

    if ((targetRasterLine == line_count) && raster_int_enabled())
      vic_interrupt = vic_interrupt | 1 | 128;

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

