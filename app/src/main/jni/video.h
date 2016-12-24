//
// Created by johan on 2016/12/02.
//

#ifndef MY_APPLICATION_VIDEO_H
#define MY_APPLICATION_VIDEO_H

#endif //MY_APPLICATION_VIDEO_H

struct sprite_data_struct {
    int sprite_data;
    int sprite_type; //bit 1: xExpanded bit 0: multicolor
    int isForegroundSprite;
    int color_tablet[4];
    int sprite_x_pos;
    int number_pixels_to_draw;   
};


struct timer_struct getVideoInstance();
void initialise_video();
int read_vic_int_reg ();
void write_vic_int_reg(jchar value);
int vic_raster_int_occured();