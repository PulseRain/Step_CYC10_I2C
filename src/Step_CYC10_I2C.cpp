/*
###############################################################################
# Copyright (c) 2019, PulseRain Technology LLC 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (LGPL) as 
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE.  
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
*/

#include "Step_CYC10_I2C.h"

//----------------------------------------------------------------------------
//  Private function for I2C wait
//----------------------------------------------------------------------------
void Step_CYC10_I2C::_I2C_wait(uint8_t flag, uint8_t data = 0xFF)
{
  uint8_t t;
  
   do {
        t = (*REG_I2C_CSR);
   } while (!(t & flag));

   if (flag & I2C_MASTER_DATA_REQ) {
         (*REG_I2C_DATA) = data;
   }
  
} // End of _I2C_wait()




//----------------------------------------------------------------------------
// masterWrite()
//
// Parameters:
//      i2c_addr    : I2C address, 7 bit
//      reg_addr    : address for the register to be read from the slave device
//      data        : write data
//
// Return Value:
//      return 0 if it runs ok. Otherwise it will return the CSR value
//
// Remarks:
//      Function to write data to I2C slave 
//----------------------------------------------------------------------------

void Step_CYC10_I2C::masterWrite(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
   uint8_t t;

   
   (*REG_I2C_CSR) = I2C_CSR_MASTER | I2C_CSR_WRITE | I2C_CSR_SYNC_RESET;
   
   t = ((uint8_t)i2c_addr) & 127;
   t <<= 1;
   
   _I2C_wait (I2C_MASTER_IDLE_FLAG);
     
   (*REG_I2C_DATA) = t;

   (*REG_I2C_CSR) = I2C_CSR_START | I2C_CSR_WRITE | I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;

   _I2C_wait (I2C_MASTER_DATA_REQ, reg_addr);


   _I2C_wait (I2C_MASTER_DATA_REQ, data); 

   _I2C_wait (I2C_MASTER_DATA_REQ); 
  
   (*REG_I2C_CSR) = I2C_CSR_STOP | I2C_CSR_WRITE | I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;

   _I2C_wait (I2C_MASTER_IDLE_FLAG);
   
} // End of masterWrite()


//----------------------------------------------------------------------------
// masterRead()
//
// Parameters:
//      i2c_addr    : I2C address, 7 bit
//      reg_addr    : address for the register to be read from the slave device
//      buf         : data buffer to be filled
//      buf_size    : read length in bytes
//
// Return Value:
//      return 0 if it runs ok. Otherwise it will return the CSR value
//
// Remarks:
//      Function to read data from I2C slave 
//----------------------------------------------------------------------------

void Step_CYC10_I2C::masterRead(uint8_t i2c_addr, uint8_t reg_addr, uint8_t buf[], uint8_t buf_size)
{
   uint8_t t, i;

   uint32_t interrupt_saved_mstatus    = read_csr (mstatus);
    
   
   (*REG_I2C_CSR) = I2C_CSR_MASTER | I2C_CSR_WRITE | I2C_CSR_SYNC_RESET;
   
   t = ((uint8_t)i2c_addr) & 127;
   t <<= 1;

   _I2C_wait (I2C_MASTER_IDLE_FLAG);

    
   (*REG_I2C_DATA) = t;
   (*REG_I2C_CSR)  = I2C_CSR_START | I2C_CSR_WRITE | I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;

   
    _I2C_wait (I2C_MASTER_DATA_REQ, reg_addr);

    
    t = ((uint8_t)i2c_addr) & 127;
    t <<= 1;
 
    _I2C_wait (I2C_MASTER_DATA_REQ, t);
   

    (*REG_I2C_CSR) = I2C_CSR_RESTART |  I2C_CSR_READ | I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;

    _I2C_wait (I2C_MASTER_IDLE_FLAG);

    (*REG_I2C_CSR) = I2C_CSR_START| I2C_CSR_READ | I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;

    write_csr (mstatus, 0);
      for (i = 0; i < buf_size; ++i) {
          _I2C_wait (I2C_MASTER_DATA_READY);
          
          buf[i] = (*REG_I2C_DATA);
          (*REG_I2C_DATA) = 0;
          
      }
    write_csr (mstatus, interrupt_saved_mstatus);
     
   (*REG_I2C_CSR) = I2C_CSR_STOP| I2C_CSR_MASTER | I2C_CSR_IRQ_DISABLE;
  
   _I2C_wait(I2C_MASTER_IDLE_FLAG);
    
} // End of masterRead()



Step_CYC10_I2C I2C;
