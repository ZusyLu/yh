#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
#include "serial-rx-queue.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��������ؽ��յ��豸��RF����󣬴��ڶ����У�
Ȼ��ͨ�����ڷ��͵�W���ص�����Host��
ȡ���ݶ��к�����
rfPopRingBufferOneFrame
���20msȡ��������
__________________________________________________*/

#define RF_BUFFER_LEN_MAX  1600
#define RF_POP_TIMEOUT     25

typedef struct
{
    uint16_t Head;
    uint16_t Tail;
    uint16_t Length;
    uint8_t Ring_Buff[RF_BUFFER_LEN_MAX];
}WirelessRingBuff_t;

WirelessRingBuff_t rfRingBuffer;
uint16_t  lastPopRfTime;
/*@brief  rfRingBufferInit
 * @param  void
 4* @return void
 * @author JIN
 * @date   2019
 * @version v1.0
 * @note   ��ʼ�����λ�����
 */
void rfRingBufferInit(void)
{
   //��ʼ�������Ϣ
   rfRingBuffer.Head = 0;
   rfRingBuffer.Tail = 0;
   rfRingBuffer.Length = 0;
}

/**
 * @brief  rfWriteRingBuffer
 * @param  uint8_t data
 * @return FLASE:环形缓冲区已满，写入失败;TRUE:写入成功
 * @author Jin
 * @date   2019
 * @version v1.0
 * @note   往环形缓冲区写入u8类型的数据
 */
uint8_t rfWriteRingBuffer(uint8_t data)
{
   if(rfRingBuffer.Length >= RF_BUFFER_LEN_MAX) //判断缓冲区是否已满
    {
      return false;
    }
    rfRingBuffer.Ring_Buff[rfRingBuffer.Tail] = data;
    rfRingBuffer.Tail = (rfRingBuffer.Tail+1)%RF_BUFFER_LEN_MAX;//防止越界非法访问
    rfRingBuffer.Length++;
    return true;
}

/**
 * @brief  rfReadRingBuffer
 * @param  uint8_t *rData�����ڱ����ȡ������
 * @return FLASE:���λ�����û�����ݣ���ȡʧ��;TRUE:��ȡ�ɹ�
 * @author �ܽ�
 * @date   2018
 * @version v1.0
 * @note   �ӻ��λ�������ȡһ��u8���͵�����
 */
uint8_t rfReadRingBuffer(uint8_t *rData)
{
     if(rfRingBuffer.Length == 0)//�жϷǿ�
     {
         return false;
     }
     *rData = rfRingBuffer.Ring_Buff[rfRingBuffer.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��

     rfRingBuffer.Head = (rfRingBuffer.Head+1)%RF_BUFFER_LEN_MAX;//��ֹԽ��Ƿ�����
     rfRingBuffer.Length--;
     return true;
}

uint8_t rfPushBufferToRing(uint8_t *buffer,uint8_t length)
{
   uint8_t i;
   if(rfRingBuffer.Length + length >= RF_BUFFER_LEN_MAX) //�жϻ������Ƿ�����
    {
         return false;
    }
    else
    {
         for(i = 0;i < length;i++)
         {
             rfRingBuffer.Ring_Buff[rfRingBuffer.Tail] = *(buffer+i);
             rfRingBuffer.Tail = (rfRingBuffer.Tail+1)%RF_BUFFER_LEN_MAX;//��ֹԽ��Ƿ�����
             rfRingBuffer.Length++;
         }
    }
   return true;

}
uint8_t rfPopRingBufferData(uint8_t *buffer,uint8_t length)
{

     uint8_t i;
     if(rfRingBuffer.Length < length)//�жϷǿ�
     {
         return false;
     }
     for(i = 0;i < length;i++)
     {
         *(buffer + i) = rfRingBuffer.Ring_Buff[rfRingBuffer.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��

         rfRingBuffer.Head = (rfRingBuffer.Head+1)%RF_BUFFER_LEN_MAX;//��ֹԽ��Ƿ�����
         rfRingBuffer.Length--;
     }
     return true;
}
void rfPopRingBufferOneFrame(void)
{

    int16u time;
    uint8_t length;
    uint8_t buffer[90];
    time = halCommonGetInt16uMillisecondTick();
    if (elapsedTimeInt16u(lastPopRfTime, time) > RF_POP_TIMEOUT)
    {
        lastPopRfTime = time;
        if(rfReadRingBuffer(&length))
        {

            if(rfPopRingBufferData(buffer,length))
            {
                emberSerialWriteData(HAL_SERIAL_PORT_USART2, buffer, length);
                emberSerialWaitSend(HAL_SERIAL_PORT_USART2);
            }
        }
    }
}






