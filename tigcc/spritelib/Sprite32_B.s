| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite32_B

.text
Sprite32_B:
	mulu.w #30,%d1
	adda.w %d1,%a1
	move.w %d0,%d1
	move.w %d0,%d5
	asr.w #3,%d1
	and.w #0xFFFE,%d1
	adda.w %d1,%a1
	and.w #15,%d0
	move.w %d0,%d6
	eor.w #31,%d6
	moveq #-1,%d4
	lsl.l %d6,%d4
	add.l %d4,%d4 |%d6 doesn't quite shift it enough
	moveq #-1,%d3
	lsr.l %d0,%d3
	rts

