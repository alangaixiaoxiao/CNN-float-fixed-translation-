#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>

#define CNT              25
#define SIGN_BIT         0x8000
#define SIGN_BIT_IGNORE  0x7fff

short float2fixed_fun(float fdata, int Qn);
float fixed2float_fun(short sdata, int Qn);
short multi_fixed(short data1, short data2, int Qn);
short add_fixed(short data1, short data2);

int main()
{
	//=====================================================test
	//float value;
	//float *var = &value;
	//int value_int;
	//int *var_int = &value_int;

	//scanf("%f", var);
	//scanf("%d", var_int);

	//printf("%x\n", *((int*)var));//%x 16进制输出整数
	//printf("%x\n", *var_int);
	//=======================================================
	float dat1[CNT], dat2[CNT], result[CNT],result_short_translate[CNT];
	short   dat1_short[CNT], dat2_short[CNT], result_short[CNT];
	int i;
	FILE *fp;
	fp = fopen("filter1.dat", "r");
	for (i = 0; i < CNT; i++){
		fscanf(fp, "%f\n", &dat1[i]);
	}
	fclose(fp);
	fp = fopen("filter2.dat", "r");
	for (i = 0; i < CNT; i++){
		fscanf(fp, "%f\n", &dat2[i]);
	}
	fclose(fp);
	for (i = 0; i < CNT; i++){
		result[i] = dat1[i] + dat2[i];
	}
	for (i = 0; i < CNT; i++){
		dat1[i] = float2fixed_fun(dat1[i],14);
		dat2[i] = float2fixed_fun(dat2[i], 14);
		//result_short[i] = multi_fixed(dat1[i], dat2[i],14);
		result_short[i] = add_fixed(dat1[i], dat2[i]);
		result_short_translate[i] =  fixed2float_fun(result_short[i],14);
	}
	for (i = 0; i < CNT; i++){
		printf("%f  //******************// %f\n", result[i], result_short_translate[i]);
	}

    //===============================================================================test
	//short a = multi_fixed(float2fixed_fun(-0.125, 14), float2fixed_fun(-0.8, 14),14);
	//short b = add_fixed(float2fixed_fun(-0.125, 14), float2fixed_fun(0.8, 14));
	//printf("0x%x\n", float2fixed_fun(-0.125,14));
	//printf("0x%x\n", float2fixed_fun(-0.8, 14));
	//printf("%f\n", fixed2float_fun(b, 14));
	//===============================================================================
	//printf("the resutlt is: ");
	//for (i = 0; i < CNT; i++){
	//	printf("%f\n", result[i]);
	//}

	return 0;

}

//=========================================================16bit量化
short float2fixed_fun(float fdata, int Qn){
	short sdata;
	int temp;
	int integer = 2 << (Qn - 1);

	if (fdata > 0){
		temp = int((fdata * integer));
		sdata = (temp == short(temp)) ? temp : (temp >> 31) ^ 0x7fff;
	}
	else if (fdata < 0){
		fdata = -fdata;
		temp = int((fdata * integer));
		sdata = (temp == short(temp)) ? temp : (temp >> 31) ^ 0x7fff;
		sdata = sdata ^ SIGN_BIT;
	}
	else
		sdata = 0;
	return sdata;
}

float fixed2float_fun(short sdata, int Qn){
	int sign_flag = sdata & SIGN_BIT;//1->负数 0->正数
	float fdata,temp;
	short temp1;
	int integer = 2 << (Qn - 1);

	if(sign_flag == 0){ //该定点数为正
		fdata = float(sdata)/integer;
	}
	else{
		temp1 = sdata ^ SIGN_BIT;		
		//temp = float(sdata ^ SIGN_BIT);直接使用此语句，结果不正确，不知为何
		temp = float(temp1);
		fdata = temp / integer;
		fdata = -fdata;
	}
	return fdata;

}

short multi_fixed(short data1,short data2 ,int Qn){
	short result_data;
	int sign_flag1, sign_flag2;
	int temp;
	short temp_short;
	sign_flag1 = data1 &  SIGN_BIT;
	sign_flag2 = data2 &  SIGN_BIT;
	temp = (data1 & SIGN_BIT_IGNORE)*(data2 & SIGN_BIT_IGNORE);//忽视最高位进行运算
	temp = temp >> Qn;
	if (sign_flag1 ^ sign_flag2){//不为0，说明两个乘积的结果为负数；
		temp_short = temp;
		result_data = temp_short ^ SIGN_BIT;
	}
	else{
		result_data = temp;
	}
	return result_data;
}

short add_fixed(short data1, short data2){
	short result_data;
	int sign_flag1, sign_flag2;
	short temp_short,data_exp1,data_exp2;
	sign_flag1 = data1 &  SIGN_BIT;//取符号位
	sign_flag2 = data2 &  SIGN_BIT;
	data_exp1 = data1 & SIGN_BIT_IGNORE;
	data_exp2 = data2 & SIGN_BIT_IGNORE;
	if (sign_flag1 ^ sign_flag2){//不为0，说明两个加数的符号不相同；
		if (data_exp1 > data_exp2){
			temp_short = data_exp1 - data_exp2;
			result_data = temp_short ^ sign_flag1;
		}
		else{
			temp_short = data_exp2 - data_exp1;
			result_data = temp_short ^ sign_flag2;
		}	   
	}
	else{
		temp_short = data_exp1  +  data_exp2;
		result_data = temp_short ^ sign_flag1;//取任意一个的符号
	}
	return result_data;
}