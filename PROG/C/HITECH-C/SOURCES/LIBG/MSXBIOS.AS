;struct REGS {
;	 uchar	 f;	 /* f-flag */
;	 uchar	 a;	 /* accu */
;	 int	 bc;	 /* BC */
;	 int	 de;	 /* DE */
;	 int	 hl; }	 /* HL */
;void	 calbio(uint,char *);	 /* call bios(routine,*regs) */
;void	 calbas(uint,char *);	 /* call basic(routine,*regs) */
;void	 calsub(uint,char *);	 /* call subrom(routine,*regs) */
;void	 calslt(uchar,uint,char *); /* callslot(slot,address,*regs) */
;uchar	 rdslt(uchar,uint);	    /* read (slot,address) */
;void	 wrslt(uchar,uint,uchar);   /* write (slot,address,data */
;void	 callx(uint,char *);	    /* call (routine,*regs) */


; void calbio(adrs, reg)
; unsigned int adrs;
; struct REGS *reg;

	psect	text
	global	_calbio,csv,cret

_calbio:
	call	csv
	push	ix
	push	iy
	ld	l,(ix+6)
	ld	h,(ix+7)	; address
	push	hl
	pop	ix		; routine address in ix
	ld	l,(ix+8)
	ld	h,(ix+9)	; &registers
	push	hl		; save pointer to registers
	ld	iy,0		; save SP in IY
	add	iy,sp
	di
	ld	sp,hl		; copy to registers
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy		; restore true stack pointer
	push	iy		; save iy=true stack pointer
	ld	iy,(0fcc0h)	; EXPTBL-1
	call	001ch		; interslotcall
	jp	copreg

;--------------------------------------------------------------

; void calsub(adrs, reg)
; unsigned int adrs;
; struct REGS *reg;

	psect	text
	global	_calsub,csv,cret

_calsub:
	call	csv
	push	ix
	push	iy
	ld	l,(ix+6)
	ld	h,(ix+7)	; address
	push	hl
	pop	ix		; routine address in ix
	ld	l,(ix+8)
	ld	h,(ix+9)	; &registers
	push	hl		; save pointer to registers
	ld	iy,0		; save SP in IY
	add	iy,sp
	di
	ld	sp,hl		; copy to registers
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy		; restore true stack pointer
	push	iy		; save iy=true stack pointer
	ld	iy,(0faf7h)	; EXBRSA-1
	call	001ch		; interslotcall
	jp	copreg


;--------------------------------------------------------------

; void calslt(slot, adrs, reg)
; unsigned char slot;
; unsigned int adrs;
; struct REGS *reg;

	psect	text
	global	_calslt,csv,cret

_calslt:
	call	csv
	ld	a,(ix+6)	; slot
	ld	(TEMP),a
	push	ix
	push	iy
	ld	l,(ix+8)
	ld	h,(ix+9)	; address
	push	hl
	pop	ix		; routine address in ix
	ld	l,(ix+10)
	ld	h,(ix+11)	 ; &registers
	push	hl		; save pointer to registers
	ld	iy,0		; save SP in IY
	add	iy,sp
	di
	ld	sp,hl		; copy to registers
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy		; restore true stack pointer
	push	iy		; save iy=true stack pointer
	ld	iy,(TEMP-1)
	call	001ch		; interslotcall
	jp	copreg
TEMP:
	defw	00


;--------------------------------------------------------------

; void calbas(adrs, reg)
; unsigned int adrs;
; char *reg;

;	Call a ROM BASIC routine from C program
;
	psect	text
	global	_calbas,csv,cret

_calbas:
	call	csv
	push	ix
	push	iy
	ld	l,(ix+6)
	ld	h,(ix+7)	; address
	push	hl
	pop	ix		; routine address in ix
	ld	l,(ix+8)
	ld	h,(ix+9)	; &registers
	push	hl		; save pointer to registers
	ld	iy,0		; save SP in IY
	add	iy,sp
	di
	ld	sp,hl		; copy to registers
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy		; restore true stack pointer
	push	iy		; save iy=true stack pointer
	call	_CALBAS
	jp	copreg


