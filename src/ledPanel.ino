/*
current checklist
_ mqtt controls
_ mqtt status
_ change direction to be grads 0..99 or degrees 0..359
_ make clock full face
_ add flashing arrow - -- --- ---- ----- ----->
_ allow arrow to be pointed
_ allow 90 degree rotation of display


want to incorporate/unify all of the commands
percent survives other commands
    - fade to black or fade to target change the overall brightness without touching the saved value
color can survive most other commands
- some scenes/modes use the set color
- other scenes/modes use their own color

does scene/mode?
off could just fade to black without changing the scene/mode
  - this would change the effect immediately
  - if scene/mode is still active, it may set effects as it desires (meteor)
  - last scene/mode can be preserved by off and used by on
on could just ramp up from the last known scene/mode

    how do you start/select a group?
        are groups predefined, so you just pick a number/name <<<easiest to do
        are group dynamically defined
            create at will
            destroy when done
    how many groups can control a particular LED
        zero -- for special case LEDs
        one -- normal
            make it easy to assign... just a group# or 255 for none
        more than one -- for layering, maybe sprites
    --need a list. assume most LED arrays use a single set of numbering 0..NUM_LEDS-1
    we want to be able to assign a group of LEDs in a rectangular array to be a clock
    we want to be able to assign a group of LEDs to be the surrounding marquee
        this means that the group defined as an array of LED numbers
            this array could be shorted to a bit map
                no it cannot... it must be ordered for marquee effects
        to access the led: groups->members->led
        This adds a level of indirection to the processing

    Can groups be dynamically reassigned
        have a set of groups for horizontal rows
        switch to a set of groups for vertical columns
        maybe a group for the periphery marquee

    is it possible to have a looping mechanism?
        something like
            GROUP_REPEAT_FOREVER,
                GROUP_ON,
                GROUP_WAIT, 200,
                GROUP_OFF,
                GROUP_WAIT, 200,
            GROUP_REPEAT_END
        or
            GROUP_REPEAT_COUNT, 6,
                GROUP_ON,
                GROUP_WAIT, 200,
                GROUP_OFF,
                GROUP_WAIT, 200,
            GROUP_REPEAT_END
        to prevent multiple instances, GROUP_REPEATx could check the loop count
            this would require zeroing the counter, it the process was interupted (or restarted)
            should not allow interpretation within the loop

need to be able to "call" subroutines
something like:

groupFlashType1:
  GROUP ON
  GROUP WAIT
  GROUP OFF
  GROUP WAIT

ledFlashType1:
  LED ON
  LED WAIT
  LED ON
  LED WAIT

DIRECTOR:
  GROUP 1 groupFlashType1
  GROUP 3 groupFlashType1
  LED 2 ledFlashType1
  GROUP WAIT 15 * 1000

to make this more symmetrical how about:

scene1
  GROUP START 1 groupFlashType1
  GROUP START 3 groupFlashType1
  LED START 2 ledFlashType1
  GROUP WAIT 3 * 1000
  LED START 2 ledFlashType2
  GROUP WAIT 3 * 1000

directorGroup1
  GROUP START 1 scene1
  GROUP WAIT 3 * 1000
  GROUP START 1 scene2
  GROUP WAIT 3 * 1000
  GROUP START 1 scene3
  GROUP WAIT 3 * 1000


need to had a transparent color to allow color to shine from lower layers
need to have color set by group, not as a whole
need to control dimming by whole and by group

transparency
  maybe think of this like a drawing
    it consists of a set of objects
    the objects are ordered from top to bottom
  in this case the objects are (for now) groups and LEDs
  the ordering can be a ordered list
    LEDs are normally at the bottom, but not necessarily so
    groups may be in any order.
 to find the color of a pixel, dive down through the objects until action one found

 think about sprites... sort of in the sense of the CodeBug sprites
   they can be one or two dimensions
   they can move from one point to another at some velocity
   two dimentional sprites maybe a string of characters

 DIRECTOR
   GROUP 1 multiColorMarquee
   GROUP 2 background
   GROUP 3 flyInStringFromRight
 DIRECTOR END
 DIRECTOR
   GROUP 1 multiColorMarquee
   GROUP 2 background2
   GROUP 3 scrollInStringFromRight
 DIRECTOR END

    some commands are for immediate execution (GROUP_COLOR, GROUP_LOAD_PATTERN)
    some commands wait for a timer (GROUP_WAIT)
    some commands pass control to the FSM (e.g. GROUP_CLOCK)

    this can and should change the group FSM state, the calling function can do more

    Or is there another philosopy at play here.
        do actions until done
        done when no more to do (STOP) or wait for timer (WAIT)
        commands that do not change state should not FastLED.show()
        commands that do change to a stable state should do a FastLED.show()

     this latter philosophy would need a loop here.


j

new thing
  want to set up a mask that defines two groups: 0 and 1.
  want to use commanded colors 0 and 1, unless specified
  want to assign a color to 0 and 1
  want to swap colors 0 and 1

  GROUP_SET_MASK: (8 16-bit numbers)
  GROUP_MASK_SWAP
  GROUP_BACKGROUND: (color)
  GROUP_USE_COLORS // set foreground and background colors.

This is getting messed up with different philosophies

nice to have indirect way of loading groups as they are sometimes common

*/



// **** INCLUDES ****

//Load Fast LED Library and set it's options.
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#define PARTICLE_NO_ARDUINO_COMPATIBILITY 1
#include <MQTT.h>


//**** CONSTANTS ****




//HOSTNAME is the name of the local node for MQTT
#define HOSTNAME "panel1"
// KEEP_ALIVE is number of seconds without MQTT message
#define KEEP_ALIVE 60
#define TIME unsigned long
#define NUM_LEDS 256
#define FASTLED_PIN D6
#define ledBias 2 /* minimum led value during sine wave */

//select one of the following for LED layouts
#define LINEAR_ARRAY false
#define SERPENTINE_16_ARRAY true

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
    GROUP_SET_MASK, //33
    GROUP_SET_MASK_COLORS,
    GROUP_MASK_SWAP, //35
    GROUP_PHASE_DELAY,
//should be GROUP_PHASE_DELAY_RIGHT
//should add GROUP_PHASE_DELAY_LEFT
    GROUP_LFORK,
    GROUP_LFORK_ALL,
    GROUP_LKILL,
    GROUP_LKILL_ALL, //40
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

uint32_t life_map_04 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_05 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_06 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_07 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_08 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_09 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};

uint32_t life_map_10 [] = {
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000,
   (uint32_t)0b0000000000000000 <<16 |
   (uint32_t)0b0000000000000000
};


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



