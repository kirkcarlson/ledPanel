//select one of the following for LED layouts
#define LINEAR_ARRAY false
#define SERPENTINE_16_ARRAY true
#define NUM_LEDS 256

// divide LEDs into quadrants
#define NUM_LEDS_Q1 0
#define NUM_LEDS_Q2 (NUM_LEDS >> 2)                 //6
#define NUM_LEDS_Q3 (NUM_LEDS >> 1)                 //12
#define NUM_LEDS_Q4 (NUM_LEDS - (NUM_LEDS >> 2))    //18
#define NUM_LEDS_Q5 NUM_LEDS                        //24

// divide LEDs into eights
#define NUM_LEDS_81 0
#define NUM_LEDS_82 (NUM_LEDS >> 3)                     //3
#define NUM_LEDS_83 (NUM_LEDS >> 2)                     //6
#define NUM_LEDS_84 ((NUM_LEDS >> 2) + (NUM_LEDS >> 3)) //9
#define NUM_LEDS_85 (NUM_LEDS >> 1)                     //12
#define NUM_LEDS_86 ((NUM_LEDS >> 1) + (NUM_LEDS >> 3)) //15
#define NUM_LEDS_87 ((NUM_LEDS >> 1) + (NUM_LEDS >> 2)) //18
#define NUM_LEDS_88 (NUM_LEDS - (NUM_LEDS >> 3))        //21
#define NUM_LEDS_89 NUM_LEDS                            //24

#define NUM_GROUPS 6 // number of possible groups, tentatively 0 is all LEDs

#define CLOCK_EFFECT_TIME 100 // how often to check on clock effect

const int RED = (255<<16) + (  0<<8) +   0;
const int YELLOW = (255<<16) + (255<<8) +   0;
const int WHITE = (255<<16) + (255<<8) + 255;
const int BLUE = (  0<<16) + (  0<<8) + 255;
const int GREEN = (  0<<16) + (128<<8) +   0;
const int ORANGE = (255<<16) + (165<<8) +   0;
const int VIOLET = (238<<16) + (130<<8) + 238;
const int BLACK = (  0<<16) + (  0<<8) +   0;

const int colors [] = {
  RED,
  YELLOW,
  WHITE,
  BLUE,
  GREEN,
  ORANGE,
  VIOLET,
  BLACK
};


enum ledModes {
    offMode,
    demo1,
    demo2,
    demo3,
    demo4,
    demo5,
    demo6,
    singleColor,
    staticMultiColor,
    twinklingMultiColor,
    staticRainbow,
    staticWheel,
    colorWave,
    clockFace,
    marqueeLeft,
    marqueeRight,
    allFlash,
    quadFlasher,
    quadFlasher2,
    quadFlasher3,
    quadFlasher4,
    quadFlasher5,
    quadFlasher6,
    quadFlasher7,
    octaFlasher,
    octaFlasher2,
    circulator,
    pendulum,
    breathe,
    flashSynched,
    flashPhased,
    winkSynched,
    winkPhased,
    blinkSynched,
    blinkPhased,
    meteorSynched,
    meteorPhased,
    life01,
    life02,
    life03,
    life04,
    life05,
    life06,
    life07,
    life08,
    life09,
    life10,
    life11,
    life12,
    life13,
    life15,
    NUM_LED_MODES
};

String ledModeNames []= {
    "offMode",
    "demo1",
    "demo2",
    "demo3",
    "demo4",
    "demo5",
    "demo6",
    "singleColor",
    "staticMultiColor",
    "twinklingMultiColor",
    "staticRainbow",
    "staticWheel",
    "colorWave",
    "clockFace",
    "marqueeLeft",
    "marqueeRight",
    "allFlash",
    "quadFlasher",
    "quadFlasher2",
    "quadFlasher3",
    "quadFlasher4",
    "quadFlasher5",
    "quadFlasher6",
    "quadFlasher7",
    "octaFlasher",
    "octaFlasher2",
    "circulator",
    "pendulum",
    "breathe",
    "flashSynched",
    "flashPhased",
    "winkSynched",
    "winkPhased",
    "blinkSynched",
    "blinkPhased",
    "meteorSynched",
    "meteorPhased",
    "life01",
    "life02",
    "life03",
    "life04",
    "life05",
    "life06",
    "life07",
    "life08",
    "life09",
    "life10",
    "life11",
    "life12",
    "life13",
    "life15",

};

