#include <stdio.h>
#include <string.h>

#define SKIPSPACE(p) while(*p==' '||*p=='\t') p++;

/* #define DEBUG */

struct sAlias {
	char *name;
	char *body;
} *alias;
int aliasMax=0;

struct sMacro {
	char *name;
	char *body;
	int argc;
} *macro;
int macroMax=0;

char *margv[] = { "%%0","%%1","%%2","%%3","%%4","%%5" };
char *directive[] = {
	".macro",".MACRO",".alias",".ALIAS",".include",".INCLUDE"
};
int directiveMax = 6;

char *bufferTail=NULL;
FILE *iFile = stdin;
FILE *oFile = stdout;

int processCalled = 0;
int processLine = 0;
char *processFile = "        .   ";

int prErr(char *);
char *strchg(char *,const char *,const char *);
int ExtractAlias(char *);
int ExtractMacro(char *);
void SetAlias(char *,char *);
void SetMacro(char *,char *);
void ExtractLine(char *);
void ExtractFile(char *);


/* �G���[���� */
int prErr(char *message)
{
	fprintf(stderr,"%s:(%d) error:%s",processFile,processLine,message);
	exit(-1);
}

/* ������̒u���istr1 ���� str2 �� str3 �ɒu���j */
char *strchg(char *str1,const char *str2,const char *str3)
{
	char *p,*src;
	int len;

	p=strstr(str1,str2);
	if(p!=NULL) {
		len=strlen(str3); src=p+strlen(str2);
		memmove(p+len,src,strlen(src)+1);
		memcpy(p,str3,len);
	}
	return p;
}


/* �G�C���A�X�̑��݃`�F�b�N�E�W�J */
int ExtractAlias(char *buf)
{
	int i,len,lenMax = 0;
	struct sAlias *hit = NULL;
	
	/* �G�C���A�X�̑��݃`�F�b�N */
	for(i=0;i<aliasMax;i++) {
		if(strstr(buf,alias[i].name)!=NULL) {
			len=strlen(alias[i].name);
			if(lenMax<len) {
				lenMax=len;
				hit=&alias[i];
			}
		}
	}
	/* ���� */
	if(hit!=NULL) {
		strchg(buf,hit->name,hit->body);
		ExtractLine(buf);
		return 1;
	} else return 0;
}

/* �}�N���̑��݃`�F�b�N�ƓW�J */
int ExtractMacro(char *buf)
{
	char *p,*q;
	int i,j,len,lenMax;
	char tmp[BUFSIZ];
	char argv[16][16];
	struct sMacro *hit;

	/* �}�N���ƈ�v���邩�`�F�b�N */
	hit=NULL; lenMax=0;
	for(i=0;i<macroMax;i++) {
		p=strstr(buf,macro[i].name);
		len=strlen(macro[i].name);
		if(p!=NULL&&(*(p+len)==' '||*(p+len)=='\t'||*(p+len)=='\n')) {
			if(lenMax<len) { 
				lenMax=len;
				hit=&macro[i];
			}
		}
	}
	/* �}�N������ */
	if(hit!=NULL) {
		p=strstr(buf,hit->name);
		*p='\0';
		ExtractLine(buf);
		p+=lenMax;
		SKIPSPACE(p);
		i=j=0;
		/* �����̔c�� */
		while(1) {
			while(*p!=','&&*p!='\n') argv[i][j++]=*p++;
			argv[i][j]='\0';
			if(j) i++;
			if(*p=='\n') break;
			p++; j=0;
		}
		if(i!=hit->argc) prErr("�����̐��������܂���B");
		
		/* �}�N���̓W�J */
		p=hit->body;
		while(*p!='\0') {
			q=tmp;
			while(*p!='\n') *q++=*p++;
			*p++;
			*q++='\n'; *q='\0';
			for(i=0;i<hit->argc;i++)
				while(strchg(tmp,margv[i],argv[i])!=NULL);
			ExtractLine(tmp);
		}
		return 1;
	} else return 0;
}

/* �G�C���A�X��ݒ� */
void SetAlias(char *buf,char *p)
{
	int i;

	/* �G�C���A�X���̊l�� */
	SKIPSPACE(p);
	alias[aliasMax].name=bufferTail;
	while(*p!=' '&&*p!='\n') *bufferTail++=*p++;
	*bufferTail++='\0';
	SKIPSPACE(p);
	
	/* �����̃G�C���A�X���폜 */
	alias[aliasMax].body=bufferTail;
	for(i=0;i<aliasMax;i++)
		if(strcmp(alias[aliasMax].name,alias[i].name)==0)
			alias[i].body=alias[aliasMax].body;
	
	/* �G�C���A�X�{�̂̋L�� */
	while(*p!='\n') *bufferTail++=*p++;
	*bufferTail++='\0';
	aliasMax++;
}


