	.file	"code00.c"
	.option pic
	.text
	.align	1
	.globl	func
	.type	func, @function
func:
	addi	sp,sp,-16
	sd	s0,8(sp)
	addi	s0,sp,16
	la	a5,a
	lw	a5,0(a5)
	fcvt.s.w	fa4,a5
	la	a5,f
	flw	fa5,0(a5)
	fadd.s	fa5,fa4,fa5
	fcvt.w.s a5,fa5,rtz
	sext.w	a5,a5
	mv	a0,a5
	ld	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	func, .-func
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
