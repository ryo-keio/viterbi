// 大槻研究室B3春輪講
// ビタビアルゴリズム実装演習

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define RAND ((double)rand() / RAND_MAX)    // 0から1までの擬似乱数
#define PI 3.141592653

// 以下の変数値は各自指定された環境によって変更すること
#define S_REG 3                             // シフトレジスタの数
#define LENGTH 259                          // 2の8乗 + 3(シフトレジスタの数)
#define REVERSECODERATE 2							// 符号化率の逆数
#define CONSTRAINTLENGTH 4					//拘束長
#define CL2 REVERSECODERATE * CONSTRAINTLENGTH
#define LT3 LENGTH * REVERSECODERATE                  // 3はシフトレジスタの数に基づく．適宜変数名変更すること
// 最初はバグのことも考慮してテスト回数は小さめの値から始めること
#define TEST 1                            //テスト回数

double awgn(double);

int main() {    
	int tdata[LENGTH], tcode[LT3], rcode[LT3], rdata[LENGTH], gpolynomial[CL2] = {1,1,1,1,0,1,1,1}, m[CONSTRAINTLENGTH];
	// tdata...符号化前の送信データ
	// tcode...符号化後の送信データ
	// rcode...復号化前の送信データ
	// rdata...復号化後の送信データ

	double sn, ber, transmit[LT3], receive[LT3];
	// transmit[]...送信信号
	// receive[] ...受信信号
	int i, j, k;
	long ok = 0, error = 0;

	srand((unsigned)time(NULL));

	printf("# SNR BER\n");
	for(sn = 0.0; sn <= 0.2; sn += 0.25) {
		m[0] = 0; m[1] = 0; m[2] = 0; m[3] = 0;
		ok = error = 0;
		i = 0;

		while (i < TEST) {   // 誤りが200回起きるまで，または100000回繰り返すまで
			i++;
			// 送信データの生成
			for (j = 0; j < LENGTH - 3; j++){
				if (RAND < 0.5) {
					tdata[j] = 0;
				} else {
					tdata[j] = 1;
				}
			}

			/* 終端ビット系列の付加 */
			tdata[LENGTH - 3] = 0;
			tdata[LENGTH - 2] = 0;
			tdata[LENGTH - 1] = 0;

			/* 畳み込み符号化 */
			for (j = 0; j < LENGTH; j++) {
				m[0] = tdata[j];
				for (k = 0; k < CONSTRAINTLENGTH; k++) {
					if(gpolynomial[2*k])
						tcode[2*j] ^= m[k];
					if(gpolynomial[2*k +1])
						tcode[2*j + 1] ^= m[k];
				}
				for (k = CONSTRAINTLENGTH - 1; k >= 1; k--)
					m[k] = m[k-1];
			}

			/* BPSK変調 */
			for (j = 0; j < LT3; j++) {
				transmit[j] = -1.0;
				if(tcode[j])
					transmit[j] = 1.0;
			}

			/* 伝送 */
			for (j = 0; j < LT3; j++){
				receive[j] = transmit[j] + awgn(sn);
			}

			/* BPSK復調 */
			for (j = 0; j < LT3; j++) {
				rcode[j] = 0;
				if(receive[j] > 0)
					rcode[j] = 1;
			}

			/* ビタビ復号 */
			int sum000[259], sum001[259], sum010[259], sum011[259], sum100[259], sum101[259], sum110[259], sum111[259];
			int pre000[259], pre001[259], pre010[259], pre011[259], pre100[259], pre101[259], pre110[259], pre111[259];
			sum000[0] = rcode[2*0] + rcode[2*0 + 1];
			sum001[0] = (1-rcode[2*0]) + (1-rcode[2*0 + 1]);
			pre000[0] = 0;
			pre001[0] = 0;
			sum000[1] = sum000[0] + rcode[2*1] + rcode[2*1 + 1]; 
			sum001[1] = sum000[0] + (1-rcode[2*1]) + (1-rcode[2*1 + 1]);
			sum010[1] = sum001[0] + (1-rcode[2*1]) + (1-rcode[2*1 + 1]); 
			sum011[1] = sum001[0] + rcode[2*1] + rcode[2*1 + 1];
			pre000[1] = 0;
			pre001[1] = 0;
			pre010[1] = 4;
			pre011[1] = 4;
			sum000[2] = sum000[1] +    rcode[2*2]  + rcode[2*2 + 1]; 
			sum001[2] = sum000[1] + (1-rcode[2*2]) + (1-rcode[2*2 + 1]);
			sum010[2] = sum001[1] + (1-rcode[2*2]) + (1-rcode[2*2 + 1]); 
			sum011[2] = sum001[1] +    rcode[2*2]  + rcode[2*2 + 1];
			sum100[2] = sum010[1] +    rcode[2*2]  + (1-rcode[2*2 + 1]);
			sum101[2] = sum010[1] + (1-rcode[2*2]) + rcode[2*2 + 1];
			sum110[2] = sum011[1] + (1-rcode[2*2]) + rcode[2*2 + 1];
			sum111[1] = sum011[1] +    rcode[2*2]  + (1-rcode[2*2 + 1]);
			pre000[2] = 0;
			pre001[2] = 0;
			pre010[2] = 4;
			pre011[2] = 4;
			pre100[2] = 2;
			pre101[2] = 2;
			pre110[2] = 6;
			pre111[2] = 6;

			int dh1, dh2, rh1, rh2;
			for (j = 3; j < 259; j++) {
				dh1 = rcode[2*j];
				dh2 = rcode[2*j+1];
				rh1 = 1-rcode[2*j];
				rh2 = 1-rcode[2*j+1];

				sum000[j] = (sum000[j-1] + dh1 + dh2) < (sum100[j-1] + rh1 + rh2) ? (sum000[j-1] + dh1 + dh2) : (sum100[j-1] + rh1 + rh2);
				sum001[j] = (sum000[j-1] + rh1 + rh2) < (sum100[j-1] + dh1 + dh2) ? (sum000[j-1] + rh1 + rh2) : (sum100[j-1] + dh1 + dh2); 
				sum010[j] = (sum001[j-1] + rh1 + rh2) < (sum101[j-1] + dh1 + dh2) ? (sum001[j-1] + rh1 + rh2) : (sum101[j-1] + dh1 + dh2);
				sum011[j] = (sum001[j-1] + dh1 + dh2) < (sum101[j-1] + rh1 + rh2) ? (sum001[j-1] + dh1 + dh2) : (sum101[j-1] + rh1 + rh2);
				sum100[j] = (sum010[j-1] + dh1 + rh2) < (sum110[j-1] + rh1 + dh2) ? (sum010[j-1] + dh1 + rh2) : (sum110[j-1] + rh1 + dh2);
				sum101[j] = (sum010[j-1] + rh1 + dh2) < (sum110[j-1] + dh1 + rh2) ? (sum010[j-1] + rh1 + dh2) : (sum110[j-1] + dh1 + rh2);
				sum110[j] = (sum011[j-1] + rh1 + dh2) < (sum111[j-1] + dh1 + rh2) ? (sum011[j-1] + rh1 + dh2) : (sum111[j-1] + dh1 + rh2);
				sum111[j] = (sum011[j-1] + dh1 + rh2) < (sum111[j-1] + rh1 + dh2) ? (sum011[j-1] + dh1 + rh2) : (sum111[j-1] + rh1 + dh2);

				pre000[j] = (sum000[j-1] + dh1 + dh2) < (sum100[j-1] + rh1 + rh2) ? 0 : 1;
				pre001[j] = (sum000[j-1] + rh1 + rh2) < (sum100[j-1] + dh1 + dh2) ? 0 : 1;
				pre010[j] = (sum001[j-1] + rh1 + rh2) < (sum101[j-1] + dh1 + dh2) ? 4 : 5;
				pre011[j] = (sum001[j-1] + dh1 + dh2) < (sum101[j-1] + rh1 + rh2) ? 4 : 5;
				pre100[j] = (sum010[j-1] + dh1 + rh2) < (sum110[j-1] + rh1 + dh2) ? 2 : 3; 
				pre101[j] = (sum010[j-1] + rh1 + dh2) < (sum110[j-1] + dh1 + rh2) ? 2 : 3; 
				pre110[j] = (sum011[j-1] + rh1 + dh2) < (sum111[j-1] + dh1 + rh2) ? 6 : 7; 
				pre111[j] = (sum011[j-1] + dh1 + rh2) < (sum111[j-1] + rh1 + dh2) ? 6 : 7; 
			}
			int state = 0;
			for (j = 258; j >= 0; j--) {
				rdata[j] = (state == 0 || state == 1 || state == 2 || state == 3) ? 0 : 1;
				state = (state==0) ? pre000[j] : ((state==1) ? pre100[j] : ((state==2) ? pre010[j] : ((state==3) ? pre110[j] : ((state==4) ? pre001[j] : ((state==5) ? pre101[j] : ((state==6) ? pre011[j]: pre111[j]))))));
			}

			/* 誤り回数計算 */
			for (j = 0; j < LENGTH; j++){
				if (rdata[j] == tdata[j]){
					ok++;
				} else {
					error++;
				}
			}
			printf("error : %ld\n",error);
		}

		/* BER計算 */
		ber = (double)error / (double)(ok + error);

		/* 結果表示 */
		printf("%f, %e\n", sn, ber);
	}
}

/**
 * 関数awgnはSN比[dB]を基に白色ガウス雑音の大きさを返す．
 * ガウス分布に従う正規乱数
 */
double awgn(double sn) {
	double var;//snより雑音分散を計算する
	double noise = 0.0;


	//ガウス分布にしたがう乱数を発生させる
	//

	return noise;
}
