//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #2: FP12 (12-bit floating point) Representation
//
//  September 28, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

#include "pa2.h"

/* Convert 32-bit signed integer to 12-bit floating point */
fp12 int_fp12(int n)
{	
	fp12 result = 0;
	if(n==0){return result;}
	if(n<0){
		result += 0xF800; // 앞 sign 5 bits에 1 넣기. 1111100000000000
		n -= 1;			// two's complement 양수화
		n = ~n; } 
	//exp 부터 구하기. 넘으면 inf로 가니까. 근데 여기선 안 감.
	fp12 m = 0;
	int msb = 0x80000000;
	while ((n&msb) != msb){m++;n<<=1;}
	 
	fp12 exp = 62-m;
	//if (m<0){result += 2016;return result;} // infinity. 11111100000

	// fraction, and rounding.
	char L = ((n&0x04000000)>0) ? 1:0;
	char R = ((n&0x02000000)>0) ? 1:0;
	if (L==0){ 
		if(R==1&&((n&0x01FFFFFF)>0)){ n+=0x04000000;}			
		}
	else{if(R==1){n+=0x04000000;
			if((n&0x80000000)==0){exp+=1;}}
	}
	fp12 frac = (fp12)((n&0x7C000000)>>26);
	exp <<= 5;
	result += exp;
	result += frac;

	return result; // return 16 bits fp12
}

union Typo{
	int i;
	unsigned int ui;
};
/* Convert 12-bit floating point to 32-bit signed integer */
int fp12_int(fp12 x)
{ 	
	union Typo result;
	result.ui = 0 ;
	fp12 exp12 = (x&0x07E0)>>5;    //float 12bit에서의 exponent

	if(exp12==0x003F){return 0x80000000;} //exp==111111 는 NaN 아님 inf 이므로
	if(exp12==0x0000){return 0;}
	exp12 -= 31;
	if (exp12<0){return 0;} // 0.xxx.. 
	
	fp12 frac = (x&0x001F)+32; // 1xxxxx

	if ((x&0x8000)!=0){
		if(exp12>31||(exp12==31&&frac>32))
			return 0x80000000;
	}else{
		if(exp12>30) return 0x80000000;
	}
	
	if(exp12<=5){		
		result.ui += (frac>>(5-exp12));
	}
	else{	
		result.ui += frac<<(exp12-5);
	}
	if((x&0x8000)!=0){result.ui= ~(result.ui) +1;}

	return result.ui;
	}

union Type {
	float fl;
	unsigned int in;
};


/* Convert 32-bit single-precision floating point to 12-bit floating point */
fp12 float_fp12(float f)
{	
	fp12 result=0;

	union Type t;
	t.fl = f;
	if(((t.in>>31)&1)==1){result+=0xF800;} // 음수일 경우 sign 5 bits
	if(f==0){return result;}  // 0일 경우 

	unsigned int exp32 = (t.in&0x7F800000) >>23;    //float 32bit에서의 exponent
	if(exp32==0x000000FF){
		result += 0x07E0;    // exp = 111111
		if((t.in<<9)!=0){return result+1;}	// NaN
		else{return result;}  				// infinity
		} 
	short exp = exp32 - 127;                      // real exponent can be minus. 그러나 실제는 unsigned. 
	fp12 exp12;
	


	unsigned int buff; // fraction part
	//rounding
	
///여기서부터
	if(exp==-36||exp==-31){
		buff = (t.in&0x007FFFFF) + (1<<23);
		unsigned char R = (buff>>(-exp-13))&1;    
		unsigned char L = (buff>>(-exp-12))&1;
		
		if (L==0){ 
			if(R==1&&(buff<<(32-(-exp-13)))>0){
				buff+=(1<<(-exp-13));			
		}}
		else{if(R==1){buff+=(1<<(-exp-13));    // 뒤에 자르는 갯수가 다름..  여기서 만약 안 된다면 0으로 감.
		}
	}
		if((buff&(1<<23))==0){
			buff >>= 1;
			exp += 1;
	}

	}
	else if(-35<=exp&&exp<=-32){ 
							      //denormalized fp12 where exp12 = 000000
		buff = (t.in&0x007FFFFF) + (1<<23);
		unsigned char R = (buff>>(-exp-13))&1;   
		unsigned char L = (buff>>(-exp-12))&1;
		
		if (L==0){ 
			if(R==1&&(buff<<(32-(-exp-13)))>0){
				buff+=(1<<(-exp-13));			
		}}
		else{if(R==1){buff+=(1<<(-exp-13));    // 뒤에 자르는 갯수가 다름..  여기서 만약 안 된다면 0으로 감.
		}
	}

	}else if(-31<exp&&exp<=31){ //  
							// normalized float.
		buff = (t.in&0x007FFFFF) + (1<<23);
		unsigned char R = (buff>>17)&1;
		unsigned char L = (buff>>18)&1;		
							// 자르는 개수 5개로 일정.   
		if (L==0){ 
			if(R==1&&(buff&0x0001FFFF)>0){
				buff+=(1<<18);			
		}}
		else{if(R==1){buff+=(1<<18);
		}
	}
		if((buff&(1<<23))==0){   // RE-NORM
		buff >>= 1;
		exp += 1;
	}
	}


	if(-35<=exp&&exp<=-31){   //denormalized fraction
		buff >>= (-exp-12);    // 뒤에서 bits 가져오기
		exp12 = 0;
	}else if(-30<=exp&&exp<=31){
		buff >>= 18;    // 앞에서 5bits 가져오기
		buff -= (1<<5);  // 1 제거
		exp12 = exp + 31;
	}else if(exp<-35){
		return result;   // zero
	}else{
		exp12 = 0x07E0;
		return result+exp12;  //infinity
		
	}
	
	return result + (exp12<<5) + (fp12)buff;
	}

union Type2{
	float f;
	unsigned int i;
};

/* Convert 12-bit floating point to 32-bit single-precision floating point */
float fp12_float(fp12 x)
{
	
	union Type2 result; 
	result.i = 0;		//result f 를 출력할 것이다!
	
	if(((x>>11)&1)==1){result.i += (1<<31);} // 음수일 경우 sign 1 bits

	fp12 exp12 = (x&0x07E0)>>5;    //float 12bit에서의 exponent
	fp12 frac = (x&0x001F);
											
	if(exp12==0){							// exp = 000000
		if(frac==0){return result.f;}		// 0
		else{								// denormalized num
			char m =0;
			while((frac&0x0010) ==0){
				m++;
				frac <<=1;
			}
			frac <<=1;
			result.i += (frac&0x1F)<<18;
			result.i += (-31-m +127)<<23;
			
		}
	}else if(exp12==0x003F){					// exp = 111111
		result.i += (0x000000FF<<23);
		if(frac!=0){
			result.i +=1;
				// NaN
		}
	}else{
		unsigned int exp32 = exp12-31+127;
		result.i += exp32<<23;
		result.i += ((unsigned int)frac) <<18;

	}
	
	
	return result.f;
}
