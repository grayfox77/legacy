// -------------------------------------------------------------
//	TITLE	:	MSX screen2 anime �t���[���E�t���[���ԏ���
//	INPUT	:	(C)2001	t.hara (HRA!)
//	HISTORY	:	2001/06/13	����
// -------------------------------------------------------------

// =============================================================
//	�C���N���[�h

#include <memory.h>

#include "anicnv.h"

// =============================================================
//	�^

//	�o�C�g�^
typedef unsigned char BYTE;

//	�p���b�g�^
typedef struct tPAL {
	int	red;
	int	green;
	int	blue;
} PAL;

//	���ʕ\�^�i�P�������X�g�j
typedef struct tDISTANCE {
	tDISTANCE		*next;
	unsigned int	dist;
	int				parts;
} DISTANCE;

// =============================================================
//	�ݒ�

//	�C�����C���W�J
#ifdef	__cplusplus
#define	INLINE	inline
#else
#define	INLINE	static	//	�R���p�C���Ǝ��g���̃C�����C���W�J������ꍇ�� static �ƒu�������Ă�������
#endif

//	SCREEN2 VRAM �T�C�Y
#define	_VRAMSIZE		0x4000

//	�L�����N�^��
#define	_PARTS			768

//	�p�^�[���W�F�l���[�^�e�[�u���̃A�h���X
#define	_PATGENE		0x0000

//	�p�^�[���l�[���e�[�u���̃A�h���X
#define	_PATNAME		0x1800

//	�p���b�g�e�[�u���̃A�h���X
#define	_PALTABLE		0x1B80

//	�J���[�e�[�u���̃A�h���X
#define	_PATCOL			0x2000

//	�f�[�^
#define	DAT_MOVE128		128
#define	DAT_CHAR		129		//	�����P�U�o�C�g���p�[�c�f�[�^
#define	DAT_VWAIT		130
#define	DAT_EXIT		131

//	���̓t�@�C����
#define	_LOADDAT	"F%07X.SC2"

// =============================================================
//	�v���g�^�C�v

static int _bload( BYTE *pvram, const char *fname );
static int _save_header( FILE *fp, BYTE *pvram );
static int _difference( FILE *fp, BYTE *pframe, const BYTE *pnext, PAL *pal, int diff, int size );
static unsigned int _distance_parts( const BYTE *pp1, const BYTE *pc1, const BYTE *pp2, const BYTE *pc2, const PAL *pal );
INLINE unsigned int _distance_line( BYTE p1, BYTE c1, BYTE p2, BYTE c2, const PAL *pal );
INLINE unsigned int _pow2( int n );
static void _get_palette( PAL *pal, const BYTE *pvram );

