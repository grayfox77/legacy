/******************* Miscellaneous Definitions ***********************/

typedef char jmp_buf[32];

typedef union {
    struct {
       char a, c, b, e, d, l, h;
    } byte;
    struct {
       char a;
       int  bc, de, hl, ix, iy;
    } word;
} REGS;

/***************************************************************/
 $P SA9AX P X EJLEFT$   P  G  LEFT$   AX  