| Assembly Source File
| Created 9/27/2005; 7:36:25 PM

	.xdef ClipSprite32

.text
ClipSprite32:
	movem.l %d3-%d7,-(%a7)
	pea Sprite32_Jumpin |This function uses the same redirector as the regular Sprite32
  	jbra ClipSprite32_B

