/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Header file containing RAJA index set iteration template methods 
 *          using for Intel Cilk Plus execution.
 *
 *          These methods work only on platforms that support Cilk Plus. 
 *
 * \author  Rich Hornung, Center for Applied Scientific Computing, LLNL
 * \author  Jeff Keasler, Applications, Simulations And Quality, LLNL 
 *
 ******************************************************************************
 */

#ifndef RAJA_forall_cilk_icc_HXX
#define RAJA_forall_cilk_icc_HXX

#include "config.hxx"

#include "datatypes.hxx"

#include "execpolicy.hxx"

#include "ISet.hxx"

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_max.h>


namespace RAJA {

//
//////////////////////////////////////////////////////////////////////
//
// Function templates that iterate over range index sets.
//
//////////////////////////////////////////////////////////////////////
//

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over index range.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            Index_type begin, Index_type end, 
            LOOP_BODY loop_body)
{
#pragma ivdep
   cilk_for ( Index_type ii = begin ; ii < end ; ++ii ) {
      loop_body( ii );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  iteration over index range set object.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            const RangeISet& is,
            LOOP_BODY loop_body)
{
   const Index_type begin = is.getBegin();
   const Index_type end   = is.getEnd();
#pragma ivdep
   cilk_for ( Index_type ii = begin ; ii < end ; ++ii ) {
      loop_body( ii );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  minloc reduction over index range.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   Index_type begin, Index_type end,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  min_tmp = *min;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models 

   cilk::reducer_min_index<Index_type, T> reduction;

   cilk_for ( Index_type ii = begin ; ii < end ; ++ii ) {
      loop_body( ii, &min_tmp, &loc_tmp );
      reduction.calc_min( ii, min_tmp );
   }

   if ( reduction.get_value() < *min ) {
      *min = reduction.get_value() ;
      *loc = reduction.get_index() ;
   } 
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  minloc reduction over range index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   const RangeISet& is,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_minloc(cilk_for_exec(),
                 is.getBegin(), is.getEnd(),
                 min, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  maxloc reduction over index range.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   Index_type begin, Index_type end,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  max_tmp = *max;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models

   cilk::reducer_max_index<Index_type, T> reduction;

   cilk_for ( Index_type ii = begin ; ii < end ; ++ii ) {
      loop_body( ii, &max_tmp, &loc_tmp );
      reduction.calc_max( ii, max_tmp );
   }

   if ( reduction.get_value() > *max ) {
      *max = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  maxloc reduction over range index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   const RangeISet& is,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_maxloc(cilk_for_exec(),
                 is.getBegin(), is.getEnd(),
                 max, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over index range.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                Index_type begin, Index_type end,
                T* sum,
                LOOP_BODY loop_body)
{
   const int nthreads = __cilkrts_get_nworkers();

   /* Should we align these temps to coherence boundaries? */
   T  sum_tmp[nthreads];

   for ( int i = 0; i < nthreads; ++i ) {
      sum_tmp[i] = 0 ;
   }

   cilk_for ( Index_type ii = begin ; ii < end ; ++ii ) {
      loop_body( ii, &sum_tmp[__cilkrts_get_worker_number()] );
   }

   for ( int i = 0; i < nthreads; ++i ) {
      *sum += sum_tmp[i];
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over range index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                const RangeISet& is,
                T* sum,
                LOOP_BODY loop_body)
{
   forall_sum(cilk_for_exec(),
              is.getBegin(), is.getEnd(),
              sum,
              loop_body);
}


//
//////////////////////////////////////////////////////////////////////
//
// Function templates that iterate over range index sets with stride.
//
//////////////////////////////////////////////////////////////////////
//

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over index range with stride.
 *         
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            Index_type begin, Index_type end, Index_type stride,
            LOOP_BODY loop_body)
{                    
#pragma ivdep
   cilk_for ( Index_type ii = begin ; ii < end ; ii += stride ) {
      loop_body( ii );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over range index set with stride object.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            const RangeStrideISet& is,
            LOOP_BODY loop_body)
{
   const Index_type begin  = is.getBegin();
   const Index_type end    = is.getEnd();
   const Index_type stride = is.getStride();
#pragma ivdep
   cilk_for ( Index_type ii = begin ; ii < end ; ii += stride ) {
      loop_body( ii );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  minloc reduction over index range with stride.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   Index_type begin, Index_type end, Index_type stride,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  min_tmp = *min;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models 

   cilk::reducer_min_index<Index_type, T> reduction;

   cilk_for ( Index_type ii = begin ; ii < end ; ii += stride ) {
      loop_body( ii, &min_tmp, &loc_tmp );
      reduction.calc_min( ii, min_tmp );
   }

   if ( reduction.get_value() < *min ) {
      *min = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for minloc reduction over range index set with stride object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   const RangeStrideISet& is,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_minloc(cilk_for_exec(),
                 is.getBegin(), is.getEnd(), is.getStride(),
                 min, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  maxloc reduction over index range with stride.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   Index_type begin, Index_type end, Index_type stride,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  max_tmp = *max;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models

   cilk::reducer_max_index<Index_type, T> reduction;

   cilk_for ( Index_type ii = begin ; ii < end ; ii += stride ) {
      loop_body( ii, &max_tmp, &loc_tmp );
      reduction.calc_max( ii, max_tmp );
   }

   if ( reduction.get_value() > *max ) {
      *max = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for maxloc reduction over range index set with stride object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   const RangeStrideISet& is,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_maxloc(cilk_for_exec(),
                 is.getBegin(), is.getEnd(), is.getStride(),
                 max, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over index range with stride.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                Index_type begin, Index_type end, Index_type stride,
                T* sum,
                LOOP_BODY loop_body)
{
   const int nthreads = __cilkrts_get_nworkers();

   /* Should we align these temps to coherence boundaries? */
   T  sum_tmp[nthreads];

   for ( int i = 0; i < nthreads; ++i ) {
      sum_tmp[i] = 0 ;
   }

   cilk_for ( Index_type ii = begin ; ii < end ; ii += stride ) {
      loop_body( ii, &sum_tmp[__cilkrts_get_worker_number()] );
   }

   for ( int i = 0; i < nthreads; ++i ) {
      *sum += sum_tmp[i];
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over range index set with stride object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                const RangeStrideISet& is,
                T* sum,
                LOOP_BODY loop_body)
{
   forall_sum(cilk_for_exec(),
              is.getBegin(), is.getEnd(), is.getStride(),
              sum,
              loop_body);
}



//
//////////////////////////////////////////////////////////////////////
//
// Function templates that iterate over unstructured index sets.
//
//////////////////////////////////////////////////////////////////////
//

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over indices in indirection array.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            const Index_type* __restrict__ idx, const Index_type len,
            LOOP_BODY loop_body)
{
#pragma ivdep
   cilk_for ( Index_type k = 0 ; k < len ; ++k ) {
      loop_body( idx[k] );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over unstructured index set object.
 *
 ******************************************************************************
 */
template <typename LOOP_BODY>
RAJA_INLINE
void forall(cilk_for_exec,
            const UnstructuredISet& is,
            LOOP_BODY loop_body)
{
   const Index_type* __restrict__ idx = is.getIndex();
   const Index_type len = is.getLength();
#pragma ivdep
   cilk_for ( Index_type k = 0 ; k < len ; ++k ) {
      loop_body( idx[k] );
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  minloc reduction over given indirection array.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   const Index_type* __restrict__ idx, const Index_type len,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  min_tmp = *min;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models 

   cilk::reducer_min_index<Index_type, T> reduction;

   cilk_for ( Index_type k = 0 ; k < len ; ++k ) {
      loop_body( idx[k], &min_tmp, &loc_tmp );
      reduction.calc_min( idx[k], min_tmp );
   }

   if ( reduction.get_value() < *min ) {
      *min = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  minloc reduction over unstructured index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc(cilk_for_exec,
                   const UnstructuredISet& is,
                   T* min, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_minloc(cilk_for_exec(),
                 is.getIndex(), is.getLength(),
                 min, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  maxloc reduction over given indirection array.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   const Index_type* __restrict__ idx, const Index_type len,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   T  max_tmp = *max;
   Index_type loc_tmp = *loc;  // Not used; but here for consistency with
                               // other models

   cilk::reducer_max_index<Index_type, T> reduction;

   cilk_for ( Index_type k = 0 ; k < len ; ++k ) {
      loop_body( idx[k], &max_tmp, &loc_tmp );
      reduction.calc_max( idx[k], max_tmp );
   }

   if ( reduction.get_value() > *max ) {
      *max = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  maxloc reduction over unstructured index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc(cilk_for_exec,
                   const UnstructuredISet& is,
                   T* max, Index_type *loc,
                   LOOP_BODY loop_body)
{
   forall_maxloc(cilk_for_exec(),
                 is.getIndex(), is.getLength(),
                 max, loc,
                 loop_body);
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over given indirection array.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                const Index_type* __restrict__ idx, const Index_type len,
                T* sum,
                LOOP_BODY loop_body)
{
   const int nthreads = __cilkrts_get_nworkers();

   /* Should we align these temps to coherence boundaries? */
   T  sum_tmp[nthreads];

   for ( int i = 0; i < nthreads; ++i ) {
      sum_tmp[i] = 0 ;
   }

   cilk_for ( Index_type k = 0 ; k < len ; ++k ) {
      loop_body( idx[k], &sum_tmp[__cilkrts_get_worker_number()] );
   }

   for ( int i = 0; i < nthreads; ++i ) {
      *sum += sum_tmp[i];
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for  sum reduction over unstructured index set object.
 *
 ******************************************************************************
 */
template <typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum(cilk_for_exec,
                const UnstructuredISet& is,
                T* sum,
                LOOP_BODY loop_body)
{
   forall_sum(cilk_for_exec(),
              is.getIndex(), is.getLength(),
              sum,
              loop_body);
}


//
//////////////////////////////////////////////////////////////////////
//
// The following function templates iterate over hybrid index set
// segments using cilk_for. Segment execution is defined by segment
// execution policy template parameter.
//
//////////////////////////////////////////////////////////////////////
//

/*!
 ******************************************************************************
 *
 * \brief  cilk_for iteration over segments of hybrid index set and 
 *         use execution policy template parameter to execute segments.
 *
 ******************************************************************************
 */
template <typename SEG_EXEC_POLICY_T,
          typename LOOP_BODY>
RAJA_INLINE
void forall( std::pair<cilk_for_segit, SEG_EXEC_POLICY_T>,
             const HybridISet& is, LOOP_BODY loop_body )
{
   const int num_seg = is.getNumSegments();
   cilk_for ( int isi = 0; isi < num_seg; ++isi ) {

      const HybridISet::Segment& seg = is.getSegment(isi);

      switch ( seg.m_type ) {

         case HybridISet::_Range_ : {
            forall(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeISet*>(seg.m_segment)),
               loop_body
            );
            break;
         }

#if 0  // RDH RETHINK
         case HybridISet::_RangeStride_ : {
            forall(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeStrideISet*>(seg.m_segment)),
               loop_body
            );
            break;
         }
#endif

         case HybridISet::_Unstructured_ : {
            forall(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const UnstructuredISet*>(seg.m_segment)),
               loop_body
            );
            break;
         }

         default : {
         }

      }  // switch on segment type

   } // iterate over parts of hybrid index set
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for minloc reduction over segments of hybrid index set and
 *         use execution policy template parameter to execute segments.
 *
 ******************************************************************************
 */
template <typename SEG_EXEC_POLICY_T,
          typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_minloc( std::pair<cilk_for_segit, SEG_EXEC_POLICY_T>,
                    const HybridISet& is,
                    T* min, Index_type *loc,
                    LOOP_BODY loop_body )
{
   T  min_tmp = *min;
   Index_type loc_tmp = *loc; 

   cilk::reducer_min_index<Index_type, T> reduction;

   const int num_seg = is.getNumSegments();
   cilk_for ( int isi = 0; isi < num_seg; ++isi ) {

      const HybridISet::Segment& seg = is.getSegment(isi);

      switch ( seg.m_type ) {

         case HybridISet::_Range_ : {
            forall_minloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeISet*>(seg.m_segment)),
               &min_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }

#if 0  // RDH RETHINK
         case HybridISet::_RangeStride_ : {
            forall_minloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeStrideISet*>(seg.m_segment)),
               &min_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }
#endif

         case HybridISet::_Unstructured_ : {
            forall_minloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const UnstructuredISet*>(seg.m_segment)),
               &min_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }

         default : {
         }

      }  // switch on segment type

      reduction.calc_min( loc_tmp, min_tmp );

   } // iterate over segments of hybrid index set

   if ( reduction.get_value() < *min ) {
      *min = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for maxloc  reduction over segments of hybrid index set and
 *         use execution policy template parameter to execute segments.
 *
 ******************************************************************************
 */
template <typename SEG_EXEC_POLICY_T,
          typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_maxloc( std::pair<cilk_for_segit, SEG_EXEC_POLICY_T>,
                    const HybridISet& is,
                    T* max, Index_type *loc,
                    LOOP_BODY loop_body )
{
   T  max_tmp = *max;
   Index_type loc_tmp = *loc; 

   cilk::reducer_max_index<Index_type, T> reduction;

   const int num_seg = is.getNumSegments();
   cilk_for ( int isi = 0; isi < num_seg; ++isi ) {

      const HybridISet::Segment& seg = is.getSegment(isi);

      switch ( seg.m_type ) {

         case HybridISet::_Range_ : {
            forall_maxloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeISet*>(seg.m_segment)),
               &max_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }

#if 0  // RDH RETHINK
         case HybridISet::_RangeStride_ : {
            forall_maxloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeStrideISet*>(seg.m_segment)),
               &max_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }
#endif

         case HybridISet::_Unstructured_ : {
            forall_maxloc(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const UnstructuredISet*>(seg.m_segment)),
               &max_tmp,
               &loc_tmp,
               loop_body
            );
            break;
         }

         default : {
         }

      }  // switch on segment type

      reduction.calc_max( loc_tmp, max_tmp );

   } // iterate over segments of hybrid index set

   if ( reduction.get_value() > *max ) {
      *max = reduction.get_value() ;
      *loc = reduction.get_index() ;
   }
}

/*!
 ******************************************************************************
 *
 * \brief  cilk_for sum  reduction over segments of hybrid index set and
 *         use execution policy template parameter to execute segments.
 *
 ******************************************************************************
 */
template <typename SEG_EXEC_POLICY_T,
          typename T,
          typename LOOP_BODY>
RAJA_INLINE
void forall_sum( std::pair<cilk_for_segit, SEG_EXEC_POLICY_T>,
                 const HybridISet& is,
                 T* sum,
                 LOOP_BODY loop_body )
{
   const int nthreads = __cilkrts_get_nworkers();

   /* Should we align these temps to coherence boundaries? */
   T  sum_tmp[nthreads];

   for ( int i = 0; i < nthreads; ++i ) {
      sum_tmp[i] = 0 ;
   }

   const int num_seg = is.getNumSegments();
   cilk_for ( int isi = 0; isi < num_seg; ++isi ) {

      const HybridISet::Segment& seg = is.getSegment(isi);

      switch ( seg.m_type ) {

         case HybridISet::_Range_ : {
            forall_sum(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeISet*>(seg.m_segment)),
               &sum_tmp[__cilkrts_get_worker_number()],
               loop_body
            );
            break;
         }

#if 0  // RDH RETHINK
         case HybridISet::_RangeStride_ : {
            forall_sum(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const RangeStrideISet*>(seg.m_segment)),
               &sum_tmp[__cilkrts_get_worker_number()],
               loop_body
            );
            break;
         }
#endif

         case HybridISet::_Unstructured_ : {
            forall_sum(
               SEG_EXEC_POLICY_T(),
               *(static_cast<const UnstructuredISet*>(seg.m_segment)),
               &sum_tmp[__cilkrts_get_worker_number()],
               loop_body
            );
            break;
         }

         default : {
         }

      }  // switch on segment type

   } // iterate over segments of hybrid index set

   for ( int i = 0; i < nthreads; ++i ) {
      *sum += sum_tmp[i];
   }
}


}  // closing brace for namespace statement

#endif  // closing endif for header file include guard