;--------------------------------------------------------------

;	Call a ROM BASIC routine.
;
;	Only used internally.
;
;	The  problem is we can never pass IX register that CALBAS wants.
;	What must be done is to set IX register and jump to CALBAS after
;	the BIOS ROM is inter-slot called.
;
;	Build  the  following  interface routine on the stack.	Make NMI
;	hook jump to it.  Inter-slot call BIOS NMI entry (never used and
;	only hook is provided).
;
;	+0	inc	sp
;	+1	inc	sp
;	+2	ld	ix,<interpreter entry>
;	+6	nop
;	+7	jp	CALBAS
;
CALBAS	equ	0159h
NMI	equ	0066h		; Non-maskable interrupt
H_NMI   equ     0FDD6h          ; Hook for NMI

	psect	text
	global	_CALBAS

_CALBAS:
	exx			; Save argument registers over setup
	ex	af,af'
	ld	hl,CALBAS
	push	hl
	ld	hl,0C300h
	push	hl
	push	ix
	ld	hl,021DDh
	push	hl
	ld	hl,03333h
	push	hl
	ld	hl,0
	add	hl,sp
	ld	a,0C3h
        ld      (H_NMI),a
        ld      (H_NMI+1),hl
	ex	af,af'          ; Restore registers
	exx
;
	ld	ix,NMI
        ld      iy,(0fcc0h)
        call    001ch
;
	ld	hl,10		; Throw away the interface routine
	add	hl,sp
	ld	sp,hl

	ret


;--------------------------------------------------------------

; unsigned char rdslt(slot, adr)
; unsigned char slot;
; unsigned int adr;

	psect	text
	global	_rdslt,csv,cret

_rdslt:
	call	csv
	ld	a,(ix+6)
	ld	l,(ix+8)
	ld	h,(ix+9)
	push	ix
	push	iy
	call	000Ch
	ei			;*** make sure interrupts are enabled
	pop	iy
	pop	ix
	ld	l,a
	ld	h,0
	jp	cret


;--------------------------------------------------------------

; void wrslt(slot, adr, value)
; unsigned char slot;
; unsigned int adr;
; unsigned char value;

	psect	text
	global	_wrslt,csv,cret

_wrslt:
	call	csv
	ld	a,(ix+6)	; slot
	ld	l,(ix+8)
	ld	h,(ix+9)	; adr
	ld	e,(ix+10)	; value
	push	ix
	push	iy
	call	0014h
	pop	iy
	pop	ix
	ei			;*** make sure interrupts are enabled
	jp	cret

;--------------------------------------------------------------

; void callx(adrs, regs)
; unsigned int adrs;
; char * regs;

;	Call machine language subroutine. 

SPSIZE  equ     8

	psect	text
	global	_callx,csv,cret

_callx:
	call	csv
	push	ix
	push	iy
	ld	l,(ix+6)
	ld	h,(ix+7)	; hl = routine
	ld	(clrtn),hl
	ld	l,(ix+8)
	ld	h,(ix+9)	; hl = *regs
	push	hl
	ld	iy,0		; save SP in IY
	add	iy,sp
	di
	ld	sp,hl		; copy to registers
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy		; restore true stack pointer
	push	iy		; save true sp
	call	0
clrtn	equ	$-2		; call routine
	jp	copreg
copreg:
	pop	iy		; true sp
	pop	ix		; pointer to REGS
	di
	ld	sp,ix		; copy from registers
	inc	sp
	inc	sp
	inc	sp
	inc	sp
	inc	sp
	inc	sp
	inc	sp
	inc	sp
	push	hl
	push	de
	push	bc
	push	af
	ei
	ld	sp,iy		; restore true stack
	pop	iy
	pop	ix
	jp	cret

;--------------------------------------------------------------
;end of source
          