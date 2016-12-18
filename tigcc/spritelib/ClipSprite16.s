| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite16

.text
ClipSprite16:
	movem.l %d3-%d5,-(%a7)
	jbsr ClipSprite16_B
	move.w 4*4(%a7),%d5
	subq.w #1,%d5
	moveq #0,%d3
	not.w %d3
	lsl.l %d0,%d3
	and.l %d4,%d3
	not.l %d3 |ANDing flag used in RPLC mode
	lea -30(%a1),%a1
cs16_loop:
	subq.w #1,%d2
	blt cs16_rts
	lea 30(%a1),%a1
	moveq #0,%d1
	move.w (%a0)+,%d1
	cmp.w #1,%d5
	beq cs16_AND
	lsl.l %d0,%d1
	and.l %d4,%d1
	tst.w %d5
	blt cs16_XOR
	beq cs16_OR
|RPLC
	and.l %d3,(%a1)
	or.l %d1,(%a1)
	bra cs16_loop
cs16_XOR:
	eor.l %d1,(%a1)
	bra cs16_loop
cs16_OR:
	or.l %d1,(%a1)
	bra cs16_loop
cs16_AND:
	not.w %d1
	lsl.l %d0,%d1
	and.l %d4,%d1
	not.l %d1
	and.l %d1,(%a1)
	bra cs16_loop
cs16_rts:
	movem.l (%a7)+,%d3-%d5
	rts

