/*
 * LedMatrix32x32Test01-yoko3.c (���a�搶��led_matrix_32_32_k7_k1.c�̃R�s�[����3���j
 *
 *�@���t�@�����X��H�Q�@�̃e�X�g�v���O�����@LED�}�g���N�X�P�̃e�X�g
 *
 *  Copied on: 2023/09/24
 *             2023/10/01 ������
 *      Author: mupch�@�|��x-lib
 */
/*******************************************/
/* LED�h�b�g�}�g���N�X�̉ۑ�7                      */
/* LED�h�b�g�}�g���N�X 32x32 2021/07/27 yokomizo�@�@ */
/* �g�p�}�C�R���{�[�h Nucleo  */
/*�@�g�pLED�h�b�g�}�g���N�X�i32x32 color�@�j            */
/*******************************************/
/* A:PIN1(PA0:A0) DCBA=0000��LINE0,8          */
/* B:PIN1(PA1:A1) 0001��LINE1,9               */
/* C:PIN2(PA4:A2)�@0010��LINE2,10              */
/* D:PIN3(PA5:D13) 1111��LINE15,31             */
/*�@R0_SIN:PIN(PA6:D12)�@�ԃV���A���f�[�^ROW1-16�@�@�@   */
/*�@R1_SIN:PIN(PA7:D11)�@�ԃV���A���f�[�^ROW17-32�@    */
/*�@G0_SIN:PIN(PA8:D7)�@�΃V���A���f�[�^ROW1-16�@�@�@�@   */
/*�@G1_SIN:PIN(PA9:D8)�@�΃V���A���f�[�^ROW17-32�@�@�@   */
/*�@B0_SIN:PIN(PA10:D2�j �V���A���f�[�^ROW1-16�@�@�@�@   */
/*�@B1_SIN:PIN(PA11:CN10_EV_105)�@�V���A���f�[�^ROW17-32�@�@�@   */
/*�@CLK:PIN(PB5:D4)�@�V�t�g�N���b�N     �@            */
/*�@STB:PIN(PB6:CN10_17)�@1�ŃX���[�A0�ŕێ�     �@�@ �@    */
/*�@OE:PIN(PB7:CN7_21)�@�@0�ŏo�� �@�@�@�@�@�@�@�@�@�@�@       */
/* MON(PB8:D15) */
//�C���N���[�h�t�@�C��
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

//�O���[�o���ϐ�
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
//�֐��̃v���g�^�C�v

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
  SysTick_Config(RCC_Clocks.HCLK_Frequency/30000);	//�R�O��Hz�͕K�v�B
//  SysTick_Config(RCC_Clocks.HCLK_Frequency/500);
//  	  SysTick_Config(RCC_Clocks.HCLK_Frequency/1000); //�S�R�x���B



  GPIOA->ODR = 0x0;
  GPIOB->ODR =0x0;
  //�f�[�^�̃R�s�[
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
	GPIOB->ODR |=OE_HI; //OE �\����~

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

	//�f�[�^�o��
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



    //�N���b�N�o��
    GPIOB->ODR &= ~CLK_HI;       //SCK=0
    GPIOB->ODR |= CLK_HI;        //SCK=1
    GPIOB->ODR &= ~CLK_HI;       //SCK=0

    //���b�`
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

    //�\���ĊJ
    GPIOB->ODR &= ~OE_HI;

}
