/*
 * LedMatrix32x32Test01-yoko3.c (横溝先生のled_matrix_32_32_k7_k1.cのコピー改変3号）
 *
 *　リファレンス回路２　のテストプログラム　LEDマトリクス単体テスト
 *
 *  Copied on: 2023/09/24
 *             2023/10/01 見直し
 *      Author: mupch　－＞x-lib
 */
/*******************************************/
/* LEDドットマトリクスの課題7                      */
/* LEDドットマトリクス 32x32 2021/07/27 yokomizo　　 */
/* 使用マイコンボード Nucleo  */
/*　使用LEDドットマトリクス（32x32 color　）            */
/*******************************************/
/* A:PIN1(PA0:A0) DCBA=0000でLINE0,8          */
/* B:PIN1(PA1:A1) 0001でLINE1,9               */
/* C:PIN2(PA4:A2)　0010でLINE2,10              */
/* D:PIN3(PA5:D13) 1111でLINE15,31             */
/*　R0_SIN:PIN(PA6:D12)　赤シリアルデータROW1-16　　　   */
/*　R1_SIN:PIN(PA7:D11)　赤シリアルデータROW17-32　    */
/*　G0_SIN:PIN(PA8:D7)　緑シリアルデータROW1-16　　　　   */
/*　G1_SIN:PIN(PA9:D8)　緑シリアルデータROW17-32　　　   */
/*　B0_SIN:PIN(PA10:D2） 青シリアルデータROW1-16　　　　   */
/*　B1_SIN:PIN(PA11:CN10_EV_105)　青シリアルデータROW17-32　　　   */
/*　CLK:PIN(PB5:D4)　シフトクロック     　            */
/*　STB:PIN(PB6:CN10_17)　1でスルー、0で保持     　　 　    */
/*　OE:PIN(PB7:CN7_21)　　0で出力 　　　　　　　　　　　       */
/* MON(PB8:D15) */
//インクルードファイル
#include <stddef.h>
#include "stm32f10x.h"
//#include "stm32f4xx.h"
#define waite_time 1

#define A_HI 0x1
#define B_HI 0x2
#define C_HI 0x10
#define D_HI 0x20
#define LINE_MASK 0x33
#define R0_HI 0x40
#define R1_HI 0x80
#define G0_HI 0x100
#define G1_HI 0x200
#define B0_HI 0x400
#define B1_HI 0x800
#define CLK_HI 0x20
#define STB_HI 0x40
#define OE_HI 0x80
#define MON_HI 0x100

//グローバル変数
int delay_time;


