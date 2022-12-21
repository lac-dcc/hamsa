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
typedef  int /*<<< orphan*/  xb_vec2Nx8 ;
typedef  int /*<<< orphan*/  int32_t ;
typedef  int /*<<< orphan*/  MORPH_IDT_2Nx8 ;

/* Variables and functions */
 int /*<<< orphan*/  IVP_EXTRN_2X32 (int /*<<< orphan*/ ,int /*<<< orphan*/ ) ;
 int /*<<< orphan*/  IVP_LAV2NX8_XP (int /*<<< orphan*/ ,int /*<<<
orphan*/ ,int /*<<< orphan*/ ,int) ;
 int /*<<< orphan*/  IVP_MOVNX16_FROM2NX8 (int /*<<< orphan*/ ) ;
 int /*<<< orphan*/  IVP_MOVN_2X32_FROMNX16 (int /*<<< orphan*/ ) ;
 int /*<<< orphan*/  MORPH_OP_ALIGN_LOAD_2Nx8 (int /*<<< orphan*/ ,int
/*<<< orphan*/ ,int) ;
 int /*<<< orphan*/  MORPH_OP_MULQA (int /*<<< orphan*/ ,int /*<<<
orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/ ,int /*<<< orphan*/
,int /*<<< orphan*/ ) ;
 int /*<<< orphan*/  dacc1 ;
 int /*<<< orphan*/  dacc2 ;
 int /*<<< orphan*/  dacc3 ;
 int /*<<< orphan*/  dacc4 ;
 int inCh ;
 int inDataPitch2 ;
 int numInCh ;
 int /*<<< orphan*/  pdvecCoeff1 ;
 int /*<<< orphan*/  pdvecCoeff2 ;
 int /*<<< orphan*/  pdvecCoeff3 ;
 int /*<<< orphan*/  pdvecCoeff4 ;
 int /*<<< orphan*/  pdvecIn1 ;
 int /*<<< orphan*/  pdvecIn2 ;
 int /*<<< orphan*/  pdvecIn3 ;
 int /*<<< orphan*/  pdvecIn4 ;
 int /*<<< orphan*/  vaCoeff1 ;
 int /*<<< orphan*/  vaCoeff2 ;
 int /*<<< orphan*/  vaCoeff3 ;
 int /*<<< orphan*/  vaCoeff4 ;


//-------------------------
void foo () {
        for (inCh = 0; inCh < numInCh - 3; inCh += 4)
        {
          /* input vectors are read from 4 input depths at at time
           * Scalar 32 bit coeff are extracted from the coeff vectors */
          MORPH_IDT_2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;

          /* Read vector input data from 1st depth */
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData1, pdvecIn1, 4 * inDataPitch2);

          /* Read vector input data from 2nd depth */
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData2, pdvecIn2, 4 * inDataPitch2);

          /* Read vector input data from 3rd depth */
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData3, pdvecIn3, 4 * inDataPitch2);

          /* Read vector input data from 4th depth */
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData4, pdvecIn4, 4 * inDataPitch2);

          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          IVP_LAV2NX8_XP(dvecCoeff1, vaCoeff1, pdvecCoeff1, 4);
          IVP_LAV2NX8_XP(dvecCoeff2, vaCoeff2, pdvecCoeff2, 4);
          IVP_LAV2NX8_XP(dvecCoeff3, vaCoeff3, pdvecCoeff3, 4);
          IVP_LAV2NX8_XP(dvecCoeff4, vaCoeff4, pdvecCoeff4, 4);

          int32_t coeff1 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \

(IVP_MOVNX16_FROM2NX8(dvecCoeff1)), 0);
          int32_t coeff2 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \

(IVP_MOVNX16_FROM2NX8(dvecCoeff2)), 0);
          int32_t coeff3 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \

(IVP_MOVNX16_FROM2NX8(dvecCoeff3)), 0);
          int32_t coeff4 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \

(IVP_MOVNX16_FROM2NX8(dvecCoeff4)), 0);

          MORPH_OP_MULQA(dacc1, dvecData4, dvecData3, dvecData2,
dvecData1, coeff1);
          MORPH_OP_MULQA(dacc2, dvecData4, dvecData3, dvecData2,
dvecData1, coeff2);
          MORPH_OP_MULQA(dacc3, dvecData4, dvecData3, dvecData2,
dvecData1, coeff3);
          MORPH_OP_MULQA(dacc4, dvecData4, dvecData3, dvecData2,
dvecData1, coeff4);
        } /* end of for (inCh = 0; inCh < numInCh - 3; inCh += 4)*/

}
