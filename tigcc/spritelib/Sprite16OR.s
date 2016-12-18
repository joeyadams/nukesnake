| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite16OR

.text
Sprite16OR:
	jbsr Sprite16_B
s16co_loop:
	subq.w #1,%d2
	blt s16co_rts
	moveq #0,%d1
	move.w (%a0)+,%d1
	lsl.l %d0,%d1
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s16co_loop
s16co_rts:
	rts

