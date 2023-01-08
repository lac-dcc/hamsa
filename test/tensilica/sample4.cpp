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
typedef  int /*<<< orphan*/  vbool2N ;
typedef  int /*<<< orphan*/  valign ;
typedef  size_t uint16_t ;
typedef  scalar_t__ int32_t ;
typedef  int /*<<< orphan*/  MORPH_IDT_VECTOR ;
typedef  scalar_t__ MORPH_IDT_SCALAR ;

/* Variables and functions */
 int /*<<< orphan*/  IVP_ADDNX16U (int /*<<< orphan*/ ,scalar_t__) ; 
 int /*<<< orphan*/  IVP_ADDNX16UT (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_EQNX16 (scalar_t__,scalar_t__) ; 
 int /*<<< orphan*/  IVP_EXTRACTB2N (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_GT2NX8 (scalar_t__,int /*<<< orphan*/ ) ; 
 size_t IVP_MOV16U_FROMNX16U (int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  IVP_SEQ2NX8U () ; 
 int /*<<< orphan*/  IVP_SEQNX16U () ; 
 int /*<<< orphan*/  IVP_ZALIGN () ; 
 int /*<<< orphan*/  MAX_INDEX_2NX8 (scalar_t__,scalar_t__,size_t) ; 
 int /*<<< orphan*/  MORPH_OP_BMAX (int /*<<< orphan*/ ,scalar_t__,int /*<<< orphan*/ ,scalar_t__) ; 
 int /*<<< orphan*/  MORPH_OP_FLUSH_SAPOS (int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_LOAD_IP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_MAX_VAL (scalar_t__,scalar_t__) ; 
 scalar_t__ MORPH_OP_MIN_VALUE ; 
 int /*<<< orphan*/  MORPH_OP_MOVE (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ) ; 
 int /*<<< orphan*/  MORPH_OP_PRIME (int /*<<< orphan*/ *) ; 
 int /*<<< orphan*/  MORPH_OP_VAR_STORE_XP (int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ *,scalar_t__) ; 
 void XAI_ERROR_STATUS () ; 
 scalar_t__ XCHAL_IVPN_SIMD_WIDTH ; 
 scalar_t__ dim1Size ; 
 int dim2Size ; 
 int dim3Size ; 
 int /*<<< orphan*/  dvbGT ; 
 int /*<<< orphan*/  dvecIn ; 
 int /*<<< orphan*/  dvecInTemp ; 
 scalar_t__ dvecMaxVal ; 
 int /*<<< orphan*/  dvecMin ; 
 int inDataPitch1 ; 
 int inDataPitch2 ; 
 size_t k ; 
 size_t maxIdx ; 
 scalar_t__ maxVal ; 
 size_t numLargestVal ; 
 int outIdxPitch1 ; 
 int outIdxPitch2 ; 
 int /*<<< orphan*/ * outTileIdx ; 
 int /*<<< orphan*/ * outTileVal ; 
 int outValPitch1 ; 
 int outValPitch2 ; 
 scalar_t__* pBufArray ; 
 scalar_t__* pInput ; 
 size_t* pOutputIdx1 ; 
 scalar_t__* pOutputVal1 ; 
 int /*<<< orphan*/ * pdvecIn ; 
 int /*<<< orphan*/ * pdvecInArr ; 
 int /*<<< orphan*/  vbGtH ; 
 int /*<<< orphan*/  vbGtL ; 
 scalar_t__ vectorizationWidth ; 
 scalar_t__ x ; 
 int y ; 
 int z ; 


//-------------------------
void foo() {
for (z = 0; z < dim3Size; z++) /* Loop along the third dimension */
  {
    for (y = 0; y < dim2Size; y++) /* Loop along the second dimension */
    {
      xb_vecNx16U vecRemIndex = 0;

      /* Input Data Pointer */
      pdvecIn = (MORPH_IDT_VECTOR *) (pInput + (z * inDataPitch2) + (y * inDataPitch1));

      /* Copy the input dim1 to the array */
      pdvecInArr = (MORPH_IDT_VECTOR *) pBufArray;

      valign vaInData = MORPH_OP_PRIME(pdvecIn);
      valign vaOutArr = IVP_ZALIGN();

      for (x = 0; x < dim1Size; x += vectorizationWidth)
      {
        MORPH_OP_LOAD_IP(dvecIn, vaInData, pdvecIn);
        MORPH_OP_VAR_STORE_XP(dvecIn, vaOutArr, pdvecInArr, (dim1Size - x));
      }
      MORPH_OP_FLUSH_SAPOS(vaOutArr, pdvecInArr);

      for (k = 0; k < numLargestVal; k++)
      {
        /*Output and index tiles pointers */
        MORPH_IDT_SCALAR *pOutputVal2 = pOutputVal1 + (z * outValPitch2) + (y * outValPitch1);
        uint16_t *pOutputIdx2         = pOutputIdx1 + (z * outIdxPitch2) + (y * outIdxPitch1);

        /*Load input data and store it in the output */
        pdvecInArr = (MORPH_IDT_VECTOR *) pBufArray;
        vaInData   = MORPH_OP_PRIME(pdvecInArr);

        dvecMaxVal = MORPH_OP_MIN_VALUE;
        xb_vecNx16U vecMaxIdxL = IVP_SEQNX16U();
        xb_vecNx16U vecMaxIdxH = IVP_ADDNX16U(IVP_SEQNX16U(), XCHAL_IVPN_SIMD_WIDTH);
        xb_vecNx16U vecX       = IVP_SEQNX16U();

        for (x = 0; x <= dim1Size - vectorizationWidth; x += vectorizationWidth)
        {
          /* Load the input vector*/
          MORPH_OP_LOAD_IP(dvecInTemp, vaInData, pdvecInArr);

          /* Compare the loaded input with maxVal vector.
             Considering two 64 length vectors are compared, "dvecMaxVal" is the previous
             64 element vector and "dvecInTemp" is the next vector and generate flag "dvbGT"
             corresponding to the max values.*/

          /* Update maxval vector */
          MORPH_OP_BMAX(dvbGT, dvecMaxVal, dvecInTemp, dvecMaxVal);

          /*Split the vbool2N to 2 vboolN, this is done since the addresses are U16 */
          IVP_EXTRACTB2N(vbGtH, vbGtL, dvbGT);

          /*Update the index vector */
          IVP_ADDNX16UT(vecMaxIdxL, vecX, 0, vbGtL);
          IVP_ADDNX16UT(vecMaxIdxH, IVP_ADDNX16U(vecX, XCHAL_IVPN_SIMD_WIDTH), 0, vbGtH);
          vecX = IVP_ADDNX16U(vecX, vectorizationWidth);
        }
        if (x < dim1Size)
        {
          /* remX: remaining elements in dimension-1 */
          int32_t remX = (dim1Size - x);

          /* vbRemX: Flag used when the number of elements in dimension1 is not a
             multiple of 64. When remX is < 64, (64 - remX) elements are filled
             with min value.*/
          vbool2N vbRemX = IVP_GT2NX8(remX, IVP_SEQ2NX8U());

          /* Load the input vector*/
          MORPH_OP_LOAD_IP(dvecInTemp, vaInData, pdvecInArr);

          /*When remX < 64, (64 - remX) elements of dvecInTemp is filled with min value */
          dvecInTemp = MORPH_OP_MOVE(dvecInTemp, dvecMin, vbRemX);

          /* Compare the loaded input with maxVal vector.
             Considering two 64 length vectors are compared, "dvecMaxVal" is the previous
             64 element vector and "dvecInTemp" is the next vector and generate flag "dvbGT"
             corresponding to the max values.*/

          /* Update maxval vector */
          MORPH_OP_BMAX(dvbGT, dvecMaxVal, dvecInTemp, dvecMaxVal);

          /*Split the vbool2N to 2 vboolN, this is done since the addresses are U16 */
          IVP_EXTRACTB2N(vbGtH, vbGtL, dvbGT);

          /*Update the index vector */
          IVP_ADDNX16UT(vecMaxIdxL, vecX, 0, vbGtL);
          IVP_ADDNX16UT(vecMaxIdxH, IVP_ADDNX16U(vecX, XCHAL_IVPN_SIMD_WIDTH), 0, vbGtH);
        }

        /* Find max among all the values */
        MORPH_OP_MAX_VAL(dvecMaxVal, maxVal);

        /* Estimate the maximum indices along dim1.
           Here "maxVal" is compared to all locations in the vector of maximum values and
           corresponding lanes are enabled. */
        MAX_INDEX_2NX8(dvecMaxVal, maxVal, maxIdx);

        /* The "vecRemIndex" variable is used to estimate the number of valid stores in
           the output that are not min value. In case "vecRemIndex < numLargestVal",
           there are elements in dimension1 that are min value and need to be stored. The
           case of storing min value is handled later in code. */
        IVP_ADDNX16UT(vecRemIndex, vecRemIndex, 1, IVP_EQNX16(MORPH_OP_MIN_VALUE, maxVal));

        /* Store index corresponding to the maximum value */
        if ((outTileIdx != NULL))
        {
          pOutputIdx2[k] = maxIdx;
        }

        /*Store the "k"th maximum value to output */
        if ((outTileVal != NULL))
        {
          pOutputVal2[k] = maxVal;
        }

        /* Set the index of the greatest element in the array to min value */
        pBufArray[maxIdx] = MORPH_OP_MIN_VALUE;
      } /* Loop to find k-largest values */

      if ((maxVal == MORPH_OP_MIN_VALUE) && (outTileIdx != NULL))
      {
        /* Code to handle min value to be stored in output tile
           "count" is the number of values that have already been stored to the output tiles.
           (numLargestVal-count) are the min value values that need to be stored to the output. */
        uint16_t remIndex = IVP_MOV16U_FROMNX16U(vecRemIndex);
        k = 0;
        uint16_t nValidIdxOccurence = numLargestVal - remIndex;
        while (remIndex)
        {
          if (pInput[(z * inDataPitch2) + (y * inDataPitch1) + k] == MORPH_OP_MIN_VALUE)
          {
            /* Store min value index in output tile */
            pOutputIdx1[(z * outIdxPitch2) + (y * outIdxPitch1) + nValidIdxOccurence] = k;
            remIndex--;
            nValidIdxOccurence++;
          }
          k++;
        }
      }
    } /* Loop along the second dimension */
  }   /* Loop along the third dimension */

  return(XAI_ERROR_STATUS());
}

