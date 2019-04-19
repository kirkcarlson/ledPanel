// individual LED command arrays
// these loop unless directed not to with LED_STOP
int flash250 [] = {
    LED_ON,
    LED_WAIT, 250,
    LED_OFF,
    LED_WAIT, 250,
    LED_LOOP
};

int wink [] = {
    LED_ON,
    LED_WAIT, 2500,
    LED_OFF,
    LED_WAIT, 100,
    LED_LOOP
};

int blink [] = {
    LED_OFF,
    LED_WAIT, 2500,
    LED_ON,
    LED_WAIT, 100,
    LED_LOOP
};

int meteor [] = {
    LED_ON,
    LED_WAIT, 30,
    LED_FADE_TO_BLACK, 80,
    LED_OFF,
    LED_WAIT, 1000,
    LED_LOOP
};


// group LED command arrays
// these loop unless directed not to with GROUP_STOP
int offGC [] = {
    GROUP_STOP_ALL, // stop everything
    GROUP_SET_ALL, // turn everything off
    GROUP_LKILL_ALL,
    GROUP_OFF,
    GROUP_STOP
};

int singleColorGC [] = {
    GROUP_ON,
    GROUP_STOP
};

int multiColorGC [] = {
    GROUP_ON_MULTI,
    GROUP_STOP
};

int multiColorTwinkleGC [] = {
    GROUP_ON_MULTI,
    GROUP_TWINKLE,
    GROUP_STOP
};

