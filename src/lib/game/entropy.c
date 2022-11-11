//***************************************************************************************
//  PETI FOSS Virtual Pet - Entropy/Random Number Generations Code
//
//  Manages and exposes the PRNG used by various elements of the game.
//
//  Zac Adam-MacEwen (Arcana Labs)
//  August 2022
//***************************************************************************************


#include <msp430.h>
#include <math.h>
#include "driverlib.h"
#include "main.h"
#include "entropy.h"


// The PRNG at the heart of the entropy package is a simplistic LCG with an apparent period of roughly 30.
// The enterprising could get better or worse performance simply by adjusting these numbers.
const float multiplier = 214013; // term a
const float modulus = 2147483648;
const float carrier = 2531011; // term c
unsigned int increment;

// Calls the ADC in order to randomly select a float from out of the Device Descriptor ROM.
// These device descriptors are nominally per-controller trandom per TI.
// In practical effect this seems to almost always give one result, very rarely giving a second, radically different result.
float RNG_getSeedWord(void) {
  //Initialize the ADC
    int seed_target;
    float seed_result;
    ADC12_B_initParam initParam= {0};
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = ADC12_B_MAPINTCH0;
    ADC12_B_init(ADC12_B_BASE, &initParam);
    ADC12_B_enable(ADC12_B_BASE);
    ADC12_B_setupSamplingTimer(ADC12_B_BASE,
                               ADC12_B_CYCLEHOLD_4_CYCLES,
                               ADC12_B_CYCLEHOLD_4_CYCLES,
                               ADC12_B_MULTIPLESAMPLESDISABLE);
    ADC12_B_configureMemoryParam configureMemoryParam = {0};
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    //ADC A9 is the MSP430FR5994 launchpad's pin 4.1. At time of writing this is a floating pin, so I'd expect some chaos
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_A9;
    // Uses VSS as a voltage reference, which may mean this gets more random on battery power (that'd be nice)
    configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_EXTPOS_VREFNEG_VSS;
    configureMemoryParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

  //Measure ADC sample
    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0, ADC12_B_SINGLECHANNEL);
    while(ADC12_B_isBusy(ADC12_B_BASE)); // Pause for it to finish its work
    seed_target = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);

  //Disable the ADC Block
    ADC12_B_disable(ADC12_B_BASE);

  //Mod 8 that sample
    seed_target %= 16; //Wanted: An integer between 0 and 15 that we can use as a memory offset.
    seed_target += 0x01A30; // treats as offset to the TLV memory addresses
    float *seed_pointer = (float *)seed_target; // Turn the darn thing into a pointer.

  //Fetch selected byte as a float from the TLV Random Number. This is specific to each individual device and is totally unknown without direct memory inspection.
    seed_result = *(seed_pointer);

  //Return it
    return seed_result;
}


// A simple function that takes an input seed then passes it through the LCG before setting the state variable. This could be used to
// Reset the RNG in future if so desired.
void RNG_initialize(float initial_seed){
    RNG_session_seed = initial_seed;
    RNG_current_state = fmodf((multiplier * RNG_session_seed + carrier), modulus);
}

// Called once per program loop to update the RNG state. In doing this you make the RNG more unpredictable in spite of its low period.
// It would take some speedrunner-style knack in order to keep track of these for RNG manipulation as they're called at irregular intervals.
void RNG_forceShuffle(void){
    RNG_current_state = fmodf((multiplier * RNG_current_state + carrier), modulus);
}

// A randomization primitive, returning a float normalled between 0 and 1 when called, and incrementing the RNG state.
// This could be used in a later function to handle probabilistic pass fail or used raw for other situations.
float RNG_drawFloat(void){
    RNG_current_state = fmodf((multiplier * RNG_current_state + carrier), modulus);
    float rand;
    rand = RNG_current_state / 0xFFFFFFFF;
    return rand;
}
