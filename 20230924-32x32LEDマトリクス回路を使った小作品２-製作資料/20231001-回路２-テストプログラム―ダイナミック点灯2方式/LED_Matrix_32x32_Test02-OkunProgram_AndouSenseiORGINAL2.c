/*
 * LED_Matrix_32x32_Test02-OkunProgram_AndouSenseiORGINAL2.c
 *
 *  Created on: 2023/10/01
 *      Author: x-lib
 */

/*
 *
 * SW1~4�̉����ɉ����āA��ʕ\���Ɗy�Ȃ�ς���B
 *
 *
 *�ߘa4�N�x�C�����@O�N�@�k�������o�����������̃��[�`���𗬗p(�I���W�i�������搶�j
 *�@�|�[�e�B���O��Ƃ̕s��ہjPA�̃R���t�B�M�����[�V�������W�X�^�̌�l�㏑���ɋC�Â����A���Ԃ�Q��
 *
 *
 *�@10��1����œ���s�ǁB
 *�@10��2����œ���s�ǁB��`�̈�̃R�s�[�����܂������Ȃ��B
 *�@10���B�����A�����搶�̊֐��B��`�̈�̃R�s�[�����܂������Ȃ��s��͎������B
 */

#include <stddef.h>
#include <string.h>
#include "stm32f10x.h"




//�e��V���{����`

//�R�Q���R�QLED�}�g���N�X�_������֌W�̒萔

//�_���F�̃f�[�^
#define R 0x1								//00 00 00 01		0000 0001
#define G 0x4								//00 00 01 00		0000 0100
#define Y 0x5								//00 00 01 01
#define B 0x10								//00 01 00 00		0001 0000
#define M 0x11								//00 01 00 01
#define C 0x14								//00 01 01 00
#define W 0x15								//00 01 01 01

//�M����
#define UE 0x15								//00 01 01 01
#define SITA 0x2A							//00 10 10 10
#define ROW_NO_MASK 0x033					//00 11 00 11


//�|�[�g�̐ݒ�

#define CLK 0x20					//PB5		0000 0000 0010 0000
#define STB 0x40					//PB6		0000 0000 0100 0000
#define OE_ 0x80					//PB7		0000 0000 1000 0000


//#define A_HI 0x1					//PA0		0000 0000 0000 0001
//#define B_HI 0x2					//PA1		0000 0000 0000 0010
//#define C_HI 0x10					//PA4		0000 0000 0001 0000
//#define D_HI 0x20					//PA5		0000 0000 0010 0000
//#define R0 0x40					//PA6		0000 0000 0100 0000
//#define R1 0x80					//PA7		0000 0000 1000 0000
//#define G0 0x100					//PA8		0000 0001 0000 0000
//#define G1 0x200					//PA9		0000 0010 0000 0000
//#define B0 0x400					//PA10		0000 0100 0000 0000
//#define B1 0x800					//PA11		0000 1000 0000 0000

//���d�X�s�[�J�[
#define SPK			0x01			//PB0		0000 0000 0000 0001

//�X�C�b�`

//          0bFEDCBA9876543210
#define SW1 0b0000000000010000	//Sw1(PC4)�p�̃}�X�N�l
#define SW2 0b0000000000100000	//Sw2(PC5)�p�̃}�X�N�l
#define SW3 0b0000000001000000	//Sw3(PC6)�p�̃}�X�N�l
#define SW4 0b0000000010000000	//Sw4(PC7)�p�̃}�X�N�l


//��Ԃ̏��
#define	STOP	0	//��~���
#define RUN		1	//�J�E���g���


//TIM�ɂ��u�U�[���K�f�[�^
#define C4 523 //��
#define D4 587 //��
#define E4 659 //��
#define F4 698 //�ӂ�
#define G4 783 //��
#define A4 880 //��
#define B4 987 //��
#define C5 1046 //��
#define D5 1174 //��

#define LEN1	500
#define LEN		60

