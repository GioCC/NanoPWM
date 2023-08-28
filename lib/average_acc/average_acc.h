/**
  ******************************************************************************
  * File Name          : average_acc.h
  * Description        : Averaging data accumulator
  * Author             : Giorgio Croci Candiani - 2018
  ******************************************************************************
*/
#ifndef AVERAGE_ACC_INCLUDED
#define AVERAGE_ACC_INCLUDED

#include <stdint.h>

/// Efficient value accumulator for averaging
/// Accumulate passed values over a specified length
/// implementing a FIR filter equivalent to a 1st order analog filter
/// (classic 1-pole response)
/// Uses no buffer and no expensive math operations either.

class AverageAcc
{

private:
    uint32_t    accum;  // sum of all values
    uint16_t    count;
    uint16_t    avgVal;
    //uint16_t   length;
    uint8_t     length_l2;
    // Use log2(length)-1 for efficiency:
    // Nr of averaged samples = 2^(length_l2+1)
    // e.g. 1 => 4 samples,
    //      2 => 8 samples,
    //      3 => 16 samples,
    //      4 => 32 samples,
    //      7 => 256 samples etc

    /// Return the computed average value of the accumulator
    uint32_t accum_avg(void) { return ((accum + (1<<(length_l2-1)))>>(length_l2)); }

public:

    AverageAcc(void);
    explicit AverageAcc(uint8_t);

    void setLength(uint8_t);

    /// Return true if an accumulator has enough data for a sensible value
    bool        ready(void)   { return (count >= (uint16_t)(1<<(length_l2-1))); }

    /// Adds new value to accumulation
    uint16_t    addVal(uint16_t);

    /// Return the stored (current) average value of the accumulator
    uint16_t    average(void) { return avgVal; }
};

#endif  //AVERAGE_ACC_INCLUDED
