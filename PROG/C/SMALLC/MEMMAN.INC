/* MEMMAN.INC  0.2  OtB 1993 */
#define PSEG   0
#define FSEG   255
#asm
    EXTRN   mHeapA, mHeapD, mHeapM, mXTSRC, mTSRCl, mGetID, mFdTSR
    EXTRN   mUse0, mUse1, mUse2, mIniCh, mVersn, mAlloc, mDeAll, mGetSg
    EXTRN   mCurSg, mNumSg, mSetRs, mClrRs, mFreSg, mDblSg, mDos2a, mCurSg
    EXTRN   mStoSg, mRstSg, mGetTP, mSetSP, mGetSP, mClBAS
#endasm
                    