//�����f�[�^�̍\����
typedef struct{
	int pitch;
	int duration;
} Note;



//SysTick_Handler�Ăяo���Ԋu
#define SYSTICK_FREQ	30000




//�O���[�o���ϐ�

//int line_bits=0;




int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int time_data = 0;	//���ԃf�[�^


int State_ID=STOP;


//�X�C�b�`�֌W�̕ϐ�
int up=0;		//�X�C�b�`�̗����オ�茟�o�t���O�A0�ŏ�����
int sw_now=0;	//���݂̃X�C�b�`������ԁA0�ŏ�����




//Dyn_Display�֐��͂��̗̈�̃f�[�^��LED�}�g���N�X�ɕ\������B
char matrix[32][32] ={
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//10
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//20
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//30
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	};//	1					10					20					30



char background_matrix[32][32] ={
			{W,W,W,W,W,W,W,W,W,W,W,W,0,0,0,0,0,0,0,0,0,0,0,W,W,W,W,W,W,W,W,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//10
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//20
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//30
			{W,G,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
	};//	1					10					20					30


//�X�^�[�g���
 char prob0_matrix[32][32] ={
			{W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0},
			{0,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,W},
			{W,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,0,0,0,W,W,0,0,W,W,W,W,W,W,0,W},//
			{W,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,W,0,0,W,W,0,0,W,W,0,0,0,0,0,0},
			{0,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,W,W,0,W,W,0,0,W,W,0,0,0,0,0,W},//
			{W,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,0,W,W,W,W,0,0,W,W,W,W,W,W,0,0},//LINE�@�o�t�y�y�k�d
			{0,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,0,0,W,W,W,0,0,W,W,0,0,0,0,0,W},
			{W,0,W,W,W,W,W,W,0,0,W,W,0,0,0,W,W,0,0,0,W,W,0,0,W,W,W,W,W,W,0,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},//
			{W,W,W,W,0,0,W,0,0,0,W,0,W,W,W,W,0,W,W,W,W,0,W,0,0,0,0,W,W,W,W,0},
			{0,W,0,0,W,0,W,0,0,0,W,0,0,0,0,W,0,0,0,0,W,0,W,0,0,0,0,W,0,0,0,W},//
			{W,W,0,0,W,0,W,0,0,0,W,0,0,0,W,0,0,0,0,W,0,0,W,0,0,0,0,W,0,0,0,0},
			{0,W,W,W,0,0,W,0,0,0,W,0,0,W,W,0,0,0,W,W,0,0,W,0,0,0,0,W,W,W,W,W},//
			{W,W,0,0,0,0,W,0,0,0,W,0,0,W,0,0,0,0,W,0,0,0,W,0,0,0,0,W,0,0,0,0},//
			{0,W,0,0,0,0,W,0,0,0,W,0,W,0,0,0,0,W,0,0,0,0,W,0,0,0,0,W,0,0,0,W},
			{W,W,0,0,0,0,0,W,W,W,0,0,W,W,W,W,0,W,W,W,W,0,W,W,W,W,0,W,W,W,W,0},//
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,0,0,R,R,R,0,0,R,0,0,R,0,0,R,R,R,0,R,0,0,R,0,0,0,0,R,R,R,R,R,0},
			{0,0,0,R,0,0,R,0,R,0,0,R,0,R,0,0,0,0,R,0,0,R,0,0,0,0,R,R,R,R,R,W},
			{W,0,0,R,R,R,0,0,R,0,0,R,0,0,R,R,0,0,R,R,R,R,0,0,0,0,R,R,R,R,R,0},
			{0,0,0,R,0,0,0,0,R,0,0,R,0,0,0,0,R,0,R,0,0,R,0,0,0,0,R,R,R,R,R,W},
			{W,0,0,R,0,0,0,0,0,R,R,0,0,R,R,R,0,0,R,0,0,R,0,0,0,0,R,R,R,R,R,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,W},
			{W,0,0,R,R,R,0,R,R,R,0,0,R,R,0,0,R,R,R,R,0,R,R,R,0,0,0,0,R,0,0,0},
			{0,0,R,0,0,0,0,0,R,0,0,R,0,0,R,0,R,0,0,R,0,0,R,0,0,0,0,0,R,0,0,W},
			{W,0,0,R,R,0,0,0,R,0,0,R,R,R,R,0,R,R,R,R,0,0,R,0,0,R,R,R,R,R,R,R},//�r�s�`�q�s
			{0,0,0,0,0,R,0,0,R,0,0,R,0,0,R,0,R,R,0,0,0,0,R,0,0,0,R,R,R,R,R,W},
			{W,0,R,R,R,0,0,0,R,0,0,R,0,0,R,0,R,0,R,R,0,0,R,0,0,0,0,R,R,R,0,0},
			{0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,R,W,0,W}
	};


//�t�H���g���
char number_font_data[][8][8]
={	{{0,0,W,W,W,W,0,0},
	{0,W,0,0,0,0,W,0},
	{W,0,0,0,0,W,0,W},
	{W,0,0,0,W,0,0,W},
	{W,0,0,W,0,0,0,W},
	{W,0,W,0,0,0,0,W},
	{0,W,0,0,0,0,W,0},
	{0,0,W,W,W,W,0,0}},

	{{0,0,0,0,W,0,0,0},
	{0,0,0,W,W,0,0,0},
	{0,0,W,W,W,0,0,0},
	{0,0,0,0,W,0,0,0},
	{0,0,0,0,W,0,0,0},
	{0,0,0,0,W,0,0,0},
	{0,0,0,0,W,0,0,0},
	{0,W,W,W,W,W,W,0}},

	{{0,0,W,W,W,W,W,0},
	{0,W,0,0,0,0,0,W},
	{0,W,0,0,0,0,0,W},
	{0,0,0,W,W,W,W,0},
	{0,0,0,0,0,0,0,W},
	{0,W,0,0,0,0,0,W},
	{0,W,0,0,0,0,0,W},
	{0,0,W,W,W,W,W,0}},

	{{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W},
	{W,W,W,W,W,W,W,W}}


};


char *font_address;	//�\���������������̃t�H���g���̐擪�A�h���X




//�y�Ȃ̉����f�[�^
Note melody[][20]={
		{{D5,LEN1},{B4,LEN1},{A4,LEN1},{G4,LEN1},{F4,LEN1},{E4,LEN1},
				{F4,LEN1},{C5,LEN1},{C5,LEN1},
				{B4,LEN1},{A4,LEN1},{G4,LEN1},{F4,LEN1},{F4,LEN1},{E4,LEN1},{D4,LEN1},{C4,LEN1},{C4,LEN1}},
		{{B4,LEN1},{G4,LEN1},{D4,LEN1},{G4,LEN1},{A4,LEN1},{D5,LEN1*2},
				{D4,LEN1},{A4,LEN1},{B4,LEN1},{A4,LEN1},{D4,LEN1},{G4,LEN1*2},{0,0}},
		{{C4,LEN1},{C4,LEN1},{G4,LEN1},{G4,LEN1},{A4,LEN1},{A4,LEN1},{G4,LEN1*2},
				{F4,LEN1},{F4,LEN1},{E4,LEN1},{E4,LEN1},{D4,LEN1},{D4,LEN1},{C4,LEN1*2},{0,0}},
				{{C4,LEN1},{D4,LEN1},{E4,LEN1},{F4,LEN1},{G4,LEN1},{A4,LEN1},{B4,LEN1},{C5,LEN1},{D5,LEN1*2}}
		};

Note* melody_address;	//���t������Ȃ̊y���f�[�^�̐擪�A�h���X




//�֐��̃v���g�^�C�v�錾
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handler���g���҂����ԏ���


void Dyn_Display(void); //O�Nginou_2.c(�I���W�i�������搶�j
//void Copy_RectangleArea_to_Background(char[][],char[][],int,int,char[][],int,int);
void Copy_RectangleArea_to_Background(
		char output_data[32][32],
		char background_data[32][32],
		int  width_of_rectangle_area,
		int  height_of_rectangle_area,
		char rectangle_area_data[width_of_rectangle_area][height_of_rectangle_area],
		int  pos_x,
		int  pos_y);

void Set_Figure_on_Back(char data[32][32],
						char back_data[32][32],
						int  size1,
						int  size2,
						char figure[size1][size2],
						int  pos1,
						int  pos2);


//�u�U�[�o�͊֌W
void play_tone(unsigned int frequency);
void stop_tone();
void play_music(Note data[]);



int main(void){


	my_init_system();
	memcpy(matrix, background_matrix, sizeof(matrix));			//�X�^�[�g��ʂ̕\��
	font_address=number_font_data[0];

	int pos_x, pos_y;	//��`�̈�z�u��̍��W

	while(1){

		while(!up);	//�X�C�b�`�������������܂ő҂B
		up=0;		//�����ꂽ��A�t���O���N���A����B

		switch(sw_now){	//�����ꂽ�X�C�b�`�̏�Ԃɂ���āA�y�ȂƊG���̃f�[�^��ς���B

			case SW1:
				font_address=number_font_data[0];
				pos_x=5; pos_y=10;
				melody_address=melody[0];

				break;

			case SW2:
				font_address=number_font_data[1];
				pos_x=10; pos_y=10;
				melody_address=melody[1];

				break;

			case SW3:
				font_address=number_font_data[2];
				pos_x=15; pos_y=10;
				melody_address=melody[2];
				break;

			case SW4:
				font_address=number_font_data[3];
				pos_x=20; pos_y=10;
				break;

			}


		Copy_RectangleArea_to_Background(
				matrix,
				background_matrix,
				8,8,
				font_address,
				pos_x,pos_y);


//		Set_Figure_on_Back(matrix,
//								background_matrix,
//								8,
//								8,
//								font_address,
//								pos_x,
//								pos_y);



		play_music(melody_address);





	}
}

//
//�n�[�h�E�F�A�̏�����
//

void my_init_system(void)
{

	/* �N���b�N���擾�^�ݒ�ׂ̈̍\���̒�` */
	RCC_ClocksTypeDef RCC_Clocks;

	/* �V�X�e���̃N���b�N���̎擾 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick �^�C�}�̐ݒ� */
	/*SysTick �^�C�}�����݂̊Ԋu�� 1ms �ɐݒ� */
	// ���荞�݊Ԋu��SysTick_Config()�̈�����RCC_Clocks.HCLK_Frequency / 1000Hz�@�Ƃ��āA1msec
	//SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//

	SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQ);	//



	//GPIO�@A,B,C��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN|RCC_APB2ENR_IOPCEN;

	//TIM3 �L�������āAPB0��TIM3�Ƃ���
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;	//TIM3�̗L����


	//�|�[�gA�̐ݒ�
	//			  76543210
	GPIOA->CRL =0x11114411; // PA7-4,1,0 OUT
	//			  FEDCBA98
	GPIOA->CRH =0x44441111; // PA8-11 OUT


	//�|�[�gB�̓��o�͐ݒ�@GPIOB->CRL�ݒ�́A�|�[�gB�̓��o�͐ݒ�@pb0��TIM3�Ƃ���
	//PB0�@CNF=10�AMODE=11�́@�I���^�l�[�g�o�́E�v�b�V���v��
	//PB5-7 OUT
	//			 76543210
	GPIOB->CRL=0x1114444B;

	//			 FEDCBA98
	GPIOB->CRH=0x44444444;



	//�|�[�gC�̓��o�͐ݒ�
	//PB4-7 INPUT
	//			 76543210
	GPIOC->CRL=0x44444444;

	//			 FEDCBA98
	GPIOC->CRH=0x44444444;





    // PWM���[�h��I�����APWM�M�����s���ɏo�͂���
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // �v���X�P�[����ݒ肷��
    TIM3->PSC = 720-1;

    // �^�C�}��L���ɂ���R���g���[�����W�X�^�B�@�^�C�}��ON�EOFF�Ȃǐݒ�
    TIM3->CR1 |= TIM_CR1_CEN;


}











