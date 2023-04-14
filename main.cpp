/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Motor.h"
#include "SDFileSystem.h"
#include "uLCD_4DGL.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>

using namespace std;

uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
SDFileSystem sd(p5, p6, p7, p8, "sd");
Motor m(p23, p6, p5); // pwm, fwd, rev


int volatile state = 0;
int volatile level = 0;
float volatile speed = 0.2;
float multiplier = 1.0;
InterruptIn pb1(p15);
InterruptIn pb2(p16);
InterruptIn sw(p17);

// should have a state {0: powered, 1: on, 2: off}
// current motor rotation/level?
// constant float speed of motor

void actuate_motor (float time, float direction) {
	// set motor speed
	// wait time
	// brake
    // direction: 1 is tightening, -1 is loosening
    m.speed(speed*direction);
    wait_us(time*1000000);
    m.speed(0);
}

void pb1_interrupt (void){
	// actuate motor to increase tension
    actuate_motor(multiplier, 1);
    level += 1;   
    //need to account for max and min limits
}
void pb2_interrupt (void){
	// actuate motor to decrease tension (opposite motion of pb1_interrpt)
    actuate_motor(multiplier, -1);
    level -= 1;
    //account for limits
}
void sd_write (void) {
	// make a directory mkdir
// create a file
// open the file 
// write the current level of the motor
// close the file
    mkdir("/sd/mydir", 0777);
    FILE *fp = fopen("/sd/mydir/level.txt", "w");
    if(fp == NULL) {
        error("Could not open file for write\n");
    }
    stringstream ss;
    ss << level;
    string level_str;
    ss >> level_str; 
    fprintf(fp, level);
    fclose(fp); 
}
std::string sd_read (void) {
// open the file 
// read the current level of the motor
// close the file
    char buffer[50];
    std::string out_buffer;
    FILE *fp = fopen("/sd/mydir/level.txt", "r");
    if(fp == NULL) {
        error("Could not open file for read\n");
    }
    int result = 1;
    while (result > 0) {
        result = fread(buffer, 1, 50, fp);
        for (int i = 0; i < result; i++) {
            out_buffer += buffer[i];
        }
    } 
    fclose(fp); 
    return out_buffer;
 
    
}
void switch_interrupt (void) {
	// if device is OFF 
	// read tension level data from SD card 
	// actuate motor to raise arm to saved level  
	// if device is ON
    // save tension level to SD card
	// actuate motor to ease all tension
    // if the device is on for the first time, no switch action
    if (state == 2) {
        string s = sd_read();
        stringstream ss;
        ss << level;
        int lvl_int = 0;
        ss >> lvl_int;
       level = lvl_int;
       actuate_motor(multiplier*level, 1);
    } else if (state == 1) {
        sd_write();
        actuate_motor(multiplier*level, -1);
    }
}
int main (void) {
    //Update LCD while in while loop
    while(1){
       uLCD.color(RED);
       //uLCD.printf("Level");
       //uLCD.printf(level);
       printf(level);
       
       //uLCD.printf("%D", level);
    }
}


