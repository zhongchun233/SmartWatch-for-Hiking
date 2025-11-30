/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file drv_adapter_port_temphumioment.c
 * 
 * @par dependencies 
 * - i2c_port.h
 * - i2c.h
 * - iic_hal.h
 * - os_freertos
 * - osal_mutex
 * @author chengfeng26
 * 
 * @brief Provide the HAL APIs of AHT21 and corresponding opetions.
 * 
 * Processing flow:
 * 
 * 2.Then the users could all the IOs from instances of bsp_aht21_driver_t.
 * 
 * @version V1.0 2025-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
#include "Protocol_driver_Port.h"


#include "stm32f1xx_hal.h"
/**
  * @brief SDA线输入模式配置
  * @param None
  * @retval None
  */
void SDA_Input_Mode(iic_bus_t *bus)
{
    GPIO_device_InitTypeDef GPIO_InitStructure_t = {0};

    GPIO_InitStructure_t.Pin = bus->IIC_SDA_PIN;
    GPIO_InitStructure_t.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure_t.Pull = GPIO_PULLUP;
    GPIO_InitStructure_t.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_device_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure_t);
}

/**
  * @brief SDA线输出模式配置
  * @param None
  * @retval None
  */
void SDA_Output_Mode(iic_bus_t *bus)
{
    GPIO_device_InitTypeDef GPIO_InitStructure_t = {0};

    GPIO_InitStructure_t.Pin = bus->IIC_SDA_PIN;
    GPIO_InitStructure_t.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure_t.Pull = GPIO_NOPULL;
    GPIO_InitStructure_t.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_device_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure_t);
}

/**
  * @brief SDA线输出一个位
  * @param val 输出的数据
  * @retval None
  */
void SDA_Output(iic_bus_t *bus, uint16_t val)
{
    if ( val )
    {
        GPIO_device_WritePin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN, GPIO_device_PIN_SET);
    }
    else
    {
        GPIO_device_WritePin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN, GPIO_device_PIN_RESET);
    }
}

/**
  * @brief SCL线输出一个位
  * @param val 输出的数据
  * @retval None
  */
void SCL_Output(iic_bus_t *bus, uint16_t val)
{
    if ( val )
    {
        GPIO_device_WritePin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN, GPIO_device_PIN_SET);
    }
    else
    {
        GPIO_device_WritePin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN, GPIO_device_PIN_RESET);
    }
}

/**
  * @brief SDA输入一位
  * @param None
  * @retval GPIO读入一位
  */
uint8_t SDA_Input(iic_bus_t *bus)
{
	if(GPIO_device_ReadPin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN) == GPIO_PIN_SET){
		return 1;
	}else{
		return 0;
	}
}

/**
  * @brief IIC起始信号
  * @param None
  * @retval None
  */
void IICStart(iic_bus_t *bus)
{
    SDA_Output(bus,1);
    //delay1(DELAY_TIME);
		delay_us(2);
    SCL_Output(bus,1);
		delay_us(1);
    SDA_Output(bus,0);
		delay_us(1);
    SCL_Output(bus,0);
		delay_us(1);
}

/**
  * @brief IIC结束信号
  * @param None
  * @retval None
  */
void IICStop(iic_bus_t *bus)
{
    SCL_Output(bus,0);
		delay_us(2);
    SDA_Output(bus,0);
		delay_us(1);
    SCL_Output(bus,1);
		delay_us(1);
    SDA_Output(bus,1);
		delay_us(1);

}

/**
  * @brief IIC等待确认信号
  * @param None
  * @retval None
  */
unsigned char IICWaitAck(iic_bus_t *bus)
{
    unsigned short cErrTime = 5;
    SDA_Input_Mode(bus);
    SCL_Output(bus,1);
    while(SDA_Input(bus))
    {
        cErrTime--;
				delay_us(1);
        if (0 == cErrTime)
        {
            SDA_Output_Mode(bus);
            IICStop(bus);
            return ERROR;
        }
    }
    SDA_Output_Mode(bus);
    SCL_Output(bus,0);
		delay_us(2);
    return SUCCESS;
}

/**
  * @brief IIC发送确认信号
  * @param None
  * @retval None
  */
void IICSendAck(iic_bus_t *bus)
{
    SDA_Output(bus,0);
		delay_us(1);
    SCL_Output(bus,1);
		delay_us(1);
    SCL_Output(bus,0);
		delay_us(1);
}

/**
  * @brief IIC发送非确认信号
  * @param None
  * @retval None
  */
void IICSendNotAck(iic_bus_t *bus)
{
    SDA_Output(bus,1);
		delay_us(1);
    SCL_Output(bus,1);
		delay_us(1);
    SCL_Output(bus,0);
		delay_us(2);

}

/**
  * @brief IIC发送一个字节
  * @param cSendByte 需要发送的字节
  * @retval None
  */
void IICSendByte(iic_bus_t *bus,unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        SCL_Output(bus,0);
        delay_us(2);
        SDA_Output(bus, cSendByte & 0x80);
				delay_us(1);
        cSendByte += cSendByte;
				delay_us(1);
        SCL_Output(bus,1);
				delay_us(1);
    }
    SCL_Output(bus,0);
		delay_us(2);
}

/**
  * @brief IIC接收一个字节
  * @param None
  * @retval 接收到的字节
  */
unsigned char IICReceiveByte(iic_bus_t *bus)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode(bus);
    while (i--)
    {
        cR_Byte += cR_Byte;
        SCL_Output(bus,0);
				delay_us(2);
        SCL_Output(bus,1);
				delay_us(1);
        cR_Byte |=  SDA_Input(bus);
    }
    SCL_Output(bus,0);
    SDA_Output_Mode(bus);
    return cR_Byte;
}

uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data)
{				   	  	    																 
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	    
	if(IICWaitAck(bus))	//等待应答
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);	   	 										  		   
	IICSendByte(bus,data);     						   
	IICWaitAck(bus);  		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
}

uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[])
{			
	unsigned char i;	
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	    
	if(IICWaitAck(bus))
	{
		IICStop(bus);
		return 1;
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);	
	for(i=0;i<length;i++)
	{
		IICSendByte(bus,buff[i]);     						   
		IICWaitAck(bus); 
	}		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
} 

unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg)
{
	unsigned char dat;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);
	IICWaitAck(bus);
	IICSendByte(bus,reg);
	IICWaitAck(bus);
	
	IICStart(bus);
	IICSendByte(bus,(daddr<<1)+1);
	IICWaitAck(bus);
	dat = IICReceiveByte(bus);
	IICSendNotAck(bus);
	IICStop(bus);
	return dat;
}


uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[])
{
	unsigned char i;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);
	if(IICWaitAck(bus))
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);
	IICWaitAck(bus);
	
	IICStart(bus);
	IICSendByte(bus,(daddr<<1)+1);
	IICWaitAck(bus);
	for(i=0;i<length;i++)
	{
		buff[i] = IICReceiveByte(bus);
		if(i<length-1)
		{IICSendAck(bus);}
	}
	IICSendNotAck(bus);
	IICStop(bus);
	return 0;
}


//
void IICInit(iic_bus_t *bus)
{
    GPIO_device_InitTypeDef GPIO_InitStructure = {0};

		//bus->CLK_ENABLE();
    
    GPIO_InitStructure.Pin = bus->IIC_SDA_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_device_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.Pin = bus->IIC_SCL_PIN ;
    GPIO_device_Init(bus->IIC_SCL_PORT, &GPIO_InitStructure);
}