/*
 * �҂����Ԋ֐��@�iSysTick_Handler�ƘA�g�j
 */

void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B


}





/*
**	get_sw --- �P���Ɍ��݂̃X�C�b�`�̒l�𐳘_���i�A�N�e�B�u�n�C�j�Ŏ擾����֐�
*/

int get_sw(void)
{
	int sw;

	sw = GPIOC->IDR;		// ���݂̃X�C�b�`�̒l��ǂݍ���
	sw = ~sw;				// ���_���Ȃ̂Ńr�b�g�𔽓]����
	sw = sw & (SW1|SW2|SW3|SW4);	// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

	return sw;
}

/*
**    read_sw --- �`���^�����O�h�~�@�\�t���̃X�C�b�`�̊m��l��ǂݍ��ފ֐�
*/

int read_sw(void)
{
	static int swc  = 0;	// ����ǂݍ��񂾃X�C�b�`�̒l
	static int swp1 = 0;	// �O��ǂݍ��񂾃X�C�b�`�̒l
	static int swp2 = 0;	// �O�X��ɓǂݍ��񂾃X�C�b�`�̒l
	static int sw   = 0;	// ���݂̃X�C�b�`�̊m��l

	swp2 = swp1;				// �O�X��̒l��ۑ�����
	swp1 = swc;					// �O��̒l��ۑ�����
	swc  = get_sw();			// �V���ɍ���̃X�C�b�`�̒l��ǂݍ���

	if ((swp2 == swp1) && (swp1 == swc)) {	// ����A�O��A�O�X��̒l���S�ē�����
		sw = swc;							// �ꍇ�A����̒l�����݂̊m��l�Ƃ���
	}

	return sw;		// ���݂̃X�C�b�`�̊m��l��Ԃ�
}






