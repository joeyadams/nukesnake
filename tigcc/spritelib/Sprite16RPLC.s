| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite16RPLC

.text
Sprite16RPLC:
	jbsr Sprite16_B
	move.l %d3,-(%a7)
	moveq #0,%d3
	not.w %d3
	lsl.l %d0,%d3
	not.l %d3 |ANDing flag
s16cr_loop:
	subq.w #1,%d2
	blt s16cr_rts
	moveq #0,%d1
	move.w (%a0)+,%d1
	lsl.l %d0,%d1
	and.l %d3,(%a1)
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s16cr_loop
s16cr_rts:
	move.l (%a7)+,%d3
	rts

