| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef Sprite8_B

.text
Sprite8_B:
	mulu.w #30,%d1
	adda.l %d1,%a1
	move.w %d0,%d1
	asr.w #3,%d1
	and.w #0xFFFE,%d1
	adda.w %d1,%a1
	not.w %d0
	and.w #15,%d0
	add.w #9,%d0
	rts
	
