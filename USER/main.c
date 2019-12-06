#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "smg.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"


// 共阴数字数组
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F,小数点,全灭
u8 smg_num[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00}; 
//DHT11变量
u16 dht11_t=0;//dht11采样时间
u8 temperature;//温度值  	    
u8 humidity;//湿度值
//ADC变量
static u16 adcx=0;
static u16 adcx1=0;
static float temp=0;
//数码管变量
u8 num=0;//数码管值
u8 smg_wei=0;//数码管位选
u8 adc_t=0;//adc采样时间值
u8 led_t=0;//led显示时间值


int main(void)
{	
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9); //设置时钟,72M
    delay_init(72);                 //初始化延时函数
	uart_init(9600);	 	        //串口初始化为9600
	LED_Init();				        //初始化与LED连接的硬件接口
    LED_SMG_Init();	                //数码管初始化
 	Adc_Init();		  		        //ADC初始化	    
    TIM2_Init(4000-1,7199);          //0.4s触发一次  方波信号  2.5Hz
//	TIM2_Init(33000-1,7199); //0.3Hz方波信号  //GUI波形准确范围  0.3Hz~10Hz
	TIM3_Init(19,7199);             //数码管2ms定时显示	
	LED_Init();				        //初始化与LED连接的硬件接口
    LED_SMG_Init();	                //数码管初始化
	
	
	//在PC8引脚产生一个2.5Hz方波 供测试
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//开启GPIOC时钟
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_13;    //PC8
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	PCout(9)=1;//PC9 高电平
	PCout(13)=0;//PC13 低电平
	while(1)
	{	
		
	} 			
}


//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM2_Handler))
    {
		static int i = 0;
		i=!i;
		PCout(8)=i;//PC8输出电平定时翻转 
	}
	if(htim==(&TIM3_Handler))
    {
         adc_t++;dht11_t++;//两者都是定时2ms增加一次
		 if(dht11_t==500)//DHT11 1S采样
		{
			dht11_t=0;
			DHT11_Read_Data(&temperature,&humidity);//读取温湿度值
		}
		 if(adc_t==25)//ADC 0.05s采样(数据源刷新)
		 {
			adc_t=0;85
			adcx=Get_Adc_Average(ADC_CHANNEL_9,3);//ADC原始值 
			temp=(float)adcx*(3.3/4096);//ADC电压值  temp是浮点数
			adcx1=temp;//adcx1是整型，只取了temp的整数部分
			temp-=adcx1;//temp减去自身的整数部分得到小数部分
			temp*=1000; //小数部分取三位 如1.678911取得是678.911 （浮点数） 最终只显示2位小数 现在多取一位是为了保证第二位小数准确
			float num1=adcx1; 
			printf("%d%d%1.3f",temperature,humidity,num1+temp/1000);//每0.05秒发送新数据给串口
		 }
		 switch(smg_wei)//数码管扫描
		 {
			   case 0:  num = smg_num[temperature/10]; break;//温度值
			   case 1:  num =smg_num[temperature%10]|0x01;break; 
			   case 2:  num = smg_num[humidity/10]; break;//湿度值
			   case 3:  num =smg_num[humidity%10]|0x01;break;      
			   //ADC换算后的电压值
			   case 4:  num = smg_num[adcx1]|0x01;break;//整数部分（按位或上0x01使最后一位变为1即实现带小数点 具体看数码管段码查询工具 共阴、排列顺序a-dp）
			   case 5:  num = smg_num[(u16)temp/100]; break; //第一位小数  678.911 -> 6
			   case 6:  num = smg_num[(u16)temp%100/10];break; //第二位小数  678.911 -> 7   最终电压1.67
			   case 7:  num = smg_num[(u16)temp%10]; break;//第三位小数 678.911 -> 8 
		 }
		LED_Write_Data(num,smg_wei);//每2ms写数据到数码管
		LED_Refresh();//更新显示	 数码管2ms刷新一位	
		smg_wei++;
		if(smg_wei==8) smg_wei=0;
		led_t++;
		if(led_t==250)//500ms闪烁
		{
			led_t=0;
			LED0=!LED0;
		}
    }
	
}


