| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite8_B

.text
ClipSprite8_B:
	tst.w %d1
	bge cs8_1
	add.w %d1,%d2
	suba.w %d1,%a0
	moveq #0,%d1
cs8_1:
	move.w %d1,%d3
	add.w %d2,%d1
	sub.w #128,%d1
	ble cs8_2
	sub.w %d1,%d2
cs8_2:
	mulu.w #30,%d3
	adda.w %d3,%a1
	move.w %d0,%d1
	not.w %d0
	and.w #15,%d0
	moveq #-1,%d4
	add.w #9,%d0
	asr.w #3,%d1
	and.w #0xFFFE,%d1
	bge cs8_3
	cmp.w #-2,%d1
	blt cs8_killLoop
	not.l %d4
	not.w %d4
cs8_3:
	cmp.w #28,%d1
	blt cs8_5
	beq cs8_4
cs8_killLoop:
	moveq #0,%d2
cs8_4:
	not.w %d4
cs8_5:
	adda.w %d1,%a1
	rts

