| Assembly Source File
| Created 9/12/2005; 4:18:28 PM

	.xdef Sprite16
	
.text
Sprite16:
	jbsr Sprite16_B
	move.w %d4,-(%a7)
	move.l %d3,-(%a7)
	move.w 4+6(%a7),%d4
	subq.w #1,%d4
	moveq #0,%d3
	not.w %d3
	lsl.l %d0,%d3
	not.l %d3 |ANDing flag used in RPLC mode
	lea -30(%a1),%a1
s16_loop:
	subq.w #1,%d2
	blt s16_rts
	lea 30(%a1),%a1
	moveq #0,%d1
	move.w (%a0)+,%d1
	tst.w %d4
	blt s16_Xm
	beq s16_Om
	cmp.w #1,%d4
	beq s16_Am
	|RPLC mode
	lsl.l %d0,%d1
	and.l %d3,(%a1)
	or.l %d1,(%a1)
	bra s16_loop
s16_Xm: |XOR mode
	lsl.l %d0,%d1
	eor.l %d1,(%a1)
	bra s16_loop
s16_Om: |OR mode
	lsl.l %d0,%d1
	or.l %d1,(%a1)
	bra s16_loop
s16_Am: |AND mode
	not.w %d1
	lsl.l %d0,%d1
	not.l %d1
	and.l %d1,(%a1)
	bra s16_loop
s16_rts:
	move.l (%a7)+,%d3
	move.w (%a7)+,%d4
	rts

