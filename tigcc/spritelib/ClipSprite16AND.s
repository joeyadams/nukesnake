| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite16AND

.text
ClipSprite16AND:
	movem.l %d3-%d5,-(%a7)
	jbsr ClipSprite16_B
cs16ca_loop:
	subq.w #1,%d2
	blt cs16ca_rts
	moveq #0,%d1
	move.w (%a0)+,%d1
	not.w %d1
	lsl.l %d0,%d1
	and.l %d4,%d1
	not.l %d1
	and.l %d1,(%a1)
	lea 30(%a1),%a1
	bra cs16ca_loop
cs16ca_rts:
	movem.l (%a7)+,%d3-%d5
	rts