int life1[] = { // period of 2
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


int life2[] = { // period of 3
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

int life3[] = { // glider
    GROUP_SET_MASK, (uint32_t)&life_map_03,
    //GROUP_SET_MEMBERS, 10,
    //                    19,33,35,50,51,
    //                      188,189,204,206,220,
    GROUP_ON,
    GROUP_WAIT, 500, // just to see it
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
    GROUP_GFORK, 1, (int)&life1,
    GROUP_WAIT, 10000,

    GROUP_LOOP
};


int demo4GC [] = { // more of a life demo
    GROUP_STOP_ALL, // just in case something is running
    GROUP_SET_ALL,
    GROUP_OFF,

    GROUP_GCOLOR, 1, GREEN,
    GROUP_GFORK, 1, (int)&life1, // oscillator period of 2
    GROUP_WAIT, 10000,

    GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_GCOLOR, 1, BLUE,
    GROUP_GFORK, 1, (int)&life2, // oscillator period of 3
    GROUP_WAIT, 10000,

    GROUP_SET_ALL,
    GROUP_OFF,
    GROUP_GCOLOR, 1, YELLOW,
    GROUP_GFORK, 1, (int)&life3, // glider
    GROUP_WAIT, 10000,

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

/*
//the array is numbers 0 1 2 3 4 5...
// the high order bit of << 16)  is bit 0
// the high order bit of) is bit 16

function maskToMembers (unsigned int *mask[], int groupNumber) {
  memberNumber = 0;
  maskOffset = 0;
  bitmap = map[ maskOffset];
  maskOffset = maskOffset + 1;
  for (int i=0; i< NUM_LEDS; i++) {
    if (mask &(0b1000000000000000 != 0) {
        ledGroupStates[ groupNumber].members[memberNumber] = i;
        memberNumber = memberNumber + 1;
    }
    if ((i % 32) == 31) {
      bitmap = bitmap = map[ maskOffset];
      maskOffset = maskOffset + 1;
      if (maskOffset > sizeof (mask)/ sizeof( int) {
         REPORT AN ERROR;
      }
    }
  }
}
*/


//**** TYPEDEFs ****

// LedState is the state information of indivually controlled LEDs
struct LedState {
    int ledNumber;              // number of LED being controlled
    int *currentCommandArray;   // pointer to array of commands
    int currentCommandIndex;    // index into current array of commands
    int (*currentState)( LedState*, ledEvent, TIME);
    uint8_t fadeStepNumber; // color is proportioned 255*(fadeSteps - fadeStepNumber)/fadeSteps
    uint8_t fadeSteps;      // total number of steps in fade
    CRGB fromColor;
    CRGB toColor;
    CRGB currentColor;
    TIME startTime; // time upon entering the current state
    unsigned int timeToWait;           // time to remain in the current state
};


struct LedGroupState {
    int groupNumber;        // number of LED group being controlled
    int *currentCommandArray;   // pointer to array of commands
    int currentCommandIndex;    // index into current array of commands
    int fromLED;
    int toLED;
    int onLED;
    int waveCount;//within wave effect
    int count;//within a REPEAT
    int repeatIndex;    // index into current array of commands for start of repeat
    uint8_t fadeStepNumber; // color is proportioned 255*(fadeSteps - fadeStepNumber)/fadeSteps
    uint8_t fadeSteps;      // total number of steps in fade
    uint8_t breatheIndex; // state of breathing for group
    uint8_t breathStep;   // amount to step index for each period
    CRGB fromColor; // fading from color
    CRGB toColor;   // fading to color
    CRGB color; // color of LEDs
    CRGB pattern [NUM_LEDS]; // array of colors
    int members [NUM_LEDS];    // ordered array of LED numbers in group for marquees
    CRGB ledStatus [NUM_LEDS];    // array of LED current status for this group
    uint8_t *groupMask;    // array of the bit masks for LED in this group
    int numLeds;            // number of LED members in group
    int (*currentState)( LedGroupState*, groupEvent, TIME);
    TIME startTime; // time upon entering the current state
    unsigned int timeToWait;           // time to remain in the current state
};


//**** GLOBALS ****

CRGB leds[NUM_LEDS]; // main LED array which is directly written to the LEDs


int ledBrightness; // level of over all brightness desired 0..100 percent
int ledDirection = 0; // 0..359 LED direction in degrees for selected modes
//int LEDSpeed = 0; // 0..100 speed of LED display mode
CRGB ledColor = CRGB::White; // default color of individual LEDs
int selectedGroup = 0; // currently selected group

ledModes ledMode = offMode; // LED display mode, see above


LedState ledStates [ NUM_LEDS];
LedGroupState ledGroupStates [NUM_GROUPS];


//**** FUNCTION PROTOTYPES FOR FORWARD REFERENCES ****

int groupNullSPF ( LedGroupState *group, groupEvent event, TIME currentTime);
int groupStopSPF ( LedGroupState *group, groupEvent event, TIME currentTime);
int ledNullSPF ( LedState *state, ledEvent event, TIME currentTime);
void ledAllStatesSPF( LedState *state, ledEvent event, TIME currentTime); // handles messages for all states
void groupAllStatesSPF( LedGroupState *group, groupEvent event, TIME currentTime);
void callback(char* topic, byte* payload, unsigned int length);
NSFastLED::CRGB makeColor (uint8_t red, uint8_t green, uint8_t blue);



// **** MQTT FUNCTIONS ****

byte server[] = { 192,168,4,1 };
MQTT client(server, 1883, KEEP_ALIVE, callback);

#define MAX_PAYLOAD_SIZE 100
// for QoS2 MQTTPUBREL message.
// this messageid maybe have store list or array structure.
uint16_t qos2messageid = 0;
char lastPayload [MAX_PAYLOAD_SIZE+1];
unsigned long lastPayloadTime; // epoch of last payload

// receive message
void callback(char* topic, byte* payload, unsigned int length) {
    int passedMode;
    int passedBrightness;
    int passedDirection;
    int passedColor;
    if (length > MAX_PAYLOAD_SIZE) {
        length = MAX_PAYLOAD_SIZE;
    }
    memcpy(lastPayload, payload, length);
    lastPayload[length] = '\0';
    lastPayloadTime = Time.now();
    //String topicS = String( (const char*) topic);
    //String payloadS = String( (const char*) lastPayload);
    String topicS = String( (char*) topic);
    String payloadS = String( (char*) lastPayload);

    Serial.println( "Received message topic:" + topicS + " payload:" + payloadS);

    if (topicS.compareTo( String( HOSTNAME) + String ("/command/mode")) == 0) {
        // payload expected to be 0..n (mode index)
        passedMode = payloadS.toInt();
        if (passedMode >= 0 && passedMode < NUM_LED_MODES) {
            Serial.println( "Set LED mode " + String( passedMode, DEC) + " " + ledModeNames [ passedMode]);
            setLedMode( selectedGroup, passedMode);
        } else {
            Serial.println( "Mode payload was bad: " + payloadS);
        }
    } else if (topicS.compareTo( String( HOSTNAME) + String ("/command/brightness")) == 0) {
        // payload expected to be 0..100 (percent)
        passedBrightness = payloadS.toInt();
        if (passedBrightness > 0 && passedBrightness <= 100) {
            FastLED.setBrightness( (int) (passedBrightness*255/100));
            Serial.println( "Brightness set to: " + String( passedBrightness, DEC) + "%");
        } else {
            Serial.println( "Brightness payload was bad: " + payloadS);
        }
    } else if (topicS.compareTo( String( HOSTNAME) + String ("/command/direction")) == 0) {
        // payload expected to be 0..359 (degrees)
        passedDirection = payloadS.toInt();
        if (passedDirection > 0 && passedDirection <= 360) {
            ledDirection = passedDirection;
            Serial.println( "Direction set to: " + String( passedDirection, DEC) + "%");
        } else {
            Serial.println( "Direction payload was bad: " + payloadS);
        }
    } else if (topicS.compareTo( String( HOSTNAME) + String ("/command/color")) == 0) {
        // payload expected to be some sort of HTML color in the form:
        // index into a box of crayons
        // 0xFFFFFF, 0xFFF, names, hue angles, lists, etc., can come later
        // want a multi color, but that may be the same thing as a list
        passedColor = payloadS.toInt();
        if (passedColor > 0 && passedColor <= (int)(sizeof( colors)/sizeof(int))) {
            ledColor = passedColor;
            Serial.println( "Set color " + String( passedColor, DEC) + " " + colors [ passedColor]);
        } else {
            Serial.println( "Color payload was bad: " + payloadS);
        }
    }
}


void publish (String topic, String payload) {
#define BUFFER_LEN 30
    char topicBuffer[BUFFER_LEN];
    char payloadBuffer[BUFFER_LEN];
    topic.toCharArray(topicBuffer, BUFFER_LEN);
    payload.toCharArray(payloadBuffer, BUFFER_LEN);
    client.publish( topicBuffer, payloadBuffer);
    Serial.println(String( "MQTT: ") + String( topicBuffer) + String( ": ") + String( payloadBuffer));
}


// **** FUNCTIONS ****

#define SERPENTINE_ROW 16
void setupAllLedFSMs ()
{
    if ( LINEAR_ARRAY) {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            ledStates[i].ledNumber = i;
        }
    } else if ( SERPENTINE_16_ARRAY) {
        int ledMagic = 0;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int rowNumber = i/SERPENTINE_ROW;
            if ( rowNumber%2 == 0) { // even rows are normal
                ledStates[i].ledNumber = i;
                ledMagic = SERPENTINE_ROW *(rowNumber+2); //0 ==>32
            } else { // odd rows are backward
                //16 should be 31, and 31 should be 16
                // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
                //31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
                ledMagic = ledMagic - 1;
                ledStates[i].ledNumber = ledMagic;
            }
       }

    }
}


void showLeds ( bool changed)
{
    if( changed) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds [ ledStates[i].ledNumber] = ledStates[i].currentColor;
            //Serial.printf( "%d/%d,", i, ledStates[i].currentColor);
        }
        FastLED.show();
    }
}

void stopAllLedFSMs ()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        ledStates[i].currentState = ledNullSPF;
        ledStates[i].timeToWait = 0;
    }
}


void setupAllLedGroupFSMs ()
{
    CRGB color = CRGB::White;
    for (int i = 0; i < NUM_GROUPS; i++)
    {
        ledGroupStates[i].groupNumber = i;
        ledGroupStates[i].color = (color.r<<16) + (color.g<<8) + color.b;
        //Serial.printf("setup i:%d FSM:%X\r\n", i, (ledGroupStates[i].color.r<<16)+(ledGroupStates[i].color.g<<8)+ledGroupStates[i].color.b);
    }
}


void stopAllLedGroupFSMs()
{
    for (int i = 0; i < NUM_GROUPS; i++)
    {
        ledGroupStates[i].currentState = groupNullSPF;
        ledGroupStates[i].timeToWait = 0;
    }
}


NSFastLED::CRGB makeColor (uint8_t red, uint8_t green, uint8_t blue)
{
    CRGB newColor;
    newColor.r = red;
    newColor.g = green;
    newColor.b = blue;
    return newColor;
}


void fillLedColor( NSFastLED::CRGB color)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        //Serial.printf ("fillLedColor %d R%d G%d B%d\r\n", i, color.r, color.g, color.b);
	ledStates [ i].currentColor = color;
    }
}


void partialFillLEDcolor( int start, int end, NSFastLED::CRGB color)
{
    for (int i = start; i < end; i++) {
	ledStates [ i].currentColor = color;
    }
}


void fillLedMultiColor ()
{
    for (int i = 0; i < NUM_LEDS; i++) {
	ledStates [ i].currentColor.r = rand() % 256;
	ledStates [ i].currentColor.g = rand() % 256;
	ledStates [ i].currentColor.b = rand() % 256;
    }
}


NSFastLED::CRGB wheel(uint8_t wheelPos); // prototype to help compiler

