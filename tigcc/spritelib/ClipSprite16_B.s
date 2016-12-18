| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite16_B

.text
ClipSprite16_B:
	tst.w %d1
	bge cs16_1
	add.w %d1,%d2
	add.w %d1,%d1
	suba.w %d1,%a0
	moveq #0,%d1
cs16_1:
	move.w %d1,%d3
	add.w %d2,%d1
	sub.w #128,%d1
	ble cs16_2
	sub.w %d1,%d2
cs16_2:
	mulu.w #30,%d3
	adda.w %d3,%a1
	move.w %d0,%d1
	not.w %d0
	and.w #15,%d0
	moveq #-1,%d4
	addq.w #1,%d0
	asr.w #3,%d1
	and.w #0xFFFE,%d1
	bge cs16_3
	cmp.w #-2,%d1
	blt cs16_killLoop
	not.l %d4
	not.w %d4
cs16_3:
	cmp.w #28,%d1
	blt cs16_5
	beq cs16_4
cs16_killLoop:
	moveq #0,%d2
cs16_4:
	not.w %d4
cs16_5:
	adda.w %d1,%a1
	rts

