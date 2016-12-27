/* ----------------------------------------------------------------------   
 * Copyright (C) 2010 ARM Limited. All rights reserved.   
 *   
 * $Date:        15. July 2011  
 * $Revision:   V1.0.10  
 *   
 * Project:         CMSIS DSP Library   
 * Title:        arm_math.h
 *   
 * Description:  Public header file for CMSIS DSP Library
 *   
 * Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
 *  
 * Version 1.0.10 2011/7/15 
 *    Big Endian support added and Merged M0 and M3/M4 Source code.  
 *   
 * Version 1.0.3 2010/11/29  
 *    Re-organized the CMSIS folders and updated documentation.   
 *    
 * Version 1.0.2 2010/11/11   
 *    Documentation updated.    
 *   
 * Version 1.0.1 2010/10/05    
 *    Production release and review comments incorporated.   
 *   
 * Version 1.0.0 2010/09/20    
 *    Production release and review comments incorporated.   
 * -------------------------------------------------------------------- */

#ifndef _ARM_MATH_H
#define _ARM_MATH_H

#define __CMSIS_GENERIC              /* disable NVIC and Systick functions */

#if defined (ARM_MATH_CM4)
  #include "core_cm4.h"
#elif defined (ARM_MATH_CM3)
  #include "core_cm3.h"
#elif defined (ARM_MATH_CM0)
  #include "core_cm0.h"
#else
#include "ARMCM4.h"
#warning "Define either ARM_MATH_CM4 OR ARM_MATH_CM3...By Default building on ARM_MATH_CM4....."
#endif

#undef  __CMSIS_GENERIC              /* enable NVIC and Systick functions */
#include "string.h"
    #include "math.h"
#ifdef  __cplusplus
extern "C"
{
#endif


#define DELTA_Q31           (0x100)
#define DELTA_Q15           0x5
#define INDEX_MASK          0x0000003F
#define PI                  3.14159265358979f

#define TABLE_SIZE          256
#define TABLE_SPACING_Q31   0x800000
#define TABLE_SPACING_Q15   0x80

  /* 1.31(q31) Fixed value of 2/360 */
  /* -1 to +1 is divided into 360 values so total spacing is (2/360) */
#define INPUT_SPACING           0xB60B61


  typedef enum
    {
      ARM_MATH_SUCCESS = 0,              
      ARM_MATH_ARGUMENT_ERROR = -1,      
      ARM_MATH_LENGTH_ERROR = -2,        
      ARM_MATH_SIZE_MISMATCH = -3,       
      ARM_MATH_NANINF = -4,              
      ARM_MATH_SINGULAR = -5,            
      ARM_MATH_TEST_FAILURE = -6         
    } arm_status;

  typedef int8_t q7_t;

  typedef int16_t q15_t;

  typedef int32_t q31_t;

  typedef int64_t q63_t;

  typedef float float32_t;

  typedef double float64_t;

#define __SIMD32(addr)  (*(int32_t **) & (addr))

#if defined (ARM_MATH_CM3) || defined (ARM_MATH_CM0)

#define __PKHBT(ARG1, ARG2, ARG3)      ( (((int32_t)(ARG1) <<  0) & (int32_t)0x0000FFFF) | \
                                         (((int32_t)(ARG2) << ARG3) & (int32_t)0xFFFF0000)  )

#endif


#ifndef ARM_MATH_BIG_ENDIAN

#define __PACKq7(v0,v1,v2,v3) ( (((int32_t)(v0) <<  0) & (int32_t)0x000000FF) | \
                                (((int32_t)(v1) <<  8) & (int32_t)0x0000FF00) | \
                                (((int32_t)(v2) << 16) & (int32_t)0x00FF0000) | \
                                (((int32_t)(v3) << 24) & (int32_t)0xFF000000)  )
#else                               

#define __PACKq7(v0,v1,v2,v3) ( (((int32_t)(v3) <<  0) & (int32_t)0x000000FF) | \
                                (((int32_t)(v2) <<  8) & (int32_t)0x0000FF00) | \
                                (((int32_t)(v1) << 16) & (int32_t)0x00FF0000) | \
                                (((int32_t)(v0) << 24) & (int32_t)0xFF000000)  )
                                
#endif


  static __INLINE q31_t clip_q63_to_q31(
                    q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFFFFFF ^ ((q31_t) (x >> 63)))) : (q31_t) x;
  }

  static __INLINE q15_t clip_q63_to_q15(
                    q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFF ^ ((q15_t) (x >> 63)))) : (q15_t) (x >> 15);
  }

  static __INLINE q7_t clip_q31_to_q7(
                      q31_t x)
  {
    return ((q31_t) (x >> 24) != ((q31_t) x >> 23)) ?
      ((0x7F ^ ((q7_t) (x >> 31)))) : (q7_t) x;
  }

  static __INLINE q15_t clip_q31_to_q15(
                    q31_t x)
  {
    return ((q31_t) (x >> 16) != ((q31_t) x >> 15)) ?
      ((0x7FFF ^ ((q15_t) (x >> 31)))) : (q15_t) x;
  }

  static __INLINE q63_t mult32x64(
                  q63_t x,
                  q31_t y)
  {
    return ((((q63_t) (x & 0x00000000FFFFFFFF) * y) >> 32) +
            (((q63_t) (x >> 32) * y)));
  }


#if defined (ARM_MATH_CM0) && defined ( __CC_ARM   )
#define __CLZ __clz
#endif 

#if defined (ARM_MATH_CM0) && ((defined (__ICCARM__)) ||(defined (__GNUC__)) || defined (__TASKING__) )

    uint32_t __CLZ(q31_t data);


  static __INLINE uint32_t __CLZ(q31_t data)
  {
      uint32_t count = 0;
      uint32_t mask = 0x80000000;

      while((data & mask) ==  0)
      {
          count += 1u;
          mask = mask >> 1u;
      }

      return(count);

  }

#endif 

  static __INLINE uint32_t arm_recip_q31(
                     q31_t in,
                     q31_t * dst,
                     q31_t * pRecipTable)
  {

    uint32_t out, tempVal;
    uint32_t index, i;
    uint32_t signBits;

    if(in > 0)
      {
    signBits = __CLZ(in) - 1;
      }
    else
      {
    signBits = __CLZ(-in) - 1;
      }

    /* Convert input sample to 1.31 format */
    in = in << signBits;

    /* calculation of index for initial approximated Val */
    index = (uint32_t) (in >> 24u);
    index = (index & INDEX_MASK);

    /* 1.31 with exp 1 */
    out = pRecipTable[index];

    /* calculation of reciprocal value */
    /* running approximation for two iterations */
    for (i = 0u; i < 2u; i++)
      {
    tempVal = (q31_t) (((q63_t) in * out) >> 31u);
    tempVal = 0x7FFFFFFF - tempVal;
    /*      1.31 with exp 1 */
    //out = (q31_t) (((q63_t) out * tempVal) >> 30u);
    out = (q31_t) clip_q63_to_q31(((q63_t) out * tempVal) >> 30u);
      }

    /* write output */
    *dst = out;

    /* return num of signbits of out = 1/in value */
    return (signBits + 1u);

  }

  static __INLINE uint32_t arm_recip_q15(
                     q15_t in,
                     q15_t * dst,
                     q15_t * pRecipTable)
  {

    uint32_t out = 0, tempVal = 0;
    uint32_t index = 0, i = 0;
    uint32_t signBits = 0;

    if(in > 0)
      {
    signBits = __CLZ(in) - 17;
      }
    else
      {
    signBits = __CLZ(-in) - 17;
      }

    /* Convert input sample to 1.15 format */
    in = in << signBits;

    /* calculation of index for initial approximated Val */
    index = in >> 8;
    index = (index & INDEX_MASK);

    /*      1.15 with exp 1  */
    out = pRecipTable[index];

    /* calculation of reciprocal value */
    /* running approximation for two iterations */
    for (i = 0; i < 2; i++)
      {
    tempVal = (q15_t) (((q31_t) in * out) >> 15);
    tempVal = 0x7FFF - tempVal;
    /*      1.15 with exp 1 */
    out = (q15_t) (((q31_t) out * tempVal) >> 14);
      }

    /* write output */
    *dst = out;

    /* return num of signbits of out = 1/in value */
    return (signBits + 1);

  }


  /*
   * @brief C custom defined intrinisic function for only M0 processors
   */
#if defined(ARM_MATH_CM0)

  static __INLINE q31_t __SSAT(
                   q31_t x,
                   uint32_t y)
  {
    int32_t posMax, negMin;
    uint32_t i;

    posMax = 1;
    for (i = 0; i < (y - 1); i++)
      {
    posMax = posMax * 2;
      }

    if(x > 0)
      {
    posMax = (posMax - 1);

    if(x > posMax)
      {
        x = posMax;
      }
      }
    else
      {
    negMin = -posMax;

    if(x < negMin)
      {
        x = negMin;
      }
      }
    return (x);


  }

#endif /* end of ARM_MATH_CM0 */



  /*
   * @brief C custom defined intrinsic function for M3 and M0 processors
   */
