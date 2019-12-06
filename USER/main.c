#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "smg.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"


// ������������
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F,С����,ȫ��
u8 smg_num[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00}; 
//DHT11����
u16 dht11_t=0;//dht11����ʱ��
u8 temperature;//�¶�ֵ  	    
u8 humidity;//ʪ��ֵ
//ADC����
static u16 adcx=0;
static u16 adcx1=0;
static float temp=0;
//����ܱ���
u8 num=0;//�����ֵ
u8 smg_wei=0;//�����λѡ
u8 adc_t=0;//adc����ʱ��ֵ
u8 led_t=0;//led��ʾʱ��ֵ


int main(void)
{	
    HAL_Init();                    	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9); //����ʱ��,72M
    delay_init(72);                 //��ʼ����ʱ����
	uart_init(9600);	 	        //���ڳ�ʼ��Ϊ9600
	LED_Init();				        //��ʼ����LED���ӵ�Ӳ���ӿ�
    LED_SMG_Init();	                //����ܳ�ʼ��
 	Adc_Init();		  		        //ADC��ʼ��	    
    TIM2_Init(4000-1,7199);          //0.4s����һ��  �����ź�  2.5Hz
//	TIM2_Init(33000-1,7199); //0.3Hz�����ź�  //GUI����׼ȷ��Χ  0.3Hz~10Hz
	TIM3_Init(19,7199);             //�����2ms��ʱ��ʾ	
	LED_Init();				        //��ʼ����LED���ӵ�Ӳ���ӿ�
    LED_SMG_Init();	                //����ܳ�ʼ��
	
	
	//��PC8���Ų���һ��2.5Hz���� ������
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//����GPIOCʱ��
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_13;    //PC8
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	PCout(9)=1;//PC9 �ߵ�ƽ
	PCout(13)=0;//PC13 �͵�ƽ
	while(1)
	{	
		
	} 			
}


//�ص���������ʱ���жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM2_Handler))
    {
		static int i = 0;
		i=!i;
		PCout(8)=i;//PC8�����ƽ��ʱ��ת 
	}
	if(htim==(&TIM3_Handler))
    {
         adc_t++;dht11_t++;//���߶��Ƕ�ʱ2ms����һ��
		 if(dht11_t==500)//DHT11 1S����
		{
			dht11_t=0;
			DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��ֵ
		}
		 if(adc_t==25)//ADC 0.05s����(����Դˢ��)
		 {
			adc_t=0;85
			adcx=Get_Adc_Average(ADC_CHANNEL_9,3);//ADCԭʼֵ 
			temp=(float)adcx*(3.3/4096);//ADC��ѹֵ  temp�Ǹ�����
			adcx1=temp;//adcx1�����ͣ�ֻȡ��temp����������
			temp-=adcx1;//temp��ȥ������������ֵõ�С������
			temp*=1000; //С������ȡ��λ ��1.678911ȡ����678.911 ���������� ����ֻ��ʾ2λС�� ���ڶ�ȡһλ��Ϊ�˱�֤�ڶ�λС��׼ȷ
			float num1=adcx1; 
			printf("%d%d%1.3f",temperature,humidity,num1+temp/1000);//ÿ0.05�뷢�������ݸ�����
		 }
		 switch(smg_wei)//�����ɨ��
		 {
			   case 0:  num = smg_num[temperature/10]; break;//�¶�ֵ
			   case 1:  num =smg_num[temperature%10]|0x01;break; 
			   case 2:  num = smg_num[humidity/10]; break;//ʪ��ֵ
			   case 3:  num =smg_num[humidity%10]|0x01;break;      
			   //ADC�����ĵ�ѹֵ
			   case 4:  num = smg_num[adcx1]|0x01;break;//�������֣���λ����0x01ʹ���һλ��Ϊ1��ʵ�ִ�С���� ���忴����ܶ����ѯ���� ����������˳��a-dp��
			   case 5:  num = smg_num[(u16)temp/100]; break; //��һλС��  678.911 -> 6
			   case 6:  num = smg_num[(u16)temp%100/10];break; //�ڶ�λС��  678.911 -> 7   ���յ�ѹ1.67
			   case 7:  num = smg_num[(u16)temp%10]; break;//����λС�� 678.911 -> 8 
		 }
		LED_Write_Data(num,smg_wei);//ÿ2msд���ݵ������
		LED_Refresh();//������ʾ	 �����2msˢ��һλ	
		smg_wei++;
		if(smg_wei==8) smg_wei=0;
		led_t++;
		if(led_t==250)//500ms��˸
		{
			led_t=0;
			LED0=!LED0;
		}
    }
	
}


