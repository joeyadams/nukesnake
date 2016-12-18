| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite16AND

.text
Sprite16AND:
	jbsr Sprite16_B
s16ca_loop:
	subq.w #1,%d2
	blt s16ca_rts
	moveq #0,%d1
	move.w (%a0)+,%d1
	not.w %d1
	lsl.l %d0,%d1
	not.l %d1
	and.l %d1,(%a1)
	lea 30(%a1),%a1
	bra s16ca_loop
s16ca_rts:
	rts

