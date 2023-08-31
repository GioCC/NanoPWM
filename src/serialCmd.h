// =======================================================================
// @file        serialCmd.h
//
// @project     NanoPWM
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-27
// @modifiedby  GiorgioCC - 2023-08-31 21:04
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __SERIALCMD__H__
#define __SERIALCMD__H__

#include "main.h"

void processCmds(unsigned long now);

void printAllValues(void);

#endif  //!__SERIALCMD__H__