int allFlashGC [] = {
    GROUP_COLOR, RED,
    GROUP_ON,
    GROUP_WAIT, 250,
    GROUP_OFF,
    GROUP_WAIT, 250,
    GROUP_COLOR, BLUE,
    GROUP_ON,
    GROUP_WAIT, 250,
    GROUP_OFF,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int quadFlashGC [] = {
    GROUP_LOAD_PATTERN, 24,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
    GROUP_WAIT, 250,
    GROUP_LOAD_PATTERN, 24,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,RED,RED,RED,RED,RED,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int quadFlashGC2 [] = {
    GROUP_LOAD_PATTERN, 24,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
    GROUP_WAIT, 250,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_RIGHT,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int quadFlashGC3 [] = {
    GROUP_LOAD_PATTERN, 24,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,RED,RED,RED,RED,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
    GROUP_WAIT, 250,
    GROUP_ROTATE_RIGHT_COUNT, NUM_LEDS/4,
    GROUP_WAIT, 250,
    GROUP_LOOP
};



int quadFlashGC4 [] = {
    GROUP_COLOR_SEGMENT, RED, 0, NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, BLACK, NUM_LEDS/4, NUM_LEDS/2-1,
    GROUP_COLOR_SEGMENT, RED, NUM_LEDS/2, 3*NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, BLACK, 3*NUM_LEDS/4, NUM_LEDS-1,
    GROUP_WAIT, 250,
    GROUP_COLOR_SEGMENT, BLACK, 0, NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, RED, NUM_LEDS/4, NUM_LEDS/2-1,
    GROUP_COLOR_SEGMENT, BLACK, NUM_LEDS/2, 3*NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, RED, 3*NUM_LEDS/4, NUM_LEDS-1,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int quadFlashGC5 [] = {
    GROUP_COLOR_SEGMENT, RED, 0, NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, BLACK, NUM_LEDS/4, NUM_LEDS/2-1,
    GROUP_COLOR_SEGMENT, RED, NUM_LEDS/2, 3*NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, BLACK, 3*NUM_LEDS/4, NUM_LEDS-1,
    GROUP_WAIT, 250,
    GROUP_ROTATE_RIGHT_COUNT, NUM_LEDS/4,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int quadFlashGC6 [] = {
    GROUP_COLOR_SEGMENT, BLACK, 0, NUM_LEDS-1,
    GROUP_COLOR_SEGMENT, RED, 0, NUM_LEDS/4-1,
    GROUP_COLOR_SEGMENT, RED, NUM_LEDS/2, 3*NUM_LEDS/4-1,
    GROUP_WAIT, 250,
    GROUP_CHASE_CW_PAIR, NUM_LEDS/4-1, NUM_LEDS/2-1, NUM_LEDS/4, 30, //fromLED, toLED, count, delay
    GROUP_WAIT, 250,
    GROUP_CHASE_CW_PAIR, NUM_LEDS/2-1, 3*NUM_LEDS/4-1, NUM_LEDS/4, 30, //fromLED, toLED, count, delay
    GROUP_LOOP
};


int quadFlashGC7 [] = {
    GROUP_LOAD_PATTERN, 24,
                        RED,WHITE,BLUE,BLUE,WHITE,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
                        RED,WHITE,BLUE,BLUE,WHITE,RED,
                        BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
    GROUP_WAIT, 250,
    GROUP_CHASE_CW_PAIR, NUM_LEDS/4-1, NUM_LEDS/2-1, NUM_LEDS/4, 30, //fromLED, toLED, count, delay
    GROUP_WAIT, 250,
    GROUP_CHASE_CW_PAIR, NUM_LEDS/2-1, 3*NUM_LEDS/4-1, NUM_LEDS/4, 30, //fromLED, toLED, count, delay
    GROUP_LOOP
};

int quadFlashGC8 [] = {
    GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_COLOR_SEGMENT, RED, 0, 7,
    GROUP_COLOR_SEGMENT, RED, 16, 23,
    GROUP_COLOR_SEGMENT, RED, 32, 39,
    GROUP_COLOR_SEGMENT, RED, 48, 55,
    GROUP_COLOR_SEGMENT, RED, 64, 71,
    GROUP_COLOR_SEGMENT, RED, 80, 87,
    GROUP_COLOR_SEGMENT, RED, 96, 103,
    GROUP_COLOR_SEGMENT, RED, 112, 119,
    GROUP_COLOR_SEGMENT, RED, 136, 143,
    GROUP_COLOR_SEGMENT, RED, 152, 159,
    GROUP_COLOR_SEGMENT, RED, 168, 175,
    GROUP_COLOR_SEGMENT, RED, 184, 191,
    GROUP_COLOR_SEGMENT, RED, 200, 207,
    GROUP_COLOR_SEGMENT, RED, 216, 223,
    GROUP_COLOR_SEGMENT, RED, 232, 239,
    GROUP_COLOR_SEGMENT, RED, 248, 255,
    GROUP_WAIT, 250,
    GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_COLOR_SEGMENT, RED, 8, 15,
    GROUP_COLOR_SEGMENT, RED, 24, 31,
    GROUP_COLOR_SEGMENT, RED, 40, 47,
    GROUP_COLOR_SEGMENT, RED, 56, 63,
    GROUP_COLOR_SEGMENT, RED, 72, 79,
    GROUP_COLOR_SEGMENT, RED, 88, 95,
    GROUP_COLOR_SEGMENT, RED, 104, 111,
    GROUP_COLOR_SEGMENT, RED, 120, 127,
    GROUP_COLOR_SEGMENT, RED, 128, 135,
    GROUP_COLOR_SEGMENT, RED, 144, 151,
    GROUP_COLOR_SEGMENT, RED, 160, 167,
    GROUP_COLOR_SEGMENT, RED, 176, 183,
    GROUP_COLOR_SEGMENT, RED, 192, 199,
    GROUP_COLOR_SEGMENT, RED, 208, 215,
    GROUP_COLOR_SEGMENT, RED, 224, 231,
    GROUP_COLOR_SEGMENT, RED, 240, 247,
    GROUP_WAIT, 250,
    GROUP_LOOP
};

uint32_t quadFlash5GC [] = {
    GROUP_COLOR, RED,
    GROUP_SET_MASK, (uint32_t)&quadSquareA,
    GROUP_ON,
    GROUP_WAIT, 200,
    GROUP_OFF,

    GROUP_SET_MASK, (uint32_t)&quadSquareB,
    GROUP_ON,
    GROUP_WAIT, 200,
    GROUP_OFF,
    GROUP_LOOP
};

int octaFlashGC [] = {
    GROUP_LOAD_PATTERN, 24,
                        RED,RED,RED,BLACK,BLACK,BLACK,
                        RED,RED,RED,BLACK,BLACK,BLACK,
                        RED,RED,RED,BLACK,BLACK,BLACK,
                        RED,RED,RED,BLACK,BLACK,BLACK,
    GROUP_WAIT, 250,
    GROUP_ROTATE_RIGHT_COUNT, NUM_LEDS/8,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int octaFlashGC2 [] = {
    GROUP_COLOR_SEGMENT, BLACK, 0, NUM_LEDS-1,
    GROUP_COLOR_SEGMENT, RED, 0,              NUM_LEDS/8-1,
    GROUP_COLOR_SEGMENT, RED, 2*NUM_LEDS/8, 3*NUM_LEDS/8-1,
    GROUP_COLOR_SEGMENT, RED, 4*NUM_LEDS/8, 5*NUM_LEDS/8-1,
    GROUP_COLOR_SEGMENT, RED, 6*NUM_LEDS/8, 7*NUM_LEDS/8-1,
    GROUP_WAIT, 250,
    GROUP_ROTATE_RIGHT_COUNT, NUM_LEDS/8,
    GROUP_WAIT, 250,
    GROUP_LOOP
};


int rainbowGC [] = {
    GROUP_RAINBOW,
    GROUP_STOP
};

int wheelGC [] = {
    GROUP_WHEEL,
    GROUP_STOP
};

int breatheGC [] = {
    GROUP_BREATHE,
    GROUP_STOP
};

int waveGC [] = {
    GROUP_WAVE,
    GROUP_WAIT, 30,
    GROUP_LOOP
};

int marqueeLeftGC [] = {
    GROUP_ROTATE_LEFT,
    GROUP_WAIT, 200,
    GROUP_LOOP
};

int marqueeRightGC [] = {
    GROUP_ROTATE_RIGHT,
    GROUP_WAIT, 200,
    GROUP_LOOP
};

int clockGC [] = {
    GROUP_CLOCK,
    GROUP_STOP
};

int circulatorGC [] = {
    GROUP_COLOR_SEGMENT, BLACK, 0, NUM_LEDS-1,
    GROUP_COLOR_SEGMENT, YELLOW, NUM_LEDS/2, NUM_LEDS/2+4,
    GROUP_REPEAT_COUNT, NUM_LEDS - 5,
        GROUP_ROTATE_RIGHT,
        GROUP_WAIT, 40,
    GROUP_REPEAT_END,
    GROUP_REPEAT_COUNT, 5,
        GROUP_ROTATE_RIGHT,
        GROUP_WAIT, 200,
    GROUP_REPEAT_END,
    GROUP_LOOP
};

int pendulumGC [] = {
    GROUP_COLOR_SEGMENT, BLACK, 0, NUM_LEDS-1,
    GROUP_COLOR_SEGMENT, YELLOW, 3/NUM_LEDS/8, 5*NUM_LEDS/8,
    GROUP_REPEAT_COUNT, NUM_LEDS/4,
        GROUP_ROTATE_LEFT,
        GROUP_WAIT, 100,
    GROUP_REPEAT_END,
    GROUP_REPEAT_COUNT, NUM_LEDS/4,
        GROUP_ROTATE_RIGHT,
        GROUP_WAIT, 100,
    GROUP_REPEAT_END,
    GROUP_LOOP
};


int borderMarquee1GC [] = {
    GROUP_MASK, (uint32_t) &borderMask,
    GROUP_SET_MEMBERS, 60, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
        31,47,63,79,95,111,127,143,159,175,191,207,223,239,
        255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,
        224,208,192,176,160,144,128,112,96,80,64,48,32,16,
    GROUP_LOAD_REPEAT_PATTERN, 6, RED,WHITE,BLUE,BLUE,WHITE,RED,
    GROUP_REPEAT_FOREVER,
        GROUP_ROTATE_RIGHT,
        GROUP_WAIT, 100,
    GROUP_REPEAT_END
};

int borderMarquee1GCL [] = {
    GROUP_REPEAT_FOREVER,
        GROUP_ROTATE_LEFT,
        GROUP_WAIT, 100,
    GROUP_REPEAT_END
};


int borderMarquee2GC [] = {
    GROUP_SET_MEMBERS, 60, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
        31,47,63,79,95,111,127,143,159,175,191,207,223,239,
        255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,
        224,208,192,176,160,144,128,112,96,80,64,48,32,16,
    GROUP_LOAD_REPEAT_PATTERN, 12, RED,RED,RED,WHITE,WHITE,WHITE,BLUE,BLUE,BLUE,BLACK,BLACK,BLACK,
    GROUP_REPEAT_FOREVER,
        GROUP_ROTATE_LEFT,
        GROUP_WAIT, 100,
    GROUP_REPEAT_END
};

//this whould be better to have a bitmap and mask, and shift the bitmap
// from a start postion to an end position to simulate motion
// set this up to move in four movements
// better if it is 5/8, 6/8, 7/8, 8/8
//subtract 2 for every eighths
// this assumes that the field is black when starting
int rightArrowGC [] = {
   GROUP_SET_MEMBERS, 40, 113,114,115,116, 117,118,119,120,
                          129,130,131,132, 133,134,135,136,
                           18,35,52,69, 86,103,
                           17,34,51,68, 85,102,
                           226,211,196,181, 166,151,
                           225,210,195,180, 165,150,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 44, 113,114,115,116, 117,118,119,120,
                          121,122,
                          129,130,131,132, 133,134,135,136,
                          137,138,
                           20,37,54,71, 88,105,
                           19,36,53,70, 87,104,
                           228,213,198,183, 168,153,
                           227,212,197,182, 167,152,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 48, 113,114,115,116, 117,118,119,120,
                          121,122,123,124,
                          129,130,131,132, 133,134,135,136,
                          137,138,139,140,
                           22,39,56,73, 90,107,
                           21,38,55,72, 89,106,
                           230,215,200,185, 170,155,
                           229,214,199,184, 169,154,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 52, 113,114,115,116, 117,118,119,120,
                          121,122,123,124, 125,126,
                          129,130,131,132, 133,134,135,136,
                          137,138,139,140, 141,142,
                           24,41,58,75, 92,109,
                           23,40,57,74, 91,108,
                           232,217,202,187, 172,157,
                           231,216,201,186, 171,156,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_LOOP,
};

int leftArrowGC [] = {
   GROUP_SET_MEMBERS, 40, 119,120,
                          121,122,123,124, 125,126,
                          135,136,
                          137,138,139,140, 141,142,
			  104,89,74,59,44,29,
                          105,90,75,60,45,30,
                          152,169,186,203,220,237,
                          153,170,187,204,221,238,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 44, 117,118,119,120,
                          121,122,123,124, 125,126,
                          133,134,135,136,
                          137,138,139,140, 141,142,
			  102,87,72,57,42,27,
                          103,88,73,58,43,28,
                          150,167,184,201,218,235,
                          151,168,185,202,219,236,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 48, 115,116, 117,118,119,120,
                          121,122,123,124, 125,126,
                          131,132, 133,134,135,136,
                          137,138,139,140, 141,142,
			  100,85,70,55,40,25,
                          101,86,71,56,41,26,
                          148,165,182,199,216,233,
                          149,166,183,200,217,234,

   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_SET_MEMBERS, 52, 113,114,115,116, 117,118,119,120,
                          121,122,123,124, 125,126,
                          129,130,131,132, 133,134,135,136,
                          137,138,139,140, 141,142,
			  98,83,68,53,38,23,
                          99,84,69,54,39,24,
                          146,163,180,197,214,231,
                          147,164,181,198,215,232,
   GROUP_ON,
   GROUP_WAIT, 500,
   GROUP_OFF,
   GROUP_LOOP,
};



int life01GC[] = { // period of 2
    GROUP_SET_MASK, (uint32_t)&life_map_01,
    //GROUP_SET_MEMBERS, 12,
    //                        102,103,104,
    //                        132,148,164,
    //                        138,154,170,
    //                        198,199,200,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};


int life02GC[] = { // period of 3
    GROUP_SET_MASK, (uint32_t)&life_map_02,
    //GROUP_SET_MEMBERS, 48,
    //                        20,21,22, 26,27,28,
    //                        50,55,57,62,
    //                        66,71,73,78,
    //                        82,87,89,94,
    //                        100,101,102, 106,107,108,
    //                        132,133,134, 138,139,140,
    //                        146,151,153,158,
    //                        162,167,169,174,
    //                        178,183,185,190,
    //                        212,213,214, 218,219,220,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life03GC[] = { // glider
    GROUP_SET_MASK, (uint32_t)&life_map_03,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life04GC[] = { // heavy weight space ship
    GROUP_SET_MASK, (uint32_t)&life_map_04,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life05GC[] = { // light weight space ship
    GROUP_SET_MASK, (uint32_t)&life_map_05,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life06GC[] = { // beacons
    GROUP_SET_MASK, (uint32_t)&life_map_06,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life07GC[] = { // pentathalon
    GROUP_SET_MASK, (uint32_t)&life_map_07,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life08GC[] = { // heavy weight space ship
    GROUP_SET_MASK, (uint32_t)&life_map_08,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life09GC[] = { // oscillator 14
    GROUP_SET_MASK, (uint32_t)&life_map_09,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life10GC[] = { // tumbler
    GROUP_SET_MASK, (uint32_t)&life_map_10,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life11GC[] = { // unix period 6 oscillator
    GROUP_SET_MASK, (uint32_t)&life_map_11,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life12GC[] = { // great on-off
    GROUP_SET_MASK, (uint32_t)&life_map_12,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life13GC[] = { // birther
    GROUP_SET_MASK, (uint32_t)&life_map_13,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};

int life15GC[] = { // IT-ology
    GROUP_SET_MASK, (uint32_t)&life_map_15,
    GROUP_ON,
    GROUP_WAIT, 1500, // just to see it
    GROUP_REPEAT_FOREVER,
        GROUP_WAIT, 300,
        GROUP_LIFE, // changes membership of group
    GROUP_REPEAT_END,
};


int demo1GC [] = {
    //GROUP_SET_GROUP, 1, // set this group to 1, not sure if this is a good thing to do
    GROUP_STOP_ALL, // just in case something is running
    GROUP_GCOLOR, 0, BLUE,
    GROUP_GFORK, 0, (int)&singleColorGC,
    GROUP_WAIT, 3000,

    GROUP_GCOLOR, 0, YELLOW,
    GROUP_GFORK, 0, (int)&singleColorGC,
    GROUP_WAIT, 3000,

    GROUP_GCOLOR, 0, GREEN,
    GROUP_GFORK, 0, (int)&singleColorGC,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&allFlashGC,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&quadFlashGC6,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&quadFlashGC7,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&pendulumGC,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&circulatorGC,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&clockGC,
    GROUP_WAIT, 3000,

    GROUP_GFORK, 0, (int)&marqueeRightGC,
    GROUP_WAIT, NUM_LEDS * 200,

    GROUP_GFORK, 0, (int)&marqueeLeftGC,
    GROUP_WAIT, NUM_LEDS * 200,

    GROUP_GCOLOR, 0, BLUE,
    GROUP_GFORK, 0, (int)&singleColorGC,
    GROUP_GFORK, 0, (int)&breatheGC,
    GROUP_WAIT, 3000,

    GROUP_SET_ALL,
    GROUP_PHASE_DELAY, 30,
    GROUP_LFORK_ALL, (int)&flash250,
    GROUP_WAIT, 3000,

    GROUP_LKILL_ALL,
    GROUP_GCOLOR, 0, BLACK,
    GROUP_GFORK, 0, (int)&singleColorGC,

    // the folloning changes the members of the director group dynamically
    // I don't think that was the intent of the GROUP_SET_MEMBERS function.
    // This was intended to be on group-group rather than a director-group.
    // Let's see if it works.
    GROUP_SET_MEMBERS, 12, 0,1,2,3,4,5,6,7,8,9,10,11,
    GROUP_PHASE_DELAY, 30,
    GROUP_LFORK_ALL, (int)&meteor,
    GROUP_SET_MEMBERS, 12, 12,13,14,15,16,17,18,19,20,21,22,23,
    GROUP_PHASE_DELAY, 30,
    GROUP_LFORK_ALL, (int)meteor,
    GROUP_WAIT, 3000,

    GROUP_SET_ALL,  //need to set context to all LEDs, before killing them
    GROUP_LKILL_ALL,
    GROUP_LOOP
};

int demo2GC [] = {
    GROUP_STOP_ALL, // just in case something is running
    GROUP_SET_ALL,
    GROUP_OFF,

    GROUP_GCOLOR, 1, RED,
    GROUP_GFORK, 1, (int)&rightArrowGC,
    GROUP_WAIT, 5000,

    GROUP_GFORK, 2, (int)&borderMarquee1GC,
    GROUP_WAIT, 5000,

    GROUP_GFORK, 1, (int)&offGC,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&leftArrowGC,
    GROUP_WAIT, 5000,

    GROUP_GFORK, 2, (int)&borderMarquee2GC,
    GROUP_WAIT, 5000,

    GROUP_GFORK, 1, (int)&offGC,
    GROUP_GFORK, 2, (int)&offGC,
    GROUP_WAIT, 2000,

    GROUP_GFORK, 2, (int)&quadFlashGC8,
    GROUP_WAIT, 10000,

    GROUP_GFORK, 2, (int)&offGC,

    GROUP_LOOP
};

int demo3GC [] = { // accident, but good run of life
    GROUP_STOP_ALL, // just in case something is running
    GROUP_GCOLOR, 1, GREEN,
    GROUP_GFORK, 1, (int)&life01GC,
    GROUP_WAIT, 10000,

    GROUP_LOOP
};


int demo4GC [] = { // more of a life demo
    GROUP_STOP_ALL, // just in case something is running
    GROUP_SET_ALL,
    GROUP_OFF,

    GROUP_GCOLOR, 1, GREEN,
    GROUP_GFORK, 1, (int)&life01GC, // oscillator period of 2
    GROUP_WAIT, 5000,

    //GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life02GC, // oscillator period of 3
    GROUP_WAIT, 10000,

    //GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_GCOLOR, 1, YELLOW,
    GROUP_GFORK, 1, (int)&life03GC, // glider
    GROUP_WAIT, 10000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life04GC, // heavy weight space ship
    GROUP_WAIT, 15000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, RED,
    GROUP_GFORK, 1, (int)&life05GC, // light weight space ship
    GROUP_WAIT, 20000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, WHITE,
    GROUP_GFORK, 1, (int)&life06GC, // beacons
    GROUP_WAIT, 5000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, GREEN,
    GROUP_GFORK, 1, (int)&life07GC, // pentathalon
    GROUP_WAIT, 15000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life08GC, // heavy weight space ship
    GROUP_WAIT, 8000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, YELLOW,
    GROUP_GFORK, 1, (int)&life09GC, // oscillator 14
    GROUP_WAIT, 10000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, WHITE,
    GROUP_GFORK, 1, (int)&life10GC, // tumbler
    GROUP_WAIT, 10000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, RED,
    GROUP_GFORK, 1, (int)&life11GC, // unix period 6 oscillator
    GROUP_WAIT, 15000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life12GC, // great on-off
    GROUP_WAIT, 5000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, GREEN,
    GROUP_GFORK, 1, (int)&life13GC, // birther
    GROUP_WAIT, 23000,

    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life15GC, // IT-ology
    GROUP_WAIT, 20000,

    GROUP_LOOP
};


int demo5GCM [] = { // control the marquee direction, assume group 1
    GROUP_GFORK, 2, (int)&borderMarquee1GC,
    GROUP_WAIT, 10000,

    GROUP_GFORK, 2, (int)&borderMarquee1GCL,
    GROUP_WAIT, 10000,

    GROUP_LOOP
};


int demo5GC [] = { // want flasher with background color (image?) and marque
    GROUP_STOP_ALL, // just in case something is running

    //GROUP_SET_ALL, // assume group 0
    //GROUP_ON,

    GROUP_GFORK, 1, (int)&demo5GCM, //uses groups 1 & 2

    GROUP_GFORK, 3, (int)&quadFlash5GC, // uses group 3
    GROUP_WAIT, 20000,

/*
set members of the group
show background
use group color
turn group on
wait
set members of the group
show background
turn group on
wait
loop
...for a while
    GROUP_WAIT, 10000,
*/

// slide an arrow sprite right
// for a while
//    GROUP_WAIT, 10000,

// slide an arrow sprite left
// for a while
//    GROUP_WAIT, 10000,

// if background is dynamic, need to have some sort of redraw command possiblity
// this could be easy because each group/led has a state, just invoke the state without advancing it


    GROUP_LOOP
};

