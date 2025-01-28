/*
 * LED_Matrix_32x32_Test02-OkunProgram_AndouSenseiORGINAL2.c
 *
 *  Created on: 2023/10/01
 *      Author: x-lib
 */

/*
 *
 * SW1~4の押下に応じて、画面表示と楽曲を変える。
 *
 *
 *令和4年度修了生　O君　ＬｉｎｅＰｕｚｚｌｅのルーチンを流用(オリジナル安藤先生）
 *　ポーティング作業の不手際）PAのコンフィギュレーションレジスタの誤値上書きに気づかず、時間を浪費
 *
 *
 *　10月1日夜で動作不良。
 *　10月2日夜で動作不良。矩形領域のコピーがうまくいかない。
 *　10月③日朝、安藤先生の関数。矩形領域のコピーがうまくいかない不具合は治った。
 */

#include <stddef.h>
#include <string.h>
#include "stm32f10x.h"




//各種シンボル定義

//３２ｘ３２LEDマトリクス点灯制御関係の定数

//点灯色のデータ
#define R 0x1								//00 00 00 01		0000 0001
#define G 0x4								//00 00 01 00		0000 0100
#define Y 0x5								//00 00 01 01
#define B 0x10								//00 01 00 00		0001 0000
#define M 0x11								//00 01 00 01
#define C 0x14								//00 01 01 00
#define W 0x15								//00 01 01 01

//信号線
#define UE 0x15								//00 01 01 01
#define SITA 0x2A							//00 10 10 10
#define ROW_NO_MASK 0x033					//00 11 00 11


//ポートの設定

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

//圧電スピーカー
#define SPK			0x01			//PB0		0000 0000 0000 0001

//スイッチ

//          0bFEDCBA9876543210
#define SW1 0b0000000000010000	//Sw1(PC4)用のマスク値
#define SW2 0b0000000000100000	//Sw2(PC5)用のマスク値
#define SW3 0b0000000001000000	//Sw3(PC6)用のマスク値
#define SW4 0b0000000010000000	//Sw4(PC7)用のマスク値


//状態の情報
#define	STOP	0	//停止状態
#define RUN		1	//カウント状態


//TIMによるブザー音階データ
#define C4 523 //ど
#define D4 587 //れ
#define E4 659 //み
#define F4 698 //ふぁ
#define G4 783 //そ
#define A4 880 //ら
#define B4 987 //し
#define C5 1046 //ど
#define D5 1174 //れ

#define LEN1	500
#define LEN		60

//音符データの構造体
typedef struct{
	int pitch;
	int duration;
} Note;



//SysTick_Handler呼び出し間隔
#define SYSTICK_FREQ	30000




//グローバル変数

//int line_bits=0;




int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）

int time_data = 0;	//時間データ


int State_ID=STOP;


//スイッチ関係の変数
int up=0;		//スイッチの立ち上がり検出フラグ、0で初期化
int sw_now=0;	//現在のスイッチ押下状態、0で初期化




//Dyn_Display関数はこの領域のデータをLEDマトリクスに表示する。
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


//スタート画面
 char prob0_matrix[32][32] ={
			{W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0},
			{0,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,W},
			{W,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,0,0,0,W,W,0,0,W,W,W,W,W,W,0,W},//
			{W,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,W,0,0,W,W,0,0,W,W,0,0,0,0,0,0},
			{0,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,W,W,0,W,W,0,0,W,W,0,0,0,0,0,W},//
			{W,0,W,W,0,0,0,0,0,0,W,W,0,0,0,W,W,0,W,W,W,W,0,0,W,W,W,W,W,W,0,0},//LINE　ＰＵＺＺＬＥ
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
			{W,0,0,R,R,0,0,0,R,0,0,R,R,R,R,0,R,R,R,R,0,0,R,0,0,R,R,R,R,R,R,R},//ＳＴＡＲＴ
			{0,0,0,0,0,R,0,0,R,0,0,R,0,0,R,0,R,R,0,0,0,0,R,0,0,0,R,R,R,R,R,W},
			{W,0,R,R,R,0,0,0,R,0,0,R,0,0,R,0,R,0,R,R,0,0,R,0,0,0,0,R,R,R,0,0},
			{0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,0,W,R,W,0,W}
	};


//フォント情報
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


char *font_address;	//表示させたい文字のフォント情報の先頭アドレス




//楽曲の音符データ
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

Note* melody_address;	//演奏させる曲の楽譜データの先頭アドレス




//関数のプロトタイプ宣言
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handlerを使う待ち時間処理


void Dyn_Display(void); //O君ginou_2.c(オリジナル安藤先生）
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


//ブザー出力関係
void play_tone(unsigned int frequency);
void stop_tone();
void play_music(Note data[]);



