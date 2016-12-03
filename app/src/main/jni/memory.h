//
// Created by johan on 2016/08/30.
//

#ifndef MY_APPLICATION_MEMORY_H
#define MY_APPLICATION_MEMORY_H

#endif //MY_APPLICATION_MEMORY_H

unsigned char memory_read(int address);
void memory_write(int address, unsigned char value);
void memory_read_batch(int *batch, int address, int count);
