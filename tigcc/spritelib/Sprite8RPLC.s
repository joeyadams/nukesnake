| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite8RPLC

.text
Sprite8RPLC:
	jbsr Sprite8_B
	move.l %d3,-(%a7)
	moveq #0,%d3
	not.b %d3
	lsl.l %d0,%d3
	not.l %d3 |ANDing flag
s8cr_loop:
	subq.w #1,%d2
	blt s8cr_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	lsl.l %d0,%d1
	and.l %d3,(%a1)
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s8cr_loop
s8cr_rts:
	move.l (%a7)+,%d3
	rts