// =============================================================
//	�֐���`

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�ϊ�����
//	2.	����
//		fname	...	(I)	�ۑ��t�@�C����
//	3.	�Ԓl
//		0	...	���s
//		n	...	�ϊ������t���[����
//	4.	���l
//		�Ȃ�
// -------------------------------------------------------------
int ac_sc2anime( const char *fname, int diff ) {
	BYTE	fbuf[ _VRAMSIZE ];								//	�t���[���o�b�t�@
	BYTE	nbuf[ _VRAMSIZE ];								//	���t���[���o�b�t�@
	FILE	*fp;
	char	sname[ 260 ];
	int		n;
	int		size = 0;
	PAL		pal[16];

	//	�␳
	if( diff > _PARTS ) diff = _PARTS;
	if( diff < 1 ) diff = 1;
	printf( "Update parts par frame = %d\n", diff );

	//	�t�@�C�����J��
	fp = fopen( fname, "wb" );
	if( fp == NULL ) goto _error;

	//	��P�t���[���̓ǂݍ���
	n = 0;
	sprintf( sname, _LOADDAT, n );
	if( !_bload( fbuf, sname ) ) goto _error;
	++n;

	//	�w�b�_�E��P�t���[���̏o��
	_save_header( fp, fbuf );
	_get_palette( pal, fbuf );

	//	��Q�t���[���ȍ~
	for( ;; ) {
		//	�t���[���̓ǂݍ���
		sprintf( sname, _LOADDAT, n );
		if( !_bload( nbuf, sname ) ) break;
		++n;
		printf( "%d\r", n );
		//	�����̍쐬�ƃt���[���o�b�t�@�̍X�V
		size = _difference( fp, fbuf, nbuf, pal, diff, size );
	}

	//	�I���L��
	size = 0x4000 - (size % 0x4000);
	if( size == 0 ) size = 0x4000;
	memset( fbuf, DAT_EXIT, size );
	fwrite( fbuf, size, 1, fp );

	//	����
	fclose( fp );
	return n;

	//	�G���[�I��
_error:
	if( fp != NULL ) fclose( fp );
	return 0;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		BSAVE�`���� SCREEN2 �摜�f�[�^��ǂݍ���
//	2.	����
//		pvram	...	(I)	�u�q�`�l�C���[�W�A�h���X
//		fname	...	(I)	�t�@�C����
//	3.	�Ԓl
//		0	...	���s
//		1	...	����
//	4.	���l
//		�I�t�Z�b�g 0000 ����̃f�[�^�̂ݑΉ�
// -------------------------------------------------------------
static int _bload( BYTE *pvram, const char *fname ) {
	FILE	*fp;

	//	�t�@�C�����J��
	fp = fopen( fname, "rb" );
	if( fp == NULL ) return 0;

	//	�ǂݍ���
	fread( pvram, 7, 1, fp );
	fread( pvram, 1, _VRAMSIZE, fp );
	fclose( fp );

	return 1;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		AS2�`���̃t�@�C���w�b�_���o�͂���
//	2.	����
//		fp		...	(I)	�o�͐�t�@�C���X�g���[��
//		pvram	...	(I)	�u�q�`�l�C���[�W�A�h���X
//	3.	�Ԓl
//		0	...	���s
//		1	...	����
//	4.	���l
//		�Ȃ�
// -------------------------------------------------------------
static int _save_header( FILE *fp, BYTE *pvram ) {
	BYTE	head[4];

	//	�t�@�C���w�b�_�̏o��
	head[0] = 'A';		//	Animation
	head[1] = 'S';		//	full Screen
	head[2] = '2';		//	screen 2
	head[3]	= 0;		//	�\��i�[���ɂ��邱�Ɓj

	fwrite( head, sizeof(head), 1, fp );

	//	�p���b�g�̏o��
	fwrite( pvram + _PALTABLE, 32, 1, fp );

	//	��P�t���[���̏o��
	fwrite( pvram + _PATGENE, 0x1800, 1, fp );
	fwrite( pvram + _PATCOL , 0x1800, 1, fp );

	return 1;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�Q�t���[���̍��������߂ĐV�����t���[�����쐬����
//	2.	����
//		fp		...	(I)		�����f�[�^�o�͐�t�@�C���X�g���[��
//		pframe	...	(I/O)	�t���[���o�b�t�@
//		pnext	...	(I)		���t���[��
//		diff	...	(I)		�X�V�p�[�c�ő吔
//		size	...	(I)		����܂łɏ������񂾃T�C�Y
//	3.	�Ԓl
//		�������ݍς݃T�C�Y
//	4.	���l
//		�Ȃ�
// -------------------------------------------------------------
static int _difference( FILE *fp, BYTE *pframe, const BYTE *pnext, PAL *pal, int diff, int size ) {
	DISTANCE		distance[_PARTS];			//	�����\
	DISTANCE		update[_PARTS];				//	�ʒn��
	DISTANCE		*pdis, *psearch, *ptop, *pupd, *pprev;
	int				i, d, c;
	int				cur;						//	�J�����g�|�W�V����
	BYTE			buf[2];

	//	�N���A
	memset( distance, 0, sizeof( distance	) );
	memset( update	, 0, sizeof( update		) );

	//	�����\��}���\�[�g�ō쐬����
	ptop = pdis = distance;
	for( i = 0; i < _PARTS; ++i ) {
		pdis->parts	= i;
		pdis->dist	= _distance_parts(	pframe + i*8 + _PATGENE, pframe + i*8 + _PATCOL, 
										pnext  + i*8 + _PATGENE, pnext  + i*8 + _PATCOL, pal );
		if( i != 0 ) {
			//	�}���ꏊ�����߂�
			pprev	= NULL;
			psearch = ptop;
			for( ;; ) {
				//	�傫�����m�F����
				if( (psearch->dist) <= (pdis->dist) ) {
					//	�擪���X�V�̏ꍇ
					if( psearch == ptop ) ptop = pdis;
					//	�O�}��
					if( pprev != NULL ) pprev->next = pdis;
					pdis->next		= psearch;
					break;
				} else if( (psearch->next == NULL) ) {
					//	��ǉ�
					psearch->next	= pdis;
					break;
				}
				//	��
				pprev	= psearch;
				psearch = psearch->next;
			}
		}
		//	��
		++pdis;
	}

	//	�����̏�� diff �ʂ܂ł� parts �ʒu�ő傫�����ɑ}���\�[�g����
	pupd = pdis = update;
	for( i = 0; i < diff; ++i ) {
		*pdis		= *ptop;
		pdis->next	= NULL;
		ptop		= ptop->next;
		if( i != 0 ) {
			//	�}���ꏊ�����߂�
			pprev	= NULL;
			psearch = pupd;
			for( ;; ) {
				//	�傫�����m�F����
				if( (psearch->parts) < (pdis->parts) ) {
					//	�擪���X�V�̏ꍇ
					if( psearch == pupd ) pupd = pdis;
					//	�O�}��
					if( pprev != NULL ) pprev->next = pdis;
					pdis->next		= psearch;
					break;
				} else if( (psearch->next == NULL) ) {
					//	��ǉ�
					psearch->next	= pdis;
					break;
				}
				//	��
				pprev	= psearch;
				psearch = psearch->next;
			}
		}
		//	��
		++pdis;
	}

	//	�t�@�C���֏o�͂���
	pdis	= pupd;
	cur		= _PARTS-1;		//	���݈ʒu
	for( i = 0; i < diff; ++i ) {
		//	���݈ʒu����̈ړ������擾
		d = cur - (pdis->parts);
		//	�ړ��f�[�^���o��
		buf[0] = DAT_MOVE128;
		while( d > 127 ) {
			fwrite( buf, 1, 1, fp );	size += 1;
			d -= 128;
		}
		if( d ) {
			buf[0] = (BYTE)d;
			buf[1] = DAT_CHAR;
			fwrite( &buf, 2, 1, fp );	size += 2;
		} else {
			buf[0] = DAT_CHAR;
			fwrite( &buf, 1, 1, fp );	size += 1;
		}
		//	�`��f�[�^���o��
		cur	= (pdis->parts);
		c	= (*(pnext + cur + _PATNAME) + (cur & 0x300)) * 8;
		fwrite( pnext + c + _PATGENE, 8, 1, fp );	size += 8;
		fwrite( pnext + c + _PATCOL , 8, 1, fp );	size += 8;
		//	�t���[���o�b�t�@���X�V
		memcpy( pframe + c + _PATGENE, pnext + c + _PATGENE, 8 );
		memcpy( pframe + c + _PATCOL , pnext + c + _PATCOL , 8 );
		//	��
		pdis = pdis->next;
	}
	//	�t���[���̋�؂�ڂ��o��
	buf[0] = DAT_VWAIT;
	fwrite( buf, 1, 1, fp );	size += 1;
	return size;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�Q�̃p�[�c�̉��������߂�
//	2.	����
//		pp1	...	(I)	�p�[�c�P�`����̃A�h���X
//		pc1	...	(I)	�p�[�c�P�F���̃A�h���X
//		pp2	...	(I)	�p�[�c�Q�`����̃A�h���X
//		pc2	...	(I)	�p�[�c�Q�F���̃A�h���X
//		pal	...	(I)	�p���b�g�̃A�h���X
//	3.	�Ԓl
//		����
//	4.	���l
//		�Ԃ������͑召�֌W�̔�r�݂̂�ۏ؂��A���l�̐�ΓI�ȈӖ�
//		�͕ۏ؂��Ȃ��i�K���������֌W�ł͂Ȃ��j
// -------------------------------------------------------------
static unsigned int _distance_parts( const BYTE *pp1, const BYTE *pc1, const BYTE *pp2, const BYTE *pc2, const PAL *pal ) {
	unsigned int	sum;
	int				i;

	//	�����̌v�Z
	sum = 0;
	for( i = 0; i < 8; ++i ) {

		//	�P���C�����̋���
		sum += _distance_line( *pp1, *pc1, *pp2, *pc2, pal );
		++pp1;
		++pc1;
		++pp2;
		++pc2;
	}

	return sum;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�Q�̃��C���̉��������߂�
//	2.	����
//		p1	...	(I)	�p�[�c�P�`��
//		c1	...	(I)	�p�[�c�P�F
//		p2	...	(I)	�p�[�c�Q�`��
//		c2	...	(I)	�p�[�c�Q�F
//		pal	...	(I)	�p���b�g�̃A�h���X
//	3.	�Ԓl
//		����
//	4.	���l
//		�Ԃ������͑召�֌W�̔�r�݂̂�ۏ؂��A���l�̐�ΓI�ȈӖ�
//		�͕ۏ؂��Ȃ��i�K���������֌W�ł͂Ȃ��j
// -------------------------------------------------------------
INLINE unsigned int _distance_line( BYTE p1, BYTE c1, BYTE p2, BYTE c2, const PAL *pal ) {
	unsigned int	sum;
	int				i;
	int				col1[2], col2[2];
	const PAL		*pal1, *pal2;

	//	�F
	col1[0] = c1 & 0x0F;	col1[1] = (c1 >> 4) & 0x0F;
	col2[0] = c2 & 0x0F;	col2[1] = (c2 >> 4) & 0x0F;

	//	�����̌v�Z
	sum = 0;
	for( i = 0; i < 8; ++i ) {

		//	�p���b�g�擾
		pal1 = &pal[ col1[ p1 & 1 ] ];
		pal2 = &pal[ col2[ p2 & 1 ] ];

		//	�s�N�Z���ԉ������Z
		sum += _pow2( (pal1->red  ) - (pal2->red  ) )
			 + _pow2( (pal1->green) - (pal2->green) )
			 + _pow2( (pal1->blue ) - (pal2->blue ) );

		//	��
		p1 = p1 >> 1;
		p2 = p2 >> 1;
	}

	return sum;
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�Q������߂�
//	2.	����
//		n	...	(I)	���l
//	3.	�Ԓl
//		�Q��l
//	4.	���l
//		�Ȃ�
// -------------------------------------------------------------
INLINE unsigned int _pow2( int n ) {

	return( (unsigned int) (n*n) );
}

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�p���b�g���擾����
//	2.	����
//		pal		...	(O)	�p���b�g�i�[��
//		pvram	...	(I)	�t���[���f�[�^
//	3.	�Ԓl
//		�Ȃ�
//	4.	���l
//		pal �͕K�� 16�F�� �ȏ㑶�݂��Ȃ���΂Ȃ�Ȃ�
// -------------------------------------------------------------
static void _get_palette( PAL *pal, const BYTE *pvram ) {
	int		i;

	pvram += _PALTABLE;
	for( i = 0; i < 16; ++i ) {

		pal->red	= ((*pvram) >> 4) & 0x07;
		pal->blue	=  (*pvram)		  & 0x07; ++pvram;
		pal->green	=  (*pvram)		  & 0x07; ++pvram;
		++pal;
	}
}