///////////////////////////////////////////////////////////////////////////////
//32��32 �_�C�i�~�b�N�_��
void Dyn_Display(void)
{
	static unsigned int row_no;
	static unsigned int color;
	static int i;
	static int j = 0;
	static int line_bits;

	//row_no = ((unsigned int)j << 8) & ROW_NO_MASK;
    line_bits = ((j & 0xc)<<2) |(j & 0x3);
	row_no =  line_bits & ROW_NO_MASK;

	GPIOB->ODR |= OE_;		// �_���I�t

	for (i = 0; i < 32; i++) {
		if (j > 15) {
			color  = (((unsigned int)matrix[j][i]<< 1)& SITA) << 6;	// �������̍s�̐F��ݒ�
		} else {
			color  = (((unsigned int)matrix[j][i])& UE) << 6;		// �㔼���̍s�̐F��ݒ�


		}
		GPIOA->ODR = row_no | color;	// ���M�f�[�^���Z�b�g
		GPIOB->ODR &= ~CLK;				// �N���b�N�̑��M�i���������j
		GPIOB->ODR |= CLK;				// �N���b�N�̑��M�i�����グ�j
	}

	GPIOB->ODR &= ~STB;		// ���b�`�I�t
	GPIOB->ODR |= STB;		// ���b�`�I��
	GPIOB->ODR &= ~OE_;		// �_���I��

	if (j == 31) {
		j = 0;
	} else {
		j++;
	}
}


