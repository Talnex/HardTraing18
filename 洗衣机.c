#include<stc.h>
#define DataPort P0
#define COUNT_XMIN 1
#define COUNT_XS 10
static unsigned int count_1s = 20;
static unsigned int count_xs = COUNT_XS;
static unsigned int count_xmin = COUNT_XMIN *2;
static unsigned int cycle = 5;

static unsigned int SCALE = 1;
static unsigned int flag_water = 0;
static unsigned int flag_xs = 0;
static unsigned int flag_xmin = 0;
static unsigned int flag_cycle = 0;

static unsigned int zhuan = 0;

sbit led_1 = P1^0;//表示水流方向的三个灯
sbit led_2 = P1^1;
sbit led_3 = P1^2;

sbit duan =	P1^3;//数码管段选 
sbit wei = P1^4;//数码管位选
sbit led_state = P1^7;//系统指示灯

sbit switch_1 = P3^0;
sbit switch_2 = P3^1;
sbit motor1 = P3^6;//电机引脚
sbit motor2 = P3^7;

sbit KEY1 = P2^0;//表示确认、开始的按键
sbit KEY2 = P2^1;//表示暂停的按键
sbit KEY3 = P2^2;//模式选择的按键
sbit KEY4 = P2^6;//手动模式减少时间
sbit KEY5 = P2^7;//手动模式增加时间

sbit key_out = P2^4;//模拟出水结束中断
sbit key_in = P2^5;//模拟进水结束中断

sbit music = P2^3;//蜂鸣器

unsigned char code DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// 显示段码值0~9
unsigned char code WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分别对应相应的数码管点亮,即位码
unsigned char TempData[8]; //存储显示值的全局变量


void moto_run();
void autorun();

void setdisplay(unsigned char a,unsigned char b,unsigned char c,unsigned char d,
				unsigned char e,unsigned char f,unsigned char g,unsigned char h){
	TempData[0]=DuanMa[a];
    TempData[1]=DuanMa[b];
    TempData[2]=DuanMa[c];
    TempData[3]=DuanMa[d];
    TempData[4]=DuanMa[e];
    TempData[5]=DuanMa[f];
	TempData[6]=DuanMa[g];
    TempData[7]=DuanMa[h];


}
void displayclr(){
	TempData[0]=0x00;
    TempData[1]=0x00;
    TempData[2]=0x00;
    TempData[3]=0x00;
    TempData[4]=0x00;
    TempData[5]=0x00;
	TempData[6]=0x00;
    TempData[7]=0x00;
}

void settimer0(){

	TH0 = 0x4C;
	TL0 = 0x00;

}

void settimer1(){

 	TH0=(256-250)/256;		
 	TL0=(256-250)%256;

}
void init(){
  P1 = 0x00;
  P2 = 0xFF;
  TMOD|=0x21;
  settimer0();
  settimer1();
  ET1 = 1;
  EA = 1;
  TR1 = 1;
//  P3 = 0xff;
  //EA = 1;
}

void DelayUs2x(unsigned char t)
{   
 while(--t);
}

void delayms(unsigned char t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
//数码管显示程序
void Display(unsigned char FirstBit,unsigned char Num)
{
      static unsigned char i=0;	  

	   DataPort=0;   //清空数据，防止有交替重影
       duan=1;     //段锁存
       duan=0;

       DataPort=WeiMa[i+FirstBit]; //取位码 
       wei=1;     //位锁存
       wei=0;

       DataPort=TempData[i]; //取显示数据，段码
       duan=1;     //段锁存
       wei=0;
       
	   delayms(1);
	   i++;
       if(i==Num)
	      i=0;
}
//停止进出水
void water_stop(){
   EX0 = 0;
   flag_water = 0;
   switch_1 = 0;
   switch_2 = 0;
   led_state = 0;
   led_1 = 1;
   led_2 = 1;
   led_3 = 1;
}
//继电器1打开、进水
void water_in(){
   switch_1 = 1;
   switch_2 = 0;
   EX0 = 1;
   EA = 1;
   flag_water = 0;
   while(!flag_water){
   	  led_state = flag_water;
	  led_1 = 0;
	  led_2 = 1;
	  led_3 = 1;
	  
	  delayms(50);

	  led_1 = 1;
	  led_2 = 0;
	  led_3 = 1;
	  
	  delayms(50);

	  led_1 = 1;
	  led_2 = 1;
	  led_3 = 0;
	  
	  delayms(50);
   }
	water_stop();

}
//继电器2打开、出水
void water_out(){
   switch_1 = 0;
   switch_2 = 1;
   EX0 = 1;
   flag_water = 0;
   while(!flag_water){
   	  led_state = flag_water;
	  led_1 = 1;
	  led_2 = 1;
	  led_3 = 0;
	  
	  delayms(50);

	  led_1 = 1;
	  led_2 = 0;
	  led_3 = 1;
	  
	  delayms(50);

	  led_1 = 0;
	  led_2 = 1;
	  led_3 = 1;
	  
	  delayms(50);
   }
	water_stop();
}

//电机正转
void moto_run(){
	motor1 = 1;
	motor2 = 0;
}
//电机反转
void moto_nur(){
	motor1 = 0;
	motor2 = 1;
}
//模拟全速甩干
void moto_all(){
	motor1 = 0;
	motor2 = 0;
}

//电机停止
void moto_stop(){
	motor1 = 1;
	motor2 = 1;
}

//按键扫描程序
unsigned char KeyScan(void)
{
/********************************************************/  
if(!KEY1)  //如果检测到低电平，说明按键按下
    {
	 delayms(10); //延时去抖，一般10-20ms
     if(!KEY1)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!KEY1);//如果确认按下按键等待按键释放，没有则退出
	       {
		   return 1;
	 		}
	   }
	}
