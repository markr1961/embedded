/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    eeprom.h
 *
 * Description:
 *    Expose public routines for communicating over I2C bus
 *
 *****************************************************************************/
#ifndef _EEPROM_H
#define _EEPROM_H

#include "i2c.h"

tS8 eepromWrite(tU16 addr,
                tU8* pData,
                tU16 len);

tS8 eepromPageRead(tU16 address, 
                   tU8* pBuf, 
                   tU16 len);

tS8 eepromPoll(void);

tS8 lm75Read(tU8 address, tU8* pBuf, tU16 len);
tS8 pca9532(tU8* pBuf, tU16 len, tU8* pBuf2, tU16 len2);

#endif
