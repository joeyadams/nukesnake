| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite32XOR

.text
ClipSprite32XOR:
	movem.l %d3-%d7,-(%a7)
	jbsr ClipSprite32_B
	jbra Sprite32_C_XOR

