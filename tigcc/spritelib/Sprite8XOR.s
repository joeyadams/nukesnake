| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite8XOR

.text
Sprite8XOR:
	jbsr Sprite8_B
s8cx_loop:
	subq.w #1,%d2
	blt s8cx_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	lsl.l %d0,%d1
	eor.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s8cx_loop
s8cx_rts:
	rts
	
