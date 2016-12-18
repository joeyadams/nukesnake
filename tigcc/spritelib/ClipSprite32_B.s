| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite32_B

.text
ClipSprite32_B:
	tst.w %d1
	bge cs32b_nonegY
	add.w %d1,%d2
	asl.w #2,%d1
	suba.w %d1,%a0
	moveq #0,%d1
cs32b_nonegY:
	move.w %d1,%d3
	add.w %d2,%d1
	sub.w #128,%d1
	ble cs32b_nooverY
	sub.w %d1,%d2
cs32b_nooverY:
	move.w %d3,%d1
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
	tst.w %d5
	bge cs32b_nonegX
	swap %d3
	clr.w %d3
	swap %d3
	cmp.w #-16,%d5
	bge cs32b_nonegX
	moveq #0,%d3
	cmp.w #-32,%d5
	bgt cs32b_nonegX
	moveq #0,%d2 |thus killing the entire sprite loop because no loop is necessary.
cs32b_nonegX:
	cmp.w #240-32,%d5
	ble cs32b_nooverX
	moveq #0,%d4
	cmp.w #240-16,%d5
	blt cs32b_nooverX
	clr.w %d3
	cmp.w #240,%d5
	blt cs32b_nooverX
	moveq #0,%d2
cs32b_nooverX:
	rts

