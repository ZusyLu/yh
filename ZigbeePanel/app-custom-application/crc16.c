#include <stdlib.h>
#include "crc16.h"
unsigned int getCrc16(unsigned char* data, unsigned char start,unsigned int len)
{
   int i,j;
   unsigned int byteH,byteL;
   unsigned int value = 0xffff;
   if((len == 0))
     return 0;
   
   for (i = start; i < start + len; ++i)
   {
      int v = (data[i] & 0xff);
      value ^= v;
      for (j = 0; j < 8; ++j)
      {
          int bitL = value & 0x01;
          value >>= 1;
          if (bitL == 1)
          {
              value ^= 0xA001;
          }
      }
   }
   byteH = (value & 0xff00) >> 8;
   byteL = value & 0x00ff;
   return (byteL << 8) + byteH;
}