//��`�̈�̓\��t���i���ʓI�ɁA�����搶�̔ł��R�s�[�j
void Copy_RectangleArea_to_Background(
		char output_data[32][32],
		char background_data[32][32],
		int  width_of_rectangle_area,
		int  height_of_rectangle_area,
		char rectangle_area_data[width_of_rectangle_area][height_of_rectangle_area],
		int  pos_x,
		int  pos_y)
{
	int i, j;

	//�I���W�i���̔w�i����Ɨp��ɃR�s�[
	memcpy(output_data, background_data, sizeof(char)*32*32);

	for (i = 0; i < width_of_rectangle_area; i++) {
		for (j = 0; j < height_of_rectangle_area; j++) {
			if ((0 <= (i+pos_x)) && ((i+pos_x) < 32) &&
			    (0 <= (j+pos_y)) && ((j+pos_y) < 32)) {
				if (rectangle_area_data[i][j] != 0) {
					output_data[i+pos_x][j+pos_y] = rectangle_area_data[i][j];
				}
			}
		}
	}
}

//��`�̈�R�s�[�@�����搶��
//void Set_Figure_on_Back(char data[32][32],
//						char back_data[32][32],
//						int  size1,
//						int  size2,
//						char figure[size1][size2],
//						int  pos1,
//						int  pos2)
//{
//	int i, j;
//
//	memcpy(data, back_data, 1024);
//
//	for (i = 0; i < size1; i++) {
//		for (j = 0; j < size2; j++) {
//			if ((0 <= (i+pos1)) && ((i+pos1) <= 31) &&
//			    (0 <= (j+pos2)) && ((j+pos2) <= 31)) {
//				if (figure[i][j] != 0) {
//					data[i+pos1][j+pos2] = figure[i][j];
//				}
//			}
//		}
//	}
//}



