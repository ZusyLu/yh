#include <stdint.h>
#include <stdbool.h>
#include "serial-rx-queue.h"
/*------------------------------------------------
 此C文件的功能是网关串口接收环形队列，缓存网关主控
HOST端的串口数据，
__________________________________________________*/

RingBuff_t ringBuff;//创建一个ringBuff的缓冲区

/*@brief  ringBufferInit
 * @param  void
 4* @return void
 * @author JIN
 * @date   2019
 * @version v1.0
 * @note   初始化环形缓冲区
 */
void ringBufferInit(void)
{
   //初始化相关信息
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Length = 0;
}

/**
 * @brief  writeRingBuffer
 * @param  uint8_t data
 * @return FLASE:环形缓冲区已满，写入失败;TRUE:写入成功
 * @author Jin
 * @date   2019
 * @version v1.0
 * @note   往环形缓冲区写入u8类型的数据
 */
uint8_t writeRingBuffer(uint8_t data)
{
   if(ringBuff.Length >= RINGBUFF_LEN) //判断缓冲区是否已满
    {
      return false;
    }
    ringBuff.Ring_Buff[ringBuff.Tail] = data;
    ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
    ringBuff.Length++;
    return true;
}

/**
 * @brief  readRingBuffer
 * @param  uint8_t *rData，用于保存读取的数据
 * @return FLASE:环形缓冲区没有数据，读取失败;TRUE:读取成功
 * @author 杰杰
 * @date   2018
 * @version v1.0
 * @note   从环形缓冲区读取一个u8类型的数据
 */
uint8_t readRingBuffer(uint8_t *rData)
{
     if(ringBuff.Length == 0)//判断非空
     {
         return false;
     }
     *rData = ringBuff.Ring_Buff[ringBuff.Head];//先进先出FIFO，从缓冲区头出

     ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
     ringBuff.Length--;
     return true;
}

uint8_t pushBufferToRing(uint8_t *buffer,uint8_t length)
{
   uint8_t i;
   if(ringBuff.Length + length >= RINGBUFF_LEN) //判断缓冲区是否已满
    {
         return false;
    }
    else
    {
         for(i = 0;i < length;i++)
         {
             ringBuff.Ring_Buff[ringBuff.Tail] = *(buffer+i);
             ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
             ringBuff.Length++;
         }
    }
   return true;

}
uint8_t popRingBufferData(uint8_t *buffer,uint8_t length)
{

     uint8_t i;
     if(ringBuff.Length < length)//判断非空
     {
         return false;
     }
     for(i = 0;i < length;i++)
     {
         *(buffer + i) = ringBuff.Ring_Buff[ringBuff.Head];//先进先出FIFO，从缓冲区头出

         ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
         ringBuff.Length--;
     }
     return true;
}