/* �}�N����ݒ� */
void SetMacro(char *buf,char *p)
{
	char argv[16][16];
	int i,j;

	/* �}�N�����̐ݒ� */
	SKIPSPACE(p);
	macro[macroMax].name=bufferTail;
	while(*p!=' '&&*p!='\t'&&*p!='\n') *bufferTail++=*p++;
	*bufferTail++='\0';
	SKIPSPACE(p);
	i=j=0;

	/* ������c�� */
	while(1) {
		while(*p!=','&&*p!='\n') argv[i][j++]=*p++;
		argv[i][j]='\0';
		if(j) i++;
		if(*p=='\n') break;
		p++; j=0;
	}
	macro[macroMax].argc=i;
	macro[macroMax].body=bufferTail;

	/* �����̃}�N�����`�F�b�N */
	for(i=0;i<macroMax;i++)
		if(strcmp(macro[macroMax].name,macro[i].name)==0) {
			macro[i].body=macro[macroMax].body;
			macro[i].argc=macro[macroMax].argc;
		}

	/* �}�N���{�̂̋L�� */
	while(1) {
		if(fgets(bufferTail,BUFSIZ,iFile)==NULL)
			prErr(".endm��������܂���B");
		processLine++;
		if(strstr(bufferTail,".endm")!=NULL||strstr(bufferTail,".ENDM")!=NULL)
			break;
		/* ����������`���ɕϊ� */
		for(i=0;i<macro[macroMax].argc;i++)
			while(strchg(bufferTail,argv[i],margv[i])!=NULL);
		bufferTail+=strlen(bufferTail);
	}
	*bufferTail++='\0';

	/* �f�o�O�p */
#ifdef DEBUG
	fprintf(stderr,"%d�Ԃ�ϸ� %s (���� %d: "
		,macroMax,macro[macroMax].name,macro[macroMax].argc);
	for(i=0;i<macro[macroMax].argc;i++)
		fprintf(stderr,"%s ",argv[i]);
	fprintf(stderr,") ���`\n--\n%s--\n",macro[macroMax].body);
#endif
	macroMax++;
}


/* ��s��W�J */
void ExtractLine(char *buf)
{
	int i;
	char *p,*q;

	/* �����Ȃ��s�ƃR�����g�s���p�X���� */
	SKIPSPACE(buf);
	if(*buf=='\n'||*buf=='#'||*buf=='\0') return;
	
	/* aspp �̖��߂��܂܂�Ă��邩�ǂ������ׂ� */
	for(i=0;i<directiveMax;i++) {
		p=strstr(buf,directive[i]);
		if(p!=NULL) {
			p+=strlen(directive[i]);
			break;
		}
	}
	/* ���߂��Ƃɕ��� */
	switch(i) {
		case 0:
		case 1:
			/* �}�N���̐ݒ� */
			SetMacro(buf,p);
			break;

		case 2:
		case 3:
			/* �G�C���A�X�̐ݒ� */
			SetAlias(buf,p);
			break;

		case 4:
		case 5:
			/* �C���N���[�h���� */
			SKIPSPACE(p);
			if(*p++!='"') prErr("�t�@�C�������w�肳��Ă��܂���B");
			q=p;
			while(*p!='"') p++;
			*p='\0';
			ExtractFile(q);
			break;

		default:
			/* �ǂ�ɂ����Ă͂܂�Ȃ���΃C���f���g���ďo�� */
			if(!ExtractAlias(buf)&&!ExtractMacro(buf)) {
				p=buf;
				SKIPSPACE(p);
				if((strchr(p,':')==NULL)&&(*p!='.')) fputc('\t',oFile);
				fputs(p,oFile);
			}
			break;
	}
}


/* �t�@�C���P�ʂł̏��� */
void ExtractFile(char *fileName)
{
	FILE *file,*tmpFile;
	char buf[BUFSIZ];
	int tmpLine;
	char *tmpFileName;
	
	tmpLine=processLine; processLine=1;
	tmpFileName=processFile; processFile=fileName;
	if(fileName==NULL) {
		fileName="�W������";
		iFile=stdin;
	} else
		if((file=fopen(fileName,"rt"))==NULL)
			prErr("�t�@�C����������܂���B");
	strcpy(processFile,fileName);
	tmpFile=iFile; iFile=file;
	
	/* �W�J */
	while(fgets(buf,BUFSIZ,iFile)!=NULL) {
		ExtractLine(buf);
		processLine++;
	}
	processLine=tmpLine;
	processFile=tmpFileName;
	iFile=tmpFile;
}


int main(int argc,char *argv[])
{
	/* �G�C���A�X�E�}�N���o�b�t�@�̊m�� */
	bufferTail=malloc(1024*1024*sizeof(char));
	if(bufferTail==NULL) prErr("������������܂���B");
	alias=(struct sAlias *)bufferTail;
	macro=(struct sMacro *)(bufferTail+64*1024);
	bufferTail+=25*1024;

	/* �t�@�C����W�J */
	ExtractFile((argc==2)? argv[1]:NULL);
	free(alias);
	return 0;
}

