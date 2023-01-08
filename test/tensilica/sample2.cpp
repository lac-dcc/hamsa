#define NULL ((void*)0)
typedef unsigned long size_t;  // Customize by platform.
typedef long intptr_t; typedef unsigned long uintptr_t;
typedef long scalar_t__;  // Either arithmetic or pointer type.
/* By default, we understand bool (as a convenience). */
typedef int bool;
#define false 0
#define true 1

/* Forward declarations */

/* Type definitions */
typedef  int /*<<< orphan*/  xb_vecNx16 ;
typedef  int /*<<< orphan*/  xb_vecN_2x32v ;
typedef  int /*<<< orphan*/  valign ;
typedef  int int32_t ;
typedef  int /*<<< orphan*/  int16_t ;
typedef  int /*<<< orphan*/  MORPH_ODT_VECTOR ;
typedef  int /*<<< orphan*/  MORPH_ODT_SCALAR ;
typedef  int /*<<< orphan*/  MORPH_IDT_VECTOR ;
typedef  int /*<<< orphan*/  MORPH_IDT_SCALAR ;

/* Variables and functions */
 scalar_t__ IVP_CVT48SNX32 (int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_MAXNX16 (int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_MINNX16 (int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_PACKVRNX48 (scalar_t__,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_ZALIGN () ; 
 int MORPH_BYTES_PER_PIXEL ; 
 int /*<<< orphan*/  MORPH_IP_LOAD (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_IP_LOAD_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,int) ; 
 int /*<<< orphan*/  MORPH_IP_MSUB_ALPHAMUL (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  MORPH_IP_PRIME (int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_STORE (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_STORE_FLUSH (int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_STORE_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,int /*<<< orphan*/ *,int,int) ; 
 int /*<<< orphan*/  SCALE_SHIFT_BIASADD_PACKTO_32 (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 void XAI_ERROR_STATUS () ; 
 scalar_t__ XCHAL_IVPN_SIMD_WIDTH ; 
 int XT_MAX (int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  XT_MIN (int,int) ; 
 scalar_t__ acc1 ; 
 scalar_t__ acc2 ; 
 int /*<<< orphan*/  alphaVal ; 
 int /*<<< orphan*/  betaVal ; 
 size_t inDepth ; 
 int inHeight ; 
 int inP1 ; 
 size_t inP2 ; 
 scalar_t__ inWidth ; 
 int /*<<< orphan*/  meanVal ; 
 int outP1 ; 
 size_t outP2 ; 
 int /*<<< orphan*/ * pAlpha ; 
 int /*<<< orphan*/ * pBeta ; 
 int /*<<< orphan*/ * pIn1 ; 
 int /*<<< orphan*/ * pIn2 ; 
 scalar_t__ pInData ; 
 int /*<<< orphan*/ * pMean ; 
 int /*<<< orphan*/ * pOut1 ; 
 int /*<<< orphan*/ * pOut2 ; 
 scalar_t__ pOutData ; 
 int /*<<< orphan*/ * pRecip ; 
 int /*<<< orphan*/  packBool ; 
 int /*<<< orphan*/ * pvecIn1 ; 
 int /*<<< orphan*/ * pvecIn2 ; 
 int /*<<< orphan*/ * pvecOut1 ; 
 int /*<<< orphan*/ * pvecOut2 ; 
 int /*<<< orphan*/  recipVal ; 
 int /*<<< orphan*/  shift1 ; 
 int /*<<< orphan*/  shift2 ; 
 scalar_t__ tx ; 
 int ty ; 
 int /*<<< orphan*/  vecIn1 ; 
 int /*<<< orphan*/  vecIn2 ; 
 int /*<<< orphan*/  vecMax ; 
 int /*<<< orphan*/  vecMin ; 
 int /*<<< orphan*/  vecOut1 ; 
 int /*<<< orphan*/  vecOut2 ; 
 scalar_t__ x ; 
 int y ; 
 size_t z ; 


//-------------------------
void foo() {
for (z = 0; z < inDepth; z++)
  {
    meanVal  = pMean[z];
    recipVal = pRecip[z];
    alphaVal = pAlpha[z];
    betaVal  = pBeta[z];

    xb_vecNx16 vecMean     = (int16_t) meanVal;
    xb_vecN_2x32v hvecBeta = betaVal;
    xb_vecNx16 vecAlpha    = alphaVal;

    acc1 = acc2 = 0;

    for (y = 0; y <= (inHeight - 2); y += 2)
    {
      /* Pointer update */
      pIn1 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + y * inP1);
      pIn2 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + y * inP1 + inP1);

      pOut1 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + y * outP1);
      pOut2 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + y * outP1 + outP1);

      pvecIn1 = (MORPH_IDT_VECTOR *) pIn1;
      pvecIn2 = (MORPH_IDT_VECTOR *) pIn2;

      pvecOut1 = (MORPH_ODT_VECTOR *) pOut1;
      pvecOut2 = (MORPH_ODT_VECTOR *) pOut2;

      /* alignment vectors for input load*/
      valign vaIn1 = MORPH_IP_PRIME(pvecIn1);
      valign vaIn2 = MORPH_IP_PRIME(pvecIn2);

      valign vaOut1 = IVP_ZALIGN();
      valign vaOut2 = IVP_ZALIGN();

      for (x = 0; x <= (inWidth - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        MORPH_IP_LOAD(vecIn1, vaIn1, pvecIn1);
        MORPH_IP_LOAD(vecIn2, vaIn2, pvecIn2);

        xb_vecN_2x32v hvecData1H;
        xb_vecN_2x32v hvecData1L;
        xb_vecN_2x32v hvecData2H;
        xb_vecN_2x32v hvecData2L;

        MORPH_IP_MSUB_ALPHAMUL(vecIn1, vecMean, vecAlpha, hvecData1H, hvecData1L);
        MORPH_IP_MSUB_ALPHAMUL(vecIn2, vecMean, vecAlpha, hvecData2H, hvecData2L);

        xb_vecN_2x32v hvecRes1H, hvecRes1L;
        xb_vecN_2x32v hvecRes2H, hvecRes2L;

        SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData1H, hvecData1L, hvecRes1H, hvecRes1L, hvecBeta, hvecBeta, shift1, shift2, packBool);
        SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData2H, hvecData2L, hvecRes2H, hvecRes2L, hvecBeta, hvecBeta, shift1, shift2, packBool);

        acc1 = IVP_CVT48SNX32(hvecRes1H, hvecRes1L);
        acc2 = IVP_CVT48SNX32(hvecRes2H, hvecRes2L);

        vecOut1 = IVP_PACKVRNX48(acc1, 0);
        vecOut2 = IVP_PACKVRNX48(acc2, 0);

        vecOut1 = IVP_MAXNX16(IVP_MINNX16(vecOut1, vecMax), vecMin);
        vecOut2 = IVP_MAXNX16(IVP_MINNX16(vecOut2, vecMax), vecMin);

        MORPH_OP_STORE(vecOut1, vecOut2, vaOut1, vaOut2, pvecOut1, pvecOut2);
      }
      MORPH_OP_STORE_FLUSH(vaOut1, pvecOut1);
      MORPH_OP_STORE_FLUSH(vaOut2, pvecOut2);
    }

    if (inWidth > x)       /* tail handling along input width */
    {
      for (ty = 0; ty <= inHeight - 2; ty += 2)
      {
        /* flags for tail along height */
        int32_t remY1 = XT_MAX(0, XT_MIN(1, inHeight - ty - 1));

        /* Pointer update */
        pIn1 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + ty * inP1 + x);
        pIn2 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + ty * inP1 + x + inP1 * remY1);

        pOut1 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + ty * outP1 + x);
        pOut2 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + ty * outP1 + x + outP1 * remY1);

        pvecIn1 = (MORPH_IDT_VECTOR *) pIn1;
        pvecIn2 = (MORPH_IDT_VECTOR *) pIn2;

        pvecOut1 = (MORPH_ODT_VECTOR *) pOut1;
        pvecOut2 = (MORPH_ODT_VECTOR *) pOut2;

        /* alignment vectors for input load*/
        valign vaIn1 = MORPH_IP_PRIME(pvecIn1);
        valign vaIn2 = MORPH_IP_PRIME(pvecIn2);

        valign vaOut1 = IVP_ZALIGN();
        valign vaOut2 = IVP_ZALIGN();

        for (tx = x; tx < inWidth; tx += XCHAL_IVPN_SIMD_WIDTH)
        {
          int32_t remX = inWidth - tx;

          /* load remaining contiguous bytes along the input width from the four consecutive heights*/
          MORPH_IP_LOAD_XP(vecIn1, vaIn1, pvecIn1, MORPH_BYTES_PER_PIXEL * remX);
          MORPH_IP_LOAD_XP(vecIn2, vaIn2, pvecIn2, MORPH_BYTES_PER_PIXEL * remX * remY1);

          xb_vecN_2x32v hvecData1H;
          xb_vecN_2x32v hvecData1L;
          xb_vecN_2x32v hvecData2H;
          xb_vecN_2x32v hvecData2L;

          MORPH_IP_MSUB_ALPHAMUL(vecIn1, vecMean, vecAlpha, hvecData1H, hvecData1L);
          MORPH_IP_MSUB_ALPHAMUL(vecIn2, vecMean, vecAlpha, hvecData2H, hvecData2L);

          xb_vecN_2x32v hvecRes1H, hvecRes1L;
          xb_vecN_2x32v hvecRes2H, hvecRes2L;

          SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData1H, hvecData1L, hvecRes1H, hvecRes1L, hvecBeta, hvecBeta, shift1, shift2, packBool);
          SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData2H, hvecData2L, hvecRes2H, hvecRes2L, hvecBeta, hvecBeta, shift1, shift2, packBool);

          acc1 = IVP_CVT48SNX32(hvecRes1H, hvecRes1L);
          acc2 = IVP_CVT48SNX32(hvecRes2H, hvecRes2L);

          vecOut1 = IVP_PACKVRNX48(acc1, 0);
          vecOut2 = IVP_PACKVRNX48(acc2, 0);

          vecOut1 = IVP_MAXNX16(IVP_MINNX16(vecOut1, vecMax), vecMin);
          vecOut2 = IVP_MAXNX16(IVP_MINNX16(vecOut2, vecMax), vecMin);

          MORPH_OP_STORE_XP(vecOut1, vecOut2, vaOut1, vaOut2, pvecOut1, pvecOut2, remX, remY1);
        }
        MORPH_OP_STORE_FLUSH(vaOut1, pvecOut1);
        MORPH_OP_STORE_FLUSH(vaOut2, pvecOut2);
      }
    }
    if (inHeight > y)          /* tail handling along input height */
    {
      /* flags for tail along height */
      int32_t remY1 = XT_MAX(0, XT_MIN(1, inHeight - y - 1));

      /* Pointer update */
      pIn1 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + y * inP1);
      pIn2 = (MORPH_IDT_SCALAR *) (pInData + z * inP2 + y * inP1 + inP1 * remY1);

      pOut1 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + y * outP1);
      pOut2 = (MORPH_ODT_SCALAR *) (pOutData + z * outP2 + y * outP1 + outP1 * remY1);

      pvecIn1 = (MORPH_IDT_VECTOR *) pIn1;
      pvecIn2 = (MORPH_IDT_VECTOR *) pIn2;

      pvecOut1 = (MORPH_ODT_VECTOR *) pOut1;
      pvecOut2 = (MORPH_ODT_VECTOR *) pOut2;

      /* alignment vectors for input load*/
      valign vaIn1 = MORPH_IP_PRIME(pvecIn1);
      valign vaIn2 = MORPH_IP_PRIME(pvecIn2);

      valign vaOut1 = IVP_ZALIGN();
      valign vaOut2 = IVP_ZALIGN();

      for (tx = 0; tx < inWidth; tx += XCHAL_IVPN_SIMD_WIDTH)
      {
        int32_t remX = inWidth - tx;
        /* load remaining contiguous bytes along the input width from the four consecutive heights*/
        MORPH_IP_LOAD_XP(vecIn1, vaIn1, pvecIn1, MORPH_BYTES_PER_PIXEL * remX);
        MORPH_IP_LOAD_XP(vecIn2, vaIn2, pvecIn2, MORPH_BYTES_PER_PIXEL * remX * remY1);

        xb_vecN_2x32v hvecData1H;
        xb_vecN_2x32v hvecData1L;
        xb_vecN_2x32v hvecData2H;
        xb_vecN_2x32v hvecData2L;

        MORPH_IP_MSUB_ALPHAMUL(vecIn1, vecMean, vecAlpha, hvecData1H, hvecData1L);
        MORPH_IP_MSUB_ALPHAMUL(vecIn2, vecMean, vecAlpha, hvecData2H, hvecData2L);

        xb_vecN_2x32v hvecRes1H, hvecRes1L;
        xb_vecN_2x32v hvecRes2H, hvecRes2L;

        SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData1H, hvecData1L, hvecRes1H, hvecRes1L, hvecBeta, hvecBeta, shift1, shift2, packBool);
        SCALE_SHIFT_BIASADD_PACKTO_32(recipVal, recipVal, hvecData2H, hvecData2L, hvecRes2H, hvecRes2L, hvecBeta, hvecBeta, shift1, shift2, packBool);

        acc1 = IVP_CVT48SNX32(hvecRes1H, hvecRes1L);
        acc2 = IVP_CVT48SNX32(hvecRes2H, hvecRes2L);

        vecOut1 = IVP_PACKVRNX48(acc1, 0);
        vecOut2 = IVP_PACKVRNX48(acc2, 0);

        vecOut1 = IVP_MAXNX16(IVP_MINNX16(vecOut1, vecMax), vecMin);
        vecOut2 = IVP_MAXNX16(IVP_MINNX16(vecOut2, vecMax), vecMin);

        MORPH_OP_STORE_XP(vecOut1, vecOut2, vaOut1, vaOut2, pvecOut1, pvecOut2, remX, remY1);
      }
      MORPH_OP_STORE_FLUSH(vaOut1, pvecOut1);
      MORPH_OP_STORE_FLUSH(vaOut2, pvecOut2);
    }
  }
  return(XAI_ERROR_STATUS());
}

