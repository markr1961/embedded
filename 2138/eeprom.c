/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    eeprom.c
 *
 * Description:
 *    Implements routines for communicating over I2C bus
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../pre_emptive_os/api/general.h"
#include "i2c.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define EEPROM_SIZE       0x0800
#define LOCAL_EEPROM_ADDR 0x0
#define EEPROM_ADDR       0xA0

#define I2C_EEPROM_ADDR   (EEPROM_ADDR + (LOCAL_EEPROM_ADDR << 1))
#define I2C_EEPROM_RCV    (EEPROM_ADDR + (LOCAL_EEPROM_ADDR << 1) + 0x01)
#define I2C_EEPROM_SND    (EEPROM_ADDR + (LOCAL_EEPROM_ADDR << 1) + 0x00)


/******************************************************************************
 * Implementation of public functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Waits till I2C returns ACK (after BURN cycle)
 *
 *    Note: After a stop condition, you may need a bus free time before you 
 *          can generate a new start condition.
 *
 * Returns:
 *    I2C_CODE_OK or I2C_CODE_ERROR
 *
 *****************************************************************************/
tS8 
eepromPoll(void)
{
  tS8 retCode  = I2C_CODE_OK;
  tU8 status   = 0;
  tBool burnEnd = FALSE;

  while(burnEnd == FALSE)
  {
getI2cLock();
    /* Generate Start condition */
    retCode = i2cStart();

    /* Transmit SLA+W */
    if(retCode == I2C_CODE_OK)
    {
      /* Write SLA+W */
      retCode = i2cPutChar(I2C_EEPROM_SND);
      while(retCode == I2C_CODE_BUSY)
      {
        retCode = i2cPutChar(I2C_EEPROM_SND);
      }
    }

    if(retCode == I2C_CODE_OK)
    {
      /* Wait until SLA+W transmitted */
      /* Get new status */
      status = i2cCheckStatus();

      if(status == 0x18)
      {
        /* data transmitted and ACK received */
        burnEnd = TRUE;
      }
      else if(status == 0x20)
      {
        /* data transmitted and ACK not received */
        // send start bit, start again
        burnEnd = FALSE;
      }
      else if( status != 0xf8 )
      {
        /* error ---*/
        retCode = I2C_CODE_ERROR;
        burnEnd = TRUE;
      }
    }

    /* Generate Stop condition */
    i2cStop();
releaseI2cLock();

  } // end of while 


  return retCode;

}


/******************************************************************************
 *
 * Description:
 *    Start a read
 *
 * Params:
 *    [in] devAddr - device address
 *    [in] address - eeprom address
 *
 * Returns:
 *    I2C_CODE_OK or I2C_CODE_ERROR
 *
 *****************************************************************************/