enum ledEvent {
    LED_TIME_OUT,
    LED_ON,
    LED_OFF,
    LED_ON_COLOR, // color
    LED_FADE_TO_BLACK, //duration
    LED_FADE_TO_COLOR, //duration, color
    LED_WAIT, // duration
    LED_LOOP,
    LED_STOP
};

enum groupEvent {
    GROUP_TIME_OUT, //0
    GROUP_ON, //1
    GROUP_ON_COLOR,
    GROUP_OFF,
    GROUP_ON_MULTI,
    GROUP_COLOR, //5
    GROUP_COLOR_SEGMENT,
    GROUP_LOAD_PATTERN,
    GROUP_LOAD_REPEAT_PATTERN,
    GROUP_RAINBOW,
    GROUP_WHEEL, //10
    GROUP_WAVE,
// this wipes in a new color one LED at a time
// should have one that changes all colors at the same time
    GROUP_TWINKLE,
    GROUP_BREATHE,
    GROUP_ROTATE_RIGHT,
    GROUP_ROTATE_LEFT, //15
    GROUP_ROTATE_RIGHT_COUNT,
    GROUP_ROTATE_LEFT_COUNT,
    GROUP_REPEAT_FOREVER,
    GROUP_REPEAT_COUNT,
    GROUP_REPEAT_END, //20
    GROUP_CLOCK,
    GROUP_LIFE,
    GROUP_CHASE_CW_PAIR,
//should be GROUP_CHASE_RIGHT_PAIR
//should add GROUP_CHASE_LEFT_PAIR
//should add GROUP_CHASE_RIGHT
//should add GROUP_CHASE_LEFT
    GROUP_WAIT, //24
    GROUP_FADE_TO_BLACK,
    GROUP_FADE_TO_COLOR,
    GROUP_GCOLOR,
    GROUP_GCOLOR_RANDOM,
    GROUP_GFORK,
    GROUP_SET_GROUP, //30
    GROUP_SET_ALL,
    GROUP_SET_MEMBERS,
    GROUP_MASK, //33
    GROUP_SET_MASK, //34
    GROUP_SET_MASK_COLORS,
    GROUP_MASK_SWAP, //37
    GROUP_PHASE_DELAY,
//should be GROUP_PHASE_DELAY_RIGHT
//should add GROUP_PHASE_DELAY_LEFT
    GROUP_LFORK,
    GROUP_LFORK_ALL,
    GROUP_LKILL,
    GROUP_LKILL_ALL, //42
    GROUP_LOOP,
    GROUP_STOP_ALL,
    GROUP_STOP
};


// group LED masks

uint32_t quadDotsA [] = {
    (0b0000000000000000 << 16) +
    (0b0000000000111000),
    (0b0000000001111100 << 16) +
    (0b0000000011111110),
    (0b0000000011111110 << 16) +
    (0b0000000011111110),
    (0b0000000001111100 << 16) +
    (0b0000000000111000),
    (0b0000111000000000 << 16) +
    (0b0001111100000000),
    (0b0011111110000000 << 16) +
    (0b0011111110000000),
    (0b0011111110000000 << 16) +
    (0b0001111100000000),
    (0b0000111000000000 << 16) +
    (0b0000000000000000)
};

uint32_t quadDotsB [] = {
   (0b0000000000000000 << 16) +
   (0b0000111000000000),
   (0b0001111100000000 << 16) +
   (0b0011111110000000),
   (0b0011111110000000 << 16) +
   (0b0011111110000000),
   (0b0001111100000000 << 16) +
   (0b0000111000000000),
   (0b0000000000111000 << 16) +
   (0b0000000001111100),
   (0b0000000011111110 << 16) +
   (0b0000000011111110),
   (0b0000000011111110 << 16) +
   (0b0000000001111100),
   (0b0000000000111000 << 16) +
   (0b0000000000000000)
};

uint32_t quadSquareA [] = {
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111)
};

uint32_t quadSquareB [] = {
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b0000000011111111 << 16) +
   (uint32_t)(0b0000000011111111),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000),
   (uint32_t)(0b1111111100000000 << 16) +
   (uint32_t)(0b1111111100000000)
};