NSFastLED::CRGB wheel(uint8_t wheelPos) //from Adafruit and modified
/*
  WheelPos 0..255 position about a circle (like 2Pi radians)
*/
{
    //WheelPos = 255 - WheelPos;
    if(wheelPos < 85) // first third of wheel
    {
        // red falling, no green, blue rising
        return makeColor(255- wheelPos * 3, 0, wheelPos * 3);
    }
    else if(wheelPos < 170) // second third of wheel
    {
        wheelPos -= 85;
        // no red, green rising, blue falling
        return makeColor(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    else // last third of wheel
    {
        wheelPos -= 170;
        // red rising, green falling, no blue
        return makeColor(wheelPos * 3, 255 - wheelPos * 3, 0);
    }
}

void fillWheel()
// spreading a color wheel across all of the LEDS
{
    CRGB col;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        col = wheel( i * 255 / NUM_LEDS);
        //Serial.printf("fillWheel %d %d R%d G%d B%d\r\n", i, i * 255 / NUM_LEDS, col.r, col.g, col.b);
	ledStates [ i].currentColor = wheel( i * 255 / NUM_LEDS);
    }
}


void rotateRight ()
//rotate all of the LEDs to the right
{
    CRGB saveLed = ledStates [ NUM_LEDS-1].currentColor;
    for (int i = NUM_LEDS - 1; i>0; i--)
    {
	ledStates [ i].currentColor = ledStates [ i-1].currentColor;
    }
    ledStates [ 0].currentColor = saveLed;
}


void rotateLeft ()
//rotate all of the LEDs to the left
{
    CRGB saveLed = ledStates [ 0].currentColor;
    for (int i = 0; i < NUM_LEDS - 1; i++)
    {
	ledStates [ i].currentColor = ledStates [ i+1].currentColor;
    }
    ledStates [ NUM_LEDS - 1].currentColor = saveLed;
}


void printStateInfo( LedState *state)
{
    /*
    Serial.printf("Led:%d state:%d commandArray:%d commandIndex:%d timeToWait:%d\r\n",
            state->ledNumber,
            state->currentState,
            state->currentCommandArray,
            state->currentCommandIndex,
            state->timeToWait);
    */
}


void printGroupStateInfo( LedGroupState *group)
{
    /*
    Serial.printf( "Group info group:%d command:%d index:%d state%d timeToWait:%d time:%d\r\n",
            group->groupNumber,
            group->currentCommandArray,
            group->currentCommandIndex,
            group->currentState,
            group->timeToWait,
            millis());
    */
}


unsigned int getNextGroupCommand ( LedGroupState *group)
// returns the next LED command for a particular state and updates pointers
{
    printGroupStateInfo( group);
    unsigned int command = group->currentCommandArray[ group->currentCommandIndex];
    if (command == GROUP_LOOP)
    {
        //Serial.printf("getNextGroupCommand looping\r\n", group->currentCommandIndex);
        group->currentCommandIndex = 0;
        command = group->currentCommandArray[ group->currentCommandIndex];
    }
    //Serial.printf("getNextGroupCommand %d\r\n", command);
    group->currentCommandIndex++;
    if (group->currentCommandArray[ group->currentCommandIndex] == GROUP_LOOP)
    {
        //Serial.printf("getNextGroupCommand looping\r\n", group->currentCommandIndex);
        group->currentCommandIndex = 0;
    }
    //printStateInfo( state);
    return command;
}


unsigned int getNextGroupItem ( LedGroupState *group)
// returns the next LED item (not a command) for a particular state and updates pointers
{
    printGroupStateInfo( group);
    unsigned int item = group->currentCommandArray[ group->currentCommandIndex];
    //Serial.printf("getNextGroupItem %d\r\n", item);
    group->currentCommandIndex++;
    //printStateInfo( state);
    return item;
}


unsigned int getNextLedCommand ( LedState *state)
// returns the next LED command for a particular state and updates pointers
{
    //printStateInfo( state);
    unsigned int command = state->currentCommandArray[ state->currentCommandIndex];
    //Serial.printf( "getNextLedCommand %d\r\n", command);
    state->currentCommandIndex++;
    if (state->currentCommandArray[ state->currentCommandIndex] == LED_LOOP)
    {
        state->currentCommandIndex = 0;
    }
    //printStateInfo( state);
    return command;
}


void interpretNextLedCommand ( LedState *state, TIME currentTime)
// also interprets commands and parameters
{
    state->currentState = ledNullSPF;
    while (state->currentState == ledNullSPF)
    //this won't work when you want to do a GROUP STOP
    // maybe should test for looping
    {
        //Serial.printf( "iNLC getting another command\r\n");
        ledAllStatesSPF (state, (ledEvent)getNextLedCommand( state), currentTime);
    }
}


int ledFadingSPF ( LedState *state, ledEvent event, TIME currentTime)
{
    // LED Fading State Processing Function
    // fade a bit more toward target
    int fadeFactor = (state->fadeSteps - state->fadeStepNumber)*
            255 / state->fadeSteps;
    if (state->toColor) // not black
    {
        ledStates [ state->ledNumber].currentColor = blend (state->fromColor, state->toColor,
                fadeFactor/255);
    }
    else // black
    {
        ledStates [ state->ledNumber].currentColor.r = state->fromColor.r * fadeFactor/255;
        ledStates [ state->ledNumber].currentColor.g = state->fromColor.g * fadeFactor/255;
        ledStates [ state->ledNumber].currentColor.b = state->fromColor.b * fadeFactor/255;
    }
    //Serial.printf( "Fading %d %d\r\n", state->fadeStepNumber,
    //    fadeFactor);
    state->fadeStepNumber++;
    if (state->fadeStepNumber == state->fadeSteps)
    {
        interpretNextLedCommand( state, currentTime); // set up next state
    }
    else
    {
        ledStates[ state->ledNumber].startTime = currentTime;
        // go around again
    }
        return 0;
}


int ledWaitSPF ( LedState *state, ledEvent event, TIME currentTime)
{
    // LED Wait State Processing Function
    interpretNextLedCommand( state, currentTime); // set up next state
    return 0;
}

int ledNullSPF ( LedState *state, ledEvent event, TIME currentTime)
{
    // LED Null State Processing Function
    return 0;
}


void ledAllStatesSPF( LedState *state, ledEvent event, TIME currentTime) // handles messages for all states
// also interprets commands and parameters
{
    // LED All (Normal) State Processing Function
    //Serial.printf( "ledAllStatesSPF processing another event %d led:%d state:%d\r\n",
    //        (int)event, state->ledNumber, (int)state->currentState);
    switch( event)
    {
        case LED_ON:
            //Serial.printf("LED ON command led:%d time%d\r\n", state->ledNumber, currentTime);
            ledStates [ state->ledNumber].currentColor = ledColor;
            state->timeToWait = 0; //disable timer
            state->currentState = ledNullSPF;
            break;

        case LED_ON_COLOR: // color
            //Serial.printf("LED ON COLOR command led:%d time%d\r\n", state->ledNumber, currentTime);
            ledStates [ state->ledNumber].currentColor = getNextLedCommand( state);
            state->timeToWait = 0; //disable timer
            state->currentState = ledNullSPF;
            break;

        case LED_OFF:
            //Serial.printf("LED OFF command led:%d time%d\r\n", state->ledNumber, currentTime);
            ledStates [ state->ledNumber].currentColor = CRGB::Black;
            state->timeToWait = 0; //disable timer
            state->currentState = ledNullSPF;
            break;

        case LED_WAIT: // ms
            state->startTime = currentTime;
            state->timeToWait = getNextLedCommand( state);
            //Serial.printf("LED LED command led:%d time:%d wait:%d\r\n",
            //        state->ledNumber, currentTime, state->timeToWait);
            state->currentState = ledWaitSPF;
            break;

        case LED_FADE_TO_BLACK: // ms
        {
            //Serial.println("LED FADE TO BLACK command");
            state->startTime = currentTime;
            state->fadeStepNumber = 0;
            unsigned int duration = getNextLedCommand( state);
            state->fadeSteps = min( duration, 255);
            state->timeToWait = max( 1, state->fadeSteps/255);
            state->fromColor = ledStates [ state->ledNumber].currentColor;
            state->toColor = CRGB::Black;
            state->currentState = &ledFadingSPF;
            break;
        }

        case LED_FADE_TO_COLOR: // ms color
        {
            //Serial.println("LED FADE TO COLOR command");
            state->startTime = currentTime;
            state->fadeStepNumber = 0;
            unsigned int duration = getNextLedCommand( state);
            state->fadeSteps = min( duration, 255);
            state->timeToWait = max( 1, state->fadeSteps/255);
            state->fromColor = ledStates [ state->ledNumber].currentColor;
            state->toColor = getNextLedCommand( state);
            state->currentState = &ledFadingSPF;
            break;
        }

        case LED_STOP:
            //Serial.println("LED STOP command");
            state->currentState = ledNullSPF;
            state->timeToWait = 0; // disable timer
            break;
    }
    //Serial.printf( "ledAllStatesSPF bottom %d led:%d state:%d\r\n",
    //        (int)event, state->ledNumber, (int)state->currentState);
}


void startLedFSM( LedState *state, int *commandArray)
//this replaces LED_ENTRY and ledEntrySPF...
{
    //set state command array to commandArray
    state->currentCommandArray = commandArray;
    state->currentCommandIndex = 0;

    // exeucute the first command of that commandArray
    interpretNextLedCommand ( state, millis());
}


void groupRotateRight( LedGroupState *group)
{
    CRGB saveLed = ledStates[ group->members[ group->numLeds - 1] ].currentColor;
    for (int i = group->numLeds - 1; i >= 1; i--)
    {
        ledStates[ group->members[i]].currentColor = ledStates[ group->members[i -1] ].currentColor;
        //Serial.printf( "%d:%d<%d:%d ",i, group->members[i], group->members[i -1], ledStates[ group->members[i-1] ].currentColor);
        //if (i%8 == 00) {
         // Serial.println();
        //}

    }
    ledStates[ group->members[0]].currentColor = saveLed;
    //Serial.println();
}

void groupRotateLeft( LedGroupState *group)
{
    int groupCount = group->numLeds;
    CRGB saveLed = ledStates[ group->members[ 0]].currentColor;
    for (int i = 0; i < groupCount - 1; i++)
    {
        ledStates[ group->members[i]].currentColor = ledStates[ group->members[i + 1]].currentColor;
    }
    ledStates [group->members[ groupCount - 1]].currentColor = saveLed;
}


void interpretNextGroupCommand (LedGroupState *group, TIME currentTime)
{
    group->currentState = &groupNullSPF;
    while (group->currentState == groupNullSPF)
//this won't work when you want to do a GROUP STOP
// maybe should test for looping
    {
        //Serial.printf( "groupSPF getting another command\r\n");
        groupEvent event = (groupEvent) getNextGroupCommand( group);
        groupAllStatesSPF( group, event, currentTime);
    }
}


//not referenced!
void startGroupFSM( LedGroupState *group, int *commandArray)
//this replaces GROUP_ENTRY and groupEntrySPF...
{
    //set state command array to commandArray
    group->currentCommandArray = commandArray;
    group->currentCommandIndex = 0;

    // exeucute the first command of that commandArray
    groupAllStatesSPF( group, (groupEvent)getNextGroupCommand( group), millis());
}


void interpretGroupCommand ( LedGroupState *group, int *commands, TIME currentTime)
{
    group->currentCommandArray = commands;
    group->currentCommandIndex = 0;
    interpretNextGroupCommand ( group, currentTime);
}


int groupFadingSPF ( LedGroupState *group, groupEvent event, TIME currentTime)
{
    // fade a bit more toward target
    int fadeFactor = (group->fadeSteps - group->fadeStepNumber) * 255 /
            group->fadeSteps;
    if (group->toColor) // not black
    {
        for (int j = 0; j < group->numLeds; j++)
        {
            ledStates[ group->members[j]].currentColor =
                    blend (group->fromColor, group->toColor, fadeFactor/255);
        }
    }
    else // black
    {
        for (int j = 0; j < group->numLeds; j++)
        {
            ledStates[ group->members[j]].currentColor.r =
                    group->fromColor.r * fadeFactor/255;
            ledStates[ group->members[j]].currentColor.g =
                    group->fromColor.g * fadeFactor/255;
            ledStates[ group->members[j]].currentColor.b =
                    group->fromColor.b * fadeFactor/255;
        }
    }
    //Serial.printf( "Fading %d %d\r\n", ledStates[i].fadeStepNumber,
    //    fadeFactor);
    group->fadeStepNumber++;
    if (group->fadeStepNumber == group->fadeSteps)
    {
        interpretNextGroupCommand( group, currentTime); // set up next state
    }
    else
    {
        group->startTime = currentTime;
        // go around again
    }
    return 0;
}


int groupClockSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    float hour = ((Time.hour() % 12) + Time.minute() / 60 + .5) *
            group->numLeds / 12;
    float minute = (Time.minute() + Time.second() / 60 + .5) *
            group->numLeds / 60;
    float second = (Time.second() * group->numLeds + .5)  / 60;
    //Serial.printf( "Clock %0.1f:%0.1f:%0.1f %d\r\n", hour, minute, second, (int) millis()%1000);
    //Serial.printlnf( "groupClockSPF id:%d time%d", group->groupNumber, millis());
    //draw the hands
    fillLedColor( CRGB::Black);
    ledStates[ group->members[ ((int) hour + 1) % group->numLeds]].currentColor = CRGB::Red;
    ledStates[ group->members[ (int) hour]].currentColor = CRGB::Red;
    ledStates[ group->members[ ((int) hour - 1) % group->numLeds]].currentColor = CRGB::Red;
    ledStates[ group->members[ (int) minute]].currentColor = CRGB::Green;
    ledStates[ group->members[ ((int) minute - 1) % group->numLeds]].currentColor = CRGB::Green;
    // flash the seconds hand every second
    if (millis() % 1000 < 1000-2*CLOCK_EFFECT_TIME) // just look at the milliseconds part
    {
        ledStates[ group->members[ (int) second]].currentColor = CRGB::Blue;
    }
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}