tS8 
eepromStartRead(tU8  devAddr, 
                tU16 address)
{
  tS8 retCode = 0;
  tU8 status  = 0;

  /* Generate Start condition */
  retCode = i2cStart();

  /* Transmit address */
  if(retCode == I2C_CODE_OK )
  {
    /* Write SLA+W */
    retCode = i2cPutChar(devAddr | ((tU8)(address >> 7) & 0x0e));
//    retCode = i2cPutChar(devAddr);
    while( retCode == I2C_CODE_BUSY )
    {
//      retCode = i2cPutChar( devAddr );
      retCode = i2cPutChar( devAddr | ((tU8)(address >> 7) & 0x0e));
    }
  }

  if( retCode == I2C_CODE_OK )
  {
#if 0
    /* Wait until data transmitted */
    while(1)
    {
      /* Get new status */
      status = i2cCheckStatus();

      if( (status == 0x18) || (status == 0x28) )
      {
        /* Data transmitted and ACK received */


        /* Write data */
        retCode = i2cPutChar( (tU8)(address >> 8) );
        while(retCode == I2C_CODE_BUSY)
        {
          retCode = i2cPutChar( (tU8)(address >> 8) );
        }
        break;
      }
      else if(status != 0xf8)
      {
        /* error */
        retCode = I2C_CODE_ERROR;
        break;
      }
    }
#endif

    if(retCode == I2C_CODE_OK)
    {
      /* Wait until data transmitted */
      while(1)
      {
        /* Get new status */
        status = i2cCheckStatus();

        if( (status == 0x18) || (status == 0x28) )
        {
          /* Data transmitted and ACK received */


          /* Write data */
          retCode = i2cPutChar( (tU8)(address & 0xff) );
          while( retCode == I2C_CODE_BUSY )
          {
            retCode = i2cPutChar( (tU8)(address & 0xff) );
          }

          break;
        }
        else if( status != 0xf8 )
        {
          /*  error */
          retCode = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }

  /* Wait until data transmitted */
  while(1)
  {
    /* Get new status */
    status = i2cCheckStatus();

    if(status == 0x28)
    {
      /* Data transmitted and ACK received */
      break;
    }
    else if( status != 0xf8 )
    {
      /* error */
      retCode = I2C_CODE_ERROR;
      break;
    }
  }

  /* Generate Start condition */
  retCode = i2cRepeatStart();

  /* Transmit device address */
  if( retCode == I2C_CODE_OK)
  {
    /* Write SLA+R */
    retCode = i2cPutChar( devAddr + 0x01 );
    while( retCode == I2C_CODE_BUSY )
    {
      retCode = i2cPutChar( devAddr + 0x01 );
    }
  }

  /* Wait until SLA+R transmitted */
  while(1)
  {
    /* Get new status */
    status = i2cCheckStatus();

    if(status == 0x40)
    {
      /* Data transmitted and ACK received */
      break;
    }
    else if(status != 0xf8)
    {
      /* error */
      retCode = I2C_CODE_ERROR;
      break;
    }
  }

  // Do not generate a stop bit

  return retCode;
}


/******************************************************************************
 *
 * Description:
 *    Random read from 'address'. Write read bytes to 'pBuf'.
 *
 *****************************************************************************/
tS8 
eepromPageRead(tU16 address, 
               tU8* pBuf, 
               tU16 len) 
{
  tS8  retCode = 0;
  tU8  status  = 0;
  tU16 i       = 0;

getI2cLock();
  /* Write 3/4 bytes, see 24Cxx Random Read */
  retCode = eepromStartRead(I2C_EEPROM_ADDR, address);


  if( retCode == I2C_CODE_OK )
  {
    /* wait until address transmitted and receive data */
    for(i = 1; i <= len; i++ )
    {
      /* wait until data transmitted */
      while(1)
      {
        /* Get new status */
        status = i2cCheckStatus();

        if(( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 ))
        {
          /* Data received */

          if(i == len )
          {
            /* Set generate NACK */
            retCode = i2cGetChar( I2C_MODE_ACK1, pBuf );
          }
          else
          {
            retCode = i2cGetChar( I2C_MODE_ACK0, pBuf );
          }

          /* Read data */
          retCode = i2cGetChar( I2C_MODE_READ, pBuf );
          while( retCode == I2C_CODE_EMPTY )
          {
            retCode = i2cGetChar( I2C_MODE_READ, pBuf );
          }
          pBuf++;

          break;
        }
        else if( status != 0xf8 )
        {
          /* ERROR */
          i = len;
          retCode = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }

  /* Generate Stop condition */
  i2cStop();
releaseI2cLock();

  return retCode;

}


/******************************************************************************
 *
 * Description:
 *    Start a (random) write operation
 *
 *****************************************************************************/
tS8
eepromWrite(tU16 addr,
            tU8* pData,
            tU16 len)
{
  tS8  retCode = 0;
  tU16 i       = 0;

getI2cLock();
  do
  {

    /* generate Start condition */
    retCode = i2cStart();
    if(retCode != I2C_CODE_OK)
      break;


    /* write EEPROM I2C address */
    retCode = i2cWriteWithWait(I2C_EEPROM_ADDR | ((tU8)(addr >> 7) & 0x0e));
//    retCode = i2cWriteWithWait(I2C_EEPROM_ADDR);
    if(retCode != I2C_CODE_OK)
      break;

#if 0
    /* write offset high in EEPROM space  */
    retCode = i2cWriteWithWait( (tU8)(addr >> 8));
    if(retCode != I2C_CODE_OK)
      break;
#endif

    /* write offset low in EEPROM space  */
    retCode = i2cWriteWithWait( (tU8)(addr & 0xFF));
    if(retCode != I2C_CODE_OK)
      break;


    /* write data */
    for(i = 0; i <len; i++)
    {
      retCode = i2cWriteWithWait(*pData);
      if(retCode != I2C_CODE_OK)
        break;

      pData++;
    }

  } while(0);

  /* generate Stop condition */
  i2cStop();
releaseI2cLock();


  return retCode;
}


/******************************************************************************
 *
 * Description:
 *    Read the temperature from a LM75 temperature sensor
 *
 *****************************************************************************/
tS8 
lm75Read(tU8 address, tU8* pBuf, tU16 len) 
{
  tS8  retCode = 0;
  tU8  status  = 0;
  tU16 i       = 0;

getI2cLock();
  retCode = eepromStartRead(address, 0);


  if( retCode == I2C_CODE_OK )
  {
    /* wait until address transmitted and receive data */
    for(i = 1; i <= len; i++ )
    {
      /* wait until data transmitted */
      while(1)
      {
        /* Get new status */
        status = i2cCheckStatus();

        if(( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 ))
        {
          /* Data received */

          if(i == len )
          {
            /* Set generate NACK */
            retCode = i2cGetChar( I2C_MODE_ACK1, pBuf );
          }
          else
          {
            retCode = i2cGetChar( I2C_MODE_ACK0, pBuf );
          }

          /* Read data */
          retCode = i2cGetChar( I2C_MODE_READ, pBuf );
          while( retCode == I2C_CODE_EMPTY )
          {
            retCode = i2cGetChar( I2C_MODE_READ, pBuf );
          }
          pBuf++;

          break;
        }
        else if( status != 0xf8 )
        {
          /* ERROR */
          i = len;
          retCode = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }

  /* Generate Stop condition */
  i2cStop();
releaseI2cLock();

  return retCode;

}


/******************************************************************************
 *
 * Description:
 *    Communicate with the PCA9532
 *    First pBuf/len = bytes to write
 *    Second pBuf2/len2 = bytes to read
 *
 *****************************************************************************/
tS8
pca9532(tU8* pBuf, tU16 len, tU8* pBuf2, tU16 len2) 
{
  tS8  retCode = 0;
  tU8  status  = 0;
  tU16 i       = 0;

getI2cLock();
  do
  {
    /* generate Start condition */
    retCode = i2cStart();
    if(retCode != I2C_CODE_OK)
      break;


    /* write pca9532 address */
    retCode = i2cWriteWithWait(0xc0);
    if(retCode != I2C_CODE_OK)
      break;

    /* write data */
    for(i = 0; i <len; i++)
    {
      retCode = i2cWriteWithWait(*pBuf);
      if(retCode != I2C_CODE_OK)
        break;

      pBuf++;
    }

  } while(0);

  if (len2 > 0)
  {
    /* Generate Start condition */
    retCode = i2cRepeatStart();

    /* Transmit device address */
    if( retCode == I2C_CODE_OK)
    {
      /* Write SLA+R */
      retCode = i2cPutChar( 0xc0 + 0x01 );
      while( retCode == I2C_CODE_BUSY )
      {
        retCode = i2cPutChar( 0xc0 + 0x01 );
      }
    }

    /* Wait until SLA+R transmitted */
    while(1)
    {
      /* Get new status */
      status = i2cCheckStatus();

      if(status == 0x40)
      {
        /* Data transmitted and ACK received */
        break;
      }
      else if(status != 0xf8)
      {
        /* error */
        retCode = I2C_CODE_ERROR;
        break;
      }
    }

    if( retCode == I2C_CODE_OK )
    {
      /* wait until address transmitted and receive data */
      for(i = 1; i <= len2; i++ )
      {
        /* wait until data transmitted */
        while(1)
        {
          /* Get new status */
          status = i2cCheckStatus();

          if(( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 ))
          {
            /* Data received */

            if(i == len2 )
            {
              /* Set generate NACK */
              retCode = i2cGetChar( I2C_MODE_ACK1, pBuf2 );
            }
            else
            {
              retCode = i2cGetChar( I2C_MODE_ACK0, pBuf2 );
            }

            /* Read data */
            retCode = i2cGetChar( I2C_MODE_READ, pBuf2 );
            while( retCode == I2C_CODE_EMPTY )
            {
              retCode = i2cGetChar( I2C_MODE_READ, pBuf2 );
            }
            pBuf2++;

            break;
          }
          else if( status != 0xf8 )
          {
            /* ERROR */
            i = len2;
            retCode = I2C_CODE_ERROR;
            break;
          }
        }
      }
    }
  }

  /* Generate Stop condition */
  i2cStop();
releaseI2cLock();

  return retCode;

}

