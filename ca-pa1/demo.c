#include <stdio.h>
/* TODO: Implement this function */
struct ranking {
		int num;
		char symbol ;
		char code;
		char len;
	};

int encode(const char *inp, int inbytes, char *outp, int outbytes)
{	
	if (inbytes==0){
		return 0;
	}
	//  0. 8 bits to 4 bits, store them.
	char tmp[2*inbytes];
	char back = 0xF; 
    char front = 0xF0;
	for (int i=0;i<inbytes;i++){
        
		
        char backy = inp[i] & back;
        char fronty = inp[i] & front;
        fronty >>= 4;
        fronty &= back;
		tmp[2*i] = fronty;
		tmp[2*i+1] = backy;
	}   
    
	//  1. count the symbol int count[16];
	int count[16];
	for (int i=0;i<16;i++) { count[i]= 0 ;}  // initialize with 0
	for (int i=0;i<2*inbytes;i++) { count[tmp[i]]+=1;}
    
    
	//	2. rank them
	

	struct ranking rank[16];
	for (int i=0;i<16;i++){
		rank[i].num = count[i];
		rank[i].symbol = (char)i;
		
	}
	
// 0~7

    struct ranking temp;

	for (int i=0;i<16;i++){
		for (int j=15;j>i;j--){
			if (rank[j].num > rank[j-1].num){              
				temp = rank[j];
				rank[j] = rank[j-1];
				rank[j-1] = temp;
			}
			else if(rank[j].num==rank[j-1].num){               
				if (rank[j].symbol < rank[j-1].symbol){                   
					temp = rank[j];
					rank[j] = rank[j-1];
					rank[j-1] = temp;

				}}}}
    // 8~15	
	for (int i=15;i>8;i--){
		for (int j=8;j<i;j++){
			if(rank[j].symbol>rank[j+1].symbol){
				temp = rank[j];
				rank[j] = rank[j+1];
				rank[j+1] = temp;
			}}}

	//	3. match the huffman coding and encode.
	char huffman[] = {0,1,2,3,8,9,10,11,24,25,26,27,28,29,30,31};
    for (int i=0;i<=3;i++){
		rank[i].code = huffman[i];
		rank[i].len = 3;
	}
	for (int i=4;i<8;i++){
		rank[i].code = huffman[i];
		rank[i].len = 4;
	}
	for (int i=8;i<16;i++){
		rank[i].code = huffman[i];
		rank[i].len = 5;
	}
    
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
////////////////////////////////////////////// 여기까진 해결...
    
	//rank 재배열 하기 전에 rank table 만들어 놓자
	char ranktable[4];
	for (int i=0;i<4;i++){
		ranktable[i] =(rank[2*i].symbol << 4) + rank[2*i+1].symbol;
	}
    
	//  4. make6;i++.
	// 다시 rank 를 symbol 순으로 sort. 그래야 rank[i].code 접근 matching 가능
	for (int i=15;i>0;i--){
		for (int j=0;j<i;j++){
			if(rank[j].symbol>rank[j+1].symbol){
				temp = rank[j];
				rank[j] = rank[j+1];
				rank[j+1] = temp;
			}}}
    
	//encode
	char encoded[inbytes*5/4];
	encoded[0] = 0;
	char limit = 4; // encode 첫 4 bits 0000 이므로.
	char temp_8 = 0;
	int n = 0 ;
	
    
	for(int i=0;i<2*inbytes;i++){ 
		limit += rank[tmp[i]].len;
		if (limit <= 8) {		
			temp_8 += rank[tmp[i]].code << (8-limit);
		} else {
			temp_8 += rank[tmp[i]].code >> (limit-8);
			encoded[n] = temp_8 ;
			n += 1 ;
			temp_8 = rank[tmp[i]].code << (16-limit);
			limit -= 8;
		} }
	encoded[n] = temp_8;
    
	int padding = (limit + 4 ) % 8;
	int ans_len;
	if ((limit+padding+4) == 8){
		ans_len = n+1;
	} else {
		ans_len = n+2;
	}
	ans_len += 4;  // rank table
	if (ans_len > outbytes)
		return -1;
	//rank table 넣고
	for (int i=0; i<4; i++){
		outp[i] = ranktable[i];
	}
	// end info
	encoded[0] += ( padding << 4 );
	int len_encoded = ans_len -4;
	for (int i=0; i<len_encoded; i++){
		outp[i+4] = encoded[i];
	}
		
	return ans_len;
}
 

 