uint32_t rightArrow [] = {
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000011000000),
   (uint32_t)(0b0000000001100000 << 16) +
   (uint32_t)(0b0000000000110000),
   (uint32_t)(0b0000000000011000 << 16) +
   (uint32_t)(0b0000000000001100),
   (uint32_t)(0b0000000000000110 << 16) +
   (uint32_t)(0b1111111111111111),
   (uint32_t)(0b1111111111111111 << 16) +
   (uint32_t)(0b0000000000000110),
   (uint32_t)(0b0000000000001100 << 16) +
   (uint32_t)(0b0000000000011000),
   (uint32_t)(0b0000000000110000 << 16) +
   (uint32_t)(0b0000000001100000),
   (uint32_t)(0b0000000011000000 << 16) +
   (uint32_t)(0b0000000110000000)
};

uint32_t leftArrow [] = {
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000001100000000),
   (uint32_t)(0b0000011000000000 << 16) +
   (uint32_t)(0b0000110000000000),
   (uint32_t)(0b0001100000000000 << 16) +
   (uint32_t)(0b0011000000000000),
   (uint32_t)(0b0110000000000000 << 16) +
   (uint32_t)(0b1111111111111111),
   (uint32_t)(0b1111111111111111 << 16) +
   (uint32_t)(0b0110000000000000),
   (uint32_t)(0b0011000000000000 << 16) +
   (uint32_t)(0b0001100000000000),
   (uint32_t)(0b0000110000000000 << 16) +
   (uint32_t)(0b0000011000000000),
   (uint32_t)(0b0000001100000000 << 16) +
   (uint32_t)(0b0000000110000000)
};

uint32_t upArrow [] = {
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000001111000000),
   (uint32_t)(0b0000011111100000 << 16) +
   (uint32_t)(0b0000110110110000),
   (uint32_t)(0b0001100110011000 << 16) +
   (uint32_t)(0b0011000110001100),
   (uint32_t)(0b0110000110000110 << 16) +
   (uint32_t)(0b1100000110000011),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000)
};

uint32_t downArrow [] = {
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b0000000110000000 << 16) +
   (uint32_t)(0b0000000110000000),
   (uint32_t)(0b1100000110000011 << 16) +
   (uint32_t)(0b0110000110000110),
   (uint32_t)(0b0011000110001100 << 16) +
   (uint32_t)(0b0001100110011000),
   (uint32_t)(0b0000110110110000 << 16) +
   (uint32_t)(0b0000011111100000),
   (uint32_t)(0b0000001111000000 << 16) +
   (uint32_t)(0b0000000110000000)
};

uint32_t iot [] = {
   (0b0000000000000000 << 16) +
   (0b0000000000000000),
   (0b0000000000000000 << 16) +
   (0b0000000000000000),
   (0b0110000001111110 << 16) +
   (0b0110000000011000),
   (0b0110000000011000 << 16) +
   (0b0110011110011000),
   (0b0110110011011000 << 16) +
   (0b0110110011011000),
   (0b0110110011011000 << 16) +
   (0b0110011110011000),
   (0b0000000000000000 << 16) +
   (0b0000000000000000),
   (0b0000000000000000 << 16) +
   (0b0000000000000000)
};

uint32_t borderMask [] = {
   (uint32_t)0b1111111111111111 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1000000000000001 << 16,
   (uint32_t)0b1000000000000001 |
   (uint32_t)0b1111111111111111 << 16
};

