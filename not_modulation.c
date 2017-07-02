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
#define TEST 100000                            //テスト回数

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

	int sum000[259], sum001[259], sum010[259], sum011[259], sum100[259], sum101[259], sum110[259], sum111[259];
	int pre000[259], pre001[259], pre010[259], pre011[259], pre100[259], pre101[259], pre110[259], pre111[259];
	int dh1 = 0, dh2 = 0, rh1 = 0, rh2 = 0;
	int state = 0;

	srand((unsigned)time(NULL));

	printf("# SNR BER\n");
	for(sn = 0.0; sn <= 6.0; sn += 0.25) {
		ok = error = 0;
		i = 0;

		while (i < TEST) {   // 誤りが200回起きるまで，または100000回繰り返すまで
			m[0] = 0; m[1] = 0; m[2] = 0; m[3] = 0;
			for (j = 0; j < 259; j++) {
				tdata[j] = 0;
				rdata[j] = 0;
				tcode[2*j] = 0;
				tcode[2*j+1] = 0;
				rcode[2*j] = 0;
				rcode[2*j+1] = 0;
			}
			
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

			/* BPSK変調 */
			for (j = 0; j < LT3; j++) {
				transmit[j] = -1.0;
				if(tdata[j])
					transmit[j] = 1.0;
			}

			/* 伝送 */
			for (j = 0; j < LT3; j++){
				receive[j] = transmit[j] + awgn(sn);
			}

			/* BPSK復調 */
			for (j = 0; j < LT3; j++) {
				rdata[j] = 0;
				if(receive[j] > 0)
					rdata[j] = 1;
			}

			/* ビタビ復号 */

			/* 誤り回数計算 */
			for (j = 0; j < LENGTH; j++){
				if (rdata[j] == tdata[j]){
					ok++;
				} else {
					error++;
				}
			}
			//printf("error : %ld\n",error);
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
	double z = sqrt(-2.0*log(RAND)) * sin( 2.0 * M_PI * RAND );
	var = sqrt(0.5*pow(10, -sn/10));
	noise =  var * z;

	return noise;
}
