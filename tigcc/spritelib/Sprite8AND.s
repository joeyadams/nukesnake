| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite8AND

.text
Sprite8AND:
	jbsr Sprite8_B
s8ca_loop:
	subq.w #1,%d2
	blt s8ca_rts
	moveq #0,%d1
	move.b (%a0)+,%d1
	not.b %d1
	lsl.l %d0,%d1
	not.l %d1
	and.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s8ca_loop
s8ca_rts:
	rts
	
