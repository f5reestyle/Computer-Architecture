#----------------------------------------------------------------
#
#  4190.308 Computer Architecture (Fall 2020)
#
#  Project #3: RISC-V Assembly Programming
#
#  October 26, 2020
#
#  Injae Kang (abcinje@snu.ac.kr)
#  Sunmin Jeong (sunnyday0208@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#----------------------------------------------------------------


	.text
	.align	2

#---------------------------------------------------------------------
#  int decode(const char *inp, int inbytes, char *outp, int outbytes)
#---------------------------------------------------------------------
	.globl	decode
decode:
	
	bne a1, x0, first   # inbyte == 0 일 경우 
	add a0, x0, x0
	ret
	
# ra, outp, outbyte 백업, rank table 불러오기
first:	
	addi sp, sp, -64
	sw ra, 0(sp)
	sw a0, 4(sp)
	sw a1, 8(sp)
	sw a2, 12(sp)
	slli a3, a3, 1        
	sw a3, 16(sp)
	lw a2, 0(a0)
	jal ra, LB
	sw a4, 20(sp)  # rank table 1
	
# rank table 2 만들기	
	li a0, 0	# x=0
	li a1, -1	# i=-1
	li a5, 0    # ranktable 2
	li a3, 8
outer:
	addi a1, a1, 1
	lw a4, 20(sp)
	li a2, 8
inner:
	and ra, a4, 0xf   	# here, ra is temp reg 
	beq ra, a1, outer
	srli a4, a4, 4
	addi a2, a2, -1
	bne a2, x0, inner
	slli a5, a5, 4
	add a5, a5, a1
	addi a0, a0, 1
	bne a0, a3, outer
	sw a5, 24(sp)		# ranktable 2
	sw x0, 28(sp)		# Store reg
	sw x0, 32(sp)		# storebit
	
endinfo:
	lw a0, 4(sp)
	addi a0, a0, 4		# a0 = inp 주소, rank table lw했으니 +4
	lw a2, 0(a0)		# a2 = L. (lw 되는 reg)
	jal ra, LB
	mv a2, a4
	srli a5, a2, 28    # a5 = end info
	lw a1, 8(sp)
	addi a1, a1, -4
	slli a1, a1, 3
	addi a1, a1, -4
	sub a1, a1, a5		# a1: inbit = (inbyte-4)*8-4 - endinfo
	sw a1, 44(sp)
	addi a3, x0, 28		# a3 : 32bit count. 0 되면 lw
	li a5, 8			# a5: 8 count. 0되면 store reg 다 찬 것. 8이면 empty
	slli a2, a2, 4
	beq x0, x0, in

start:					
	bne a1, x0, out
	addi a5, a5, -8
	bne a5, x0, store2
	beq x0, x0, EXIT2
out:
	bne a3, x0, in
	addi a3, x0, 32  	# 32bit count 0 됐으므로 32로 초기화
	addi a0, a0, 4		# lw 하면 inp 주소 +4 해줘야.
	lw a2, 0(a0)
	jal ra, LB			# a4 는 temp이자 LB의 output reg
	mv a2, a4

in:
	blt a2, x0, front1      # a2 에 L 이 담겨있다 가정.
in3:
	li a4, 3
	blt a3, a4, aux0
	srli a4, a2, 29		# temp는 L의 앞 3자리
	sw a2, 36(sp)		# L 잠시 stack 에 킵
	lw a2, 20(sp)		# a2 자리에 ranktable 1
	slli a4, a4, 2
	neg a4, a4
	addi a4, a4, 28		# temp = 28-4*temp
	srl a2, a2, a4
	and a2, a2, 0xf
	addi a1, a1, -3		# ???? inbit 과 32bit은 계속 유지되는지.
	addi a3, a3, -3

	lw ra, 28(sp)		# ra : S reg
	slli ra, ra, 4
	add ra, ra, a2
	lw a2, 36(sp)
	slli a2, a2, 3
	addi a5, a5, -1
	sw a0, 40(sp)		# a0 : inp 메모리주소 잠시 킵
	sw a1, 44(sp)		# a1 : inbit 잠시 킵
	lw a0, 32(sp)		# storebit (4 bit 단위)
	lw a1, 16(sp)		# outbit (4 bits 단위)
	addi a0, a0, 1
	blt a1, a0, EXIT1	# outbit < storebit
	sw a0, 32(sp)
	sw a1, 16(sp)
	lw a0, 40(sp)
	lw a1, 44(sp)
	sw ra, 28(sp)
	bne a5, x0, start
