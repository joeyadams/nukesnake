| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite32XOR
	.xdef Sprite32_C_XOR

.text
Sprite32XOR:
	movem.l %d3-%d7,-(%a7)
	jbsr Sprite32_B
  Sprite32_C_XOR:
s32cx_loop:
	subq.w #1,%d2
	blt s32cx_rts
	move.l (%a0)+,%d1
	ror.l %d0,%d1
	move.l %d1,%d5
	and.l %d3,%d1 |left side
	and.l %d4,%d5 |right side
	eor.l %d1,(%a1)+
	eor.l %d5,(%a1)
	lea 30-4(%a1),%a1
	bra s32cx_loop
s32cx_rts:
	movem.l (%a7)+,%d3-%d7
	rts

