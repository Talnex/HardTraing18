``` c
#include<reg52.h>
#define uint unsigned int
sbit LED = P1^0;
void main(){
	uint i,j;
  	while(1){
	 	LED = 0;
		for(i = 100;i>0;i--)	   //此处不能有=号，uint>=0
			for(j = 110;j>0;j--);

		LED = 1;
		for(i = 100;i>0;i--)
			for(j = 110;j>0;j--);
	}
}
```