void groupCheckLife( LedGroupState *group) {
  int modRow = 0;
  int divRow = 0;
  int rows = NUM_LEDS/SERPENTINE_ROW;
  int living [NUM_LEDS];
  bool nexGen [NUM_LEDS];
  int count = 0;

  for (int i=0; i<NUM_LEDS; i++) {
    if (ledStates[i].currentColor != (CRGB)CRGB::Black) {
      living [i] = 1;
    } else {
      living [i] = 0;
    }
  }

  for (int i=0; i<NUM_LEDS; i++) {
    modRow = i%SERPENTINE_ROW;
    divRow = i/SERPENTINE_ROW;
    if (modRow > 0 && modRow < SERPENTINE_ROW - 1 &&
         divRow > 0 && divRow < rows - 1) { //not on the edge
      //Serial.printf( "*");
      count = 0;
      count = count + living[i-SERPENTINE_ROW-1];
      count = count + living[i-SERPENTINE_ROW];
      count = count + living[i-SERPENTINE_ROW +1];
      count = count + living[i - 1];
      count = count + living[i + 1];
      count = count + living[i+SERPENTINE_ROW-1];
      count = count + living[i+SERPENTINE_ROW];
      count = count + living[i+SERPENTINE_ROW +1];
    } else {
      // if on the bottom don't count cells below bottom
      //Serial.printf( "_");
      count = 0;
      if (divRow>0 && modRow >0) {
        count = count + living[i - SERPENTINE_ROW - 1];
      }
      if (divRow>0) {
        count = count + living[i - SERPENTINE_ROW];
      }
      if (divRow>0 && modRow < SERPENTINE_ROW-1) {
        count = count + living[i - SERPENTINE_ROW + 1];
      }
      if (modRow > 0) {
        count = count + living[i - 1];
      }
      if (modRow < SERPENTINE_ROW -1) {
        count = count + living[i + 1];
      }
      if (divRow < rows-1 && modRow > 0) {
        count = count + living[i + SERPENTINE_ROW - 1];
      }
      if (divRow < rows-1) {
        count = count + living[i + SERPENTINE_ROW];
      }
      if (divRow < rows-1 && modRow < SERPENTINE_ROW-1) {
        count = count + living[i + SERPENTINE_ROW + 1];
      }
    }

    //Serial.printf( "%3d:%d%c ", i, count, living[i] ? 'L':'D' );
    if (modRow == 7 || modRow == 15) {
      //Serial.println();
    }
    if (living [i]) {
      if (count == 2 || count == 3) {
         nexGen [i] = true; // sustanence
      } else {
         nexGen [i] = false; // lonliness or overpopulation
      }
    } else {
      if (count == 3) {
        nexGen [i] = true; // birth
      } else {
        nexGen [i] = false; // conditions not favorable
      }
    }
  }

  // write the next state into the LEDs
  for (int i=0; i<NUM_LEDS; i++) {
    if (nexGen [i]) {
      ledStates[i].currentColor = group->color;
    } else {
      ledStates[i].currentColor = CRGB::Black;
    }
  }
  showLeds( true);
}


int groupTwinklingSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    // turn on last LED
    ledStates[group->onLED].currentColor = group->fromColor;
    // select a new LED to turn off
    group->onLED = random( group->numLeds);
    group->fromColor = ledStates[ group->onLED].currentColor;
    ledStates[ group->onLED].currentColor = CRGB::Black;

    group->timeToWait = random( 100, 500);
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}


int groupBreathingSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    //Serial.printf( "GROUP STATE BREATING index:%d\r\n", group->breatheIndex);
    FastLED.setBrightness( (int) min(255, ((ledBrightness *
            quadwave8( group->breatheIndex) / 100)+ledBias)));
    group->breatheIndex = group->breatheIndex + group->breathStep;
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}


int groupRotatingRightSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    groupRotateRight( group);
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}


int groupRotatingLeftSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    groupRotateLeft( group);
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}

int groupChasePairRightSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    // source, destination, count
    // first LED moved from source to destination, one pixel at a time
    // next LED moved from from source-1 to destination-1 one pixel at a time
    // ... until count LEDs are moved
    //Serial.printf( "Chase count: %d from:%d to:%d on:%d\r\n",
    //    group->count,
    //    group->fromLED,
    //    group->toLED,
    //    group->onLED
    //);
    //move one led on path toward destination
    if (group->onLED == group->toLED)
    {
        // done moving this LED
        group->count = group->count - 1;
        //Serial.printf( "Chasing i:%d\r\n", group->count);
        printGroupStateInfo( group);
        if (group->count == 0) //done moving
        {
            // execute the next group command
            group->startTime = currentTime;
            group->timeToWait = 0;
            group->currentState = groupNullSPF;
            printGroupStateInfo( group);
            interpretNextGroupCommand( group, currentTime); // set up next state
        }
        else
        {
            // back fromLED and toLED for the next LED to move
            group->fromLED = (group->fromLED - 1 + group->numLeds) %
                    group->numLeds;
            group->toLED = (group->toLED - 1 + group->numLeds) %
                    group->numLeds;
            //move the first LED of this group
            group->onLED = group->fromLED;
            group->fromColor = ledStates[group->onLED].currentColor;
            ledStates[ group->onLED].currentColor = CRGB::Black;
            ledStates[ (group->onLED + (group->numLeds>>1)) %
                    group->numLeds].currentColor = CRGB::Black;
            group->onLED = (group->onLED + 1 + group->numLeds) % group->numLeds;
            ledStates[ group->onLED].currentColor = group->fromColor;
            ledStates[ (group->onLED + group->numLeds/2) % group->numLeds].currentColor =
                    group->fromColor;
        }
    }
    else // keep on moving the onLED
    {
        group->fromColor = ledStates[group->onLED].currentColor;
        ledStates[ group->onLED].currentColor = CRGB::Black;
        ledStates[ (group->onLED + group->numLeds/2) % group->numLeds ].currentColor = CRGB::Black;

        group->onLED = (group->onLED + 1) % group->numLeds;
        ledStates[ group->onLED].currentColor = group->fromColor;
        ledStates[ (group->onLED + group->numLeds/2) % group->numLeds].currentColor =
                group->fromColor;
    }
    group->startTime = currentTime;
    showLeds( true);
    return 0;
}



int groupWaitSPF ( LedGroupState *group, groupEvent event, TIME currentTime)
{
    //Serial.printlnf("groupWaitSPF time:%d group:%d currentTime:%d",
    //        millis(), group->groupNumber, currentTime);
    interpretNextGroupCommand( group, currentTime); // set up next state
    return 0;
}


int groupStopSPF ( LedGroupState *group, groupEvent event, TIME currentTime)
{
    return 0;
}


int groupNullSPF ( LedGroupState *group, groupEvent event, TIME currentTime)
{
    return 0;
}


