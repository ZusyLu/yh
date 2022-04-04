#include <stdint.h>
#include <stdbool.h>
#include "serial-rx-queue.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��������ش��ڽ��ջ��ζ��У�������������
HOST�˵Ĵ������ݣ�
__________________________________________________*/

RingBuff_t ringBuff;//����һ��ringBuff�Ļ�����

/*@brief  ringBufferInit
 * @param  void
 4* @return void
 * @author JIN
 * @date   2019
 * @version v1.0
 * @note   ��ʼ�����λ�����
 */
void ringBufferInit(void)
{
   //��ʼ�������Ϣ
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Length = 0;
}

/**
 * @brief  writeRingBuffer
 * @param  uint8_t data
 * @return FLASE:���λ�����������д��ʧ��;TRUE:д��ɹ�
 * @author Jin
 * @date   2019
 * @version v1.0
 * @note   �����λ�����д��u8���͵�����
 */
uint8_t writeRingBuffer(uint8_t data)
{
   if(ringBuff.Length >= RINGBUFF_LEN) //�жϻ������Ƿ�����
    {
      return false;
    }
    ringBuff.Ring_Buff[ringBuff.Tail] = data;
    ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
    ringBuff.Length++;
    return true;
}

/**
 * @brief  readRingBuffer
 * @param  uint8_t *rData�����ڱ����ȡ������
 * @return FLASE:���λ�����û�����ݣ���ȡʧ��;TRUE:��ȡ�ɹ�
 * @author �ܽ�
 * @date   2018
 * @version v1.0
 * @note   �ӻ��λ�������ȡһ��u8���͵�����
 */
uint8_t readRingBuffer(uint8_t *rData)
{
     if(ringBuff.Length == 0)//�жϷǿ�
     {
         return false;
     }
     *rData = ringBuff.Ring_Buff[ringBuff.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��

     ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
     ringBuff.Length--;
     return true;
}

uint8_t pushBufferToRing(uint8_t *buffer,uint8_t length)
{
   uint8_t i;
   if(ringBuff.Length + length >= RINGBUFF_LEN) //�жϻ������Ƿ�����
    {
         return false;
    }
    else
    {
         for(i = 0;i < length;i++)
         {
             ringBuff.Ring_Buff[ringBuff.Tail] = *(buffer+i);
             ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
             ringBuff.Length++;
         }
    }
   return true;

}
uint8_t popRingBufferData(uint8_t *buffer,uint8_t length)
{

     uint8_t i;
     if(ringBuff.Length < length)//�жϷǿ�
     {
         return false;
     }
     for(i = 0;i < length;i++)
     {
         *(buffer + i) = ringBuff.Ring_Buff[ringBuff.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��

         ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
         ringBuff.Length--;
     }
     return true;
}








