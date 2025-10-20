	.file	"loop0.c"
	.option pic
	.text
	.globl	c
	.bss
	.align	3
	.type	c, @object
	.size	c, 8192
c:
	.zero	8192
	.text
	.align	1
	.globl	function
	.type	function, @function
function:
	addi	sp,sp,-16
	sd	s0,8(sp)
	sd	s1,0(sp)
	addi	s0,sp,16
	li	s1,0
	j	.L2
.L3:
	lla	a4,c
	slli	a5,s1,3
	add	a5,a4,a5
	fld	fa4,0(a5)
	lla	a5,.LC0
	fld	fa5,0(a5)
	fdiv.d	fa5,fa4,fa5
	lla	a4,c
	slli	a5,s1,3
	add	a5,a4,a5
	fsd	fa5,0(a5)
	addiw	a5,s1,1
	sext.w	s1,a5
.L2:
	mv	a4,s1
	li	a5,1023
	ble	a4,a5,.L3
	nop
	nop
	ld	s0,8(sp)
	ld	s1,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	function, .-function
	.section	.rodata
	.align	3
.LC0:
	.word	0
	.word	1073741824
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
