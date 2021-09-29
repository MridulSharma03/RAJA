
/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Header file containing RAJA index set and segment iteration
 *          template methods for OpenMP.
 *
 *          These methods should work on any platform that supports OpenMP.
 *
 ******************************************************************************
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-21, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef RAJA_forall_param_openmp_HPP
#define RAJA_forall_param_openmp_HPP

#define RAJA_OMP_DECLARE_REDUCTION_COMBINE \
      _Pragma(" omp declare reduction( combine \
        : typename std::remove_reference<decltype(f_params)>::type \
        : RAJA::expt::ParamMultiplexer::combine< \
            typename std::decay<decltype(p)>::type \
          >(omp_out, omp_in) ) \
        initializer(omp_priv = omp_orig) ")

namespace RAJA
{

namespace policy
{
namespace omp
{
namespace expt
{

  namespace internal
  {
    //
    // omp for (Auto)
    //
    template <typename Iterable, typename Func, typename ForallParam>
    RAJA_INLINE void forall_impl(const RAJA::policy::omp::Auto& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, *(begin_it + i));
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(static)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize <= 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Static<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, begin_it[i]);
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(static, ChunkSize)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize > 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Static<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(static, ChunkSize) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, begin_it[i]);
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(dynamic)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize <= 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Dynamic<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(dynamic) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, *(begin_it + i));
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(dynamic, ChunkSize)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize > 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Dynamic<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(dynamic, ChunkSize) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, *(begin_it + i));
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(guided)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize <= 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Guided<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(guided) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, *(begin_it + i));
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(guided, ChunkSize)
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize > 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Guided<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(guided, ChunkSize) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        RAJA::expt::invoke_body(f_params, loop_body, *(begin_it + i));
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(runtime)
    //
    template <typename Iterable, typename Func, typename ForallParam>
    RAJA_INLINE void forall_impl(const ::RAJA::policy::omp::Runtime& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel for schedule(runtime) reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        loop_body(begin_it[i]);
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for nowait (Auto)
    //
    template <typename Iterable, typename Func, typename ForallParam>
    RAJA_INLINE void forall_impl_nowait(const ::RAJA::policy::omp::Auto& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel
      {
      #pragma omp for nowait reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        loop_body(begin_it[i]);
      }
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(static) nowait
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize <= 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl_nowait(const ::RAJA::policy::omp::Static<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel
      {
      #pragma omp for schedule(static) nowait reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        loop_body(begin_it[i]);
      }
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }

    //
    // omp for schedule(static, ChunkSize) nowait
    //
    template <typename Iterable, typename Func, int ChunkSize, typename ForallParam,
      typename std::enable_if<(ChunkSize > 0)>::type* = nullptr>
    RAJA_INLINE void forall_impl_nowait(const ::RAJA::policy::omp::Static<ChunkSize>& p,
                                 Iterable&& iter,
                                 Func&& loop_body,
                                 ForallParam&& f_params)
    {
      RAJA::expt::ParamMultiplexer::init<RAJA::policy::omp::Auto>(f_params);
      RAJA_OMP_DECLARE_REDUCTION_COMBINE;

      RAJA_EXTRACT_BED_IT(iter);
      #pragma omp parallel
      {
      #pragma omp for schedule(static, ChunkSize) nowait reduction(combine : f_params)
      for (decltype(distance_it) i = 0; i < distance_it; ++i) {
        loop_body(begin_it[i]);
      }
      }

      RAJA::expt::ParamMultiplexer::resolve<RAJA::policy::omp::Auto>(f_params);
    }
  } //  namespace internal

  template <typename Schedule, typename Iterable, typename Func, typename ForallParam>
  RAJA_INLINE resources::EventProxy<resources::Host> forall_impl(resources::Host host_res,
                                                                 const omp_for_schedule_exec<Schedule>&,
                                                                 Iterable&& iter,
                                                                 Func&& loop_body,
                                                                 ForallParam f_params)
  {
    expt::internal::forall_impl(Schedule{}, std::forward<Iterable>(iter), std::forward<Func>(loop_body), std::forward<ForallParam>(f_params));
    return resources::EventProxy<resources::Host>(host_res);
  }
} //  namespace expt

///
/// OpenMP parallel policy implementation
///
template <typename Iterable, typename Func, typename InnerPolicy, typename FirstP, typename... RestP>
RAJA_INLINE resources::EventProxy<resources::Host> forall_impl(resources::Host host_res,
                                                    const omp_parallel_exec<InnerPolicy>&,
                                                    Iterable&& iter,
                                                    Func&& loop_body,
                                                    RAJA::expt::ForallParamPack<FirstP, RestP...> f_params)
{
  std::cout << "param call\n";
  expt::forall_impl(host_res, InnerPolicy{}, iter, loop_body, f_params);
  return resources::EventProxy<resources::Host>(host_res);
}

}  // namespace omp

}  // namespace policy

}  // namespace RAJA

#endif  // closing endif for header file include guard
