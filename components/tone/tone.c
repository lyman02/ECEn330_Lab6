#include <stdlib.h>
#include <math.h>
#include "tone.h"

#define MIN_FREQ (2*LOWEST_FREQ)
#define BIAS 0x80
#define AMPLITUDE 0x7F
#define PEAK 0xFF
#define TWO_PI (M_PI+M_PI)
#define DIVIDE_TWO(n) (n)/2
#define DIVIDE_FOUR(n) (n)/4
#define MULTIPLY_THREE(n) (n)*3
#define MULTIPLY_FOUR(n) (n)*4


static uint32_t bufferSize;
static uint8_t* toneBuffer;
static uint32_t dacRate;

//Initialize tone for the speaker
int32_t tone_init(uint32_t sample_hz){
    //If the sample hz are less than the frequency then return -1 signifying an error
    if(sample_hz < MIN_FREQ){
        return -1;
    }
    dacRate = sample_hz;
    sound_init(sample_hz);
    bufferSize = sample_hz / LOWEST_FREQ;
    toneBuffer = (uint8_t*)malloc(bufferSize);

    //If the toneBuffer is empty then return -1 to signify error
    if(toneBuffer == NULL){
        return -1;
    }
    
    return 0;
}


//De-initialize tone when finished
int32_t tone_deinit(void){
    //If the tone buffer isn't empty then free it and reset global variables.
    if(toneBuffer != NULL){
        free(toneBuffer);
        toneBuffer = NULL;
        bufferSize = 0;
        sound_deinit();
        return 0;
    }else{
        return -1;
    }
}


//Select tones for the speaker to output
void tone_start(tone_t tone, uint32_t freq){
    //Make sure the tone is within the usable states and that the frequency is less than the lowest frequency that's predefined
    if(tone >= LAST_T || (freq < LOWEST_FREQ)){
        return;
    }

    uint32_t samples = dacRate / freq;
    int i = 0;

    //Select different waveforms depending on the state variable: tone
    switch(tone){
        //Sine waveform
        case SINE_T:
            //Set the tone at each sample, i, to be a value of sine at i
            for(i = 0; i < samples; i++){
                toneBuffer[i] = BIAS + (AMPLITUDE * sinf(TWO_PI*(float)i/samples));
            }
            break;
        //Square waveform
        case SQUARE_T:
             //Set the tone at each sample, i, to be a value of a square wave at i
            for(i = 0; i < DIVIDE_TWO(samples); i++){ 
                toneBuffer[i] = BIAS + AMPLITUDE;
            }
            for(i = DIVIDE_TWO(samples); i < samples; i++){
                toneBuffer[i] = BIAS - AMPLITUDE;
            }
            
            break;
        //Triangle waveform
        case TRIANGLE_T:
            //Set the tone at each sample, i, to be a value of a triangle wave at i
            for(i = 0; i < DIVIDE_FOUR(samples); i++)
                toneBuffer[i] = BIAS + ((float)i/DIVIDE_FOUR(samples)*AMPLITUDE);
            for(    ; i < DIVIDE_FOUR(MULTIPLY_THREE(samples)); i++)
                toneBuffer[i] = BIAS + AMPLITUDE + ((float)(DIVIDE_FOUR(samples)-i)/DIVIDE_FOUR(samples)*AMPLITUDE);
            for(    ; i < samples; i++)
                toneBuffer[i] = BIAS - AMPLITUDE + ((float)(i - DIVIDE_FOUR(MULTIPLY_THREE(samples)))/DIVIDE_FOUR(samples)*AMPLITUDE);
            break;

        //Sawtooth waveform
        case SAW_T:
            //Set the tone at each sample, i, to be a value of a sawtooth wave at i
            for(i = 0; i < DIVIDE_TWO(samples); i++){
                toneBuffer[i] = BIAS + ((float)i/DIVIDE_TWO(samples)*AMPLITUDE);
            }

            for(    ; i < samples; i++){
                toneBuffer[i] = BIAS - AMPLITUDE + ((float)(i - DIVIDE_TWO(samples))/DIVIDE_TWO(samples)*AMPLITUDE);
            }
            break;

        default:
            break;
    }
    sound_cyclic(toneBuffer, samples);
}
