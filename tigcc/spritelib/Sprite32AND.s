| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite32AND
	.xdef Sprite32_C_AND

.text
Sprite32AND:
	movem.l %d3-%d7,-(%a7)
	jbsr Sprite32_B
  Sprite32_C_AND:
	not.l %d3
	not.l %d4
s32ca_loop:
	subq.w #1,%d2
	blt s32ca_rts
	move.l (%a0)+,%d1
	ror.l %d0,%d1
	move.l %d1,%d5
	or.l %d3,%d1 |left side
	or.l %d4,%d5 |right side
	and.l %d1,(%a1)+
	and.l %d5,(%a1)
	lea 30-4(%a1),%a1
	bra s32ca_loop
s32ca_rts:
	movem.l (%a7)+,%d3-%d7
	rts

