| Assembly Source File
| Created 9/9/2005; 4:11:10 PM

	.xdef Sprite32
	.xdef Sprite32_Jumpin

.text
Sprite32:
	movem.l %d3-%d7,-(%a7)
	jbsr Sprite32_B
Sprite32_Jumpin:
	move.l %d3,%d6
	move.l %d4,%d7
	not.l %d6
	not.l %d7
	move.l %d6,s32_leftmaskdropoff+2
	move.w 6*4(%a7),%d5
	subq.w #2,%d5
	bne s32_notand
	move.l %d6,%d3
	move.l %d7,%d4
s32_notand:
	lea -26(%a1),%a1
s32c_loop:
	subq.w #1,%d2
	blt s32c_rts
	lea 26(%a1),%a1
	move.l (%a0)+,%d1
	ror.l %d0,%d1
	move.l %d1,%d6
	tst.w %d5
	beq s32_Am
	and.l %d3,%d1 |left side
	and.l %d4,%d6 |right side
	tst.w %d5
	bgt s32_Rm
	cmp.w #-1,%d5
	beq s32_Om
	|XOR
	eor.l %d1,(%a1)+
	eor.l %d6,(%a1)
	bra s32c_loop
s32_Rm:	|RPLC
  s32_leftmaskdropoff:
	and.l #0xDEED,(%a1)
	or.l %d1,(%a1)+
	and.l %d7,(%a1)
	or.l %d6,(%a1)
	bra s32c_loop
s32_Om:	|OR
	or.l %d1,(%a1)+
	or.l %d6,(%a1)
	bra s32c_loop
s32_Am:	|AND
	or.l %d3,%d1 |left side
	or.l %d4,%d6 |right side
	and.l %d1,(%a1)+
	and.l %d6,(%a1)
	bra s32c_loop
s32c_rts:
	movem.l (%a7)+,%d3-%d7
	rts