store1:
	sw a1, 44(sp)		# inbit 잠시 킵
	lw a1, 12(sp)		# *outp in temp a4
	sw a2, 36(sp)		# L 잠시 킵
	mv a2, ra
	jal ra, LB			# endian 바꾸기
	sw a4, 0(a1)		# outp 애 write
	addi a1, a1, 4		# *outp +4
	sw a1, 12(sp)
	lw a1, 44(sp)		
	lw a4, 28(sp)
	add a4, x0, x0		# S reg 초기화
	sw a4, 28(sp)
	addi a5, x0, 8		# 8count 초기화
	lw a2, 36(sp)
	beq x0, x0, start
aux0:
	srli a2, a2, 29
	sw a2, 56(sp)		# 기존 msb 킵
	addi a0, a0, 4
	lw a2, 0(a0)		# 다음 L 불러오기
	jal ra, LB			# a4 는 temp이자 LB의 output reg
	mv a2, a4
	sw a5, 60(sp)
	addi a5, a3, 29
	srl a5, a2, a5
	lw a4, 56(sp)
	add a4, a4, a5		# merge
	neg a3, a3
	addi a3, a3, 3		# 3-32bit
	sll a2, a2, a3		# 쓰인 msb 왼쪽으로 밀기
	
	sw a2, 36(sp)		# L 잠시 stack 에 킵
	lw a2, 20(sp)		# a2 자리에 ranktable 1
	slli a4, a4, 2
	neg a4, a4
	addi a4, a4, 28		# temp = 28-4*temp
	srl a2, a2, a4
	and a2, a2, 0xf
	addi a1, a1, -3		# ???? inbit 과 32bit은 계속 유지되는지.
	neg a3, a3
	addi a3, a3, 32		# new 32bit = 32-(3-old 32bit)

	lw a5, 60(sp)

	lw ra, 28(sp)		# ra : S reg
	slli ra, ra, 4
	add ra, ra, a4
	addi a5, a5, -1
	sw a0, 40(sp)		# a0 : inp 메모리주소 잠시 킵
	sw a1, 44(sp)		# a1 : inbit 잠시 킵
	lw a0, 32(sp)		# storebit (4 bit 단위)
	lw a1, 16(sp)		# outbit (4 bits 단위)
	addi a0, a0, 1
	blt a1, a0, EXIT1	# outbit < storebit
	sw a0, 32(sp)
	sw a1, 16(sp)
	lw a0, 40(sp)
	lw a1, 44(sp)
	sw ra, 28(sp)
	bne a5, x0, start
	beq x0, x0, store1

front1: # L = a2
	slli a2, a2, 1
	li a4, 1
	beq a3, a4, only1	# 1자리밖에 없으면 따로 처리. 10인지 11인지 모르기 때문
	blt a2, x0, front11
	li a4, 4
	blt a3, a4, aux1	# 남은 bits 4 미만인경우
inter1:
	li a4, 4
	slli a4, a4, 29		# 앞 3자리에 4 더함. ranktable 1 index 맞추려고.
	add a2, a2, a4
	addi a1, a1, -1
	addi a3, a3, -1  
	beq x0, x0, in3

only1:
	sw a2, 56(sp)		# 기존 msb 킵
	addi a0, a0, 4
	lw a2, 0(a0)		# 다음 L 불러오기
	li a3, 33
	jal ra, LB
	mv a2, a4
	blt a2, x0,	front11		# 11 인 경우
	beq x0, x0, inter1
aux1:
	srli a2, a2, 29
	addi a2, a2, 4
	sw a2, 56(sp)		# 기존 msb 킵
	addi a0, a0, 4
	lw a2, 0(a0)		# 다음 L 불러오기
	jal ra, LB			# a4 는 temp이자 LB의 output reg
	mv a2, a4
	sw a5, 60(sp)
	addi a5, a3, 28
	srl a5, a2, a5
	lw a4, 56(sp)
	add a4, a4, a5		# merge
	neg a3, a3
	addi a3, a3, 4		#  4-32bits
	sll a2, a2, a3		# 쓰인 msb 왼쪽으로 밀기
	
	sw a2, 36(sp)		# L 잠시 stack 에 킵
	lw a2, 20(sp)		# a2 자리에 ranktable 1
	slli a4, a4, 2
	neg a4, a4
	addi a4, a4, 28		# temp = 28-4*temp
	srl a2, a2, a4
	and a2, a2, 0xf
	addi a1, a1, -4		# ???? inbit 과 32bit은 계속 유지되는지.
	neg a3, a3
	addi a3, a3, 32		# new 32bit = 32-(4-old 32bit)

	lw a5, 60(sp)

	lw ra, 28(sp)		# ra : S reg
	slli ra, ra, 4
	add ra, ra, a4
	addi a5, a5, -1
	sw a0, 40(sp)		# a0 : inp 메모리주소 잠시 킵
	sw a1, 44(sp)		# a1 : inbit 잠시 킵
	lw a0, 32(sp)		# storebit (4 bit 단위)
	lw a1, 16(sp)		# outbit (4 bits 단위)
	addi a0, a0, 1
	blt a1, a0, EXIT1	# outbit < storebit
	sw a0, 32(sp)
	sw a1, 16(sp)
	lw a0, 40(sp)
	lw a1, 44(sp)
	sw ra, 28(sp)
	bne a5, x0, start
	beq x0, x0, store1