/********************************************************/  
else if(!KEY2)  //如果检测到低电平，说明按键按下
    {
	 delayms(10); //延时去抖，一般10-20ms
     if(!KEY2)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!KEY2);//如果确认按下按键等待按键释放，没有则退出
	       {
		   return 2;
	 		}
	   }
	}
/********************************************************/  
else if(!KEY3)  //如果检测到低电平，说明按键按下
    {
	 delayms(10); //延时去抖，一般10-20ms
     if(!KEY3)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!KEY3);//如果确认按下按键等待按键释放，没有则退出
	       {
		   return 3;
	 		}
	   }
	}
/********************************************************/  
else if(!KEY4)  //如果检测到低电平，说明按键按下
    {
	 delayms(10); //延时去抖，一般10-20ms
     if(!KEY4)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!KEY4);//如果确认按下按键等待按键释放，没有则退出
	       {
		   return 4;
	 		}
	   }
	}
/********************************************************/  
else if(!KEY5)  //如果检测到低电平，说明按键按下
    {
	 delayms(10); //延时去抖，一般10-20ms
     if(!KEY5)     //再次确认按键是否按下，没有按下则退出
	   {
        while(!KEY5);//如果确认按下按键等待按键释放，没有则退出
	       {
		   return 5;
	 		}
	   }
	}
/********************************************************/  
else
    return 0;
}

//报警程序
void song(){
   unsigned int i;

 	while(1)
   {
   		for(i=0;i<200;i++)
      	{
     		 DelayUs2x(200); 
	  	     music=!music;
	    }
	    music=0;//防止一直给喇叭通电造成损坏
   		for(i=0;i<200;i++)
       {
      	     delayms(1);  
	   }       
    }
}
//手动选择模式
void diy(){

}

//功能选择
void menu(){
	unsigned char key;
	displayclr();
	TempData[6]=DuanMa[0];
    TempData[7]=DuanMa[0];
	while(1){
	  key = KeyScan();
	  if(key!=0){
	    if(key!=2){
		  if(key!=3){
		   	if(key==1){
			 	if(TempData[7]==DuanMa[0]) autorun();
				else diy();
			}else if(key==4){
			 	TempData[7]=DuanMa[0];
			}else if(key==5){
				TempData[7]=DuanMa[1];
			}
		  }
		}
	  }
	  
	}
	//while(()==0){
//	   TempData[7]=DuanMa[key];
//	   TempData[6]=DuanMa[key];
//	};
//	TempData[7]=DuanMa[key];
//	TempData[6]=DuanMa[key];
	while(1);
}

//洗涤高层程序
void wash(){
	ET0 = 1;
	EA = 1;
	TR0 = 1;
   	while(flag_cycle==0){
	  moto_run();	  
	  while(flag_xmin==0);
	  flag_xmin = 0;
	  flag_xs = 0;

	  if(flag_cycle == 1)break;

	  moto_stop();
	  while(flag_xs==0);
	  flag_xmin = 0;
	  flag_xs = 0;

	  if(flag_cycle == 1)break;

	  moto_nur();	  
	  while(flag_xmin==0);
	  flag_xmin = 0;
	  flag_xs = 0;

	  if(flag_cycle == 1)break;

	  moto_stop();
	  while(flag_xs==0);
	  flag_xmin = 0;
	  flag_xs = 0;

	  if(flag_cycle == 1)break;
    }
	flag_cycle = 0;
	cycle = 5;
}

//甩干高层程序
void dry(){
	unsigned char i;
	unsigned char j;
	unsigned char k;
	moto_all();
	
	for(i=2;i>0;i--)
		for(j=2;j>0;j--)
			for(k=2;k>0;k--)
			{
   			  led_state = ~led_state;
			  led_1 = 1;
			  led_2 = 1;
	 	   	  led_3 = 0;
	 			 
	  		  delayms(200);

	  		  led_1 = 1;
	  		  led_2 = 0;
	  		  led_3 = 1;
	  
	  		  delayms(200);

	  		  led_1 = 0;
	  		  led_2 = 1;
	  		  led_3 = 1;
	  
	 		  delayms(200);
   			}
	water_stop();
	moto_stop();

}

void autorun(){
	water_in();
	wash();
	water_out();
	dry();
	water_in();
	wash();
	water_out();
	dry();
	water_in();
	wash();
	water_out();
	dry();
	song();
}
void main(){
   init();
   menu();
   //autorun();
   //song();
   //wash();
   //water_in();
   //while(1);
}

void water_ok() interrupt 0{
 	flag_water = 1;
	led_state = flag_water;
}
void timer0_int() interrupt 1{
	count_1s--;
	TF0 = 0;
	if(count_1s == 0){
	 count_1s = 20;
	 count_xs--;
	 if(count_xs == 0){
	  	count_xs = COUNT_XS;
		flag_xs = 1;
		count_xmin--;
		cycle--;
		if(count_xmin==0){
		   count_xmin=COUNT_XMIN *2;
		   flag_xmin = 1;
		}
		if(cycle ==0){
		   flag_cycle = 1;
		}
	 }
	 else{
	 
	 } 
	}
 	settimer0();
	TR0 = 1;
}

void timer1_int() interrupt 3{
	 TR1 = 0;
 	 Display(0,8);       // 调用数码管扫描
	 TR1 = 1;
}
