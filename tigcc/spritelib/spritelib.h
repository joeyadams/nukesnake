/*
 * SpriteLib - A set of basic sprite routines for TIGCC.
 *
 * Copyright (c) 2016 Joey Adams
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef SPRITELIB_H
#define SPRITELIB_H

void Sprite8(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void Sprite8OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite8AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite8XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite8RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));

void ClipSprite8(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void ClipSprite8OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite8AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite8XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite8RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned char *sprite asm("%a0"),void *vm_addr asm("%a1"));

void Sprite16(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void Sprite16OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite16AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite16XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite16RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));

void ClipSprite16(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void ClipSprite16OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite16AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite16XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite16RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned short *sprite asm("%a0"),void *vm_addr asm("%a1"));

void Sprite32(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void Sprite32OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite32AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite32XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void Sprite32RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));

void ClipSprite32(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"),short attr);
void ClipSprite32OR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite32AND(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite32XOR(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));
void ClipSprite32RPLC(short x asm("%d0"),short y asm("%d1"),short h asm("%d2"),unsigned long *sprite asm("%a0"),void *vm_addr asm("%a1"));

enum SprtModes {SPRT_XOR, SPRT_OR, SPRT_AND, SPRT_RPLC};

#endif
