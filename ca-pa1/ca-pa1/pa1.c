
struct ranking {
		int num; // frequency of the symbol
		char symbol ; // original symbol
		char code;  //huffman code
		char len;   //code의 길이
	};

/* TODO: Implement this function */
int encode(const char *inp, int inbytes, char *outp, int outbytes)
{	
	if (inbytes==0){      // if input size is zero, return zero.
		return 0;          
	}
	//  step 0. divide 1 byte to two chuncks of 4 bits, and store them in 'tmp' array.
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
    
	//  step 1. count the symbol int count[16];
	int count[16];
	for (int i=0;i<16;i++) { count[i]= 0 ;}  // initialize with 0
	for (int i=0;i<2*inbytes;i++) { count[tmp[i]]+=1;}  
    
	// step 2. store them in ranking array, and sort elements by frequency.

	struct ranking rank[16];
	for (int i=0;i<16;i++){
		rank[i].num = count[i];
		rank[i].symbol = (char)i;		
	}
    struct ranking temp; //for temporary assignment

	for (int i=0;i<16;i++){        //sort by num(=frequency)
		for (int j=15;j>i;j--){
			if (rank[j].num > rank[j-1].num){              
				temp = rank[j];
				rank[j] = rank[j-1];
				rank[j-1] = temp;
			}
			else if(rank[j].num==rank[j-1].num){   // if nums are same, then smaller symbol has higher rank.
				if (rank[j].symbol < rank[j-1].symbol){                   
					temp = rank[j];
					rank[j] = rank[j-1];
					rank[j-1] = temp;

				}}}}
    // for 8~15th elements, sort them by symbol.	
	for (int i=15;i>8;i--){
		for (int j=8;j<i;j++){
			if(rank[j].symbol>rank[j+1].symbol){
				temp = rank[j];
				rank[j] = rank[j+1];
				rank[j+1] = temp;
			}}}

	//	step 3. match with the huffman code and encode.
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
     
	//make ranktable before we sort rank by symbol
	char ranktable[4];
	for (int i=0;i<4;i++){
		ranktable[i] =(rank[2*i].symbol << 4) + rank[2*i+1].symbol;
	}
    
	//sort by symbol so that we can access to huffman code easier 
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
	char limit = 4; // the first 4 bits are for 'end info'
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
    
	int padding = 8-limit;
	int ans_len = n+1;
	ans_len += 4;  // rank table
	if (ans_len > outbytes)
		return -1;
	//rank table 
	for (int i=0; i<4; i++){
		outp[i] = ranktable[i];
	}
	// end info
	encoded[0] += ( padding << 4 );
	int len_encoded = ans_len -4;
	//encoded code
	for (int i=0; i<len_encoded; i++){
		outp[i+4] = encoded[i];
	}
		
	return ans_len;
}
 

 