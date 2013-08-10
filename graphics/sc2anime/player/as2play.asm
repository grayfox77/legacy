;// -------------------------------------------------------------
;	.AS2 �A�j���[�V�����v���C���[.
;								(C)	2001 t.hara (HRA!)
;								http://www.imasy.or.jp/~hra/
;	MSX2/2+/turboR
;
;	History:
;		2001/06/23	v0.00a1	����J�n.
;
;	Coment
;		TAB��4 �ł�����������
;

;// -------------------------------------------------------------
;	�萔.
;

;-	�V�X�e��.
BDOS	equ	00005H	;	DOS�V�X�e���R�[�� (BDOS).
FCB		equ	0005CH	;	�R�}���h���C���̃t�@�C���R���g���[���u���b�N.
CALSLT	equ	0001CH	;	���̃X���b�g�ɂ���T�u���[�`���̌Ăяo��.
EXPTBL	equ	0FCC1H	;	�g���X���b�g�e�[�u��.

;-	MSX-DOS �t�@���N�V����.
CONOUT	equ	00002H	;	�R���\�[���P�����o��.
STROUT	equ	00009H	;	�R���\�[��������o��.
SETDTA	equ	0001AH	;	DTA �̐ݒ�.
FOPEN	equ	0000FH	;	�t�@�C���I�[�v��.
FCLOSE	equ	00010H	;	�t�@�C���N���[�Y.
FREAD	equ	00027H	;	�t�@�C�������_�����[�h.

;-	FCB �\���̃����o.
DRVNO	equ	0		;	�h���C�u�ԍ� (0:�f�t�H���g / 1:A / 2:B ... )
NAME	equ	1		;	�t�@�C����.
EXT		equ	9		;	�t�@�C���g���q.
CURBLK	equ	12		;	�J�����g�u���b�N.
BLKREC	equ	15		;	���R�[�h��[block].
RECSIZE	equ	14		;	���R�[�h�T�C�Y.
FSIZE	equ	16		;	�t�@�C���T�C�Y.
DATE	equ	20		;	���t.
TIME	equ	22		;	����.
DEVICE	equ	24		;	�f�o�C�XID
DIRLOC	equ	25		;	�f�B���N�g���ʒu.
TCLUST	equ	26		;	�擪�N���X�^.
LCLUST	equ	28		;	�ŏI�A�N�Z�X�N���X�^.
LPOINT	equ	30		;	�ŏI�N���X�^���ŏI�A�N�Z�X�|�C���g�̃I�t�Z�b�g.
CURREC	equ	32		;	�J�����g���R�[�h.
RNDREC	equ	33		;	�����_���A�N�Z�X���R�[�h.
FCBSIZE	equ	37		;	FCB �\���̃T�C�Y.

;-	BIOS
WRTVDP	equ	00047H	;	VDP ���W�X�^�֏o��. in)C=reg# B=data
CHGMOD	equ	0005FH	;	�X�N���[�����[�h�ύX. in)A=screen mode
GTTRIG	equ	000D8H	;	�W���C�X�e�B�b�N�g���K����.

;-	VDP I/O ( not support MSX1+V9938 )
VPORT0	equ	098H	;	VRAM�A�N�Z�X���W�X�^.
VPORT1	equ	099H	;	�R�}���h���W�X�^.
VPORT2	equ	09AH	;	�p���b�g���W�X�^.
VPORT3	equ	09BH	;	VDP���W�X�^���ڃA�N�Z�X.

;-	screen2 VRAM �}�b�v.
S2PAT	equ	00000H	;	�p�^�[���W�F�l���[�^�e�[�u��.
S2NAME	equ	01800H	;	�p�^�[���l�[���e�[�u��.
S2COL	equ	02000H	;	�J���[�e�[�u��.
SONAME	equ	768		;	�p�^�[���l�[���e�[�u���̃A�h���X.

;	�V�X�e�����[�N�G���A.
PUTPNT	equ	0F3F8H	;	�z�L�[���̓o�b�t�@�̃��C�g�|�C���^.
GETPNT	equ	0F3FAH	;	�z�L�[���̓o�b�t�@�̃��[�h�|�C���^.

;	DTA
DEFDTA	equ	00080H	;	�f�t�H���g�� DTA

;	���̃A�v���̐ݒ�.
NEWDTA	equ	08000H	;	DTA �̈ړ���
RSIZE	equ	04000H	;	�ő�ǂݍ��݃T�C�Y
HSIZE	equ	03024H	;	�t�@�C���w�b�_�T�C�Y

DTMOVE	equ	128		;	�ړ��f�[�^
DTCHAR	equ	129		;	�L�����N�^�`��f�[�^
DTVWAIT	equ	130		;	�t���[����؂��
DTEXIT	equ	131		;	�f�[�^�I��

;// -------------------------------------------------------------
;	�R�[�h�Z�O�����g.
;
		aseg
		org		100h
START:
		jp		S1					;	MS-DOS �N���h�~.
S1:
;-	SCREEN2 �֕ύX.
		ld		a,2
		call	SCREEN

;-	�X�v���C�g�\���֎~/�p���b�g�O�s������.
		ld		bc,02208H
		call	WRVDP

;-	DTA �Z�b�g.
		ld		de,NEWDTA			;	�V���� DTA �A�h���X.
		ld		c,SETDTA
		call	BDOS

;-	file ���J��.
		ld		de,FCB
		ld		c,FOPEN
		call	BDOS				;	file �I�[�v��.
		ld		de,MEOPEN			;	�t�@�C�����J���Ȃ�.
		or		a					;	�G���[�`�F�b�N.
		jp		nz,EXIT

;-	�A�j���[�V�����W�J���[�`��.
		call	ANPLAY

;-	file �����.
		ld		de,FCB
		ld		c,FCLOSE
		call	BDOS

		ld		de,MCOMPL			;	�������b�Z�[�W

EXIT:
;-	�p���b�g���f�t�H���g�l�ɂ���
		ld		hl,DEFPLT
		call	SETPLT

;-	SCREEN0 �֕ύX.
		push	de					;	���b�Z�[�W�A�h���X�̕ۑ�
		xor		a
		call	SCREEN

;-	DTA ���f�t�H���g�֖߂�.
		ld		de,DEFDTA			;	�f�t�H���g�� DTA �A�h���X.
		ld		c,SETDTA
		call	BDOS

;-	�L�[�o�b�t�@���N���A����.
		call	CLRKEY

;-	�I��.
		pop		de					;	���b�Z�[�W�A�h���X�̕��A
		ld		c,STROUT
		call	BDOS
		ret							;	DOS�֖߂�.

;// -------------------------------------------------------------
;	1.	����.
;		�A�j���[�V�����̕\��.
;	2.	����.
;		(FCB)	...	(I)	�I�[�v�����ꂽ FCB
;	3.	�߂�l.
;		�Ȃ�.
;	4.	�@�\.
;		(FCB) �Ŏ������t�@�C����ǂݍ��݂Ȃ���A�j���[�V������.
;		�\������B���炩���� SCREEN2 �֕ύX���Ă����悤�ɂ���.
;		VRAM �̊e�x�[�X�A�h���X�̓f�t�H���g�ł���K�v������.
;	5.	�j��.
;		�S��.
;
ANPLAY:
;-	FCB �̏�����.
		xor		a
		ld		(FCB+CURBLK),a		;	�J�����g�u���b�N			= 00H
		ld		hl,1
		ld		(FCB+RECSIZE),hl	;	���R�[�h�T�C�Y				= 1
		ld		h,a
		ld		l,a
		ld		(FCB+CURREC),a		;	�J�����g���R�[�h			= 00H
		ld		(FCB+RNDREC),hl		;	�����_���A�N�Z�X���R�[�h	= 00000000H
		ld		(FCB+RNDREC+2),hl

;-	VRAM �̃p�^�[���l�[���e�[�u��������������.
		ld		hl,S2NAME+4000h		;	�p�^�[���l�[���e�[�u��.
		call	VRAMADR
		ld		bc,SONAME			;	�p�^�[���l�[���e�[�u���̃T�C�Y.
APVINI:
		out		(VPORT0),a
		inc		a
		dec		c
		jp		nz,APVINI
		dec		b
		jp		p,APVINI

;-	�w�b�_���.
		call	READHD				;	�w�b�_�̓ǂݍ���.
		ret		z					;	�G���[�`�F�b�N.

;-	���C�����[�v.
		ld		hl,NEWDTA+RSIZE		;	�f�[�^�ǂݍ��݃C���f�b�N�X.
		ld		(SAVEAD),hl
APLP1:
;	-	�P�t���[�����̏���.
		call	DRAWFRM
		jp		z,APLE1

;	-	�t���[���E�F�C�g.
		;;;		�Ȃ�.
		jp		APLP1

;-	ending
APLE1:
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�w�b�_�̉��.
;	2.	����.
;		(FCB)	...	(I)	�I�[�v�����ꂽ FCB
;	3.	�Ԓl.
;		Z Flag	...	0	����.
;				...	1	�ُ�.
;	4.	�@�\.
;		�t�@�C���̃w�b�_��ǂݍ���ŉ�͂���.
;		�ړI�̃t�@�C�������ꍇ�̓G���[��Ԃ�.
;	5.	�j��.
;		�S��.
;
READHD:
;	-	�w�b�_�̓ǂݍ���.
		ld		de,FCB
		ld		hl,HSIZE			;	��C�ɂ܂Ƃ߂ēǂݍ���.
		ld		c,FREAD
		call	BDOS
		or		a
		ret		nz					;	�G���[�`�F�b�N.

;	-	�w�b�_�̃`�F�b�N.
		ld		bc,4
		ld		de,REFHED
		ld		hl,NEWDTA
		call	STRCMP
		ret		nz

;	-	�p���b�g�ݒ�.
		call	SETPLT

;	-	1st�t���[���i�p�^�W�F�l�j���Z�b�g.
		ld		hl,S2PAT+4000h
		call	VRAMADR
		ld		bc,VPORT0
		ld		a,18H
		ld		hl,NEWDTA+4+32
RHLT1:
		outi						;	out (C), (HL); ++HL; --B;
		jp		nz,RHLT1
		dec		a
		jp		nz,RHLT1

;	-	1st�t���[���i�J���[�j���Z�b�g.
		ld		hl,S2COL+4000h
		call	VRAMADR
		ld		bc,VPORT0
		ld		a,18H
		ld		hl,NEWDTA+4+32+1800h
RHLT2:
		outi
		jp		nz,RHLT2
		dec		a
		jp		nz,RHLT2

		inc		a					;	Z = 0
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�P�t���[���`��.
;	2.	����.
;		(FCB)	...	(I)	�I�[�v�����ꂽ FCB
;	3.	�Ԓl.
;		Z Flag	...	0	���s.
;				...	1	�f�[�^�I���.
;	4.	�@�\.
;		�P�t���[�����̉摜�f�[�^����������.
;	5.	�j��.
;		�S��.
;
DRAWFRM:
;	-	�ʒu��񏉊���.
		ld		hl,SONAME-1			;	���ڃp�[�c�ʒu.
		ld		(SAVEPT),hl
DFMLP1:
;	-	�f�[�^���݂̊m�F.
		call	GETDAT
		ret		z					;	�f�[�^�I���.

;	-	�I�y���[�V�����R�[�h�̓���.
		ld		hl,(SAVEAD)			;	����.
		ld		a,(hl)
		inc		hl					;	���̈ʒu.
		ld		(SAVEAD),hl
		cp		DTCHAR				;	if( a >= DTCHAR ) goto DFOP2
		jp		nc,DFOP2

;	-	�ړ��I�y���[�V����.
		ld		c,a
		xor		a					;	Cy = 0, a = 0
		ld		b,a
		ld		hl,(SAVEPT)
		sbc		hl,bc
		ld		(SAVEPT),hl
		jp		DFMLP1

;	-	�L�����N�^�f�[�^�I�y���[�V����.
DFOP2:
		jp		nz,DFEXIT			;	�L�����N�^�f�[�^�I�y���[�V�����łȂ��ꍇ.

;		-	�]���� VRAM �A�h���X���v�Z.
		ld		hl,(SAVEPT)
		add		hl,hl				;	VAGENE = hl * 8 + S2PAT
		add		hl,hl
		add		hl,hl
		ld		(VAGENE),hl
		ld		bc,S2COL			;	VACOL  = hl + S2COL - S2PAT
		add		hl,bc
		ld		(VACOL),hl

;		-	�]�����f�[�^�̕����m�F�i���E�ォ�ۂ��j.
		ld		hl,(SAVEAD)
		ld		d,h					;	save HL
		ld		e,l
		ld		bc,NEWDTA+RSIZE-16	;	�L�����N�^�f�[�^�� 16byte, ����������܂�ꍇ�ł�.
									;	Cy = 1 �ɂȂ�悤�� 16 ������.
		sbc		hl,bc				;	if( HL < BC ) goto DFFAST (��Cy = 0 �ɂȂ��Ă��邱�Ƃ�����)
		jp		c,DFFAST
		jp		z,DFFAST

;		-	�f�[�^�����������ꍇ�i���܂ɔ����j.
		push	hl
		ld		a,16				;	DTA �Ɏc���Ă�f�[�^�������߂�.
		sub		l
		ld		c,a					;	bc = 16 - (SAVEAD - (NEWDTA+RSIZE-16))
		ld		b,0
		ld		hl,BUFDAT			;	de = BUFDAT
		ex		de,hl				;	hl = SAVEAD
		jp		z,DFBSKP1			;	if( bc == 0 ) goto DFSKP1
		ldir						;	�]��.
DFBSKP1:
;			-	�f�B�X�N����ǂݍ��ݎc��̔��[�� BUFDAT ��.
		push	de
		call	RDDAT				;	hl = NEWDTA
		pop		de					;	de = BUFDAT + (16 - (SAVEAD - (NEWDTA+RSIZE-16)) )
		pop		bc					;	bc = SAVEAD - (NEWDTA+RSIZE-16)
		ret		z					;	�f�[�^�I���.
		ldir						;	�]��.
		ld		(SAVEAD),hl

		ld		de,BUFDAT			;	�]����.
		call	DFDRAW
		jp		DFMLP1

;		-	�f�[�^���A�����Ă���ꍇ.
DFFAST:
		call	DFDRAW
		ld		(SAVEAD),hl
		jp		DFMLP1

;		-	�I������.
DFEXIT:
		cp		DTEXIT				;	�f�[�^�I���Ȃ� Z=1 �ɂ���.
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�P�L�����`��.
;	2.	����.
;		(VAGENE)	...	(I)	�ړI�̃L�����̃p�^�W�F�l�A�h���X.
;		(VACOL)		...	(I)	�ړI�̃L�����̃J���[�A�h���X.
;		de			...	(I)	�\�[�X�f�[�^�̃A�h���X.
;	3.	�Ԓl.
;		hl			...	�\�[�X�f�[�^�̎��̈ʒu.
;	4.	�@�\.
;		�P�L�������̃f�[�^�� RAM ���� VRAM �֓]������.
;	5.	�j��.
;		�S��.
;
DFDRAW:
;		-	�]����A�h���X�Z�b�g�i�p�^�W�F�l�j.
		ld		hl,(VAGENE)
		ld		a,h
		or		40h
		ld		c,VPORT1
		out		(c),l
		out		(c),a
;		-	�o��.
		ex		de,hl
		ld		bc,0800h+VPORT0		;	8byte
		otir
;		-	�]����A�h���X�Z�b�g�i�J���[�j.
		ld		de,(VACOL)
		ld		a,d
		or		40h
		ld		c,VPORT1
		out		(c),e
		out		(c),a
;		-	�o��.
		ld		bc,0800h+VPORT0		;	8byte
		otir
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�f�[�^�̑��݊m�F.
;	2.	����.
;		(FCB)	...	(I)	�I�[�v�����ꂽ FCB
;	3.	�Ԓl.
;		Z Flag	...	0	���s.
;				...	1	�f�[�^�I���.
;	4.	�@�\.
;		�P�t���[�����̉摜�f�[�^����������.
;	5.	�j��.
;		�S��.
;
GETDAT:
;	-	�f�[�^�̑��݂��m�F����.
		ld		hl,(SAVEAD)
		ld		de,NEWDTA+RSIZE
		xor		a
		sbc		hl,de
		jp		nc,RDDAT
		inc		a					;	Z = 0
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�f�B�X�N����f�[�^��ǂ�.
;	2.	����.
;		(FCB)	...	(I)	�I�[�v�����ꂽ FCB
;	3.	�Ԓl.
;		Z Flag	...	0	���s.
;				...	1	�f�[�^�I���.
;		HL		...	NEWDTA
;	4.	�@�\.
;		�P�t���[�����̉摜�f�[�^����������.
;	5.	�j��.
;		�S��.
;
RDDAT:
		ld		de,FCB
		ld		hl,RSIZE			;	��C�ɂ܂Ƃ߂ēǂݍ���.
		ld		c,FREAD
		call	BDOS
		dec		a					;	�G���[�Ȃ� Z = 1 �ɂȂ�悤�ɂ���.
		ld		hl,NEWDTA
		ld		(SAVEAD),hl
		ret

;// -------------------------------------------------------------
;	�T�u���[�`��.
;

;// -------------------------------------------------------------
;	1.	����.
;		������̔�r.
;	2.	����.
;		bc		...	(I)	������.
;		de		...	(I)	��r�Ώƕ�����̃A�h���X�P.
;		hl		...	(I)	��r�Ώƕ�����̃A�h���X�Q.
;	3.	�Ԓl.
;		Z Flag	...	1	��v.
;				...	0	�s��v.
;		HL		...		��r���I�����ꏊ�i��v�̏ꍇ�͕�����̎��j.
;	4.	�@�\.
;		�Ȃ�.
;	5.	�j��.
;		�S��.
;
STRCMP:
		ld		a,(de)
		inc		de
		cpi
		jp		po,SCEXIT
		jr		z,STRCMP
SCEXIT:
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�p���b�g�ݒ�
;	2.	����.
;		hl		...	(I)	�p���b�g�f�[�^�̃A�h���X.
;	3.	�Ԓl.
;		hl		...	�p���b�g�f�[�^�̎�.
;	4.	�@�\.
;		�Ȃ�.
;	5.	�j��.
;		�S��.
;
SETPLT:
;	-	�p���b�g���W�X�^ R#16 ��ݒ�.
		push	hl
		ld		bc,0010h
		call	WRVDP
		pop		hl
;	-	16�p���b�g���o��.
		ld		bc,2000h+VPORT2
		otir
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�������Z�b�g.
;	2.	����.
;		A	...	�������ɃZ�b�g����l.
;		BC	...	�������̃T�C�Y.
;		HL	...	�Z�b�g�Ώۂ̐擪�A�h���X.
;	3.	�Ԓl.
;		�Ȃ�.
;	4.	�@�\.
;		[HL+0] ... [HL+BC-1] �̃������� A �̒l�Ŗ��ߐs����.
;	5.	�j��.
;		AF, BC, HL
;
MEMSET:
		ld		(hl),a
		inc		hl
		dec		c
		jp		nz,MEMSET
		dec		b
		jp		p,MEMSET
		ret

;// -------------------------------------------------------------
;	1.	����.
;		VRAM�A�h���X�̐ݒ�.
;	2.	����.
;		HL	...	�Z�b�g������ VRAM�A�h���X ( 0000H~3FFFH )
;	3.	�Ԓl.
;		�Ȃ�
;	4.	�@�\.
;		VDP �� VRAM�A�N�Z�X�p�A�h���X���W�X�^�ɒl���Z�b�g����.
;		HL bit6 ... 0: read / 1: write
;	5.	�j��.
;		C
;
VRAMADR:
		ld		c,VPORT1
		out		(c),l
		out		(c),h
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�X�N���[�����[�h�̐ݒ�i�ᑬ�j.
;	2.	����.
;		A	...	�X�N���[�����[�h�iMSX-BASIC�݊��j.
;	3.	�Ԓl.
;		�Ȃ�.
;	4.	�@�\.
;		�X�N���[�����[�h��ύX����.
;	5.	�j��.
;		���ׂ�.
;
SCREEN:
		push	af					;	�����̕ۑ�.
		ld		a,(EXPTBL)			;	MAIN-ROM �X���b�g�ԍ��擾.
		push	af
		pop		iy					;	iy ��� 8bit ���X���b�g�ԍ��ɂȂ�.
		ld		ix,CHGMOD			;	BIOS �̊Y���G���g��.
		pop		af					;	�����̕��A.
		call	CALSLT				;	�C���^�[�X���b�g�R�[��.
		ret

;// -------------------------------------------------------------
;	1.	����.
;		VDP ���W�X�^�������݁i�ᑬ�j.
;	2.	����.
;		C	...	���W�X�^�ԍ� ( 0-23, 32-46 )
;		B	...	�ݒ�l.
;	3.	�Ԓl.
;		�Ȃ�.
;	4.	�@�\.
;		VDP �̎w�背�W�X�^�Ɏw��̒l����������.
;	5.	�j��.
;		af, bc, ix, iy
;
WRVDP:
		ld		a,(EXPTBL)
		push	af
		pop		iy
		ld		ix,WRTVDP
		call	CALSLT
		ret

;// -------------------------------------------------------------
;	1.	����.
;		�L�[�o�b�t�@�N���A.
;	2.	����.
;		�Ȃ�.
;	3.	�Ԓl.
;		�Ȃ�.
;	4.	�@�\.
;		�z�o�b�t�@�ł���L�[�o�b�t�@�̓ǂݏ����|�C���^�𓯂��ɂ���.
;	5.	�j��.
;		�Ȃ�.
;
CLRKEY:
		push	hl
		ld		hl,(PUTPNT)
		ld		(GETPNT),hl
		pop		hl
		ret

;// -------------------------------------------------------------
;	�f�[�^

;-	���b�Z�[�W.
MCOMPL:	db		'Complete.',13,10,'$'
MEOPEN:	db		'ERR: Cannot open file.'
MCRLF:	db		13,10,'$'

;-	�ϐ�
SAVEAD:	dw		0					;	�ǂݍ��݃|�C���^.

REFHED:	db		'AS2',0				;	�t�@�C���w�b�_�i4byte�j

VAGENE:	dw		0
VACOL:	dw		0
SAVEPT:	dw		0
BUFDAT:	ds		16					;	�ꎞ�o�b�t�@.

DEFPLT:	dw		000h,000h,611h,733h	;	�f�t�H���g�̃p���b�g�l
		dw		117h,327h,151h,627h
		dw		171h,373h,661h,664h
		dw		411h,265h,555h,777h
	end