#if defined (ARM_MATH_CM3) || defined (ARM_MATH_CM0)

  /*
   * @brief C custom defined QADD8 for M3 and M0 processors
   */
  static __INLINE q31_t __QADD8(
                q31_t x,
                q31_t y)
  {

    q31_t sum;
    q7_t r, s, t, u;

    r = (char) x;
    s = (char) y;

    r = __SSAT((q31_t) (r + s), 8);
    s = __SSAT(((q31_t) (((x << 16) >> 24) + ((y << 16) >> 24))), 8);
    t = __SSAT(((q31_t) (((x << 8) >> 24) + ((y << 8) >> 24))), 8);
    u = __SSAT(((q31_t) ((x >> 24) + (y >> 24))), 8);

    sum = (((q31_t) u << 24) & 0xFF000000) | (((q31_t) t << 16) & 0x00FF0000) |
      (((q31_t) s << 8) & 0x0000FF00) | (r & 0x000000FF);

    return sum;

  }

  /*
   * @brief C custom defined QSUB8 for M3 and M0 processors
   */
  static __INLINE q31_t __QSUB8(
                q31_t x,
                q31_t y)
  {

    q31_t sum;
    q31_t r, s, t, u;

    r = (char) x;
    s = (char) y;

    r = __SSAT((r - s), 8);
    s = __SSAT(((q31_t) (((x << 16) >> 24) - ((y << 16) >> 24))), 8) << 8;
    t = __SSAT(((q31_t) (((x << 8) >> 24) - ((y << 8) >> 24))), 8) << 16;
    u = __SSAT(((q31_t) ((x >> 24) - (y >> 24))), 8) << 24;

    sum =
      (u & 0xFF000000) | (t & 0x00FF0000) | (s & 0x0000FF00) | (r & 0x000000FF);

    return sum;
  }

  /*
   * @brief C custom defined QADD16 for M3 and M0 processors
   */

  /*
   * @brief C custom defined QADD16 for M3 and M0 processors
   */
  static __INLINE q31_t __QADD16(
                 q31_t x,
                 q31_t y)
  {

    q31_t sum;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = __SSAT(r + s, 16);
    s = __SSAT(((q31_t) ((x >> 16) + (y >> 16))), 16) << 16;

    sum = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return sum;

  }

  /*
   * @brief C custom defined SHADD16 for M3 and M0 processors
   */
  static __INLINE q31_t __SHADD16(
                  q31_t x,
                  q31_t y)
  {

    q31_t sum;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = ((r >> 1) + (s >> 1));
    s = ((q31_t) ((x >> 17) + (y >> 17))) << 16;

    sum = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return sum;

  }

  /*
   * @brief C custom defined QSUB16 for M3 and M0 processors
   */
  static __INLINE q31_t __QSUB16(
                 q31_t x,
                 q31_t y)
  {

    q31_t sum;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = __SSAT(r - s, 16);
    s = __SSAT(((q31_t) ((x >> 16) - (y >> 16))), 16) << 16;

    sum = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return sum;
  }

  /*
   * @brief C custom defined SHSUB16 for M3 and M0 processors
   */
  static __INLINE q31_t __SHSUB16(
                  q31_t x,
                  q31_t y)
  {

    q31_t diff;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = ((r >> 1) - (s >> 1));
    s = (((x >> 17) - (y >> 17)) << 16);

    diff = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return diff;
  }

  /*
   * @brief C custom defined QASX for M3 and M0 processors
   */
  static __INLINE q31_t __QASX(
                   q31_t x,
                   q31_t y)
  {

    q31_t sum = 0;

    sum = ((sum + clip_q31_to_q15((q31_t) ((short) (x >> 16) + (short) y))) << 16) +
      clip_q31_to_q15((q31_t) ((short) x - (short) (y >> 16)));

    return sum;
  }

  /*
   * @brief C custom defined SHASX for M3 and M0 processors
   */
  static __INLINE q31_t __SHASX(
                q31_t x,
                q31_t y)
  {

    q31_t sum;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = ((r >> 1) - (y >> 17));
    s = (((x >> 17) + (s >> 1)) << 16);

    sum = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return sum;
  }


  /*
   * @brief C custom defined QSAX for M3 and M0 processors
   */
  static __INLINE q31_t __QSAX(
                   q31_t x,
                   q31_t y)
  {

    q31_t sum = 0;

    sum = ((sum + clip_q31_to_q15((q31_t) ((short) (x >> 16) - (short) y))) << 16) +
      clip_q31_to_q15((q31_t) ((short) x + (short) (y >> 16)));

    return sum;
  }

  /*
   * @brief C custom defined SHSAX for M3 and M0 processors
   */
  static __INLINE q31_t __SHSAX(
                q31_t x,
                q31_t y)
  {

    q31_t sum;
    q31_t r, s;

    r = (short) x;
    s = (short) y;

    r = ((r >> 1) + (y >> 17));
    s = (((x >> 17) - (s >> 1)) << 16);

    sum = (s & 0xFFFF0000) | (r & 0x0000FFFF);

    return sum;
  }

  /*
   * @brief C custom defined SMUSDX for M3 and M0 processors
   */
  static __INLINE q31_t __SMUSDX(
                 q31_t x,
                 q31_t y)
  {

    return ((q31_t)(((short) x * (short) (y >> 16)) -
            ((short) (x >> 16) * (short) y)));
  }

  /*
   * @brief C custom defined SMUADX for M3 and M0 processors
   */
  static __INLINE q31_t __SMUADX(
                 q31_t x,
                 q31_t y)
  {

    return ((q31_t)(((short) x * (short) (y >> 16)) +
            ((short) (x >> 16) * (short) y)));
  }

  /*
   * @brief C custom defined QADD for M3 and M0 processors
   */
  static __INLINE q31_t __QADD(
                   q31_t x,
                   q31_t y)
  {
    return clip_q63_to_q31((q63_t) x + y);
  }

  /*
   * @brief C custom defined QSUB for M3 and M0 processors
   */
  static __INLINE q31_t __QSUB(
                   q31_t x,
                   q31_t y)
  {
    return clip_q63_to_q31((q63_t) x - y);
  }

  /*
   * @brief C custom defined SMLAD for M3 and M0 processors
   */
  static __INLINE q31_t __SMLAD(
                q31_t x,
                q31_t y,
                q31_t sum)
  {

    return (sum + ((short) (x >> 16) * (short) (y >> 16)) +
            ((short) x * (short) y));
  }

  /*
   * @brief C custom defined SMLADX for M3 and M0 processors
   */
  static __INLINE q31_t __SMLADX(
                 q31_t x,
                 q31_t y,
                 q31_t sum)
  {

    return (sum + ((short) (x >> 16) * (short) (y)) +
            ((short) x * (short) (y >> 16)));
  }

  /*
   * @brief C custom defined SMLSDX for M3 and M0 processors
   */
  static __INLINE q31_t __SMLSDX(
                 q31_t x,
                 q31_t y,
                 q31_t sum)
  {

    return (sum - ((short) (x >> 16) * (short) (y)) +
            ((short) x * (short) (y >> 16)));
  }

  /*
   * @brief C custom defined SMLALD for M3 and M0 processors
   */
  static __INLINE q63_t __SMLALD(
                 q31_t x,
                 q31_t y,
                 q63_t sum)
  {

    return (sum + ((short) (x >> 16) * (short) (y >> 16)) +
            ((short) x * (short) y));
  }

  /*
   * @brief C custom defined SMLALDX for M3 and M0 processors
   */
  static __INLINE q63_t __SMLALDX(
                  q31_t x,
                  q31_t y,
                  q63_t sum)
  {

    return (sum + ((short) (x >> 16) * (short) y)) +
      ((short) x * (short) (y >> 16));
  }

  /*
   * @brief C custom defined SMUAD for M3 and M0 processors
   */
  static __INLINE q31_t __SMUAD(
                q31_t x,
                q31_t y)
  {

    return (((x >> 16) * (y >> 16)) +
            (((x << 16) >> 16) * ((y << 16) >> 16)));
  }

  /*
   * @brief C custom defined SMUSD for M3 and M0 processors
   */
  static __INLINE q31_t __SMUSD(
                q31_t x,
                q31_t y)
  {

    return (-((x >> 16) * (y >> 16)) +
            (((x << 16) >> 16) * ((y << 16) >> 16)));
  }




