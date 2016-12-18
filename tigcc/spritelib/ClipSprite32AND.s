| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite32AND

.text
ClipSprite32AND:
	movem.l %d3-%d7,-(%a7)
	jbsr ClipSprite32_B
	jbra Sprite32_C_AND

