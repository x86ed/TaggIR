#pragma once

#include <stdlib.h>

const uint8_t PP4StSeq = 0x84;
const uint8_t PP16StSeq = 0x85;

#define PULSES_PP4C 50
#define PULSES_PP16 26
#define FRAME_MAX_LENGTH 58

// Specific to Pricehax
#define PHX_CODE_PP4C 170
#define PHX_CODE_PP16 171

uint8_t gData[FRAME_MAX_LENGTH], gDataLength;
uint16_t gRepeat = 0;
bool gModePP16 = false, gSendFrame = false, gSentOk = false;

uint8_t gDataTemp[FRAME_MAX_LENGTH], gDataLengthTemp;
uint16_t gRepeatTemp = 0;
bool gModePP16Temp = false;

const uint8_t PP16Header[4] = {
    0x00,
    0x00,
    0x00,
    0x40,
};

const uint8_t PP16Key[2] = {
    0x00,
    0x00
};

const uint8_t PP16Payload[23] = {
    0x00,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
};

// Commands
const uint8_t PP16ComWakeUp[2] = {
    0x17,
    0x01,
};

const uint8_t PP16ComImgUpdate0[5] = {
    0x34,
    0x00,
    0x00,
    0x00,
    0x05,
};

const uint8_t PP16ComImgUpdate1[7] = {
    0x88,
    0x00,
    0x00, 
    0x00, 
    0x00, 
    0x00,
    0x00,
};

const uint8_t PP16ComImgData[5] = {
    0x34,
    0x00,
    0x00,
    0x00,
    0x20,
};

const uint8_t PP16ComImgEnd[5] = {
    0x34,
    0x00,
    0x00,
    0x00,
    0x01,
};

// TODO: timing section should be used elsewhere
// The timing offsets (especially for PP16) may need to be adjusted again if another uC is used
const uint8_t gOffPP4C = -5;
const uint8_t gOffPP16 = -5;

// PP4C symbols timings
const uint8_t gPP4C[4] = {
     61 + gOffPP4C,
    244 + gOffPP4C,
    122 + gOffPP4C,
    183 + gOffPP4C
};

// PP16 symbols timings
const uint8_t gPP16[16] = {
     27 + gOffPP16,
     51 + gOffPP16,
     35 + gOffPP16,
     43 + gOffPP16,
    147 + gOffPP16,
    123 + gOffPP16,
    139 + gOffPP16,
    131 + gOffPP16,
     83 + gOffPP16,
     59 + gOffPP16,
     75 + gOffPP16,
     67 + gOffPP16,
     91 + gOffPP16,
    115 + gOffPP16,
     99 + gOffPP16,
    107 + gOffPP16
};

// // Create a burst (40us for PP4C, 21us for PP16) at around 1.25MHz (16MHz/13 = 1.23MHz)
// // The number of assembly instructions of the whole loop once compiled is critical
// // If the function is changed, the timings will need to be adjusted by adding or removing NOPs
// void sendPPMBurst(uint8_t pulses) {
//     for(uint8_t i = 0; i < pulses; i++){
//         PORTD ^= (1 << LED_PIN);
//         NOP;
//         NOP;
//         PORTD ^= (1 << LED_PIN);
//         NOP;
//     }
// }



// // Send a frame using the PP4C protocol
// void sendPP4CFrame() {
//     uint8_t currentByte;
//     uint16_t repeat, symNumber, symCount;

//     // Get the number of PP4C symbols the frame contains
//     symCount = gDataLength << 2;
    
//     for (repeat = 0; repeat < gRepeat; repeat++) {
//         cli(); // Stop all interrupts to avoid added delay that will interfere with the PPM signal
//         for (symNumber = 0; symNumber < symCount; symNumber++) {

//             // Switch to the next byte once the 4 2-bit symbols of the current byte are transmitted
//             if ((symNumber & 3) == 0) {
//                 currentByte = gData[symNumber >> 2];
//             }
            
//             sendPPMBurst(PULSES_PP4C);
            
//             switch(currentByte & 3) {
//                 case 0:
//                     _delay_us(gPP4C[0]);
//                     break;
//                 case 1:
//                     _delay_us(gPP4C[1]);
//                     break;
//                 case 2:
//                     _delay_us(gPP4C[2]);
//                     break;
//                 case 3:
//                     _delay_us(gPP4C[3]);
//                     break;
//             }

//             // Switch to the next symbol
//             currentByte >>= 2;
//         }
        
//         sendPPMBurst(PULSES_PP4C);
//         sei(); // Allow interrupts
        
//         _delay_ms(2);
//     }
// }



// // Send a frame using the PP16 protocol
// void sendPP16Frame() {
//     uint8_t currentByte;
//     uint16_t repeat, symNumber, symCount;

//     // Get the number of PP16 symbols the frame contains
//     symCount = gDataLength << 1;
    
//     for (repeat = 0; repeat < gRepeat; repeat++) {
//         cli(); // Stop all interrupts to avoid added delay that will interfere with the PPM signal
//         for (symNumber = 0; symNumber < symCount; symNumber++) {

//             // Switch to the next byte once the 2 4-bit symbols of the current byte are transmitted
//             if ((symNumber & 1) == 0) {
//                 currentByte = gData[symNumber >> 1];
//             }
            
//             sendPPMBurst(PULSES_PP16);
            
//             switch(currentByte & 15) {
//                 case 0:
//                     _delay_us(gPP16[0]);
//                     break;
//                 case 1:
//                     _delay_us(gPP16[1]);
//                     break;
//                 case 2:
//                     _delay_us(gPP16[2]);
//                     break;
//                 case 3:
//                     _delay_us(gPP16[3]);
//                     break;
//                 case 4:
//                     _delay_us(gPP16[4]);
//                     break;
//                 case 5:
//                     _delay_us(gPP16[5]);
//                     break;
//                 case 6:
//                     _delay_us(gPP16[6]);
//                     break;
//                 case 7:
//                     _delay_us(gPP16[7]);
//                     break;
//                 case 8:
//                     _delay_us(gPP16[8]);
//                     break;
//                 case 9:
//                     _delay_us(gPP16[9]);
//                     break;
//                 case 10:
//                     _delay_us(gPP16[10]);
//                     break;
//                 case 11:
//                     _delay_us(gPP16[11]);
//                     break;
//                 case 12:
//                     _delay_us(gPP16[12]);
//                     break;
//                 case 13:
//                     _delay_us(gPP16[13]);
//                     break;
//                 case 14:
//                     _delay_us(gPP16[14]);
//                     break;
//                 case 15:
//                     _delay_us(gPP16[15]);
//                     break;
//             }

//             // Switch to the next symbol
//             currentByte >>= 4;
//         }
        
//         sendPPMBurst(PULSES_PP16);
//         sei(); // Allow interrupts
        
//         _delay_ms(2);
//     }
// }