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

sbit key_yes = P2^0;//表示确认、开始的按键
sbit key_pause = P2^1;//表示暂停的按键
sbit key_mode = P2^2;//模式选择的按键
sbit music = P2^3;//蜂鸣器
sbit key_out = P2^4;//模拟出水结束中断
sbit key_in = P2^5;//模拟进水结束中断
sbit key_down = P2^6;//手动模式减少时间
sbit key_up = P2^7;//手动模式增加时间

unsigned char code DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// 显示段码值0~9
unsigned char code WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分别对应相应的数码管点亮,即位码
unsigned char TempData[8]; //存储显示值的全局变量


void moto_run();

void settimer0(){

	TH0 = 0x4C;
	TL0 = 0x00;

}
void init(){
  P1 = 0x00;
  P2 = 0xff;
  TMOD|=0x11;
  settimer0();
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
	  
	  delayms(200);

	  led_1 = 1;
	  led_2 = 0;
	  led_3 = 1;
	  
	  delayms(200);

	  led_1 = 1;
	  led_2 = 1;
	  led_3 = 0;
	  
	  delayms(200);
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
void keyscan(){


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

//功能选择
void menu(){

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
}
//漂洗高层程序
void wash2(){

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
	wash2();
	water_out();
	dry();
	water_in();
	wash2();
	water_out();
	dry();
	song();
}
void main(){
   init();
   autorun();
   //song();
   //wash();
   //water_in();
   while(1);
}

void water_ok() interrupt 0{
 	flag_water = 1;
	led_state = flag_water;
}
void timer_int() interrupt 1{
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
