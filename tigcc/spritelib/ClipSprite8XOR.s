| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite8XOR

.text
ClipSprite8XOR:
	movem.l %d3-%d5,-(%a7)
	jbsr ClipSprite8_B
cs8cx_loop:
	subq.w #1,%d2
	blt cs8cx_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	lsl.l %d0,%d1
	and.l %d4,%d1
	eor.l %d1,(%a1)
	lea 30(%a1),%a1
	bra cs8cx_loop
cs8cx_rts:
	movem.l (%a7)+,%d3-%d5
	rts