void groupAllStatesSPF( LedGroupState *group, groupEvent event, TIME currentTime)
{
    Serial.printlnf( "%ld gASSPF group:%d event:%d", currentTime, group->groupNumber, event);
    switch (event)
    {
        case GROUP_ON:
        {
            //turn on all LEDs of group to the default color
            //Serial.println("GROUP ON event");
            int groupCount = group->numLeds;
            for (int i = 0; i < groupCount; i++)
            {
                //Serial.printf("GROUP ON i:%d color:%X\r\n", i, (group->color.r<<16)+(group->color.g<<8)+group->color.b);
                ledStates[ group->members[i]].currentColor = group->color;
            }
            break;
        }

        case GROUP_ON_COLOR: // color
        {
            //turn on all LEDs of group to a specified color
            //Serial.println("GROUP ON event");
            int groupCount = group->numLeds;
            unsigned int color = getNextGroupItem( group);
            for (int i = 0; i < groupCount; i++)
            {
                //Serial.printf("GROUP ON i:%d color:%X\r\n", i, (group->color.r<<16)+(group->color.g<<8)+group->color.b);
                ledStates[ group->members[i]].currentColor = color;
            }
            break;
        }

        case GROUP_OFF:
        {
            //turn off all LEDs of group
            //Serial.println("GROUP OFF event");
            int groupCount = group->numLeds;
            for (int i = 0; i < groupCount; i++)
            {
                //Serial.printf("GROUP ON i:%d color:%X\r\n", i, (group->color.r<<16)+(group->color.g<<8)+group->color.b);
                ledStates[ group->members[i]].currentColor = CRGB::Black;
            }
            break;
        }

        case GROUP_ON_MULTI:
        {
            //turn on all LEDs of group with random colors
            //Serial.println("GROUP COLOR MULTI event");
            int groupCount = group->numLeds;
            CRGB groupLedColor;
            for (int i = 0; i < groupCount; i++)
            {
                groupLedColor.r = random (255);
                groupLedColor.g = random (255);
                groupLedColor.b = random (255);
                ledStates[ group->members[i]].currentColor = groupLedColor;
            }
            break;
        }

        case GROUP_COLOR: // color
            //set color of group
            //Serial.println("GROUP COLOR event");
            group->color = getNextGroupItem( group);
            break;

        case GROUP_COLOR_SEGMENT: // color, start, end
        {
            //set color of a segment
            //Serial.println("GROUP COLOR SEGMENT event");
            //printGroupInfo( group);
            CRGB groupLedColor = getNextGroupItem( group);
            unsigned int start =  getNextGroupItem( group);
            unsigned int end = getNextGroupItem( group);

            for (unsigned int i = start; i <= end; i++)
            {
                ledStates[ group->members[i]].currentColor = groupLedColor;
            }
            break;
        }

        case GROUP_LOAD_PATTERN: //count color...
        {
            //Serial.println("GROUP LOAD PATTERN event");
            unsigned int groupCount = getNextGroupItem( group);

            for (int i = 0; i < group->numLeds; i++)
            {
                if (groupCount > 0)
                {
                    ledStates[ group->members[i]].currentColor = getNextGroupItem( group);
                    groupCount--;
                }
                else
                {
                    ledStates[ group->members[i]].currentColor = CRGB::Black;
                }
            }
            while (groupCount > 0) // in case more colors than group members
            {
                getNextGroupItem( group);
                groupCount--;
            }
            break;
        }

        case GROUP_LOAD_REPEAT_PATTERN: //count color...
        {
            //Serial.println("GROUP LOAD REPEAT PATTERN event");
            int groupCount = (int)getNextGroupItem( group);
            CRGB groupPattern[NUM_LEDS];

            for (int i = 0; i < groupCount; i++)
            {
                groupPattern[i] = getNextGroupItem( group);
            }

            int j = 0;
            for (int i = 0; i < group->numLeds; i++)
            {
                {
                    ledStates[ group->members[i]].currentColor = groupPattern[ j];
                    j = (j+1) % groupCount;
                }
            }
            break;
        }

        case GROUP_RAINBOW:
        //fill_rainbow(leds, NUM_LEDS, 0, 20);
        //*pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue=5)
        {
            //Serial.println("GROUP RAINBOW event");
            int groupCount = group->numLeds;
            CRGB scratchLEDs[ NUM_LEDS];
            group->count = 0;
            fill_rainbow( scratchLEDs, groupCount, group->count, 5);
            for (int i = 0; i < groupCount; i++)
            {
                ledStates[ group->members[ i]].currentColor = scratchLEDs[ i];
            }
            break;
        }

        case GROUP_WHEEL:
        // spreading a color wheel across all of the LEDS
        {
            //Serial.println("GROUP WHEEL event");
            int groupCount = group->numLeds;
            for (int i = 0; i < groupCount; i++)
                {
                ledStates[i].currentColor = wheel( i * 255 / groupCount);
            }
            break;
        }

        // immediate group movements
        case GROUP_ROTATE_RIGHT: // speed?
            //Serial.println("GROUP ROTATE RIGHT event");
            groupRotateRight( group);
            break;


        case GROUP_ROTATE_LEFT: // speed?
            //Serial.println("GROUP ROTATE LEFT event");
            groupRotateLeft( group);
            break;

        case GROUP_ROTATE_RIGHT_COUNT: // count
        {
            //Serial.println("GROUP ROTATE RIGHT COUNT event");
            unsigned int groupCount = getNextGroupItem( group);
            for (unsigned int i = 0; i < groupCount; i++)
            {
                groupRotateRight( group);
            }
            break;
        }

        case GROUP_ROTATE_LEFT_COUNT: // count
        {
            //Serial.println("GROUP ROTATE LEFT COUNT event");
            unsigned int groupCount = getNextGroupItem( group);
            for (unsigned int i = 0; i < groupCount; i++)
            {
                groupRotateLeft( group);
            }
            break;
        }

        // delayed group movements
        case GROUP_TWINKLE:
            //Serial.println("GROUP TWINKLE event");
            // overly simplistic... only works one LED at a time
            //prime the pump
            group->onLED = random( group->numLeds);
            group->fromColor = ledStates[ group->onLED].currentColor;
            group->startTime = currentTime;
            group->timeToWait = 100;
            group->currentState = groupTwinklingSPF;
            break;

        case GROUP_BREATHE:
            //Serial.println("GROUP BREATHE event");
            // this maybe should have a breaths/minute parameter
#define bpm 10 /* breaths per minute. 12 to 16 for adult humans */
#define numBreathSteps 255
            group->breatheIndex = 0;
            group->breathStep = 255/numBreathSteps;
            group->startTime = currentTime;
            group->timeToWait = 60000/ bpm / numBreathSteps;
            group->currentState = groupBreathingSPF;
            break;

        case GROUP_CHASE_CW_PAIR: //from, dest, count, delay
            //Serial.println("GROUP CHASE CW PAIR event");
            group->fromLED = getNextGroupItem( group);
            group->toLED = getNextGroupItem( group);
            group->count = getNextGroupItem( group);
            group->onLED = group->fromLED;
            group->startTime = currentTime;
            group->timeToWait = getNextGroupItem( group);
            group->currentState = groupChasePairRightSPF;
            break;

        case GROUP_CLOCK: // colors?
            //Serial.println("GROUP CLOCK event");
            group->startTime = currentTime;
            group->timeToWait = CLOCK_EFFECT_TIME;
            group->currentState = groupClockSPF;
            break;

        case GROUP_LIFE: //
            //Serial.println("GROUP LIFE event");
            groupCheckLife( group);
            break;

        case GROUP_WAIT: // time
            //Serial.println("GROUP WAIT event");
            showLeds( true);
            group->timeToWait = getNextGroupItem( group);
            group->startTime = currentTime;
            Serial.printlnf("GROUP WAIT id:%d time:%d wait:%d start:%d",
                    group->groupNumber, millis(), group->timeToWait, group->startTime);
            group->currentState = groupWaitSPF;
            break;

        case GROUP_FADE_TO_BLACK: // time
        {
            //Serial.println("GROUP FADE TO BLACK event");
            unsigned int duration = getNextGroupItem( group);
            group->startTime = currentTime;
            group->fadeStepNumber = 0;
            group->fromColor = group->color; // may need to do individually
            group->toColor = CRGB::Black;
            group->fadeSteps = min( duration, 255);
            group->timeToWait = max( 1, group->fadeSteps/255);
            group->startTime = currentTime;
            group->currentState = groupFadingSPF;
            break;
        }

        case GROUP_FADE_TO_COLOR: // time, color
        {
            //Serial.println("GROUP FADE TO COLOR event");
            int unsigned duration = getNextGroupItem( group);
            group->startTime = currentTime;
            group->fadeStepNumber = 0;
            group->fromColor = ledStates[ group->members[0]].currentColor;
            group->toColor = getNextGroupItem( group);
            group->color = group->color;
            group->startTime = currentTime;
            group->fadeSteps = min( duration, 255);
            group->timeToWait = max( 1, group->fadeSteps/255);
            group->currentState = groupFadingSPF;
            break;
        }

        case GROUP_WAVE:
            {
                //Serial.println("GROUP WAVE event");
                int groupCount = group->numLeds;
                CRGB scratchLEDs[ NUM_LEDS];
                group->waveCount = group->waveCount + 1;
                if (group->waveCount > 255)
                {
                    group->waveCount = 0;
                }
                //Serial.printf("GROUP WAVE count:\r\n", group->waveCount);
                fill_rainbow( scratchLEDs, groupCount, group->waveCount, 5);
                for (int i = 0; i < groupCount; i++)
                {
                    ledStates[ group->members[ i]].currentColor = scratchLEDs[ i];
                }
                group->startTime = currentTime;
                break;
            }


        // control functions
/*
        case GROUP_SET_GROUP:
        {
            //Serial.println("GROUP SET GROUP event");
            // no error testing. was it a good number?
            int groupNumber = getNextGroupItem( group);
            group = &ledGroupStates[groupNumber];
        }
            //Serial.println("group is now group: %d\r\n", group->groupNumber);
            break;
*/


        case GROUP_REPEAT_FOREVER:
            //Serial.println("GROUP REPEAT FOREVER event");
            // no error testing. was it a good number?
            group->repeatIndex = group->currentCommandIndex;
            group->count = -1;
            break;

        case GROUP_REPEAT_COUNT: // number
            //Serial.println("GROUP REPEAT COUNT event");
            group->count = getNextGroupItem( group);
            // no error testing. was it a good number?
            group->repeatIndex = group->currentCommandIndex;
            break;

        case GROUP_REPEAT_END:
            //Serial.println("GROUP REPEAT END event");
            // no error testing. was it initialized, was it forever
            if (group->count != -1)
            {
                group->count = group->count - 1;
            }
            if (group->count == -1 || group->count > 0)
            {
                group->currentCommandIndex = group->repeatIndex;
            }
            break;

        case GROUP_GCOLOR: //group, color
        {
            //Serial.println("GROUP GCOLOR event");
            int groupNumber = getNextGroupItem( group);
            ledGroupStates[groupNumber].color = (CRGB) getNextGroupItem( group);
            break;
        }

        case GROUP_GCOLOR_RANDOM: //group
        {
            //Serial.println("GROUP GCOLOR event");
            int groupNumber = getNextGroupItem( group);
            ledGroupStates[groupNumber].color.r = random (255);
            ledGroupStates[groupNumber].color.g = random (255);
            ledGroupStates[groupNumber].color.b = random (255);
            break;
        }

        case GROUP_GFORK: // group, *commandArray
        {
            // this doesn't affect any other process, so it you want to stop
            // a group, you must GROUP_GFORK a commandArray with a GROUP_STOP
            //Serial.println("GROUP GFORK event");
            int forkGroup = getNextGroupItem( group);
            int *commandArray = (int*)getNextGroupItem( group);
            ledGroupStates[ forkGroup].timeToWait = 1; // enable target timer
            ledGroupStates[ forkGroup].startTime = currentTime;
            ledGroupStates[ forkGroup].currentCommandArray = commandArray;
            ledGroupStates[ forkGroup].currentCommandIndex = 0;
            interpretGroupCommand ( &ledGroupStates[ forkGroup], commandArray,
                    currentTime);
            break;
        }

        case GROUP_SET_ALL: //
        {
            //Serial.println("GROUP SET ALL event");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                group->members[i] = i;
            }
            group->numLeds = NUM_LEDS;
            break;
        }

        case GROUP_SET_MEMBERS: // count, members
        {
            //Serial.println("GROUP SET MEMBERS event");
            int count = getNextGroupItem( group);
            group->numLeds = count;
            for (int i = 0; i < count; i++)
            {
                group->members[i] = getNextGroupItem( group);
            }
            break;
        }

        case GROUP_SET_MASK: // pointer to 8 32-bit numbers
        {
            // set the members of the selected group from a bit mask array
            int member = 0;
            uint32_t *mask = (uint32_t*)getNextGroupItem( group);
            String members = "";
            char membersCA [100 + 1];
            Serial.printlnf("%d GSM mask:%d", millis(), mask);
            for (int i = 0; i < 8; i++) // every row
            {
                // convert bit position to LED number
                Serial.printlnf("%d GSM row:%d mask:%ld", millis(), i, *mask);
                members = "GSM members:";
                for (int j = 0; j<32; j++) { // every bit in row
                    if (*mask & 1<<j) {
                        group->members[member] = i*32 + j;
                        members = members + String (" ") + String (i*32 + j);
                        Serial.printlnf("GSM: mask:%d member:%d", *mask, i*32 +j);
                        member = member + 1;
                    }
                }
                members.toCharArray(membersCA, 100);
                Serial.println( membersCA);
                mask++;
            }
            group->numLeds = member;
            for (int i = 0; i < group->numLeds; i++) // every member
            {
                Serial.printlnf("GSM: member:%d %d", i, group->members[i] );
            }
            break;
        }

        case GROUP_PHASE_DELAY: // millseconds
        {
            //Serial.println("GROUP PHASE DELAY event");
            int delay = getNextGroupItem( group);
            for (int i = 0; i < group->numLeds; i++)
            {
                ledStates[ group->members[i]].timeToWait = i * delay + 1;
                //Serial.printlnf("GPD i:%d led:%d, delay:%d",
                //			i, group->members[i] ,(i+1)*delay);
            }
            break;
        }

        case GROUP_LFORK: // ledNumber, *commandArray
        {
            // this doesn't affect any other process, so it you want to stop
            // an LED, you must GROUP_LKILL or GROUP_LFORK a commandArray with a LED_STOP
            //Serial.println("GROUP LFORK event");
            int ledNumber = getNextGroupItem( group);
            int *commandArray = (int*)getNextGroupItem( group);
            ledStates[ ledNumber].currentCommandArray = commandArray;
            ledStates[ ledNumber].currentCommandIndex = 0;
            if (ledStates[ ledNumber].timeToWait == 0)
            {
                ledStates[ ledNumber].timeToWait = 1;
            }
            ledStates[ ledNumber].currentState = ledWaitSPF;
            ledStates[ ledNumber].startTime = currentTime;
            break;
        }

        case GROUP_LFORK_ALL: // *commandArray
        { // this doesn't affect any other process, so it you want to stop
            // an LED, you must GROUP_LKILL or GROUP_LFORK a commandArray with a LED_STOP
            //Serial.printlnf("GROUP LFORK ALL event num%d", group->numLeds);
            int *commandArray = (int*)getNextGroupItem( group);
            for (int i = 0; i < group->numLeds; i++)
            {
                ledStates[ group->members[i]].currentCommandArray = commandArray;
                ledStates[ group->members[i]].currentCommandIndex = 0;
                if (ledStates[ group->members[i]].timeToWait == 0)
                {
                    ledStates[ group->members[i]].timeToWait = 1;
                }
                ledStates[ group->members[i]].currentState = ledWaitSPF;
                ledStates[ group->members[i]].startTime = currentTime;
                /*
                Serial.printlnf("GLK ALL i:%d led:%d start:%d wait:%d",
                        i,
                        group->members[i],
                        ledStates[ group->members[i]].startTime,
                        ledStates[ group->members[i]].timeToWait
                        );
                */
            }
            break;
        }

        case GROUP_LKILL: // ledNumber
        {
            int ledNumber = getNextGroupItem( group);
            //Serial.printlnf("GROUP LKILL event led:%d", ledNumber);
            ledStates[ ledNumber].timeToWait = 0; // disable timer
            ledStates[ ledNumber].currentState = ledNullSPF;
            break;
        }

        case GROUP_LKILL_ALL: //
        {
            //Serial.printlnf("GROUP LKILL ALL event num%d", group->numLeds);
            for (int i = 0; i < group->numLeds; i++)
            {
                ledStates[ group->members[i]].timeToWait = 0; // disable timer
                ledStates[ group->members[i]].currentState = ledNullSPF;
            }
            break;
        }

        case GROUP_STOP: // stop instead of looping
            //Serial.println("GROUP STOP event");
            showLeds( true); // show what lead up to the stop
            group->timeToWait = 0; // disable timer
            group->currentState = groupStopSPF;
            break;

        case GROUP_STOP_ALL: // stop all groups
            stopAllLedGroupFSMs();
            break;


        default:
            Serial.printlnf( "unexpected GROUP event: %d %d",
                    group->groupNumber, event);
            break;
    }
}


