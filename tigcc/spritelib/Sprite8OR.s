| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite8OR

.text
Sprite8OR:
	jbsr Sprite8_B
s8co_loop:
	subq.w #1,%d2
	blt s8co_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	lsl.l %d0,%d1
	or.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s8co_loop
s8co_rts:
	rts
	
