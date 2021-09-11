
GPIO	equ	$FE00
UART	equ	$FD00

	mov r3,#GPIO
	mov r1,#$FF00
	sw r1,0[r3]
	mov r4,#UART
	mov r2,#HELLO
L1:
	lb  r1,0[r2]
	or r1,r1
	beq L2
	sb  r1,0[r4]
	add r2,r2,#1
	bra L1
L2:
	bra L2
	imm #0
HELLO	.asciiz "Hello, Nano!\n"