int data_r0[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
int data_r1[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
int data_g0[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
int data_g1[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
int data_b0[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
int data_b1[16] ={0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0,
		          0x0,0x0,0x0,0x0
};
//関数のプロトタイプ

void  init_system(void);
void wait_func(int);
void Dynamic_Display(void);


int main(void)
{
  int i;
  int color;
  int data_p0[16] ={0x00000000,
		            0x000ff000,
		            0x007ffe00,
		            0x01ffff80,
		            0x03ffffc0,
		            0x07ffffe0,
		            0x0ffffff0,
		            0x1ffffff8,
		            0x1ffffff8,
		            0x3ffffffC,
		            0x3ffffffC,
		            0x3ffffffC,
		            0x7ffffffe,
		            0x7ffffffe,
		            0x7ffffffe,
		            0x7ffffffe
  };
  int data_p1[16] ={0x7ffffffe,
		            0x7ffffffe,
		            0x7ffffffe,
		            0x7ffffffe,
		            0x3ffffffc,
		            0x3ffffffc,
		            0x3ffffffc,
		            0x1ffffff8,
		            0x1ffffff8,
		            0x0ffffff0,
		            0x07ffffe0,
		            0x03ffffc0,
		            0x01ffff80,
					0x007ffe00,
					0x000ff000,
					0x00000000
  };


  RCC_ClocksTypeDef RCC_Clocks;
  init_system();
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency/30000);	//３０ｋHzは必要。
//  SysTick_Config(RCC_Clocks.HCLK_Frequency/500);
//  	  SysTick_Config(RCC_Clocks.HCLK_Frequency/1000); //全然遅い。



  GPIOA->ODR = 0x0;
  GPIOB->ODR =0x0;
  //データのコピー
  for(i=0;i<16;i++){
    data_r0[i] = 0x0;
    data_r1[i] = 0x0;
    data_g0[i] = 0x0;
    data_g1[i] = 0x0;
    data_b0[i] = 0x0;
    data_b1[i] = 0x0;
  }
  while (1)
  {
    for(color=0;color<8;color++){
	  for(i=0;i<16;i++){
        if((color &0x1)!=0){
	      data_r0[i] = data_p0[i];
	      data_r1[i] = data_p1[i];
		}
		else{
		  data_r0[i] = 0x0;
		  data_r1[i] = 0x0;
		}
		if((color &0x2)!=0){	    //if(color==1){
	      data_g0[i] = data_p0[i];
	      data_g1[i] = data_p1[i];
		}
		else{
		  data_g0[i] = 0x0;
		  data_g1[i] = 0x0;
		}
		if((color &0x4)!=0){
	      data_b0[i] = data_p0[i];
	      data_b1[i] = data_p1[i];
		}
		else{
		  data_b0[i] = 0x0;
		  data_b1[i] = 0x0;
		}
	  }
	  wait_func(10000);
    }
  }
}


void init_system(void){
	   RCC->APB2ENR |=RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN ;
	   GPIOA->CRL =0x11114411; // PA7-4,1,0 OUT
	   GPIOA->CRH =0x44441111; // PA1-8 OUT
	   GPIOB->CRL =0x11144444; // PB7-5 OUT
	   GPIOB->CRH =0x44444441; // PB8 OUT
}

void wait_func(int time){
  delay_time = time;
  while(delay_time>0);
}


void SysTick_Handler(void){

	//delay count
	if(delay_time==0)
	  delay_time=0;
	else
	  delay_time--;

	Dynamic_Display();
}

void Dynamic_Display(void){


	static int clk_count=0;
	static int line = 0;
	static int line_bits=0;

	static int line_data_r0;
	static int line_data_r1;
	static int line_data_g0;
	static int line_data_g1;
	static int line_data_b0;
	static int line_data_b1;




	//led matrix
	GPIOB->ODR |=OE_HI; //OE 表示停止

    if(clk_count>=31){
      clk_count=0;
      if (line>=15)
    	line = 0;
      else
    	 line++;
      }
    else
  	  clk_count++;

    //line
	if(clk_count==0){
      line_data_r0 = data_r0[line];
      line_data_r1 = data_r1[line];
      line_data_g0 = data_g0[line];
      line_data_g1 = data_g1[line];
      line_data_b0 = data_b0[line];
      line_data_b1 = data_b1[line];
	}
	else{
	  line_data_r0 = line_data_r0>>1;
	  line_data_r1 = line_data_r1>>1;
	  line_data_g0 = line_data_g0>>1;
	  line_data_g1 = line_data_g1>>1;
	  line_data_b0 = line_data_b0>>1;
	  line_data_b1 = line_data_b1>>1;
	}

	//データ出力
    if((line_data_r0 & 0x1)==0)
      GPIOA->ODR &= ~R0_HI;     //R0-SIN=0
    else
      GPIOA->ODR |= R0_HI;      //R0-SIN=1
    if((line_data_r1 & 0x1)==0)
      GPIOA->ODR &= ~R1_HI;     //R1-SIN=0
    else
      GPIOA->ODR |= R1_HI;      //R1-SIN=1
    if((line_data_g0 & 0x1)==0)
      GPIOA->ODR &= ~G0_HI;     //G0-SIN=0
    else
      GPIOA->ODR |= G0_HI;      //G0-SIN=1
    if((line_data_g1 & 0x1)==0)
      GPIOA->ODR &= ~G1_HI;     //G1-SIN=0
    else
      GPIOA->ODR |=G1_HI;      //G1-SIN=1
    if((line_data_b0 & 0x1)==0)
      GPIOA->ODR &= ~B0_HI;     //B0-SIN=0
    else
      GPIOA->ODR |= B0_HI;      //B0-SIN=1
    if((line_data_b1 & 0x1)==0)
      GPIOA->ODR &= ~B1_HI;     //B1-SIN=0
    else
      GPIOA->ODR |= B1_HI;      //B1-SIN=1



    //クロック出力
    GPIOB->ODR &= ~CLK_HI;       //SCK=0
    GPIOB->ODR |= CLK_HI;        //SCK=1
    GPIOB->ODR &= ~CLK_HI;       //SCK=0

    //ラッチ
    if(clk_count==31){
		  //GPIOA->ODR = (GPIOA->ODR & 0xfffffff0) + line; //line
          line_bits = ((line & 0xc)<<2) |(line & 0x3);
          GPIOA->ODR &= ~LINE_MASK | line_bits ;
          GPIOA->ODR |= LINE_MASK & line_bits ;
	      GPIOB->ODR |=  STB_HI;      //SLATB =1
	}
    if(clk_count==0){
	      GPIOB->ODR &= ~ STB_HI;     //SLATB =0
	}

    //表示再開
    GPIOB->ODR &= ~OE_HI;

}