front11:
	slli a2, a2, 1
	li a4, 5
	blt a3, a4, aux11
	srli a4, a2, 29		# temp = L 앞 세자리
	sw a2, 36(sp)		# L 잠시 stack 에 킵
	lw a2, 24(sp)		# a2 자리에 ranktable 2
	slli a4, a4, 2
	neg a4, a4
	addi a4, a4, 28		# temp = 28-4*temp
	srl a2, a2, a4
	and a2, a2, 0xf

	addi a1, a1, -5
	addi a3, a3, -5
	lw ra, 28(sp)		# ra : S reg 불러오고 왼쪽 4칸 이동.
	slli ra, ra, 4
	add ra, ra, a2
	lw a2, 36(sp)
	slli a2, a2, 3
	addi a5, a5, -1
	sw a0, 40(sp)		# a0 : inp 메모리주소 잠시 킵
	sw a1, 44(sp)		# a1 : inbit 잠시 킵
	lw a0, 32(sp)		# storebit (4 bit 단위)
	lw a1, 16(sp)		# outbyte (4 bits 단위)
	addi a0, a0, 1
	blt a1, a0, EXIT1	# outbit < storebit
	sw a0, 32(sp)
	sw a1, 16(sp)
	lw a0, 40(sp)
	lw a1, 44(sp)
	sw ra, 28(sp)		# store reg 에 킵.
	bne a5, x0, start
	beq x0, x0, store1

aux11:
	srli a2, a2, 29
	sw a2, 56(sp)		# 기존 msb 킵
	addi a0, a0, 4
	lw a2, 0(a0)		# 다음 L 불러오기
	jal ra, LB			# a4 는 temp이자 LB의 output reg
	mv a2, a4
	sw a5, 60(sp)
	addi a5, a3, 27
	srl a5, a2, a5
	lw a4, 56(sp)
	add a4, a4, a5		# merge
	neg a3, a3
	addi a3, a3, 5		#  5-32bits
	sll a2, a2, a3		# 쓰인 msb 왼쪽으로 밀기
	
	sw a2, 36(sp)		# L 잠시 stack 에 킵
	lw a2, 24(sp)		# a2 자리에 ranktable 2
	slli a4, a4, 2
	neg a4, a4
	addi a4, a4, 28		# temp = 28-4*temp
	srl a2, a2, a4
	and a2, a2, 0xf
	addi a1, a1, -5		# ???? inbit 과 32bit은 계속 유지되는지.
	neg a3, a3
	addi a3, a3, 32		# new 32bit = 32-(5-old 32bit)

	lw a5, 60(sp)

	lw ra, 28(sp)		# ra : S reg
	slli ra, ra, 4
	add ra, ra, a4
	addi a5, a5, -1
	sw a0, 40(sp)		# a0 : inp 메모리주소 잠시 킵
	sw a1, 44(sp)		# a1 : inbit 잠시 킵
	lw a0, 32(sp)		# storebit (4 bit 단위)
	lw a1, 16(sp)		# outbit (4 bits 단위)
	addi a0, a0, 1
	blt a1, a0, EXIT1	# outbit < storebit
	sw a0, 32(sp)
	sw a1, 16(sp)
	lw a0, 40(sp)
	lw a1, 44(sp)
	sw ra, 28(sp)
	bne a5, x0, start
	beq x0, x0, store1


store2:
	lw a2, 28(sp)
	lw a1, 12(sp)
	lw a0, 32(sp)
	addi a5, a5, 8
	slli a5, a5, 2		# 8count *4
	sll a2, a2, a5
	jal ra, LB
	sw a4, 0(a1)    ###
	srli a0, a0, 1
	lw ra, 0(sp)

	addi sp, sp, 64 
	
	ret

LB: # input : a2, ouput : a4, temp: a5,   constant temp: a3
	sw a5, 48(sp)
	sw a3, 52(sp)
	srli a4, a2, 24
	srli a5, a2, 8
	li a3, 0xff
	slli a3, a3, 8
	and a5, a5, a3
	add a4, a4, a5
	slli a5, a2, 8
	slli a3, a3, 8
	and a5, a5, a3
	add a4, a4, a5
	slli a5, a2, 24
	add a4, a4, a5
	lw a5, 48(sp)
	lw a3, 52(sp)

	ret

EXIT1:
	lw ra, 0(sp)
	addi a0, x0, -1
	addi sp, sp, 64
	ret
EXIT2:
	lw a0, 32(sp)
	srli a0, a0, 1
	lw ra, 0(sp)
	addi sp, sp, 64
	ret