/**
  ******************************************************************************
  * File Name          : average_acc.h
  * Description        : Averaging data accumulator
  * Author             : Giorgio Croci Candiani - 2018
  ******************************************************************************
*/
#include "average_acc.h"

AverageAcc::AverageAcc(void)
: accum(0), count(0), avgVal(0), length_l2(5)
{
}

AverageAcc::AverageAcc(uint8_t log2width)
: accum(0), count(0), avgVal(0)
{
    // length_l2 = (log2width > 16 ? 16 : log2width);
    setLength(log2width);
}

void  
AverageAcc::setLength(uint8_t log2width)
{
    length_l2 = (log2width > 16 ? 16 : log2width);
}

uint16_t
AverageAcc::addVal(uint16_t val)
{
    uint16_t res;
    if(ready()) {
        avgVal = accum_avg();
        accum -= avgVal;
        res = accum_avg();
    } else {
        ++count;
        res = avgVal = accum / count;
    }
    accum += val;
    return res;
}


// END average_acc.cpp
