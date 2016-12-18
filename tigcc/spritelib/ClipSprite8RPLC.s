| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite8RPLC

.text
ClipSprite8RPLC:
	movem.l %d3-%d5,-(%a7)
	jbsr ClipSprite8_B
	moveq #0,%d3
	not.b %d3
	lsl.l %d0,%d3
	and.l %d4,%d3
	not.l %d3 |ANDing flag used in RPLC mode
cs8cr_loop:
	subq.w #1,%d2
	blt cs8cr_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	and.l %d3,(%a1)
	lsl.l %d0,%d1
	and.l %d4,%d1
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra cs8cr_loop
cs8cr_rts:
	movem.l (%a7)+,%d3-%d5
	rts

