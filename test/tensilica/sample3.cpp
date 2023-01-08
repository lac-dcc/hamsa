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
typedef  int /*<<< orphan*/  xb_vecNx16U ;
typedef  int /*<<< orphan*/  xb_vec2Nx8 ;
typedef  int /*<<< orphan*/  valign ;
typedef  int /*<<< orphan*/  uint16_t ;
typedef  int /*<<< orphan*/  int8_t ;

/* Variables and functions */
 int /*<<< orphan*/  IVP_LA2NX8_IP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  IVP_LA2NX8_PP (int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  IVP_LANX16U_PP (int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  IVP_LAVNX16U_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,int) ; 
 int /*<<< orphan*/  IVP_SAPOS2NX8_FP (int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  IVP_SAPOSNX16U_FP (int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  IVP_SAV2NX8_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,scalar_t__) ; 
 int /*<<< orphan*/  IVP_SAVNX16U_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,int) ; 
 int /*<<< orphan*/  IVP_ZALIGN () ; 
 int /*<<< orphan*/  XAI_ARRAY_SET_DATA_PTR (int /*<<< orphan*/ *,int /*<<< orphan*/ *) ; 
 void XAI_ERROR_STATUS () ; 
 int XCHAL_IVPN_SIMD_WIDTH ; 
 scalar_t__ dim2Size ; 
 size_t dim3Size ; 
 int /*<<< orphan*/  inDataPitch1 ; 
 size_t inDataPitch2 ; 
 scalar_t__ numLargestVal ; 
 int /*<<< orphan*/  outIdxPitch1 ; 
 size_t outIdxPitch2 ; 
 int /*<<< orphan*/  outValPitch1 ; 
 size_t outValPitch2 ; 
 int /*<<< orphan*/  pExtraValCnt ; 
 scalar_t__ pIdxU ; 
 int /*<<< orphan*/ * pInput ; 
 int /*<<< orphan*/ * pOutputIdx ; 
 int /*<<< orphan*/ * pOutputVal ; 
 int /*<<< orphan*/ * pSortedIdx ; 
 int /*<<< orphan*/ * pSortedVal ; 
 scalar_t__ pVal ; 
 int /*<<< orphan*/ * pdvecOut ; 
 int /*<<< orphan*/ * pvecIdx ; 
 int /*<<< orphan*/  sort_S8 (int /*<<< orphan*/ *,int /*<<< orphan*/ ,int /*<<< orphan*/ ,scalar_t__,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  sortedIdxArr ; 
 int /*<<< orphan*/  sortedValArr ; 
 int /*<<< orphan*/  values ; 
 scalar_t__ x ; 
 scalar_t__ y ; 
 size_t z ; 


//-------------------------
void foo() {
for (z = 0; z < dim3Size; z++)              /* along the image depth(WHD)/height(DWH) */
  {
    /* input and output data pointers */
    int8_t *pIn       = &pInput[(z * inDataPitch2)];
    uint16_t *pOutIdx = &pOutputIdx[(z * outIdxPitch2)];
    int8_t *pOutVal   = &pOutputVal[(z * outValPitch2)];

    for (y = 0; y < dim2Size; y++)            /* along the image height(WHD)/width(DWH) */
    {
      pdvecOut = (xb_vec2Nx8 *) pOutVal;
      pvecIdx  = (xb_vecNx16U *) pOutIdx;

      /* Sort the array in the descending order */
      XAI_ARRAY_SET_DATA_PTR(&values, pIn);
      sort_S8(&values, sortedValArr, sortedIdxArr, numLargestVal, pExtraValCnt);
      pExtraValCnt++;

      /* vector and pointer to load and store sorted values */
      xb_vec2Nx8 dvecOut;
      pSortedVal = (xb_vec2Nx8 *) (pVal);
      valign vaSortVal = IVP_LA2NX8_PP(pSortedVal);

      /* vector and pointer to load and store index of sorted values */
      xb_vecNx16U vecIdxL, vecIdxU;
      pSortedIdx = (xb_vecNx16U *) (pIdxU);
      valign vaSortIdx = IVP_LANX16U_PP(pSortedIdx);

      valign vaOutVal = IVP_ZALIGN();
      valign vaOutIdx = IVP_ZALIGN();

      /* loop through the sorted values and index to load and store */
      for (x = 0; x < numLargestVal; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
      {
        /* load and then store the sorted values */
        IVP_LA2NX8_IP(dvecOut, vaSortVal, pSortedVal);
        IVP_SAV2NX8_XP(dvecOut, vaOutVal, pdvecOut, numLargestVal - x);

        /* load and then store the index of the sorted values */
        IVP_LAVNX16U_XP(vecIdxL, vaSortIdx, pSortedIdx, 2 * (numLargestVal - x));
        IVP_LAVNX16U_XP(vecIdxU, vaSortIdx, pSortedIdx, 2 * (numLargestVal - x - XCHAL_IVPN_SIMD_WIDTH));
        IVP_SAVNX16U_XP(vecIdxL, vaOutIdx, pvecIdx, 2 * (numLargestVal - x));
        IVP_SAVNX16U_XP(vecIdxU, vaOutIdx, pvecIdx, 2 * (numLargestVal - x - XCHAL_IVPN_SIMD_WIDTH));
      } /* end of for(x  = 0; x < numLargestVal ; x += 2 * XCHAL_IVPN_SIMD_WIDTH) */
      IVP_SAPOS2NX8_FP(vaOutVal, pdvecOut);
      IVP_SAPOSNX16U_FP(vaOutIdx, pvecIdx);

      pIn     += inDataPitch1;
      pOutVal += outValPitch1;
      pOutIdx += outIdxPitch1;
    } /* end of for (y = 0; y < dim2Size; y++)*/
  }   /* end of for (z = 0; z < dim3Size; z++) */
  return(XAI_ERROR_STATUS());
}