void play_tone(unsigned int frequency) {
    // ���������[�h���W�X�^�Ɣ�r���W�X�^�����g���ɉ����Đݒ肷��
    TIM3->ARR = (72000000 / frequency / 720) - 1;
    TIM3->EGR = TIM_EGR_UG;  // �ǉ�: ���W�X�^�̕ύX�������ɔ��f����
    TIM3->CCR3 = (TIM3->ARR + 1) / 2;  // 50% �f���[�e�B�T�C�N��
}

void stop_tone() {
    // PWM�M�����~����
    TIM3->CCR3 = 0;
}




void play_music(Note data[]){
	int i=0;
	while(data[i].pitch!=0){
		play_tone(data[i].pitch);
		Wait_Time(data[i].duration*25);
		i++;
		//time_data=i;
		stop_tone();
	}

}




/*
 *  * SysTick_Handler�֐�
*/




void SysTick_Handler(void)
{


	static int count_1000ms=0;	//1�b����邽�߂̌Ăяo�����J�E���g����ϐ��@�i1msec*1000��1�b�j

	static int sw_last = 0;	// �O��̃X�C�b�`�̊m��l

	static int dynamic_lighting_counter=0;

	//
	//�X�C�b�`�֌W�̏���
	//
	sw_now = read_sw();			// �X�C�b�`�̊m��l��ǂݍ���

	if (sw_now != sw_last) {	// �O��̃X�C�b�`�̊m��l�ƈقȂ�ꍇ�����ȉ��̏������s��
		up = sw_now & ~sw_last;	// �X�C�b�`�̗����i���ꂩ�̃X�C�b�`�������ꂽ���j�����o
		sw_last = sw_now;		// ���݂̃X�C�b�`�̊m��l�� sw_last �ɕۑ�����
	}


	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	//
	if(TimeInt>0) TimeInt--;


	//1�b�̃J�E���g�A�b�v����
	if(State_ID==RUN){
		if(count_1000ms<SYSTICK_FREQ){
			count_1000ms++;	//SysTick_Handler���Ăяo����邲�ƂɃJ�E���g��1���₷
		}else{
			time_data++;	//SYSTICK_FREQ��ڂ̌Ăяo���ŁA1�b�o�߂��J�E���g�A�b�v����B
			count_1000ms=0;	//SYSTICK_FREQ��ڂŁA�J�E���^�����Z�b�g����B
		}
	}

	//SysTick_Handler�Ăяo����ʍX�V�̎����ݒ�
	if(dynamic_lighting_counter == 10){
		dynamic_lighting_counter=0;
		Dyn_Display();
	}else{
		dynamic_lighting_counter++;
	}
}


