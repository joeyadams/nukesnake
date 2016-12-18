| Assembly Source File
| Created 9/26/2005; 11:11:55 PM

	.xdef Sprite8

.text
Sprite8:
	jbsr Sprite8_B
	move.w %d4,-(%a7)
	move.l %d3,-(%a7)
	move.w 4+6(%a7),%d4
	subq.w #1,%d4
	moveq #0,%d3
	not.b %d3
	lsl.l %d0,%d3
	not.l %d3 |ANDing flag used in RPLC mode
	lea -30(%a1),%a1
s8_loop:
	subq.w #1,%d2
	blt s8_rts
	lea 30(%a1),%a1
	moveq #0,%d1
	move.b (%a0)+,%d1
	tst.w %d4
	blt s8_Xm
	beq s8_Om
	cmp.w #1,%d4
	beq s8_Am
	|RPLC mode
	lsl.l %d0,%d1
	and.l %d3,(%a1)
	or.l %d1,(%a1)
	bra s8_loop
s8_Xm: |XOR mode
	lsl.l %d0,%d1
	eor.l %d1,(%a1)
	bra s8_loop
s8_Om: |OR mode
	lsl.l %d0,%d1
	or.l %d1,(%a1)
	bra s8_loop
s8_Am: |AND mode
	not.b %d1
	lsl.l %d0,%d1
	not.l %d1
	and.l %d1,(%a1)
	bra s8_loop
s8_rts:
	move.l (%a7)+,%d3
	move.w (%a7)+,%d4
	rts
	
