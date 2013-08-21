// -------------------------------------------------------------
//	TITLE	:	MSX screen2 anime
//	INPUT	:	(C)2001	t.hara (HRA!)
//	HISTORY	:	2001/06/13	����
// -------------------------------------------------------------

// =============================================================
//	�C���N���[�h

#include <stdio.h>
#include <stdlib.h>
#include "anicnv.h"

// =============================================================
//	�ݒ�

//	���b�Z�[�W
#define	_TITLE		"sc2anime v0.10625 (C)2001 HRA!\n"
#define	_USAGE		"Usage>SC2ANIME <save name> [diff]\n"		\
					"\tdiff = 1~768\n"
#define	_ERROR		"convert error.\n"
#define	_COMPLETE	"completed.\n"

//	�f�t�H���g�� diff�i�傫������ƍĐ��掿����A����������ƍĐ����x����j
#define	_DIFFLEVEL	150

// =============================================================
//	�֐���`

// -------------------------------------------------------------
//	1.	���{�ꖼ
//		�N���֐�
//	2.	����
//		argc	...	(I)	�R�}���h���C�������̐�
//		argv	...	(I)	�R�}���h���C������
//	3.	�Ԓl
//		0
//	4.	���l
//		�Ȃ�
// -------------------------------------------------------------
int main( int argc, char* argv[] ) {
	int	diff;

	//	�^�C�g���\��
	printf( _TITLE );

	//	�����s���`�F�b�N
	if( argc < 2 ) {
		printf( _USAGE );
		return 0;
	}
	if( argc > 2 ) {
		diff = atoi( argv[2] );
	} else {
		diff = _DIFFLEVEL;
	}

	//	�ϊ�����
	if( ac_sc2anime( argv[1], diff ) ) {
		printf( _COMPLETE );
	} else {
		printf( _ERROR );
	}
	return 0;
}