uint32_t life_map_01 [] = { // traffic signal period 2
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000001110000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000100000100000 |
   (uint32_t)0b0000100000100000 << 16,
   (uint32_t)0b0000100000100000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000001110000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_02 [] = { //period 3
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0001110001110000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0100001010000100 << 16,
   (uint32_t)0b0100001010000100 |
   (uint32_t)0b0100001010000100 << 16,
   (uint32_t)0b0001110001110000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0001110001110000 |
   (uint32_t)0b0100001010000100 << 16,
   (uint32_t)0b0100001010000100 |
   (uint32_t)0b0100001010000100 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0001110001110000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_03 [] = { // gliders
   (uint32_t)0b0001000000001000 |
   (uint32_t)0b0101000000001010 << 16,
   (uint32_t)0b0011000000001100 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0011000000001100 << 16,
   (uint32_t)0b0101000000001010 |
   (uint32_t)0b0001000000001000 << 16
};

uint32_t life_map_04 [] = { //heavy weight space ship
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0111100000000000 |
   (uint32_t)0b1000100000000000 << 16,
   (uint32_t)0b0000100000000000 |
   (uint32_t)0b1001000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_05 [] = { // light weight space ships
   (uint32_t)0b1010000000001001 |
   (uint32_t)0b0001000000010000 << 16,
   (uint32_t)0b0001000000010001 |
   (uint32_t)0b1001000000011110 << 16,
   (uint32_t)0b0111000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000001110 << 16,
   (uint32_t)0b0111100000001001 |
   (uint32_t)0b1000100000001000 << 16,
   (uint32_t)0b0000100000001000 |
   (uint32_t)0b1001000000000101 << 16
};

uint32_t life_map_06 [] = { // beacons
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0001100000110000 << 16,
   (uint32_t)0b0001100000110000 |
   (uint32_t)0b0000011011000000 << 16,
   (uint32_t)0b0000011011000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000011011000000 |
   (uint32_t)0b0000011011000000 << 16,
   (uint32_t)0b0001100000110000 |
   (uint32_t)0b0001100000110000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_07 [] = { // pentathalon
   (uint32_t)0b0000000100000000 |
   (uint32_t)0b0000000100000000 << 16,
   (uint32_t)0b0000001110000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000001110000000 << 16,
   (uint32_t)0b0000000100000000 |
   (uint32_t)0b0000000100000000 << 16,
   (uint32_t)0b0000000100000000 |
   (uint32_t)0b0000000100000000 << 16,
   (uint32_t)0b0000001110000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000001110000000 << 16,
   (uint32_t)0b0000000100000000 |
   (uint32_t)0b0000000100000000 << 16
};

uint32_t life_map_08 [] = { // heavy weight space ship
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000110000000000 << 16,
   (uint32_t)0b0010000100000000 |
   (uint32_t)0b0000000010000000 << 16,
   (uint32_t)0b0010000010000000 |
   (uint32_t)0b0001111110000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_09 [] = { // oscillator 14
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000011100000 |
   (uint32_t)0b0000000111100000 << 16,
   (uint32_t)0b1100001100011110 |
   (uint32_t)0b1100011001101110 << 16,
   (uint32_t)0b0000000111100000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000111100000 |
   (uint32_t)0b1100011001101110 << 16,
   (uint32_t)0b1100001100011110 |
   (uint32_t)0b0000000111100000 << 16,
   (uint32_t)0b0000000011100000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_10 [] = { // tumbler
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000001000100000 |
   (uint32_t)0b0000001101100000 << 16,
   (uint32_t)0b0000010101010000 |
   (uint32_t)0b0000011000110000 << 16,
   (uint32_t)0b0000001001000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_11 [] = { // unix period 6 oscillator
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000011000000000 << 16,
   (uint32_t)0b0000011000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000111000000000 << 16,
   (uint32_t)0b0000110100110000 |
   (uint32_t)0b0000001100110000 << 16,
   (uint32_t)0b0000001100000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_12 [] = { // great on-off
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000011000000000 |
   (uint32_t)0b0000100100000000 << 16,
   (uint32_t)0b0000101100000000 |
   (uint32_t)0b0001101011000000 << 16,
   (uint32_t)0b0000000110100000 |
   (uint32_t)0b0000000000100000 << 16,
   (uint32_t)0b0000000111000000 |
   (uint32_t)0b0000000100000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_13 [] = { // birther
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b1100000000000000 << 16,
   (uint32_t)0b1100000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0001000000000000 << 16,
   (uint32_t)0b1111100000000000 |
   (uint32_t)0b0000010000000000 << 16,
   (uint32_t)0b0001100000000000 |
   (uint32_t)0b0011000000000000 << 16,
   (uint32_t)0b0100000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_14 [] = {
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16
};

uint32_t life_map_15 [] = {
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b1111001111110000 << 16,
   (uint32_t)0b0110000011000000 |
   (uint32_t)0b0110000011000111 << 16,
   (uint32_t)0b0110000011000000 |
   (uint32_t)0b1111000011000000 << 16,
   (uint32_t)0b0000000000000000 |
   (uint32_t)0b0000000000000000 << 16,
   (uint32_t)0b0000100000000000 |
   (uint32_t)0b0000100000000000 << 16,
   (uint32_t)0b0100100100010101 |
   (uint32_t)0b1010101010101011 << 16,
   (uint32_t)0b0100100100011001 |
   (uint32_t)0b0000000000001001 << 16,
   (uint32_t)0b0110000110101010 |
   (uint32_t)0b0001111000010100 << 16
};
