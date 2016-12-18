| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite32RPLC
	.xdef Sprite32_C_RPLC

.text
Sprite32RPLC:
	movem.l %d3-%d7,-(%a7)
	jbsr Sprite32_B
  Sprite32_C_RPLC:
	move.l %d3,%d6
	move.l %d4,%d7
	not.l %d6
	not.l %d7
s32cr_loop:
	subq.w #1,%d2
	blt s32cr_rts
	move.l (%a0)+,%d1
	ror.l %d0,%d1
	move.l %d1,%d5
	and.l %d3,%d1 |left side
	and.l %d4,%d5 |right side
	and.l %d6,(%a1)
	or.l %d1,(%a1)+
	and.l %d7,(%a1)
	or.l %d5,(%a1)
	lea 30-4(%a1),%a1
	bra s32cr_loop
s32cr_rts:
	movem.l (%a7)+,%d3-%d7
	rts

