	.file	"loop0.c"
	.option pic
	.text
	.align	1
	.globl	function
	.type	function, @function
function:
	lla	a5,c
	lla	a4,c+8192
	fld	fa5,.LC0,a3
.L2:
	fld	fa1,0(a5)
	fld	fa2,8(a5)
	fld	fa3,16(a5)
	fld	fa4,24(a5)
	fmul.d	ft0,fa1,fa5
	fmul.d	ft1,fa2,fa5
	fmul.d	ft2,fa3,fa5
	fmul.d	ft3,fa4,fa5
	addi	a5,a5,32
	fsd	ft0,-32(a5)
	fsd	ft1,-24(a5)
	fsd	ft2,-16(a5)
	fsd	ft3,-8(a5)
	bne	a5,a4,.L2
	ret
	.size	function, .-function
	.globl	c
	.section	.rodata.cst8,"aM",@progbits,8
	.align	3
.LC0:
	.word	0
	.word	1071644672
	.bss
	.align	3
	.type	c, @object
	.size	c, 8192
c:
	.zero	8192
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