void setLedMode( int group, int mode)
/*
 * this function changes the current mode of the LED display and sets up
 * the effect(s) to be applied over time.
 * This is normally called initially and by a web page via a particle.function call


it sure looks like most of this function can be replaced with a table. index by mode or effect number and provide a pointer to an array. Question is how to extract the length of the array. Maybe that is just another column in the table...

a better simplification is to add a known marker to end of a command string
when you hit it, you either stop, LED_STOP, GROUP_STOP, or loop, LED_LOOP, GROUP_LOOP.
 */
{
    //Serial.println(ledModeNames[mode]);
    //Serial.println(ledModeNames[mode].length());
    char modeName [ledModeNames[mode].length() + 1]; //include terminating null
    ledModeNames[ mode].toCharArray(modeName, ledModeNames[mode].length()+1);\
    Serial.printf("attempting LED mode change %d %s\r\n", mode, modeName);

    //stopAllLedFSMs();
    //stopAllLedGroupFSMs(); // this isn't right, want to control each group independently
    ledGroupStates[group].timeToWait = 0; //cancel timer
    /*
       the long term stopping mechanism should have a scope of control.
       you get a scope of control to control either the group as a whole or individuals within
       that scope.

       not sure how the scope is defined and allocated, but maybe it is with a group mechanism.
     */
    TIME now = millis();

    ledMode = (ledModes) mode; // assume this works
    switch(  mode)
    {
        case offMode:
            //Serial.println("LED mode changed to offEffect");
            interpretGroupCommand ( &ledGroupStates[ group], offGC, now);
            break;

        case demo1:
            Serial.println("LED mode changed to demo1");
            interpretGroupCommand ( &ledGroupStates[ 0], demo1GC, now);
            break;

        case demo2:
            Serial.println("LED mode changed to demo2");
            interpretGroupCommand ( &ledGroupStates[ 0], demo2GC, now);
            break;

        case demo3:
            Serial.println("LED mode changed to demo3");
            interpretGroupCommand ( &ledGroupStates[ 0], demo3GC, now);
            break;

        case demo4:
            Serial.println("LED mode changed to demo4");
            interpretGroupCommand ( &ledGroupStates[ 0], demo4GC, now);
            break;

        case demo5:
            Serial.println("LED mode changed to demo5");
            interpretGroupCommand ( &ledGroupStates[ 0], demo5GC, now);
            break;

        case demo6:
            Serial.println("LED mode changed to demo6");
            interpretGroupCommand ( &ledGroupStates[ 0], demo5GCM, now);
            break;

        case singleColor:
            //Serial.println("LED mode changed to singleColor");
            interpretGroupCommand ( &ledGroupStates[ group], singleColorGC, now);
            break;

        case staticMultiColor:
            //Serial.println("LED mode changed to staticMultiColor");
            interpretGroupCommand ( &ledGroupStates[ group], multiColorGC, now);
            break;

        case twinklingMultiColor:
            //Serial.println("LED mode changed to twinklingMultiColor");
            interpretGroupCommand ( &ledGroupStates[ group], multiColorTwinkleGC, now);
            break;

        case allFlash:
            //Serial.println("LED mode changed to allFlash");
            interpretGroupCommand ( &ledGroupStates[ group], allFlashGC, now);
            break;

        case quadFlasher:
            //Serial.println("LED mode changed to quadFlasher");
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC, now);
            break;

        case quadFlasher2:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC2, now);
            break;

        case quadFlasher3:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC3, now);
            break;

        case quadFlasher4:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC4, now);
            break;

        case quadFlasher5:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC5, now);
            break;

        case quadFlasher6:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC6, now);
            break;

        case quadFlasher7:
            interpretGroupCommand ( &ledGroupStates[ group], quadFlashGC7, now);
            break;

        case octaFlasher:
            interpretGroupCommand ( &ledGroupStates[ group], octaFlashGC, now);
            break;

        case octaFlasher2:
            interpretGroupCommand ( &ledGroupStates[ group], octaFlashGC2, now);
            break;

        case circulator:
            //Serial.printf("LED mode changed to circulator %d\r\n", millis());
            interpretGroupCommand ( &ledGroupStates[ group], circulatorGC, now);
            break;

        case pendulum:
            interpretGroupCommand ( &ledGroupStates[ group], pendulumGC, now);
            break;

        case staticRainbow:
            interpretGroupCommand ( &ledGroupStates[ group], rainbowGC, now);
            break;


        case staticWheel:
            interpretGroupCommand ( &ledGroupStates[ group], wheelGC, now);
            break;

        case colorWave:
            interpretGroupCommand ( &ledGroupStates[ group], waveGC, now);
            break;

        case clockFace:
            interpretGroupCommand ( &ledGroupStates[ group], clockGC, now);
            break;

        case marqueeLeft:
            //Serial.println("LED mode changed to marquee left");
            interpretGroupCommand ( &ledGroupStates[ group], marqueeLeftGC, now);
            break;

        case marqueeRight:
            //Serial.println("LED mode changed to marquee right");
            interpretGroupCommand ( &ledGroupStates[ group], marqueeRightGC, now);
            break;

        case breathe:
            //Serial.println("LED mode changed to breathe");
            interpretGroupCommand ( &ledGroupStates[ group], breatheGC, now);
            break;

        case flashSynched:
            //Serial.println("LED mode changed to flashSynched");
            // use interpreted flash sequence
            for (int i = 0; i < NUM_LEDS; i++)
            {
                startLedFSM(&ledStates[i], flash250);
            }
            break;

        case flashPhased:
            //Serial.println("LED mode changed to flashPhased");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledStates[i].currentState = ledWaitSPF;
                ledStates[i].currentCommandArray = flash250;
                ledStates[i].currentCommandIndex = 0;
                ledStates[i].timeToWait = i * 250/24 + 1;
                ledStates[i].startTime = now;
            }
            break;

        case winkSynched:
            //Serial.println("LED mode changed to winkSynched");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                startLedFSM(&ledStates[i], wink);
            }
            break;

        case winkPhased:
            //Serial.println("LED mode changed to winkPhased");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledStates[i].currentState = ledWaitSPF;
                ledStates[i].currentCommandArray = wink;
                ledStates[i].currentCommandIndex = 0;
                ledStates[i].timeToWait = i * 30 + 1;
                ledStates[i].startTime = now;
            }
            break;

        case blinkSynched:
            //Serial.println("LED mode changed to blinkSynched");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                startLedFSM(&ledStates[i], blink);
            }
            break;

        case blinkPhased:
            //Serial.println("LED mode changed to blinkPhased");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledStates[i].currentState = ledWaitSPF;
                ledStates[i].currentCommandArray = blink;
                ledStates[i].currentCommandIndex = 0;
                ledStates[i].timeToWait = i * 30 + 1;
                ledStates[i].startTime = now;
            }
            break;

        case meteorSynched:
            //Serial.println("LED mode changed to meteorSynched");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                startLedFSM(&ledStates[i], meteor);
            }
            break;

        case meteorPhased:
            //Serial.println("LED mode changed to meteorPhased");
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledStates[i].currentState = ledWaitSPF;
                ledStates[i].currentCommandArray = meteor;
                ledStates[i].currentCommandIndex = 0;
                ledStates[i].timeToWait = i * 30 + 1;
                ledStates[i].startTime = now;
            }
            break;

        default:
            ledMode = offMode; // not quite right, should save entry value
            Serial.println("bad attempt to change LED mode");
            break;
    }
}

