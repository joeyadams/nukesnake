| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite16RPLC

.text
ClipSprite16RPLC:
	movem.l %d3-%d5,-(%a7)
	jbsr ClipSprite16_B
	moveq #0,%d3
	not.w %d3
	lsl.l %d0,%d3
	and.l %d4,%d3
	not.l %d3 |ANDing flag used in RPLC mode
cs16cr_loop:
	subq.w #1,%d2
	blt cs16cr_rts
	moveq #0,%d1
	move.w (%a0)+,%d1
	and.l %d3,(%a1)
	lsl.l %d0,%d1
	and.l %d4,%d1
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra cs16cr_loop
cs16cr_rts:
	movem.l (%a7)+,%d3-%d5
	rts