int main(void){


	my_init_system();
	memcpy(matrix, background_matrix, sizeof(matrix));			//スタート画面の表示
	font_address=number_font_data[0];

	int pos_x, pos_y;	//矩形領域配置先の座標

	while(1){

		while(!up);	//スイッチが何か押されるまで待つ。
		up=0;		//押されたら、フラグをクリアする。

		switch(sw_now){	//押されたスイッチの状態によって、楽曲と絵柄のデータを変える。

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
//ハードウェアの初期化
//

void my_init_system(void)
{

	/* クロック情報取得／設定の為の構造体定義 */
	RCC_ClocksTypeDef RCC_Clocks;

	/* システムのクロック情報の取得 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick タイマの設定 */
	/*SysTick タイマ割込みの間隔を 1ms に設定 */
	// 割り込み間隔はSysTick_Config()の引数をRCC_Clocks.HCLK_Frequency / 1000Hz　として、1msec
	//SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//

	SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQ);	//



	//GPIO　A,B,Cを有効化
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN|RCC_APB2ENR_IOPCEN;

	//TIM3 有効化して、PB0をTIM3とする
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;	//TIM3の有効化


	//ポートAの設定
	//			  76543210
	GPIOA->CRL =0x11114411; // PA7-4,1,0 OUT
	//			  FEDCBA98
	GPIOA->CRH =0x44441111; // PA8-11 OUT


	//ポートBの入出力設定　GPIOB->CRL設定は、ポートBの入出力設定　pb0をTIM3とする
	//PB0　CNF=10、MODE=11は　オルタネート出力・プッシュプル
	//PB5-7 OUT
	//			 76543210
	GPIOB->CRL=0x1114444B;

	//			 FEDCBA98
	GPIOB->CRH=0x44444444;



	//ポートCの入出力設定
	//PB4-7 INPUT
	//			 76543210
	GPIOC->CRL=0x44444444;

	//			 FEDCBA98
	GPIOC->CRH=0x44444444;





    // PWMモードを選択し、PWM信号をピンに出力する
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // プリスケーラを設定する
    TIM3->PSC = 720-1;

    // タイマを有効にするコントロールレジスタ。　タイマのON・OFFなど設定
    TIM3->CR1 |= TIM_CR1_CEN;


}











/*
 * 待ち時間関数　（SysTick_Handlerと連携）
 */

void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}





/*
**	get_sw --- 単純に現在のスイッチの値を正論理（アクティブハイ）で取得する関数
*/

int get_sw(void)
{
	int sw;

	sw = GPIOC->IDR;		// 現在のスイッチの値を読み込む
	sw = ~sw;				// 負論理なのでビットを反転する
	sw = sw & (SW1|SW2|SW3|SW4);	// スイッチ以外のビットをマスクする

	return sw;
}

/*
**    read_sw --- チャタリング防止機能付きのスイッチの確定値を読み込む関数
*/

int read_sw(void)
{
	static int swc  = 0;	// 今回読み込んだスイッチの値
	static int swp1 = 0;	// 前回読み込んだスイッチの値
	static int swp2 = 0;	// 前々回に読み込んだスイッチの値
	static int sw   = 0;	// 現在のスイッチの確定値

	swp2 = swp1;				// 前々回の値を保存する
	swp1 = swc;					// 前回の値を保存する
	swc  = get_sw();			// 新たに今回のスイッチの値を読み込む

	if ((swp2 == swp1) && (swp1 == swc)) {	// 今回、前回、前々回の値が全て等しい
		sw = swc;							// 場合、今回の値を現在の確定値とする
	}

	return sw;		// 現在のスイッチの確定値を返す
}






///////////////////////////////////////////////////////////////////////////////
//32ｘ32 ダイナミック点灯
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

	GPIOB->ODR |= OE_;		// 点灯オフ

	for (i = 0; i < 32; i++) {
		if (j > 15) {
			color  = (((unsigned int)matrix[j][i]<< 1)& SITA) << 6;	// 下半分の行の色を設定
		} else {
			color  = (((unsigned int)matrix[j][i])& UE) << 6;		// 上半分の行の色を設定


		}
		GPIOA->ODR = row_no | color;	// 送信データをセット
		GPIOB->ODR &= ~CLK;				// クロックの送信（立ち下げ）
		GPIOB->ODR |= CLK;				// クロックの送信（立ち上げ）
	}

	GPIOB->ODR &= ~STB;		// ラッチオフ
	GPIOB->ODR |= STB;		// ラッチオン
	GPIOB->ODR &= ~OE_;		// 点灯オン

	if (j == 31) {
		j = 0;
	} else {
		j++;
	}
}


//矩形領域の貼り付け（結果的に、安藤先生の版をコピー）
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

	//オリジナルの背景を作業用域にコピー
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

//矩形領域コピー　安藤先生版
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
    // 自動リロードレジスタと比較レジスタを周波数に応じて設定する
    TIM3->ARR = (72000000 / frequency / 720) - 1;
    TIM3->EGR = TIM_EGR_UG;  // 追加: レジスタの変更をすぐに反映する
    TIM3->CCR3 = (TIM3->ARR + 1) / 2;  // 50% デューティサイクル
}

void stop_tone() {
    // PWM信号を停止する
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
 *  * SysTick_Handler関数
*/




void SysTick_Handler(void)
{


	static int count_1000ms=0;	//1秒を作るための呼び出しをカウントする変数　（1msec*1000回＝1秒）

	static int sw_last = 0;	// 前回のスイッチの確定値

	static int dynamic_lighting_counter=0;

	//
	//スイッチ関係の処理
	//
	sw_now = read_sw();			// スイッチの確定値を読み込む

	if (sw_now != sw_last) {	// 前回のスイッチの確定値と異なる場合だけ以下の処理を行う
		up = sw_now & ~sw_last;	// スイッチの立上り（何れかのスイッチが押されたか）を検出
		sw_last = sw_now;		// 現在のスイッチの確定値を sw_last に保存する
	}


	//
	//待ち時間関数　Wait_Time()用の変数の処理
	//
	if(TimeInt>0) TimeInt--;


	//1秒のカウントアップ処理
	if(State_ID==RUN){
		if(count_1000ms<SYSTICK_FREQ){
			count_1000ms++;	//SysTick_Handlerが呼び出されるごとにカウントを1増やす
		}else{
			time_data++;	//SYSTICK_FREQ回目の呼び出しで、1秒経過をカウントアップする。
			count_1000ms=0;	//SYSTICK_FREQ回目で、カウンタをリセットする。
		}
	}

	//SysTick_Handler呼び出し画面更新の周期設定
	if(dynamic_lighting_counter == 10){
		dynamic_lighting_counter=0;
		Dyn_Display();
	}else{
		dynamic_lighting_counter++;
	}
}