/* try to use group logic instead...
void interpretDirectorCommand( directorCommand dirCommand)
{
    TIME startTime = millis(); // time upon entering the current state
    int count;
    int delay;
    int functionIndex;
    Serial.printlnf("iDC time:%d gr:%d now:%d",
        millis(),
        ledGroupStates[selectedGroup]->groupNumber,
        dirCommand);
    switch (dirCommand)
    {
        case GROUP_GCOLOR: //color
        {
            CRGB color = getNextGroupItem( selectedGroup);
            ledGroupStates[selectedGroup]->color = color;
            break;
        }

        case GROUP_GFORK: // group, *commandArray
        {
            // this doesn't affect any other process, so it you want to stop
            // a group, you must GROUP_GFORK a commandArray with a GROUP_STOP
            int group = getNextDirectorCommand( selectedGroup);
            int *commandArray = getNextDirectorCommand( selectedGroup);
            interpretGroupCommand ( &ledGroupStates[ group], commandArray, now);
            break;
        }

        case GROUP_SET_ALL: //
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledGroupStates[selectedGroup]->members[i] = i;
            }
            ledGroupStates[selectedGroup]->numLeds = NUM_LEDS;
            break;
        }

        case GROUP_SET_MEMBERS: // count, members
        {
            int count = getNextGroupItem( selectedGroup);
            ledGroupStates[selectedGroup]->numLeds = count;
            for (int i = 0; i < count; i++)
            {
                ledGroupStates[selectedGroup]->members[i] =
                        getNextGroupItem( selectedGroup);
            }
            break;
        }

        case GROUP_PHASE_DELAY: // count, millseconds
        {
            int count = getNextGroupItem( selectedGroup);
            int delay = getNextGroupItem( selectedGroup);
            for (int i = 0; i < selectedGroup->numLeds; i++)
            {
                ledStates[ ledGroupStates[selectedGroup]->members[i]].timeToWait = i * delay + 1;
            }
            break;
        }

        case GROUP_LFORK: // ledNumber, *commandArray
        {
            // this doesn't affect any other process, so it you want to stop
            // an LED, you must GROUP_LFORK a commandArray with a LED_STOP
            int ledNumber = getNextDirectorCommand( selectedGroup);
            int *commandArray = getNextDirectorCommand( selectedGroup);
            startLedFSM( &ledStates[ ledNumber], commandArray];
            break;
        }

        case GROUP_COLOR_SWAP:
            // really need to specify which two groups to swap colors.. current +1?
            CRGB color = ledGroupStates[selectedGroup]->color;
            ledGroupStates[selectedGroup]->color =
                    ledGroupStates[selectedGroup]->backgroundColor;
            ledGroupStates[selectedGroup]->backgroundColor = color;
            break;


        default:
            // includes GROUP_STOP
            break;
    }
}
*/

int interpretLedCommandString( String commandString)
/*
 * this function interprets a command string to control LED mode, color,
 * speed, direction, and brightness.
 *
 * The command string is sent by a web page to Particle.io which invoke this function
 */
{
    int value;
    int speed;
    int direction;
    String mode;
    int ptr;
    bool offSelected = false;
    #define lenTempStr 40
    char tempStr [ lenTempStr];
    //split command string into words
    /*
    single letter command for now
    n on
    f off
    bxxx brightness
    sxxx speed
    dxxx direction
    crrrbbbggg color
    rxxxgxxxbxxx RGb color
    Gxxx group
    mstring mode -- must be at the end, string is the name of a command
    */
    //Serial.print( "Interpreting commandString: ");
    //Serial.println( commandString);
    ptr = 0;
    char command = commandString.charAt( ptr);
    CRGB color;

    while (command > 0)
    {
        if (command != 'n' && command != 'f' && command != 'm') //not one of unary commands
        {
            value = 0;
            for (int i = ptr + 1; i < ptr + 4; i++)
            {
                if ( (int) commandString.charAt(i) == 0 )
                {
                    break;

                }
                else
                {
                    value = 10 * value + (int) commandString.charAt(i) - (int) '0';
                }
            }
            ptr = ptr + 4; // may skip over null
        }
        else
        {
            ptr = ptr + 1;
        }

        switch (command)
        {
            case 'G': //grop (group number)
                //Serial.println("Group");
                selectedGroup = value;
                if (value >= NUM_GROUPS)
                {
                    selectedGroup = 0;
                }
                break;

            case 'n': //on
                //Serial.println("on");
                //functionOnOff( 3, true, "");
                break;

            case 'f': //off
                //Serial.println("off");
                //functionOnOff( 3, false, "");
                offSelected = true;
                break;

            case 'l': //luminence (brightness)%
                if (value > 100)
                {
                    value = 100;
                }
                ledBrightness = value; //percent
                FastLED.setBrightness( (int) (ledBrightness*255/100));
                snprintf( tempStr, lenTempStr, "Brightness: %d", ledBrightness);
                //Serial.println(tempStr);
                break;

            case 'm': //mode  .. is this a string or a value --string for now
                /*
                if (value > 100)
                {
                    value = 100;
                }
                mode = value;
                */
                //result = snprintf( mode, sizeof(mode), "%s", commandString);
                mode = "Mode: " + commandString.substring(ptr); // just the rest of the string
                //snprintf( tempStr, lenTempStr, "Mode: %s", mode);
                //Serial.println(mode);
                for (int i = 0; i <  NUM_LED_MODES; i++)
                {
                    if ( commandString.substring(ptr).compareTo(ledModeNames[i]) == 0)
                    {
                        if (offSelected)
                        {
                            //Serial.println("Off selected");
                            ledBrightness = 0;
                            FastLED.setBrightness( (int) (ledBrightness*255/100));
                        }
                        //No error testing for the following??? in general this whole function sets a
                        //bunch of globals. Not exactly best practice.
                        ledGroupStates[selectedGroup].color = color;
                        ledColor = color; // default for individual LEDs
                        setLedMode( selectedGroup, i);
                        showLeds( true); //is this necessary or desired?
                        break;

                    }
                }
                ptr = commandString.length();// mode must be the last command
                break;

            case 's': //speed 0..200: -100..100
                if (value > 200)
                {
                    value = 200;
                }
                speed = value-100;
                snprintf( tempStr, lenTempStr, "Speed: %d", speed);
                //Serial.println(tempStr);
                break;

            case 'd': //direction 0..359
                if (value > 359)
                {
                    value = 0;
                }
                direction = value;
                snprintf( tempStr, lenTempStr, "Direction: %d", direction);
                //Serial.println(tempStr);
                break;

            case 'r': //red 0..255
                if (value > 255)
                {
                    value = 255;
                }
                color.r = value;
                snprintf( tempStr, lenTempStr, "Red: %d", value);
                //Serial.println(tempStr);
                break;

            case 'b': //b 0..255
                if (value > 255)
                {
                    value = 255;
                }
                color.b = value;
                snprintf( tempStr, lenTempStr, "Blue: %d", value);
                //Serial.println(tempStr);
                break;

            case 'g': //green 0..255
                if (value > 255)
                {
                    value = 255;
                }
                color.g = value;
                snprintf( tempStr, lenTempStr, "Green: %d", value);
                //Serial.println(tempStr);
                break;

            // w -speed
            // x +speed
            // y -dir
            // z +dir
            default:
                return -1; //bad command
                break;
        }
        command = commandString.charAt( ptr);
    }

    //Serial.println("Done");
    return 0; // good command
}


void ledSetup ()
// setup of the LED module
{
    Time.zone(-5); //EST
    FastLED.addLeds<WS2812, FASTLED_PIN, GRB>(leds, NUM_LEDS);

    setupAllLedFSMs();
    stopAllLedFSMs();

    setupAllLedGroupFSMs();
    stopAllLedGroupFSMs();

    // setup for web controls
    Particle.function ("ledControl", interpretLedCommandString);

    // setup group 0
    ledGroupStates[0].numLeds = NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        ledGroupStates[0].members[i] = i;
    }
    setLedMode( 0, demo4);
}



TIME lastPublish;

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial monitor started");

    ledBrightness = 1; //percent
    FastLED.setBrightness( (int) (ledBrightness*255/100));

    ledSetup();
    lastPublish = millis();
}

void loop()
{
    //groupFSM(); // handle group LED effects
    //ledFSM(); // handle individual LED effects


    LedState state;
    TIME now = millis();
    TIME last = millis();
    bool changed = false;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        // the following is just to working around millis() jumping around
        // due to interference with FastLED
        now = millis();
        while (now - last > 1000) {
            delayMicroseconds (100);
            now = millis();
        }
        last = now;

        state = ledStates[i];
        if (state.timeToWait > 0 &&
                now - state.startTime > state.timeToWait )
        {
            state.currentState(&ledStates[i], LED_TIME_OUT, now);
            changed = true; // just assume that something changed on event
        }
    }

    LedGroupState group;
    for (int i = 0; i < NUM_GROUPS; i++)
    {
        // the following is just to working around millis() jumping around
        // due to interference with FastLED
        now = millis();
        while (now - last > 1000) {
            delayMicroseconds (100);
            now = millis();
        }
        last = now;

        group = ledGroupStates[i];
        if (group.timeToWait > 0 &&
                now - group.startTime > group.timeToWait )
        {
            //Serial.printlnf("loop time:%d id:%d now:%d",
            //        millis(), group.groupNumber, now);
            group.currentState(&ledGroupStates[i], GROUP_TIME_OUT, now);
            changed = true; // just assume that something changed on event
        }
    }

    showLeds( changed);


    if (client.isConnected()) {
        client.loop();
        if (now - lastPublish > 30 * 1000) { // only want to do this every thirty seconds
            // or when something changes
            publish ( String( HOSTNAME) + String( "/mode"),  String( ledMode, DEC));
            publish ( String( HOSTNAME) + String( "/brightness"),  String( ledBrightness, DEC));
            publish ( String( HOSTNAME) + String( "/direction"),  String( ledDirection, DEC));
            publish ( String( HOSTNAME) + String( "/color/r"),  String( ledColor.r, DEC));
            publish ( String( HOSTNAME) + String( "/color/g"),  String( ledColor.g, DEC));
            publish ( String( HOSTNAME) + String( "/color/b"),  String( ledColor.b, DEC));
            lastPublish = now;
        }
    } else {
        // connect to the server
        Serial.println("Attempting to connect to MQTT broker again");
        client.connect(HOSTNAME);
        delay(1000);

        // subscribe to all commands at once with wild card
        String topic = String( HOSTNAME) + String( "/command/#");
#define TOPIC_LEN 30
        char subscribeTopic[TOPIC_LEN];

        topic.toCharArray(subscribeTopic, TOPIC_LEN);
        client.subscribe( subscribeTopic);
    }
}