#endif /* (ARM_MATH_CM3) || defined (ARM_MATH_CM0) */


  typedef struct
  {
    uint16_t numTaps;        
    q7_t *pState;            
    q7_t *pCoeffs;           
  } arm_fir_instance_q7;

  typedef struct
  {
    uint16_t numTaps;         
    q15_t *pState;            
    q15_t *pCoeffs;           
  } arm_fir_instance_q15;

  typedef struct
  {
    uint16_t numTaps;         
    q31_t *pState;            
    q31_t *pCoeffs;           
  } arm_fir_instance_q31;

  typedef struct
  {
    uint16_t numTaps;     
    float32_t *pState;    
    float32_t *pCoeffs;   
  } arm_fir_instance_f32;


  void arm_fir_q7(
          const arm_fir_instance_q7 * S,
           q7_t * pSrc,
          q7_t * pDst,
          uint32_t blockSize);


  void arm_fir_init_q7(
               arm_fir_instance_q7 * S,
               uint16_t numTaps,
               q7_t * pCoeffs,
               q7_t * pState,
               uint32_t blockSize);


  void arm_fir_q15(
           const arm_fir_instance_q15 * S,
            q15_t * pSrc,
           q15_t * pDst,
           uint32_t blockSize);

  void arm_fir_fast_q15(
            const arm_fir_instance_q15 * S,
             q15_t * pSrc,
            q15_t * pDst,
            uint32_t blockSize);

       arm_status arm_fir_init_q15(
                  arm_fir_instance_q15 * S,
                  uint16_t numTaps,
                  q15_t * pCoeffs,
                  q15_t * pState,
                  uint32_t blockSize);

  void arm_fir_q31(
           const arm_fir_instance_q31 * S,
            q31_t * pSrc,
           q31_t * pDst,
           uint32_t blockSize);

  void arm_fir_fast_q31(
            const arm_fir_instance_q31 * S,
             q31_t * pSrc,
            q31_t * pDst,
            uint32_t blockSize);

  void arm_fir_init_q31(
            arm_fir_instance_q31 * S,
            uint16_t numTaps,
            q31_t * pCoeffs,
            q31_t * pState,
            uint32_t blockSize);

  void arm_fir_f32(
           const arm_fir_instance_f32 * S,
            float32_t * pSrc,
           float32_t * pDst,
           uint32_t blockSize);

  void arm_fir_init_f32(
            arm_fir_instance_f32 * S,
            uint16_t numTaps,
            float32_t * pCoeffs,
            float32_t * pState,
            uint32_t blockSize);


  typedef struct
  {
    int8_t numStages;         
    q15_t *pState;            
    q15_t *pCoeffs;           
    int8_t postShift;         
  } arm_biquad_casd_df1_inst_q15;


  typedef struct
  {
    uint32_t numStages;      
    q31_t *pState;           
    q31_t *pCoeffs;          
    uint8_t postShift;       
  } arm_biquad_casd_df1_inst_q31;

  typedef struct
  {
    uint32_t numStages;         
    float32_t *pState;          
    float32_t *pCoeffs;         
  } arm_biquad_casd_df1_inst_f32;



  void arm_biquad_cascade_df1_q15(
                  const arm_biquad_casd_df1_inst_q15 * S,
                   q15_t * pSrc,
                  q15_t * pDst,
                  uint32_t blockSize);

  void arm_biquad_cascade_df1_init_q15(
                       arm_biquad_casd_df1_inst_q15 * S,
                       uint8_t numStages,
                       q15_t * pCoeffs,
                       q15_t * pState,
                       int8_t postShift);


  void arm_biquad_cascade_df1_fast_q15(
                       const arm_biquad_casd_df1_inst_q15 * S,
                        q15_t * pSrc,
                       q15_t * pDst,
                       uint32_t blockSize);


  void arm_biquad_cascade_df1_q31(
                  const arm_biquad_casd_df1_inst_q31 * S,
                   q31_t * pSrc,
                  q31_t * pDst,
                  uint32_t blockSize);

  void arm_biquad_cascade_df1_fast_q31(
                       const arm_biquad_casd_df1_inst_q31 * S,
                        q31_t * pSrc,
                       q31_t * pDst,
                       uint32_t blockSize);

  void arm_biquad_cascade_df1_init_q31(
                       arm_biquad_casd_df1_inst_q31 * S,
                       uint8_t numStages,
                       q31_t * pCoeffs,
                       q31_t * pState,
                       int8_t postShift);

  void arm_biquad_cascade_df1_f32(
                  const arm_biquad_casd_df1_inst_f32 * S,
                   float32_t * pSrc,
                  float32_t * pDst,
                  uint32_t blockSize);

  void arm_biquad_cascade_df1_init_f32(
                       arm_biquad_casd_df1_inst_f32 * S,
                       uint8_t numStages,
                       float32_t * pCoeffs,
                       float32_t * pState);


  typedef struct
  {
    uint16_t numRows;     
    uint16_t numCols;     
    float32_t *pData;     
  } arm_matrix_instance_f32;

  typedef struct
  {
    uint16_t numRows;     
    uint16_t numCols;     
    q15_t *pData;         
  } arm_matrix_instance_q15;

  typedef struct
  {
    uint16_t numRows;     
    uint16_t numCols;     
    q31_t *pData;         
  } arm_matrix_instance_q31;



  arm_status arm_mat_add_f32(
                 const arm_matrix_instance_f32 * pSrcA,
                 const arm_matrix_instance_f32 * pSrcB,
                 arm_matrix_instance_f32 * pDst);

  arm_status arm_mat_add_q15(
                 const arm_matrix_instance_q15 * pSrcA,
                 const arm_matrix_instance_q15 * pSrcB,
                 arm_matrix_instance_q15 * pDst);

  arm_status arm_mat_add_q31(
                 const arm_matrix_instance_q31 * pSrcA,
                 const arm_matrix_instance_q31 * pSrcB,
                 arm_matrix_instance_q31 * pDst);


  arm_status arm_mat_trans_f32(
                   const arm_matrix_instance_f32 * pSrc,
                   arm_matrix_instance_f32 * pDst);


  arm_status arm_mat_trans_q15(
                   const arm_matrix_instance_q15 * pSrc,
                   arm_matrix_instance_q15 * pDst);

  arm_status arm_mat_trans_q31(
                   const arm_matrix_instance_q31 * pSrc,
                   arm_matrix_instance_q31 * pDst);


  arm_status arm_mat_mult_f32(
                  const arm_matrix_instance_f32 * pSrcA,
                  const arm_matrix_instance_f32 * pSrcB,
                  arm_matrix_instance_f32 * pDst);

  arm_status arm_mat_mult_q15(
                  const arm_matrix_instance_q15 * pSrcA,
                  const arm_matrix_instance_q15 * pSrcB,
                  arm_matrix_instance_q15 * pDst,
                  q15_t * pState);

  arm_status arm_mat_mult_fast_q15(
                   const arm_matrix_instance_q15 * pSrcA,
                   const arm_matrix_instance_q15 * pSrcB,
                   arm_matrix_instance_q15 * pDst,
                   q15_t * pState);

  arm_status arm_mat_mult_q31(
                  const arm_matrix_instance_q31 * pSrcA,
                  const arm_matrix_instance_q31 * pSrcB,
                  arm_matrix_instance_q31 * pDst);

  arm_status arm_mat_mult_fast_q31(
                   const arm_matrix_instance_q31 * pSrcA,
                   const arm_matrix_instance_q31 * pSrcB,
                   arm_matrix_instance_q31 * pDst);


  arm_status arm_mat_sub_f32(
                 const arm_matrix_instance_f32 * pSrcA,
                 const arm_matrix_instance_f32 * pSrcB,
                 arm_matrix_instance_f32 * pDst);

  arm_status arm_mat_sub_q15(
                 const arm_matrix_instance_q15 * pSrcA,
                 const arm_matrix_instance_q15 * pSrcB,
                 arm_matrix_instance_q15 * pDst);

  arm_status arm_mat_sub_q31(
                 const arm_matrix_instance_q31 * pSrcA,
                 const arm_matrix_instance_q31 * pSrcB,
                 arm_matrix_instance_q31 * pDst);

  arm_status arm_mat_scale_f32(
                   const arm_matrix_instance_f32 * pSrc,
                   float32_t scale,
                   arm_matrix_instance_f32 * pDst);

  arm_status arm_mat_scale_q15(
                   const arm_matrix_instance_q15 * pSrc,
                   q15_t scaleFract,
                   int32_t shift,
                   arm_matrix_instance_q15 * pDst);

  arm_status arm_mat_scale_q31(
                   const arm_matrix_instance_q31 * pSrc,
                   q31_t scaleFract,
                   int32_t shift,
                   arm_matrix_instance_q31 * pDst);


  void arm_mat_init_q31(
            arm_matrix_instance_q31 * S,
            uint16_t nRows,
            uint16_t nColumns,
            q31_t   *pData);

  void arm_mat_init_q15(
            arm_matrix_instance_q15 * S,
            uint16_t nRows,
            uint16_t nColumns,
            q15_t    *pData);

  void arm_mat_init_f32(
            arm_matrix_instance_f32 * S,
            uint16_t nRows,
            uint16_t nColumns,
            float32_t   *pData);



  typedef struct
  {
    q15_t A0;    
    #ifdef ARM_MATH_CM0  
    q15_t A1;
    q15_t A2; 
    #else         
    q31_t A1;           
    #endif 
    q15_t state[3];       
    q15_t Kp;           
    q15_t Ki;           
    q15_t Kd;           
  } arm_pid_instance_q15;

  typedef struct
  {
    q31_t A0;            
    q31_t A1;            
    q31_t A2;            
    q31_t state[3];      
    q31_t Kp;            
    q31_t Ki;            
    q31_t Kd;            
  } arm_pid_instance_q31;

  typedef struct
  {
    float32_t A0;          
    float32_t A1;          
    float32_t A2;          
    float32_t state[3];    
    float32_t Kp;               
    float32_t Ki;               
    float32_t Kd;               
  } arm_pid_instance_f32;



  void arm_pid_init_f32(
            arm_pid_instance_f32 * S,
            int32_t resetStateFlag);

  void arm_pid_reset_f32(
             arm_pid_instance_f32 * S);


  void arm_pid_init_q31(
            arm_pid_instance_q31 * S,
            int32_t resetStateFlag);

 
  void arm_pid_reset_q31(
             arm_pid_instance_q31 * S);

  void arm_pid_init_q15(
            arm_pid_instance_q15 * S,
            int32_t resetStateFlag);

  void arm_pid_reset_q15(
             arm_pid_instance_q15 * S);


  typedef struct
  {
    uint32_t nValues;
    float32_t x1;
    float32_t xSpacing;
    float32_t *pYData;          
  } arm_linear_interp_instance_f32;

  typedef struct
  {
    uint16_t numRows;   
    uint16_t numCols;   
    float32_t *pData;   
  } arm_bilinear_interp_instance_f32;

  typedef struct
  {
    uint16_t numRows;   
    uint16_t numCols;   
    q31_t *pData;   
  } arm_bilinear_interp_instance_q31;

  typedef struct
  {
    uint16_t numRows;   
    uint16_t numCols;   
    q15_t *pData;   
  } arm_bilinear_interp_instance_q15;

  typedef struct
  {
    uint16_t numRows;   
    uint16_t numCols;   
    q7_t *pData;        
  } arm_bilinear_interp_instance_q7;


  void arm_mult_q7(
            q7_t * pSrcA,
            q7_t * pSrcB,
           q7_t * pDst,
           uint32_t blockSize);

  void arm_mult_q15(
             q15_t * pSrcA,
             q15_t * pSrcB,
            q15_t * pDst,
            uint32_t blockSize);

  void arm_mult_q31(
             q31_t * pSrcA,
             q31_t * pSrcB,
            q31_t * pDst,
            uint32_t blockSize);

  void arm_mult_f32(
             float32_t * pSrcA,
             float32_t * pSrcB,
            float32_t * pDst,
            uint32_t blockSize);


  typedef struct
  {
    uint16_t  fftLen;                
    uint8_t   ifftFlag;              
    uint8_t   bitReverseFlag;        
    q15_t     *pTwiddle;             
    uint16_t  *pBitRevTable;         
    uint16_t  twidCoefModifier;      
    uint16_t  bitRevFactor;          
  } arm_cfft_radix4_instance_q15;

  typedef struct
  {
    uint16_t    fftLen;              
    uint8_t     ifftFlag;            
    uint8_t     bitReverseFlag;      
    q31_t       *pTwiddle;           
    uint16_t    *pBitRevTable;       
    uint16_t    twidCoefModifier;    
    uint16_t    bitRevFactor;        
  } arm_cfft_radix4_instance_q31;

  typedef struct
  {
    uint16_t     fftLen;               
    uint8_t      ifftFlag;             
    uint8_t      bitReverseFlag;       
    float32_t    *pTwiddle;            
    uint16_t     *pBitRevTable;        
    uint16_t     twidCoefModifier;     
    uint16_t     bitRevFactor;         
    float32_t    onebyfftLen;          
  } arm_cfft_radix4_instance_f32;

  void arm_cfft_radix4_q15(
               const arm_cfft_radix4_instance_q15 * S,
               q15_t * pSrc);

  arm_status arm_cfft_radix4_init_q15(
                      arm_cfft_radix4_instance_q15 * S,
                      uint16_t fftLen,
                      uint8_t ifftFlag,
                      uint8_t bitReverseFlag);

  void arm_cfft_radix4_q31(
               const arm_cfft_radix4_instance_q31 * S,
               q31_t * pSrc);

  arm_status arm_cfft_radix4_init_q31(
                      arm_cfft_radix4_instance_q31 * S,
                      uint16_t fftLen,
                      uint8_t ifftFlag,
                      uint8_t bitReverseFlag);

  void arm_cfft_radix4_f32(
               const arm_cfft_radix4_instance_f32 * S,
               float32_t * pSrc);

  arm_status arm_cfft_radix4_init_f32(
                      arm_cfft_radix4_instance_f32 * S,
                      uint16_t fftLen,
                      uint8_t ifftFlag,
                      uint8_t bitReverseFlag);



  /*----------------------------------------------------------------------
   *        Internal functions prototypes FFT function
   ----------------------------------------------------------------------*/

  void arm_radix4_butterfly_f32(
                float32_t * pSrc,
                uint16_t fftLen,
                float32_t * pCoef,
                uint16_t twidCoefModifier);

  void arm_radix4_butterfly_inverse_f32(
                    float32_t * pSrc,
                    uint16_t fftLen,
                    float32_t * pCoef,
                    uint16_t twidCoefModifier,
                    float32_t onebyfftLen);

  void arm_bitreversal_f32(
               float32_t *pSrc,
               uint16_t fftSize,
               uint16_t bitRevFactor,
               uint16_t *pBitRevTab);

  void arm_radix4_butterfly_q31(
                q31_t *pSrc,
                uint32_t fftLen,
                q31_t *pCoef,
                uint32_t twidCoefModifier);

  void arm_radix4_butterfly_inverse_q31(
                    q31_t * pSrc,
                    uint32_t fftLen,
                    q31_t * pCoef,
                    uint32_t twidCoefModifier);
  
  void arm_bitreversal_q31(
               q31_t * pSrc,
               uint32_t fftLen,
               uint16_t bitRevFactor,
               uint16_t *pBitRevTab);

  void arm_radix4_butterfly_q15(
                q15_t *pSrc16,
                uint32_t fftLen,
                q15_t *pCoef16,
                uint32_t twidCoefModifier);

  void arm_radix4_butterfly_inverse_q15(
                    q15_t *pSrc16,
                    uint32_t fftLen,
                    q15_t *pCoef16,
                    uint32_t twidCoefModifier);

  void arm_bitreversal_q15(
               q15_t * pSrc,
               uint32_t fftLen,
               uint16_t bitRevFactor,
               uint16_t *pBitRevTab);

  typedef struct
  {
    uint32_t fftLenReal;                      
    uint32_t fftLenBy2;                       
    uint8_t  ifftFlagR;                       
    uint8_t  bitReverseFlagR;                 
    uint32_t twidCoefRModifier;               
    q15_t    *pTwiddleAReal;                  
    q15_t    *pTwiddleBReal;                  
    arm_cfft_radix4_instance_q15 *pCfft;      
  } arm_rfft_instance_q15;

  typedef struct
  {
    uint32_t fftLenReal;                        
    uint32_t fftLenBy2;                         
    uint8_t  ifftFlagR;                         
    uint8_t  bitReverseFlagR;                   
    uint32_t twidCoefRModifier;                 
    q31_t    *pTwiddleAReal;                    
    q31_t    *pTwiddleBReal;                    
    arm_cfft_radix4_instance_q31 *pCfft;        
  } arm_rfft_instance_q31;

  typedef struct
  {
    uint32_t  fftLenReal;                       
    uint16_t  fftLenBy2;                        
    uint8_t   ifftFlagR;                        
    uint8_t   bitReverseFlagR;                  
    uint32_t  twidCoefRModifier;                
    float32_t *pTwiddleAReal;                   
    float32_t *pTwiddleBReal;                   
    arm_cfft_radix4_instance_f32 *pCfft;        
  } arm_rfft_instance_f32;

  void arm_rfft_q15(
            const arm_rfft_instance_q15 * S,
            q15_t * pSrc,
            q15_t * pDst);

  arm_status arm_rfft_init_q15(
                   arm_rfft_instance_q15 * S,
                   arm_cfft_radix4_instance_q15 * S_CFFT,
                   uint32_t fftLenReal,
                   uint32_t ifftFlagR,
                   uint32_t bitReverseFlag);

  void arm_rfft_q31(
            const arm_rfft_instance_q31 * S,
            q31_t * pSrc,
            q31_t * pDst);

  arm_status arm_rfft_init_q31(
                   arm_rfft_instance_q31 * S,
                   arm_cfft_radix4_instance_q31 * S_CFFT,
                   uint32_t fftLenReal,
                   uint32_t ifftFlagR,
                   uint32_t bitReverseFlag);

  arm_status arm_rfft_init_f32(
                   arm_rfft_instance_f32 * S,
                   arm_cfft_radix4_instance_f32 * S_CFFT,
                   uint32_t fftLenReal,
                   uint32_t ifftFlagR,
                   uint32_t bitReverseFlag);

  void arm_rfft_f32(
            const arm_rfft_instance_f32 * S,
            float32_t * pSrc,
            float32_t * pDst);

  typedef struct
  {
    uint16_t N;                         
    uint16_t Nby2;                      
    float32_t normalize;                
    float32_t *pTwiddle;                
    float32_t *pCosFactor;              
    arm_rfft_instance_f32 *pRfft;        
    arm_cfft_radix4_instance_f32 *pCfft; 
  } arm_dct4_instance_f32;

  arm_status arm_dct4_init_f32(
                   arm_dct4_instance_f32 * S,
                   arm_rfft_instance_f32 * S_RFFT,
                   arm_cfft_radix4_instance_f32 * S_CFFT,
                   uint16_t N,
                   uint16_t Nby2,
                   float32_t normalize);

  void arm_dct4_f32(
            const arm_dct4_instance_f32 * S,
            float32_t * pState,
            float32_t * pInlineBuffer);

  typedef struct
  {
    uint16_t N;                         
    uint16_t Nby2;                      
    q31_t normalize;                    
    q31_t *pTwiddle;                    
    q31_t *pCosFactor;                  
    arm_rfft_instance_q31 *pRfft;        
    arm_cfft_radix4_instance_q31 *pCfft; 
  } arm_dct4_instance_q31;

  arm_status arm_dct4_init_q31(
                   arm_dct4_instance_q31 * S,
                   arm_rfft_instance_q31 * S_RFFT,
                   arm_cfft_radix4_instance_q31 * S_CFFT,
                   uint16_t N,
                   uint16_t Nby2,
                   q31_t normalize);

  void arm_dct4_q31(
            const arm_dct4_instance_q31 * S,
            q31_t * pState,
            q31_t * pInlineBuffer);

  typedef struct
  {
    uint16_t N;                         
    uint16_t Nby2;                      
    q15_t normalize;                    
    q15_t *pTwiddle;                    
    q15_t *pCosFactor;                  
    arm_rfft_instance_q15 *pRfft;        
    arm_cfft_radix4_instance_q15 *pCfft; 
  } arm_dct4_instance_q15;

  arm_status arm_dct4_init_q15(
                   arm_dct4_instance_q15 * S,
                   arm_rfft_instance_q15 * S_RFFT,
                   arm_cfft_radix4_instance_q15 * S_CFFT,
                   uint16_t N,
                   uint16_t Nby2,
                   q15_t normalize);

  void arm_dct4_q15(
            const arm_dct4_instance_q15 * S,
            q15_t * pState,
            q15_t * pInlineBuffer);

  void arm_add_f32(
           float32_t * pSrcA,
           float32_t * pSrcB,
           float32_t * pDst,
           uint32_t blockSize);

  void arm_add_q7(
          q7_t * pSrcA,
          q7_t * pSrcB,
          q7_t * pDst,
          uint32_t blockSize);

  void arm_add_q15(
            q15_t * pSrcA,
            q15_t * pSrcB,
           q15_t * pDst,
           uint32_t blockSize);

  void arm_add_q31(
            q31_t * pSrcA,
            q31_t * pSrcB,
           q31_t * pDst,
           uint32_t blockSize);

  void arm_sub_f32(
            float32_t * pSrcA,
            float32_t * pSrcB,
           float32_t * pDst,
           uint32_t blockSize);

  void arm_sub_q7(
           q7_t * pSrcA,
           q7_t * pSrcB,
          q7_t * pDst,
          uint32_t blockSize);

  void arm_sub_q15(
            q15_t * pSrcA,
            q15_t * pSrcB,
           q15_t * pDst,
           uint32_t blockSize);

  void arm_sub_q31(
            q31_t * pSrcA,
            q31_t * pSrcB,
           q31_t * pDst,
           uint32_t blockSize);

  void arm_scale_f32(
              float32_t * pSrc,
             float32_t scale,
             float32_t * pDst,
             uint32_t blockSize);

  void arm_scale_q7(
             q7_t * pSrc,
            q7_t scaleFract,
            int8_t shift,
            q7_t * pDst,
            uint32_t blockSize);

  void arm_scale_q15(
              q15_t * pSrc,
             q15_t scaleFract,
             int8_t shift,
             q15_t * pDst,
             uint32_t blockSize);

  void arm_scale_q31(
              q31_t * pSrc,
             q31_t scaleFract,
             int8_t shift,
             q31_t * pDst,
             uint32_t blockSize);

  void arm_abs_q7(
           q7_t * pSrc,
          q7_t * pDst,
          uint32_t blockSize);

  void arm_abs_f32(
            float32_t * pSrc,
           float32_t * pDst,
           uint32_t blockSize);

  void arm_abs_q15(
            q15_t * pSrc,
           q15_t * pDst,
           uint32_t blockSize);

  void arm_abs_q31(
            q31_t * pSrc,
           q31_t * pDst,
           uint32_t blockSize);

  void arm_dot_prod_f32(
             float32_t * pSrcA,
             float32_t * pSrcB,
            uint32_t blockSize,
            float32_t * result);

  void arm_dot_prod_q7(
                q7_t * pSrcA,
                q7_t * pSrcB,
               uint32_t blockSize,
               q31_t * result);

  void arm_dot_prod_q15(
             q15_t * pSrcA,
             q15_t * pSrcB,
            uint32_t blockSize,
            q63_t * result);

  void arm_dot_prod_q31(
             q31_t * pSrcA,
             q31_t * pSrcB,
            uint32_t blockSize,
            q63_t * result);

  void arm_shift_q7(
             q7_t * pSrc,
            int8_t shiftBits,
            q7_t * pDst,
            uint32_t blockSize);

  void arm_shift_q15(
              q15_t * pSrc,
             int8_t shiftBits,
             q15_t * pDst,
             uint32_t blockSize);

  void arm_shift_q31(
              q31_t * pSrc,
             int8_t shiftBits,
             q31_t * pDst,
             uint32_t blockSize);

  void arm_offset_f32(
               float32_t * pSrc,
              float32_t offset,
              float32_t * pDst,
              uint32_t blockSize);

  void arm_offset_q7(
              q7_t * pSrc,
             q7_t offset,
             q7_t * pDst,
             uint32_t blockSize);

  void arm_offset_q15(
               q15_t * pSrc,
              q15_t offset,
              q15_t * pDst,
              uint32_t blockSize);

  void arm_offset_q31(
               q31_t * pSrc,
              q31_t offset,
              q31_t * pDst,
              uint32_t blockSize);

  void arm_negate_f32(
               float32_t * pSrc,
              float32_t * pDst,
              uint32_t blockSize);

  void arm_negate_q7(
              q7_t * pSrc,
             q7_t * pDst,
             uint32_t blockSize);

  void arm_negate_q15(
               q15_t * pSrc,
              q15_t * pDst,
              uint32_t blockSize);

  void arm_negate_q31(
               q31_t * pSrc,
              q31_t * pDst,
              uint32_t blockSize);
  void arm_copy_f32(
             float32_t * pSrc,
            float32_t * pDst,
            uint32_t blockSize);

  void arm_copy_q7(
            q7_t * pSrc,
           q7_t * pDst,
           uint32_t blockSize);

  void arm_copy_q15(
             q15_t * pSrc,
            q15_t * pDst,
            uint32_t blockSize);

  void arm_copy_q31(
             q31_t * pSrc,
            q31_t * pDst,
            uint32_t blockSize);
  void arm_fill_f32(
             float32_t value,
            float32_t * pDst,
            uint32_t blockSize);

  void arm_fill_q7(
            q7_t value,
           q7_t * pDst,
           uint32_t blockSize);

  void arm_fill_q15(
             q15_t value,
            q15_t * pDst,
            uint32_t blockSize);

  void arm_fill_q31(
             q31_t value,
            q31_t * pDst,
            uint32_t blockSize);

  void arm_conv_f32(
             float32_t * pSrcA,
            uint32_t srcALen,
             float32_t * pSrcB,
            uint32_t srcBLen,
            float32_t * pDst);

  void arm_conv_q15(
             q15_t * pSrcA,
            uint32_t srcALen,
             q15_t * pSrcB,
            uint32_t srcBLen,
            q15_t * pDst);

  void arm_conv_fast_q15(
              q15_t * pSrcA,
             uint32_t srcALen,
              q15_t * pSrcB,
             uint32_t srcBLen,
             q15_t * pDst);

  void arm_conv_q31(
             q31_t * pSrcA,
            uint32_t srcALen,
             q31_t * pSrcB,
            uint32_t srcBLen,
            q31_t * pDst);

  void arm_conv_fast_q31(
              q31_t * pSrcA,
             uint32_t srcALen,
              q31_t * pSrcB,
             uint32_t srcBLen,
             q31_t * pDst);

  void arm_conv_q7(
            q7_t * pSrcA,
           uint32_t srcALen,
            q7_t * pSrcB,
           uint32_t srcBLen,
           q7_t * pDst);

  arm_status arm_conv_partial_f32(
                   float32_t * pSrcA,
                  uint32_t srcALen,
                   float32_t * pSrcB,
                  uint32_t srcBLen,
                  float32_t * pDst,
                  uint32_t firstIndex,
                  uint32_t numPoints);

  arm_status arm_conv_partial_q15(
                   q15_t * pSrcA,
                  uint32_t srcALen,
                   q15_t * pSrcB,
                  uint32_t srcBLen,
                  q15_t * pDst,
                  uint32_t firstIndex,
                  uint32_t numPoints);

  arm_status arm_conv_partial_fast_q15(
                        q15_t * pSrcA,
                       uint32_t srcALen,
                        q15_t * pSrcB,
                       uint32_t srcBLen,
                       q15_t * pDst,
                       uint32_t firstIndex,
                       uint32_t numPoints);

  arm_status arm_conv_partial_q31(
                   q31_t * pSrcA,
                  uint32_t srcALen,
                   q31_t * pSrcB,
                  uint32_t srcBLen,
                  q31_t * pDst,
                  uint32_t firstIndex,
                  uint32_t numPoints);


  arm_status arm_conv_partial_fast_q31(
                        q31_t * pSrcA,
                       uint32_t srcALen,
                        q31_t * pSrcB,
                       uint32_t srcBLen,
                       q31_t * pDst,
                       uint32_t firstIndex,
                       uint32_t numPoints);

  arm_status arm_conv_partial_q7(
                  q7_t * pSrcA,
                 uint32_t srcALen,
                  q7_t * pSrcB,
                 uint32_t srcBLen,
                 q7_t * pDst,
                 uint32_t firstIndex,
                 uint32_t numPoints);


  typedef struct
  {
    uint8_t M;                      
    uint16_t numTaps;               
    q15_t *pCoeffs;                  
    q15_t *pState;                   
  } arm_fir_decimate_instance_q15;

  typedef struct
  {
    uint8_t M;                  
    uint16_t numTaps;           
    q31_t *pCoeffs;              
    q31_t *pState;               
  } arm_fir_decimate_instance_q31;

  typedef struct
  {
    uint8_t M;                          
    uint16_t numTaps;                   
    float32_t *pCoeffs;                  
    float32_t *pState;                   
  } arm_fir_decimate_instance_f32;



  void arm_fir_decimate_f32(
                const arm_fir_decimate_instance_f32 * S,
                 float32_t * pSrc,
                float32_t * pDst,
                uint32_t blockSize);


  arm_status arm_fir_decimate_init_f32(
                       arm_fir_decimate_instance_f32 * S,
                       uint16_t numTaps,
                       uint8_t M,
                       float32_t * pCoeffs,
                       float32_t * pState,
                       uint32_t blockSize);

  void arm_fir_decimate_q15(
                const arm_fir_decimate_instance_q15 * S,
                 q15_t * pSrc,
                q15_t * pDst,
                uint32_t blockSize);

  void arm_fir_decimate_fast_q15(
                 const arm_fir_decimate_instance_q15 * S,
                  q15_t * pSrc,
                 q15_t * pDst,
                 uint32_t blockSize);



  arm_status arm_fir_decimate_init_q15(
                       arm_fir_decimate_instance_q15 * S,
                       uint16_t numTaps,
                       uint8_t M,
                       q15_t * pCoeffs,
                       q15_t * pState,
                       uint32_t blockSize);

  void arm_fir_decimate_q31(
                const arm_fir_decimate_instance_q31 * S,
                 q31_t * pSrc,
                q31_t * pDst,
                uint32_t blockSize);

  void arm_fir_decimate_fast_q31(
                 arm_fir_decimate_instance_q31 * S,
                  q31_t * pSrc,
                 q31_t * pDst,
                 uint32_t blockSize);


  arm_status arm_fir_decimate_init_q31(
                       arm_fir_decimate_instance_q31 * S,
                       uint16_t numTaps,
                       uint8_t M,
                       q31_t * pCoeffs,
                       q31_t * pState,
                       uint32_t blockSize);



  typedef struct
  {
    uint8_t L;                      
    uint16_t phaseLength;           
    q15_t *pCoeffs;                 
    q15_t *pState;                  
  } arm_fir_interpolate_instance_q15;

  typedef struct
  {
    uint8_t L;                      
    uint16_t phaseLength;           
    q31_t *pCoeffs;                  
    q31_t *pState;                   
  } arm_fir_interpolate_instance_q31;

  typedef struct
  {
    uint8_t L;                     
    uint16_t phaseLength;          
    float32_t *pCoeffs;             
    float32_t *pState;              
  } arm_fir_interpolate_instance_f32;


  void arm_fir_interpolate_q15(
                   const arm_fir_interpolate_instance_q15 * S,
                    q15_t * pSrc,
                   q15_t * pDst,
                   uint32_t blockSize);


  arm_status arm_fir_interpolate_init_q15(
                      arm_fir_interpolate_instance_q15 * S,
                      uint8_t L,
                      uint16_t numTaps,
                      q15_t * pCoeffs,
                      q15_t * pState,
                      uint32_t blockSize);

  void arm_fir_interpolate_q31(
                   const arm_fir_interpolate_instance_q31 * S,
                    q31_t * pSrc,
                   q31_t * pDst,
                   uint32_t blockSize);

  arm_status arm_fir_interpolate_init_q31(
                      arm_fir_interpolate_instance_q31 * S,
                      uint8_t L,
                      uint16_t numTaps,
                      q31_t * pCoeffs,
                      q31_t * pState,
                      uint32_t blockSize);


  void arm_fir_interpolate_f32(
                   const arm_fir_interpolate_instance_f32 * S,
                    float32_t * pSrc,
                   float32_t * pDst,
                   uint32_t blockSize);

  arm_status arm_fir_interpolate_init_f32(
                      arm_fir_interpolate_instance_f32 * S,
                      uint8_t L,
                      uint16_t numTaps,
                      float32_t * pCoeffs,
                      float32_t * pState,
                      uint32_t blockSize);

  typedef struct
  {
    uint8_t numStages;       
    q63_t *pState;           
    q31_t *pCoeffs;          
    uint8_t postShift;       
  } arm_biquad_cas_df1_32x64_ins_q31;


  void arm_biquad_cas_df1_32x64_q31(
                    const arm_biquad_cas_df1_32x64_ins_q31 * S,
                     q31_t * pSrc,
                    q31_t * pDst,
                    uint32_t blockSize);


  void arm_biquad_cas_df1_32x64_init_q31(
                     arm_biquad_cas_df1_32x64_ins_q31 * S,
                     uint8_t numStages,
                     q31_t * pCoeffs,
                     q63_t * pState,
                     uint8_t postShift);



  typedef struct
  {
    uint8_t   numStages;       
    float32_t *pState;         
    float32_t *pCoeffs;        
  } arm_biquad_cascade_df2T_instance_f32;


  void arm_biquad_cascade_df2T_f32(
                   const arm_biquad_cascade_df2T_instance_f32 * S,
                    float32_t * pSrc,
                   float32_t * pDst,
                   uint32_t blockSize);


  void arm_biquad_cascade_df2T_init_f32(
                    arm_biquad_cascade_df2T_instance_f32 * S,
                    uint8_t numStages,
                    float32_t * pCoeffs,
                    float32_t * pState);



  typedef struct
  {
    uint16_t numStages;                          
    q15_t *pState;                               
    q15_t *pCoeffs;                              
  } arm_fir_lattice_instance_q15;

  typedef struct
  {
    uint16_t numStages;                          
    q31_t *pState;                               
    q31_t *pCoeffs;                              
  } arm_fir_lattice_instance_q31;

  typedef struct
  {
    uint16_t numStages;                  
    float32_t *pState;                   
    float32_t *pCoeffs;                  
  } arm_fir_lattice_instance_f32;

  void arm_fir_lattice_init_q15(
                arm_fir_lattice_instance_q15 * S,
                uint16_t numStages,
                q15_t * pCoeffs,
                q15_t * pState);


  void arm_fir_lattice_q15(
               const arm_fir_lattice_instance_q15 * S,
                q15_t * pSrc,
               q15_t * pDst,
               uint32_t blockSize);

  void arm_fir_lattice_init_q31(
                arm_fir_lattice_instance_q31 * S,
                uint16_t numStages,
                q31_t * pCoeffs,
                q31_t * pState);


  void arm_fir_lattice_q31(
               const arm_fir_lattice_instance_q31 * S,
                q31_t * pSrc,
               q31_t * pDst,
               uint32_t blockSize);

  void arm_fir_lattice_init_f32(
                arm_fir_lattice_instance_f32 * S,
                uint16_t numStages,
                float32_t * pCoeffs,
                float32_t * pState);

  void arm_fir_lattice_f32(
               const arm_fir_lattice_instance_f32 * S,
                float32_t * pSrc,
               float32_t * pDst,
               uint32_t blockSize);

  typedef struct
  {
    uint16_t numStages;                         
    q15_t *pState;                              
    q15_t *pkCoeffs;                            
    q15_t *pvCoeffs;                            
  } arm_iir_lattice_instance_q15;

  typedef struct
  {
    uint16_t numStages;                         
    q31_t *pState;                              
    q31_t *pkCoeffs;                            
    q31_t *pvCoeffs;                            
  } arm_iir_lattice_instance_q31;

  typedef struct
  {
    uint16_t numStages;                         
    float32_t *pState;                          
    float32_t *pkCoeffs;                        
    float32_t *pvCoeffs;                        
  } arm_iir_lattice_instance_f32;

  void arm_iir_lattice_f32(
               const arm_iir_lattice_instance_f32 * S,
                float32_t * pSrc,
               float32_t * pDst,
               uint32_t blockSize);

  void arm_iir_lattice_init_f32(
                arm_iir_lattice_instance_f32 * S,
                uint16_t numStages,
                float32_t *pkCoeffs,
                float32_t *pvCoeffs,
                float32_t *pState,
                uint32_t blockSize);


  void arm_iir_lattice_q31(
               const arm_iir_lattice_instance_q31 * S,
                q31_t * pSrc,
               q31_t * pDst,
               uint32_t blockSize);


  void arm_iir_lattice_init_q31(
                arm_iir_lattice_instance_q31 * S,
                uint16_t numStages,
                q31_t *pkCoeffs,
                q31_t *pvCoeffs,
                q31_t *pState,
                uint32_t blockSize);


  void arm_iir_lattice_q15(
               const arm_iir_lattice_instance_q15 * S,
                q15_t * pSrc,
               q15_t * pDst,
               uint32_t blockSize);


  void arm_iir_lattice_init_q15(
                arm_iir_lattice_instance_q15 * S,
                uint16_t numStages,
                q15_t *pkCoeffs,
                q15_t *pvCoeffs,
                q15_t *pState,
                uint32_t blockSize);

  typedef struct
  {
    uint16_t numTaps;    
    float32_t *pState;   
    float32_t *pCoeffs;  
    float32_t mu;        
  } arm_lms_instance_f32;

  void arm_lms_f32(
           const arm_lms_instance_f32 * S,
            float32_t * pSrc,
            float32_t * pRef,
           float32_t * pOut,
           float32_t * pErr,
           uint32_t blockSize);

  void arm_lms_init_f32(
            arm_lms_instance_f32 * S,
            uint16_t numTaps,
            float32_t * pCoeffs,
            float32_t * pState,
            float32_t mu,
            uint32_t blockSize);

  typedef struct
  {
    uint16_t numTaps;    
    q15_t *pState;       
    q15_t *pCoeffs;      
    q15_t mu;            
    uint32_t postShift;  
  } arm_lms_instance_q15;


  void arm_lms_init_q15(
            arm_lms_instance_q15 * S,
            uint16_t numTaps,
            q15_t * pCoeffs,
            q15_t * pState,
            q15_t mu,
            uint32_t blockSize,
            uint32_t postShift);

  void arm_lms_q15(
           const arm_lms_instance_q15 * S,
            q15_t * pSrc,
            q15_t * pRef,
           q15_t * pOut,
           q15_t * pErr,
           uint32_t blockSize);


  typedef struct
  {
    uint16_t numTaps;    
    q31_t *pState;       
    q31_t *pCoeffs;      
    q31_t mu;            
    uint32_t postShift;  
  } arm_lms_instance_q31;

  void arm_lms_q31(
           const arm_lms_instance_q31 * S,
            q31_t * pSrc,
            q31_t * pRef,
           q31_t * pOut,
           q31_t * pErr,
           uint32_t blockSize);

  void arm_lms_init_q31(
            arm_lms_instance_q31 * S,
            uint16_t numTaps,
            q31_t *pCoeffs,
            q31_t *pState,
            q31_t mu,
            uint32_t blockSize,
            uint32_t postShift);

  typedef struct
  {
    uint16_t  numTaps;    
    float32_t *pState;    
    float32_t *pCoeffs;   
    float32_t mu;        
    float32_t energy;    
    float32_t x0;        
  } arm_lms_norm_instance_f32;

  void arm_lms_norm_f32(
            arm_lms_norm_instance_f32 * S,
             float32_t * pSrc,
             float32_t * pRef,
            float32_t * pOut,
            float32_t * pErr,
            uint32_t blockSize);

  void arm_lms_norm_init_f32(
                 arm_lms_norm_instance_f32 * S,
                 uint16_t numTaps,
                 float32_t * pCoeffs,
                 float32_t * pState,
                 float32_t mu,
                 uint32_t blockSize);


  typedef struct
  {
    uint16_t numTaps;     
    q31_t *pState;        
    q31_t *pCoeffs;       
    q31_t mu;             
    uint8_t postShift;    
    q31_t *recipTable;    
    q31_t energy;         
    q31_t x0;             
  } arm_lms_norm_instance_q31;

  void arm_lms_norm_q31(
            arm_lms_norm_instance_q31 * S,
             q31_t * pSrc,
             q31_t * pRef,
            q31_t * pOut,
            q31_t * pErr,
            uint32_t blockSize);

  void arm_lms_norm_init_q31(
                 arm_lms_norm_instance_q31 * S,
                 uint16_t numTaps,
                 q31_t * pCoeffs,
                 q31_t * pState,
                 q31_t mu,
                 uint32_t blockSize,
                 uint8_t postShift);

  typedef struct
  {
    uint16_t numTaps;    
    q15_t *pState;        
    q15_t *pCoeffs;       
    q15_t mu;            
    uint8_t postShift;   
    q15_t *recipTable;   
    q15_t energy;        
    q15_t x0;            
  } arm_lms_norm_instance_q15;

  void arm_lms_norm_q15(
            arm_lms_norm_instance_q15 * S,
             q15_t * pSrc,
             q15_t * pRef,
            q15_t * pOut,
            q15_t * pErr,
            uint32_t blockSize);


  void arm_lms_norm_init_q15(
                 arm_lms_norm_instance_q15 * S,
                 uint16_t numTaps,
                 q15_t * pCoeffs,
                 q15_t * pState,
                 q15_t mu,
                 uint32_t blockSize,
                 uint8_t postShift);

  void arm_correlate_f32(
              float32_t * pSrcA,
             uint32_t srcALen,
              float32_t * pSrcB,
             uint32_t srcBLen,
             float32_t * pDst);

  void arm_correlate_q15(
              q15_t * pSrcA,
             uint32_t srcALen,
              q15_t * pSrcB,
             uint32_t srcBLen,
             q15_t * pDst);

  void arm_correlate_fast_q15(
                   q15_t * pSrcA,
                  uint32_t srcALen,
                   q15_t * pSrcB,
                  uint32_t srcBLen,
                  q15_t * pDst);

  void arm_correlate_q31(
              q31_t * pSrcA,
             uint32_t srcALen,
              q31_t * pSrcB,
             uint32_t srcBLen,
             q31_t * pDst);

  void arm_correlate_fast_q31(
                   q31_t * pSrcA,
                  uint32_t srcALen,
                   q31_t * pSrcB,
                  uint32_t srcBLen,
                  q31_t * pDst);

  void arm_correlate_q7(
             q7_t * pSrcA,
            uint32_t srcALen,
             q7_t * pSrcB,
            uint32_t srcBLen,
            q7_t * pDst);

  typedef struct
  {
    uint16_t numTaps;             
    uint16_t stateIndex;          
    float32_t *pState;            
    float32_t *pCoeffs;           
    uint16_t maxDelay;            
    int32_t *pTapDelay;           
  } arm_fir_sparse_instance_f32;

  typedef struct
  {
    uint16_t numTaps;             
    uint16_t stateIndex;          
    q31_t *pState;                
    q31_t *pCoeffs;               
    uint16_t maxDelay;            
    int32_t *pTapDelay;           
  } arm_fir_sparse_instance_q31;

  typedef struct
  {
    uint16_t numTaps;             
    uint16_t stateIndex;          
    q15_t *pState;                
    q15_t *pCoeffs;               
    uint16_t maxDelay;            
    int32_t *pTapDelay;           
  } arm_fir_sparse_instance_q15;

  typedef struct
  {
    uint16_t numTaps;             
    uint16_t stateIndex;          
    q7_t *pState;                 
    q7_t *pCoeffs;                
    uint16_t maxDelay;            
    int32_t *pTapDelay;           
  } arm_fir_sparse_instance_q7;

  void arm_fir_sparse_f32(
              arm_fir_sparse_instance_f32 * S,
               float32_t * pSrc,
              float32_t * pDst,
              float32_t * pScratchIn,
              uint32_t blockSize);

  void arm_fir_sparse_init_f32(
                   arm_fir_sparse_instance_f32 * S,
                   uint16_t numTaps,
                   float32_t * pCoeffs,
                   float32_t * pState,
                   int32_t * pTapDelay,
                   uint16_t maxDelay,
                   uint32_t blockSize);

  void arm_fir_sparse_q31(
              arm_fir_sparse_instance_q31 * S,
               q31_t * pSrc,
              q31_t * pDst,
              q31_t * pScratchIn,
              uint32_t blockSize);

  void arm_fir_sparse_init_q31(
                   arm_fir_sparse_instance_q31 * S,
                   uint16_t numTaps,
                   q31_t * pCoeffs,
                   q31_t * pState,
                   int32_t * pTapDelay,
                   uint16_t maxDelay,
                   uint32_t blockSize);

  void arm_fir_sparse_q15(
              arm_fir_sparse_instance_q15 * S,
               q15_t * pSrc,
              q15_t * pDst,
              q15_t * pScratchIn,
              q31_t * pScratchOut,
              uint32_t blockSize);


  void arm_fir_sparse_init_q15(
                   arm_fir_sparse_instance_q15 * S,
                   uint16_t numTaps,
                   q15_t * pCoeffs,
                   q15_t * pState,
                   int32_t * pTapDelay,
                   uint16_t maxDelay,
                   uint32_t blockSize);

  void arm_fir_sparse_q7(
             arm_fir_sparse_instance_q7 * S,
              q7_t * pSrc,
             q7_t * pDst,
             q7_t * pScratchIn,
             q31_t * pScratchOut,
             uint32_t blockSize);

  void arm_fir_sparse_init_q7(
                  arm_fir_sparse_instance_q7 * S,
                  uint16_t numTaps,
                  q7_t * pCoeffs,
                  q7_t * pState,
                  int32_t *pTapDelay,
                  uint16_t maxDelay,
                  uint32_t blockSize);


  /*
   * @brief  Floating-point sin_cos function.
   * @param[in]  theta    input value in degrees 
   * @param[out] *pSinVal points to the processed sine output. 
   * @param[out] *pCosVal points to the processed cos output. 
   * @return none.
   */

  void arm_sin_cos_f32(
               float32_t theta,
               float32_t *pSinVal,
               float32_t *pCcosVal);

  /*
   * @brief  Q31 sin_cos function.
   * @param[in]  theta    scaled input value in degrees 
   * @param[out] *pSinVal points to the processed sine output. 
   * @param[out] *pCosVal points to the processed cosine output. 
   * @return none.
   */

  void arm_sin_cos_q31(
               q31_t theta,
               q31_t *pSinVal,
               q31_t *pCosVal);


  void arm_cmplx_conj_f32(
               float32_t * pSrc,
              float32_t * pDst,
              uint32_t numSamples);

  void arm_cmplx_conj_q31(
               q31_t * pSrc,
              q31_t * pDst,
              uint32_t numSamples);

  void arm_cmplx_conj_q15(
               q15_t * pSrc,
              q15_t * pDst,
              uint32_t numSamples);



  void arm_cmplx_mag_squared_f32(
                  float32_t * pSrc,
                 float32_t * pDst,
                 uint32_t numSamples);

  void arm_cmplx_mag_squared_q31(
                  q31_t * pSrc,
                 q31_t * pDst,
                 uint32_t numSamples);

  void arm_cmplx_mag_squared_q15(
                  q15_t * pSrc,
                 q15_t * pDst,
                 uint32_t numSamples);


  static __INLINE float32_t arm_pid_f32(
                    arm_pid_instance_f32 * S,
                    float32_t in)
  {
    float32_t out;

    /* y[n] = y[n-1] + A0 * x[n] + A1 * x[n-1] + A2 * x[n-2]  */
    out = (S->A0 * in) +
      (S->A1 * S->state[0]) + (S->A2 * S->state[1]) + (S->state[2]);

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);

  }

  static __INLINE q31_t arm_pid_q31(
                    arm_pid_instance_q31 * S,
                    q31_t in)
  {
    q63_t acc;
    q31_t out;

    /* acc = A0 * x[n]  */
    acc = (q63_t) S->A0 * in;

    /* acc += A1 * x[n-1] */
    acc += (q63_t) S->A1 * S->state[0];

    /* acc += A2 * x[n-2]  */
    acc += (q63_t) S->A2 * S->state[1];

    /* convert output to 1.31 format to add y[n-1] */
    out = (q31_t) (acc >> 31u);

    /* out += y[n-1] */
    out += S->state[2];

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);

  }

  static __INLINE q15_t arm_pid_q15(
                    arm_pid_instance_q15 * S,
                    q15_t in)
  {
    q63_t acc;
    q15_t out;

    /* Implementation of PID controller */

    #ifdef ARM_MATH_CM0

    /* acc = A0 * x[n]  */
    acc = ((q31_t) S->A0 )* in ;

    #else
                
    /* acc = A0 * x[n]  */
    acc = (q31_t) __SMUAD(S->A0, in);
    
    #endif

    #ifdef ARM_MATH_CM0
                           
    /* acc += A1 * x[n-1] + A2 * x[n-2]  */
    acc += (q31_t) S->A1  *  S->state[0] ;
    acc += (q31_t) S->A2  *  S->state[1] ;

    #else

    /* acc += A1 * x[n-1] + A2 * x[n-2]  */
    acc = __SMLALD(S->A1, (q31_t)__SIMD32(S->state), acc);

    #endif

    /* acc += y[n-1] */
    acc += (q31_t) S->state[2] << 15;

    /* saturate the output */
    out = (q15_t) (__SSAT((acc >> 15), 16));

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);

  }
  
  arm_status arm_mat_inverse_f32(
                 const arm_matrix_instance_f32 * src,
                 arm_matrix_instance_f32 * dst);

  
 
  static __INLINE void arm_clarke_f32(
                      float32_t Ia,
                      float32_t Ib,
                      float32_t * pIalpha,
                      float32_t * pIbeta)
  {
    /* Calculate pIalpha using the equation, pIalpha = Ia */
    *pIalpha = Ia;

    /* Calculate pIbeta using the equation, pIbeta = (1/sqrt(3)) * Ia + (2/sqrt(3)) * Ib */
    *pIbeta = ((float32_t) 0.57735026919 * Ia + (float32_t) 1.15470053838 * Ib);

  }

  static __INLINE void arm_clarke_q31(
                      q31_t Ia,
                      q31_t Ib,
                      q31_t * pIalpha,
                      q31_t * pIbeta)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */

    /* Calculating pIalpha from Ia by equation pIalpha = Ia */
    *pIalpha = Ia;

    /* Intermediate product is calculated by (1/(sqrt(3)) * Ia) */
    product1 = (q31_t) (((q63_t) Ia * 0x24F34E8B) >> 30);

    /* Intermediate product is calculated by (2/sqrt(3) * Ib) */
    product2 = (q31_t) (((q63_t) Ib * 0x49E69D16) >> 30);

    /* pIbeta is calculated by adding the intermediate products */
    *pIbeta = __QADD(product1, product2);
  }

  void arm_q7_to_q31(
             q7_t * pSrc,
             q31_t * pDst,
             uint32_t blockSize);


 

  static __INLINE void arm_inv_clarke_f32(
                      float32_t Ialpha,
                      float32_t Ibeta,
                      float32_t * pIa,
                      float32_t * pIb)
  {
    /* Calculating pIa from Ialpha by equation pIa = Ialpha */
    *pIa = Ialpha;

    /* Calculating pIb from Ialpha and Ibeta by equation pIb = -(1/2) * Ialpha + (sqrt(3)/2) * Ibeta */
    *pIb = -0.5 * Ialpha + (float32_t) 0.8660254039 *Ibeta;

  }

  static __INLINE void arm_inv_clarke_q31(
                      q31_t Ialpha,
                      q31_t Ibeta,
                      q31_t * pIa,
                      q31_t * pIb)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */

    /* Calculating pIa from Ialpha by equation pIa = Ialpha */
    *pIa = Ialpha;

    /* Intermediate product is calculated by (1/(2*sqrt(3)) * Ia) */
    product1 = (q31_t) (((q63_t) (Ialpha) * (0x40000000)) >> 31);

    /* Intermediate product is calculated by (1/sqrt(3) * pIb) */
    product2 = (q31_t) (((q63_t) (Ibeta) * (0x6ED9EBA1)) >> 31);

    /* pIb is calculated by subtracting the products */
    *pIb = __QSUB(product2, product1);

  }

  void arm_q7_to_q15(
              q7_t * pSrc,
             q15_t * pDst,
             uint32_t blockSize);

  

  static __INLINE void arm_park_f32(
                    float32_t Ialpha,
                    float32_t Ibeta,
                    float32_t * pId,
                    float32_t * pIq,
                    float32_t sinVal,
                    float32_t cosVal)
  {
    /* Calculate pId using the equation, pId = Ialpha * cosVal + Ibeta * sinVal */
    *pId = Ialpha * cosVal + Ibeta * sinVal;

    /* Calculate pIq using the equation, pIq = - Ialpha * sinVal + Ibeta * cosVal */
    *pIq = -Ialpha * sinVal + Ibeta * cosVal;

  }

  static __INLINE void arm_park_q31(
                    q31_t Ialpha,
                    q31_t Ibeta,
                    q31_t * pId,
                    q31_t * pIq,
                    q31_t sinVal,
                    q31_t cosVal)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */
    q31_t product3, product4;                    /* Temporary variables used to store intermediate results */

    /* Intermediate product is calculated by (Ialpha * cosVal) */
    product1 = (q31_t) (((q63_t) (Ialpha) * (cosVal)) >> 31);

    /* Intermediate product is calculated by (Ibeta * sinVal) */
    product2 = (q31_t) (((q63_t) (Ibeta) * (sinVal)) >> 31);


    /* Intermediate product is calculated by (Ialpha * sinVal) */
    product3 = (q31_t) (((q63_t) (Ialpha) * (sinVal)) >> 31);

    /* Intermediate product is calculated by (Ibeta * cosVal) */
    product4 = (q31_t) (((q63_t) (Ibeta) * (cosVal)) >> 31);

    /* Calculate pId by adding the two intermediate products 1 and 2 */
    *pId = __QADD(product1, product2);

    /* Calculate pIq by subtracting the two intermediate products 3 from 4 */
    *pIq = __QSUB(product4, product3);
  }

  void arm_q7_to_float(
                q7_t * pSrc,
               float32_t * pDst,
               uint32_t blockSize);

 
  static __INLINE void arm_inv_park_f32(
                    float32_t Id,
                    float32_t Iq,
                    float32_t * pIalpha,
                    float32_t * pIbeta,
                    float32_t sinVal,
                    float32_t cosVal)
  {
    /* Calculate pIalpha using the equation, pIalpha = Id * cosVal - Iq * sinVal */
    *pIalpha = Id * cosVal - Iq * sinVal;

    /* Calculate pIbeta using the equation, pIbeta = Id * sinVal + Iq * cosVal */
    *pIbeta = Id * sinVal + Iq * cosVal;

  }


  static __INLINE void arm_inv_park_q31(
                    q31_t Id,
                    q31_t Iq,
                    q31_t * pIalpha,
                    q31_t * pIbeta,
                    q31_t sinVal,
                    q31_t cosVal)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */
    q31_t product3, product4;                    /* Temporary variables used to store intermediate results */

    /* Intermediate product is calculated by (Id * cosVal) */
    product1 = (q31_t) (((q63_t) (Id) * (cosVal)) >> 31);

    /* Intermediate product is calculated by (Iq * sinVal) */
    product2 = (q31_t) (((q63_t) (Iq) * (sinVal)) >> 31);


    /* Intermediate product is calculated by (Id * sinVal) */
    product3 = (q31_t) (((q63_t) (Id) * (sinVal)) >> 31);

    /* Intermediate product is calculated by (Iq * cosVal) */
    product4 = (q31_t) (((q63_t) (Iq) * (cosVal)) >> 31);

    /* Calculate pIalpha by using the two intermediate products 1 and 2 */
    *pIalpha = __QSUB(product1, product2);

    /* Calculate pIbeta by using the two intermediate products 3 and 4 */
    *pIbeta = __QADD(product4, product3);

  }

  void arm_q31_to_float(
             q31_t * pSrc,
            float32_t * pDst,
            uint32_t blockSize);

  static __INLINE float32_t arm_linear_interp_f32(
                          arm_linear_interp_instance_f32 * S,
                          float32_t x)
  {

      float32_t y;
      float32_t x0, x1;                     /* Nearest input values */
      float32_t y0, y1;                     /* Nearest output values */
      float32_t xSpacing = S->xSpacing;     /* spacing between input values */
      int32_t i;                            /* Index variable */
      float32_t *pYData = S->pYData;        /* pointer to output table */

      /* Calculation of index */
      i =   (x - S->x1) / xSpacing;

      if(i < 0)
      {
         /* Iniatilize output for below specified range as least output value of table */
         y = pYData[0];
      }
      else if(i >= S->nValues)
      {
          /* Iniatilize output for above specified range as last output value of table */
          y = pYData[S->nValues-1]; 
      }
      else
      {  
          /* Calculation of nearest input values */
          x0 = S->x1 + i * xSpacing;
          x1 = S->x1 + (i +1) * xSpacing;
         
         /* Read of nearest output values */
          y0 = pYData[i];
          y1 = pYData[i + 1];
        
          /* Calculation of output */
          y = y0 + (x - x0) * ((y1 - y0)/(x1-x0));  
        
      }

      /* returns output value */
      return (y);
  }

  static __INLINE q31_t arm_linear_interp_q31(q31_t *pYData,
                          q31_t x, uint32_t nValues)
  {
    q31_t y;                                   /* output */
    q31_t y0, y1;                                /* Nearest output values */
    q31_t fract;                                 /* fractional part */
    int32_t index;                              /* Index to read nearest output values */
    
    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    index = ((x & 0xFFF00000) >> 20);

    if(index >= (nValues - 1))
    {
        return(pYData[nValues - 1]);
    }
    else if(index < 0)
    {
        return(pYData[0]);
    }
    else
    {

        /* 20 bits for the fractional part */
        /* shift left by 11 to keep fract in 1.31 format */
        fract = (x & 0x000FFFFF) << 11;
    
        /* Read two nearest output values from the index in 1.31(q31) format */
        y0 = pYData[index];
        y1 = pYData[index + 1u];
    
        /* Calculation of y0 * (1-fract) and y is in 2.30 format */
        y = ((q31_t) ((q63_t) y0 * (0x7FFFFFFF - fract) >> 32));
    
        /* Calculation of y0 * (1-fract) + y1 *fract and y is in 2.30 format */
        y += ((q31_t) (((q63_t) y1 * fract) >> 32));
    
        /* Convert y to 1.31 format */
        return (y << 1u);

    }

  }

  static __INLINE q15_t arm_linear_interp_q15(q15_t *pYData, q31_t x, uint32_t nValues)
  {
    q63_t y;                                   /* output */
    q15_t y0, y1;                              /* Nearest output values */
    q31_t fract;                               /* fractional part */
    int32_t index;                            /* Index to read nearest output values */ 

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    index = ((x & 0xFFF00000) >> 20u); 

    if(index >= (nValues - 1))
    {
        return(pYData[nValues - 1]);
    }
    else if(index < 0)
    {
        return(pYData[0]);
    }
    else
    {   
        /* 20 bits for the fractional part */
        /* fract is in 12.20 format */
        fract = (x & 0x000FFFFF);
    
        /* Read two nearest output values from the index */
        y0 = pYData[index];
        y1 = pYData[index + 1u];
    
        /* Calculation of y0 * (1-fract) and y is in 13.35 format */
        y = ((q63_t) y0 * (0xFFFFF - fract));
    
        /* Calculation of (y0 * (1-fract) + y1 * fract) and y is in 13.35 format */
        y += ((q63_t) y1 * (fract));
    
        /* convert y to 1.15 format */
        return (y >> 20);
    }


  }

  static __INLINE q7_t arm_linear_interp_q7(q7_t *pYData, q31_t x,  uint32_t nValues)
  {
    q31_t y;                                   /* output */
    q7_t y0, y1;                                 /* Nearest output values */
    q31_t fract;                                 /* fractional part */
    int32_t index;                              /* Index to read nearest output values */
    
    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    index = ((x & 0xFFF00000) >> 20u);


    if(index >= (nValues - 1))
    {
        return(pYData[nValues - 1]);
    }
    else if(index < 0)
    {
        return(pYData[0]);
    }
    else
    {

        /* 20 bits for the fractional part */
        /* fract is in 12.20 format */
        fract = (x & 0x000FFFFF);
    
        /* Read two nearest output values from the index and are in 1.7(q7) format */
        y0 = pYData[index];
        y1 = pYData[index + 1u];
    
        /* Calculation of y0 * (1-fract ) and y is in 13.27(q27) format */
        y = ((y0 * (0xFFFFF - fract)));
    
        /* Calculation of y1 * fract + y0 * (1-fract) and y is in 13.27(q27) format */
        y += (y1 * fract);
    
        /* convert y to 1.7(q7) format */
        return (y >> 20u);

    }

  }
  float32_t arm_sin_f32(
             float32_t x);

  q31_t arm_sin_q31(
             q31_t x);

  q15_t arm_sin_q15(
             q15_t x);

  float32_t arm_cos_f32(
             float32_t x);

  q31_t arm_cos_q31(
             q31_t x);

  q15_t arm_cos_q15(
             q15_t x);


  static __INLINE arm_status  arm_sqrt_f32(
                      float32_t in, float32_t *pOut)
  {
    if(in > 0)
    {

    #if __FPU_USED
        *pOut = __sqrtf(in);
    #else     
        *pOut = sqrtf(in);
    #endif

        return (ARM_MATH_SUCCESS);
    }
    else
    {
        *pOut = 0.0f;
        return (ARM_MATH_ARGUMENT_ERROR);
    }

  }


  arm_status arm_sqrt_q31(
              q31_t in, q31_t *pOut);

  arm_status arm_sqrt_q15(
              q15_t in, q15_t *pOut);

  static __INLINE void arm_circularWrite_f32(
                         int32_t * circBuffer,
                         int32_t L,
                         uint16_t * writeOffset,
                         int32_t bufferInc,
                         const int32_t * src,
                         int32_t srcInc,
                         uint32_t blockSize)
  {
    uint32_t i = 0u;
    int32_t wOffset;

    /* Copy the value of Index pointer that points
     * to the current location where the input samples to be copied */
    wOffset = *writeOffset;

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the input sample to the circular buffer */
    circBuffer[wOffset] = *src;

    /* Update the input pointer */
    src += srcInc;

    /* Circularly update wOffset.  Watch out for positive and negative value */
    wOffset += bufferInc;
    if(wOffset >= L)
      wOffset -= L;

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *writeOffset = wOffset;
  }



  static __INLINE void arm_circularRead_f32(
                        int32_t * circBuffer,
                        int32_t L,
                        int32_t * readOffset,
                        int32_t bufferInc,
                        int32_t * dst,
                        int32_t * dst_base,
                        int32_t dst_length,
                        int32_t dstInc,
                        uint32_t blockSize)
  {
    uint32_t i = 0u;
    int32_t rOffset, dst_end;

    /* Copy the value of Index pointer that points
     * to the current location from where the input samples to be read */
    rOffset = *readOffset;
    dst_end = (int32_t) (dst_base + dst_length);

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the sample from the circular buffer to the destination buffer */
    *dst = circBuffer[rOffset];

    /* Update the input pointer */
    dst += dstInc;

    if(dst == (int32_t *) dst_end)
      {
        dst = dst_base;
      }

    /* Circularly update rOffset.  Watch out for positive and negative value  */
    rOffset += bufferInc;

    if(rOffset >= L)
      {
        rOffset -= L;
      }

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *readOffset = rOffset;
  }

  static __INLINE void arm_circularWrite_q15(
                         q15_t * circBuffer,
                         int32_t L,
                         uint16_t * writeOffset,
                         int32_t bufferInc,
                         const q15_t * src,
                         int32_t srcInc,
                         uint32_t blockSize)
  {
    uint32_t i = 0u;
    int32_t wOffset;

    /* Copy the value of Index pointer that points
     * to the current location where the input samples to be copied */
    wOffset = *writeOffset;

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the input sample to the circular buffer */
    circBuffer[wOffset] = *src;

    /* Update the input pointer */
    src += srcInc;

    /* Circularly update wOffset.  Watch out for positive and negative value */
    wOffset += bufferInc;
    if(wOffset >= L)
      wOffset -= L;

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *writeOffset = wOffset;
  }



  static __INLINE void arm_circularRead_q15(
                        q15_t * circBuffer,
                        int32_t L,
                        int32_t * readOffset,
                        int32_t bufferInc,
                        q15_t * dst,
                        q15_t * dst_base,
                        int32_t dst_length,
                        int32_t dstInc,
                        uint32_t blockSize)
  {
    uint32_t i = 0;
    int32_t rOffset, dst_end;

    /* Copy the value of Index pointer that points
     * to the current location from where the input samples to be read */
    rOffset = *readOffset;

    dst_end = (int32_t) (dst_base + dst_length);

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the sample from the circular buffer to the destination buffer */
    *dst = circBuffer[rOffset];

    /* Update the input pointer */
    dst += dstInc;

    if(dst == (q15_t *) dst_end)
      {
        dst = dst_base;
      }

    /* Circularly update wOffset.  Watch out for positive and negative value */
    rOffset += bufferInc;

    if(rOffset >= L)
      {
        rOffset -= L;
      }

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *readOffset = rOffset;
  }


  static __INLINE void arm_circularWrite_q7(
                        q7_t * circBuffer,
                        int32_t L,
                        uint16_t * writeOffset,
                        int32_t bufferInc,
                        const q7_t * src,
                        int32_t srcInc,
                        uint32_t blockSize)
  {
    uint32_t i = 0u;
    int32_t wOffset;

    /* Copy the value of Index pointer that points
     * to the current location where the input samples to be copied */
    wOffset = *writeOffset;

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the input sample to the circular buffer */
    circBuffer[wOffset] = *src;

    /* Update the input pointer */
    src += srcInc;

    /* Circularly update wOffset.  Watch out for positive and negative value */
    wOffset += bufferInc;
    if(wOffset >= L)
      wOffset -= L;

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *writeOffset = wOffset;
  }



  static __INLINE void arm_circularRead_q7(
                       q7_t * circBuffer,
                       int32_t L,
                       int32_t * readOffset,
                       int32_t bufferInc,
                       q7_t * dst,
                       q7_t * dst_base,
                       int32_t dst_length,
                       int32_t dstInc,
                       uint32_t blockSize)
  {
    uint32_t i = 0;
    int32_t rOffset, dst_end;

    /* Copy the value of Index pointer that points
     * to the current location from where the input samples to be read */
    rOffset = *readOffset;

    dst_end = (int32_t) (dst_base + dst_length);

    /* Loop over the blockSize */
    i = blockSize;

    while(i > 0u)
      {
    /* copy the sample from the circular buffer to the destination buffer */
    *dst = circBuffer[rOffset];

    /* Update the input pointer */
    dst += dstInc;

    if(dst == (q7_t *) dst_end)
      {
        dst = dst_base;
      }

    /* Circularly update rOffset.  Watch out for positive and negative value */
    rOffset += bufferInc;

    if(rOffset >= L)
      {
        rOffset -= L;
      }

    /* Decrement the loop counter */
    i--;
      }

    /* Update the index pointer */
    *readOffset = rOffset;
  }


  void arm_power_q31(
              q31_t * pSrc,
             uint32_t blockSize,
             q63_t * pResult);

  void arm_power_f32(
              float32_t * pSrc,
             uint32_t blockSize,
             float32_t * pResult);

  void arm_power_q15(
              q15_t * pSrc,
             uint32_t blockSize,
             q63_t * pResult);

  void arm_power_q7(
             q7_t * pSrc,
            uint32_t blockSize,
            q31_t * pResult);

  void arm_mean_q7(
            q7_t * pSrc,
           uint32_t blockSize,
           q7_t * pResult);

  void arm_mean_q15(
             q15_t * pSrc,
            uint32_t blockSize,
            q15_t * pResult);

  void arm_mean_q31(
             q31_t * pSrc,
            uint32_t blockSize,
            q31_t * pResult);

  void arm_mean_f32(
             float32_t * pSrc,
            uint32_t blockSize,
            float32_t * pResult);

  void arm_var_f32(
            float32_t * pSrc,
           uint32_t blockSize,
           float32_t * pResult);

  void arm_var_q31(
            q31_t * pSrc,
           uint32_t blockSize,
           q63_t * pResult);

  void arm_var_q15(
            q15_t * pSrc,
           uint32_t blockSize,
           q31_t * pResult);

  void arm_rms_f32(
            float32_t * pSrc,
           uint32_t blockSize,
           float32_t * pResult);

  void arm_rms_q31(
            q31_t * pSrc,
           uint32_t blockSize,
           q31_t * pResult);

  void arm_rms_q15(
            q15_t * pSrc,
           uint32_t blockSize,
           q15_t * pResult);

  void arm_std_f32(
            float32_t * pSrc,
           uint32_t blockSize,
           float32_t * pResult);

  void arm_std_q31(
            q31_t * pSrc,
           uint32_t blockSize,
           q31_t * pResult);

  void arm_std_q15(
            q15_t * pSrc,
           uint32_t blockSize,
           q15_t * pResult);

  void arm_cmplx_mag_f32(
              float32_t * pSrc,
             float32_t * pDst,
             uint32_t numSamples);

  void arm_cmplx_mag_q31(
              q31_t * pSrc,
             q31_t * pDst,
             uint32_t numSamples);

  void arm_cmplx_mag_q15(
              q15_t * pSrc,
             q15_t * pDst,
             uint32_t numSamples);

  void arm_cmplx_dot_prod_q15(
                   q15_t * pSrcA,
                   q15_t * pSrcB,
                  uint32_t numSamples,
                  q31_t * realResult,
                  q31_t * imagResult);

  void arm_cmplx_dot_prod_q31(
                   q31_t * pSrcA,
                   q31_t * pSrcB,
                  uint32_t numSamples,
                  q63_t * realResult,
                  q63_t * imagResult);

  void arm_cmplx_dot_prod_f32(
                   float32_t * pSrcA,
                   float32_t * pSrcB,
                  uint32_t numSamples,
                  float32_t * realResult,
                  float32_t * imagResult);

  void arm_cmplx_mult_real_q15(
                    q15_t * pSrcCmplx,
                    q15_t * pSrcReal,
                   q15_t * pCmplxDst,
                   uint32_t numSamples);

  void arm_cmplx_mult_real_q31(
                    q31_t * pSrcCmplx,
                    q31_t * pSrcReal,
                   q31_t * pCmplxDst,
                   uint32_t numSamples);

  void arm_cmplx_mult_real_f32(
                    float32_t * pSrcCmplx,
                    float32_t * pSrcReal,
                   float32_t * pCmplxDst,
                   uint32_t numSamples);

  void arm_min_q7(
           q7_t * pSrc,
          uint32_t blockSize,
          q7_t * result,
          uint32_t * index);

  void arm_min_q15(
            q15_t * pSrc,
           uint32_t blockSize,
           q15_t * pResult,
           uint32_t * pIndex);

  void arm_min_q31(
            q31_t * pSrc,
           uint32_t blockSize,
           q31_t * pResult,
           uint32_t * pIndex);

  void arm_min_f32(
            float32_t * pSrc,
           uint32_t blockSize,
           float32_t * pResult,
           uint32_t * pIndex);

  void arm_max_q7(
           q7_t * pSrc,
          uint32_t blockSize,
          q7_t * pResult,
          uint32_t * pIndex);

  void arm_max_q15(
            q15_t * pSrc,
           uint32_t blockSize,
           q15_t * pResult,
           uint32_t * pIndex);

  void arm_max_q31(
            q31_t * pSrc,
           uint32_t blockSize,
           q31_t * pResult,
           uint32_t * pIndex);

  void arm_max_f32(
            float32_t * pSrc,
           uint32_t blockSize,
           float32_t * pResult,
           uint32_t * pIndex);

  void arm_cmplx_mult_cmplx_q15(
                    q15_t * pSrcA,
                    q15_t * pSrcB,
                   q15_t * pDst,
                   uint32_t numSamples);

  void arm_cmplx_mult_cmplx_q31(
                    q31_t * pSrcA,
                    q31_t * pSrcB,
                   q31_t * pDst,
                   uint32_t numSamples);

  void arm_cmplx_mult_cmplx_f32(
                    float32_t * pSrcA,
                    float32_t * pSrcB,
                   float32_t * pDst,
                   uint32_t numSamples);

  void arm_float_to_q31(
                   float32_t * pSrc,
                  q31_t * pDst,
                  uint32_t blockSize);

  void arm_float_to_q15(
                   float32_t * pSrc,
                  q15_t * pDst,
                  uint32_t blockSize);

  void arm_float_to_q7(
                  float32_t * pSrc,
                 q7_t * pDst,
                 uint32_t blockSize);


  void arm_q31_to_q15(
               q31_t * pSrc,
              q15_t * pDst,
              uint32_t blockSize);

  void arm_q31_to_q7(
              q31_t * pSrc,
             q7_t * pDst,
             uint32_t blockSize);

  void arm_q15_to_float(
             q15_t * pSrc,
            float32_t * pDst,
            uint32_t blockSize);


  void arm_q15_to_q31(
               q15_t * pSrc,
              q31_t * pDst,
              uint32_t blockSize);


  void arm_q15_to_q7(
              q15_t * pSrc,
             q7_t * pDst,
             uint32_t blockSize);


  static __INLINE float32_t arm_bilinear_interp_f32(
                            const arm_bilinear_interp_instance_f32 * S,
                            float32_t X,
                            float32_t Y)
  {
    float32_t out;
    float32_t f00, f01, f10, f11;
    float32_t *pData = S->pData;
    int32_t xIndex, yIndex, index;
    float32_t xdiff, ydiff;
    float32_t b1, b2, b3, b4;

    xIndex = (int32_t) X;
    yIndex = (int32_t) Y;

    /* Care taken for table outside boundary */
    /* Returns zero output when values are outside table boundary */
    if(xIndex < 0 || xIndex > (S->numRows-1) || yIndex < 0  || yIndex > ( S->numCols-1))
    {
        return(0);
    }
    
    /* Calculation of index for two nearest points in X-direction */
    index = (xIndex - 1) + (yIndex-1) *  S->numCols ;


    /* Read two nearest points in X-direction */
    f00 = pData[index];
    f01 = pData[index + 1];

    /* Calculation of index for two nearest points in Y-direction */
    index = (xIndex-1) + (yIndex) * S->numCols;


    /* Read two nearest points in Y-direction */
    f10 = pData[index];
    f11 = pData[index + 1];

    /* Calculation of intermediate values */
    b1 = f00;
    b2 = f01 - f00;
    b3 = f10 - f00;
    b4 = f00 - f01 - f10 + f11;

    /* Calculation of fractional part in X */
    xdiff = X - xIndex;

    /* Calculation of fractional part in Y */
    ydiff = Y - yIndex;

    /* Calculation of bi-linear interpolated output */
     out = b1 + b2 * xdiff + b3 * ydiff + b4 * xdiff * ydiff;

   /* return to application */
    return (out);

  }

  static __INLINE q31_t arm_bilinear_interp_q31(
                        arm_bilinear_interp_instance_q31 * S,
                        q31_t X,
                        q31_t Y)
  {
    q31_t out;                                   /* Temporary output */
    q31_t acc = 0;                               /* output */
    q31_t xfract, yfract;                        /* X, Y fractional parts */
    q31_t x1, x2, y1, y2;                        /* Nearest output values */
    int32_t rI, cI;                             /* Row and column indices */
    q31_t *pYData = S->pData;                    /* pointer to output table values */
    uint32_t nCols = S->numCols;                 /* num of rows */


    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    rI = ((X & 0xFFF00000) >> 20u);

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    cI = ((Y & 0xFFF00000) >> 20u);

    /* Care taken for table outside boundary */
    /* Returns zero output when values are outside table boundary */
    if(rI < 0 || rI > (S->numRows-1) || cI < 0  || cI > ( S->numCols-1))
    {
        return(0);
    }

    /* 20 bits for the fractional part */
    /* shift left xfract by 11 to keep 1.31 format */
    xfract = (X & 0x000FFFFF) << 11u;

    /* Read two nearest output values from the index */
    x1 = pYData[(rI) + nCols * (cI)];
    x2 = pYData[(rI) + nCols * (cI) + 1u];

    /* 20 bits for the fractional part */
    /* shift left yfract by 11 to keep 1.31 format */
    yfract = (Y & 0x000FFFFF) << 11u;

    /* Read two nearest output values from the index */
    y1 = pYData[(rI) + nCols * (cI + 1)];
    y2 = pYData[(rI) + nCols * (cI + 1) + 1u];

    /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 3.29(q29) format */
    out = ((q31_t) (((q63_t) x1 * (0x7FFFFFFF - xfract)) >> 32));
    acc = ((q31_t) (((q63_t) out * (0x7FFFFFFF - yfract)) >> 32));

    /* x2 * (xfract) * (1-yfract)  in 3.29(q29) and adding to acc */
    out = ((q31_t) ((q63_t) x2 * (0x7FFFFFFF - yfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (xfract) >> 32));

    /* y1 * (1 - xfract) * (yfract)  in 3.29(q29) and adding to acc */
    out = ((q31_t) ((q63_t) y1 * (0x7FFFFFFF - xfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (yfract) >> 32));

    /* y2 * (xfract) * (yfract)  in 3.29(q29) and adding to acc */
    out = ((q31_t) ((q63_t) y2 * (xfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (yfract) >> 32));

    /* Convert acc to 1.31(q31) format */
    return (acc << 2u);

  }

  static __INLINE q15_t arm_bilinear_interp_q15(
                        arm_bilinear_interp_instance_q15 * S,
                        q31_t X,
                        q31_t Y)
  {
    q63_t acc = 0;                               /* output */
    q31_t out;                                   /* Temporary output */
    q15_t x1, x2, y1, y2;                        /* Nearest output values */
    q31_t xfract, yfract;                        /* X, Y fractional parts */
    int32_t rI, cI;                             /* Row and column indices */
    q15_t *pYData = S->pData;                    /* pointer to output table values */
    uint32_t nCols = S->numCols;                 /* num of rows */

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    rI = ((X & 0xFFF00000) >> 20);

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    cI = ((Y & 0xFFF00000) >> 20);

    /* Care taken for table outside boundary */
    /* Returns zero output when values are outside table boundary */
    if(rI < 0 || rI > (S->numRows-1) || cI < 0  || cI > ( S->numCols-1))
    {
        return(0);
    }

    /* 20 bits for the fractional part */
    /* xfract should be in 12.20 format */
    xfract = (X & 0x000FFFFF);

    /* Read two nearest output values from the index */
    x1 = pYData[(rI) + nCols * (cI)];
    x2 = pYData[(rI) + nCols * (cI) + 1u];


    /* 20 bits for the fractional part */
    /* yfract should be in 12.20 format */
    yfract = (Y & 0x000FFFFF);

    /* Read two nearest output values from the index */
    y1 = pYData[(rI) + nCols * (cI + 1)];
    y2 = pYData[(rI) + nCols * (cI + 1) + 1u];

    /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 13.51 format */

    /* x1 is in 1.15(q15), xfract in 12.20 format and out is in 13.35 format */
    /* convert 13.35 to 13.31 by right shifting  and out is in 1.31 */
    out = (q31_t) (((q63_t) x1 * (0xFFFFF - xfract)) >> 4u);
    acc = ((q63_t) out * (0xFFFFF - yfract));

    /* x2 * (xfract) * (1-yfract)  in 1.51 and adding to acc */
    out = (q31_t) (((q63_t) x2 * (0xFFFFF - yfract)) >> 4u);
    acc += ((q63_t) out * (xfract));

    /* y1 * (1 - xfract) * (yfract)  in 1.51 and adding to acc */
    out = (q31_t) (((q63_t) y1 * (0xFFFFF - xfract)) >> 4u);
    acc += ((q63_t) out * (yfract));

    /* y2 * (xfract) * (yfract)  in 1.51 and adding to acc */
    out = (q31_t) (((q63_t) y2 * (xfract)) >> 4u);
    acc += ((q63_t) out * (yfract));

    /* acc is in 13.51 format and down shift acc by 36 times */
    /* Convert out to 1.15 format */
    return (acc >> 36);

  }

  static __INLINE q7_t arm_bilinear_interp_q7(
                          arm_bilinear_interp_instance_q7 * S,
                          q31_t X,
                          q31_t Y)
  {
    q63_t acc = 0;                               /* output */
    q31_t out;                                   /* Temporary output */
    q31_t xfract, yfract;                        /* X, Y fractional parts */
    q7_t x1, x2, y1, y2;                         /* Nearest output values */
    int32_t rI, cI;                             /* Row and column indices */
    q7_t *pYData = S->pData;                     /* pointer to output table values */
    uint32_t nCols = S->numCols;                 /* num of rows */

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    rI = ((X & 0xFFF00000) >> 20);

    /* Input is in 12.20 format */
    /* 12 bits for the table index */
    /* Index value calculation */
    cI = ((Y & 0xFFF00000) >> 20);

    /* Care taken for table outside boundary */
    /* Returns zero output when values are outside table boundary */
    if(rI < 0 || rI > (S->numRows-1) || cI < 0  || cI > ( S->numCols-1))
    {
        return(0);
    }

    /* 20 bits for the fractional part */
    /* xfract should be in 12.20 format */
    xfract = (X & 0x000FFFFF);

    /* Read two nearest output values from the index */
    x1 = pYData[(rI) + nCols * (cI)];
    x2 = pYData[(rI) + nCols * (cI) + 1u];


    /* 20 bits for the fractional part */
    /* yfract should be in 12.20 format */
    yfract = (Y & 0x000FFFFF);

    /* Read two nearest output values from the index */
    y1 = pYData[(rI) + nCols * (cI + 1)];
    y2 = pYData[(rI) + nCols * (cI + 1) + 1u];

    /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 16.47 format */
    out = ((x1 * (0xFFFFF - xfract)));
    acc = (((q63_t) out * (0xFFFFF - yfract)));

    /* x2 * (xfract) * (1-yfract)  in 2.22 and adding to acc */
    out = ((x2 * (0xFFFFF - yfract)));
    acc += (((q63_t) out * (xfract)));

    /* y1 * (1 - xfract) * (yfract)  in 2.22 and adding to acc */
    out = ((y1 * (0xFFFFF - xfract)));
    acc += (((q63_t) out * (yfract)));

    /* y2 * (xfract) * (yfract)  in 2.22 and adding to acc */
    out = ((y2 * (yfract)));
    acc += (((q63_t) out * (xfract)));

    /* acc in 16.47 format and down shift by 40 to convert to 1.7 format */
    return (acc >> 40);

  }

#ifdef  __cplusplus
}
#endif


#endif /* _ARM_MATH_H */

