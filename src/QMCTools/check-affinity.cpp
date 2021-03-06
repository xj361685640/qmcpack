//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2016 Jeongnim Kim and QMCPACK developers.
//
// File developed by: Ye Luo, yeluo@anl.gov, Argonne National Laboratory
//
// File created by: Ye Luo, yeluo@anl.gov, Argonne National Laboratory
//////////////////////////////////////////////////////////////////////////////////////

#ifdef __bgq__
#include </bgsys/drivers/ppcfloor/spi/include/kernel/location.h>
#elif __linux__
#include <sched.h>
#endif
#include <omp.h>
#include <iostream>
#include <sstream>

/*=======================================*/
/* routine to return the core ID         */
/*=======================================*/
int get_core()
{
#ifdef __bgq__
    int core = Kernel_ProcessorCoreID();
    return core;
#elif __linux__
    int cpuid = sched_getcpu();
    return cpuid;
#else
    return -1;
#endif
}

/*=======================================*/
/* routine to return the HW thread ID    */
/*=======================================*/
int get_hwthread()
{
#ifdef __bgq__
    int hwthread = Kernel_ProcessorThreadID();
    return hwthread;
#else
    return -1;
#endif
}

int main()
{
  std::cout << "Note: -1 means the property is not accessible on this platform." << std::endl << std::endl;
  #pragma omp parallel
  {
    int L1_thread_id  = omp_get_thread_num();
    int L1_num_thread = omp_get_num_threads();
    #pragma omp parallel
    {
      int L2_thread_id  = omp_get_thread_num();
      int L2_num_thread = omp_get_num_threads();

      std::ostringstream o;
      o << "Core ID " << get_core() << " HW thread ID " << get_hwthread()
        << " L1 has " << L1_num_thread << " threads and tid = " << L1_thread_id
        << " L2 has " << L2_num_thread << " threads and tid = " << L2_thread_id << std::endl;
      std::cout << o.str();
    }
  }
  return 0;
}
