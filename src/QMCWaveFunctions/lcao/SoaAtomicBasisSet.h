//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2016 Jeongnim Kim and QMCPACK developers.
//
// File developed by: 
//
// File created by: Jeongnim Kim, jeongnim.kim@intel.com, Intel Corp.
//////////////////////////////////////////////////////////////////////////////////////
    
/** @file SoaAtomicBasisSet.h
 */
#ifndef QMCPLUSPLUS_SOA_SPHERICALORBITAL_BASISSET_H
#define QMCPLUSPLUS_SOA_SPHERICALORBITAL_BASISSET_H

namespace qmcplusplus
{
  /* A basis set for a center type 
   *
   * @tparam ROT : radial function type, e.g.,NGFunctor<T>
   * @tparam SH : spherical or carteisan Harmonics for (l,m) expansion
   *
   * \f$ \phi_{n,l,m}({\bf r})=R_{n,l}(r) Y_{l,m}(\theta) \f$
   */
  template<typename ROT, typename SH>
    struct SoaAtomicBasisSet
    {
      typedef ROT RadialOrbital_t;
      typedef typename ROT::value_type value_type;
      typedef typename ROT::grid_type  grid_type;

      ///size of the basis set
      int BasisSetSize;
      ///maximum radius of this center
      value_type Rmax;
      ///spherical harmonics
      SH Ylm;
      ///radial orbitals
      ROT* MultiRnl;
      ///index of the corresponding real Spherical Harmonic with quantum numbers \f$ (l,m) \f$
      aligned_vector<int> LM;
      /**index of the corresponding radial orbital with quantum numbers \f$ (n,l) \f$ */
      aligned_vector<int> NL;
      ///container for the quantum-numbers
      std::vector<QuantumNumberType> RnlID;
      ///temporary storage 
      VectorSoaContainer<value_type,4> tempS;

      ///set of grids : keep this until completion
      std::vector<grid_type*> Grids;
      ///the constructor
      explicit SoaAtomicBasisSet(int lmax, bool addsignforM=false)
        :Ylm(lmax,addsignforM){}

      SoaAtomicBasisSet(const SoaAtomicBasisSet& in)=default;

      ~SoaAtomicBasisSet(){ } //cleanup

      SoaAtomicBasisSet<ROT,SH>* makeClone() const
      {
        SoaAtomicBasisSet<ROT,SH>* myclone=new SoaAtomicBasisSet<ROT,SH>(*this);
        myclone->MultiRnl=MultiRnl->makeClone();
        return myclone;
      }

      void checkInVariables(opt_variables_type& active)
      {
        //for(size_t nl=0; nl<Rnl.size(); nl++)
        //  Rnl[nl]->checkInVariables(active);
      }

      void checkOutVariables(const opt_variables_type& active)
      {
        //for(size_t nl=0; nl<Rnl.size(); nl++)
        //  Rnl[nl]->checkOutVariables(active);
      }

      void resetParameters(const opt_variables_type& active)
      {
        //for(size_t nl=0; nl<Rnl.size(); nl++)
        //  Rnl[nl]->resetParameters(active);
      }

      /** return the number of basis functions
      */
      inline int getBasisSetSize() const
      {
        return BasisSetSize;
      }//=NL.size(); 


      /** implement a BasisSetBase virutal function
       *
       * Set Rmax and BasisSetSize
       * @todo Should be able to overwrite Rmax to be much smaller than the maximum grid
       */
      inline void setBasisSetSize(int n)
      {
        BasisSetSize=LM.size();
        tempS.resize(std::max(Ylm.size(),RnlID.size()));
      }

      /** Set Rmax */
      template<typename T>
      inline void setRmax(T rmax)
      {
        Rmax=(rmax>0)? rmax: MultiRnl->rmax();
      }

      /** reset the target ParticleSet
       *
       * Do nothing. Leave it to a composite object which owns this
       */
      void resetTargetParticleSet(ParticleSet& P) { }

      ///set the current offset
      inline void setCenter(int c, int offset)
      { }



 template<typename LAT, typename T, typename PosType, typename VGL>
        inline void
        evaluateVGL(const LAT& lattice, const T r, const PosType& dr, const size_t offset,  VGL& vgl)
        {

          
          PosType dr_new;
          T r_new;

          CONSTEXPR T cone(1);
          CONSTEXPR T ctwo(2);

          //one can assert the alignment
          value_type* restrict phi=tempS.data(0);
          value_type* restrict dphi=tempS.data(1);
          value_type* restrict d2phi=tempS.data(2);

          //V,Gx,Gy,Gz,L
          T* restrict psi   =vgl.data(0)+offset; const T* restrict ylm_v=Ylm[0]; //value
          T* restrict dpsi_x=vgl.data(1)+offset; const T* restrict ylm_x=Ylm[1]; //gradX
          T* restrict dpsi_y=vgl.data(2)+offset; const T* restrict ylm_y=Ylm[2]; //gradY
          T* restrict dpsi_z=vgl.data(3)+offset; const T* restrict ylm_z=Ylm[3]; //gradZ
          T* restrict d2psi =vgl.data(4)+offset; const T* restrict ylm_l=Ylm[4]; //lap

          for(size_t ib=0; ib<BasisSetSize; ++ib)
          {
               psi[ib]   =0;
               dpsi_x[ib]=0; 
               dpsi_y[ib]=0; 
               dpsi_z[ib]=0; 
               d2psi[ib] =0; 
  
          }

          for (int TransX=-1; TransX<=1; TransX++ ) //loop Translation over X 
              for (int TransY=-1; TransY<=1; TransY++ ) //loop Translation over Y
                 for (int TransZ=-1; TransZ<=1; TransZ++ ) //loop Translation over Z
          {

                    dr_new[0]=dr[0]+TransX*lattice.R(0,0)+TransY*lattice.R(0,1)+TransZ*lattice.R(0,2); // Trans is translation vector
                    dr_new[1]=dr[1]+TransX*lattice.R(1,0)+TransY*lattice.R(1,1)+TransZ*lattice.R(1,2); // Trans is translation vector
                    dr_new[2]=dr[2]+TransX*lattice.R(2,0)+TransY*lattice.R(2,1)+TransZ*lattice.R(2,2); // Trans is translation vector
                    r_new=std::sqrt(dot(dr_new,dr_new));
         


          //const size_t ib_max=NL.size();
          if(r_new>Rmax) continue; 

          //SIGN Change!!
          const T x=-dr_new[0], y=-dr_new[1], z=-dr_new[2];
          Ylm.evaluateVGL(x,y,z);



          MultiRnl->evaluate(r_new,phi,dphi,d2phi);


          const T rinv=cone/r_new;

          for(size_t ib=0; ib<BasisSetSize; ++ib)
          {
            const int nl(NL[ib]);
            const int lm(LM[ib]);
            const T drnloverr=rinv*dphi[nl];
            const T ang=ylm_v[lm];
            const T gr_x=drnloverr*x;
            const T gr_y=drnloverr*y;
            const T gr_z=drnloverr*z;
            const T ang_x=ylm_x[lm];
            const T ang_y=ylm_y[lm];
            const T ang_z=ylm_z[lm];
            const T vr=phi[nl];
            psi[ib]    += ang*vr;
            dpsi_x[ib] += ang*gr_x+vr*ang_x;
            dpsi_y[ib] += ang*gr_y+vr*ang_y;
            dpsi_z[ib] += ang*gr_z+vr*ang_z;
            d2psi[ib]  += ang*(ctwo*drnloverr+d2phi[nl]) + ctwo*(gr_x*ang_x+gr_y*ang_y+gr_z*ang_z)+vr*ylm_l[lm];
          }
          }
        }

      template<typename LAT, typename T, typename PosType>
      inline void
        evaluateV(const LAT& lattice, const T r, const PosType& dr, T* restrict psi) 
        {
         

          PosType dr_new;
          T r_new;
          T psi_new;
          value_type* restrict ylm_v=tempS.data(0);
          value_type* restrict phi_r=tempS.data(1);
          for(size_t ib=0; ib<BasisSetSize; ++ib)
              psi[ib]=0;

          for (int TransX=-1; TransX<=1; TransX++ ) //loop Translation over X 
             for (int TransY=-1; TransY<=1; TransY++ ) //loop Translation over Y
                for (int TransZ=-1; TransZ<=1; TransZ++ ) //loop Translation over Z
                {
                  dr_new[0]=dr[0]+TransX*lattice.R(0,0)+TransY*lattice.R(0,1)+TransZ*lattice.R(0,2); // Trans is translation vector
                  dr_new[1]=dr[1]+TransX*lattice.R(1,0)+TransY*lattice.R(1,1)+TransZ*lattice.R(1,2); // Trans is translation vector
                  dr_new[2]=dr[2]+TransX*lattice.R(2,0)+TransY*lattice.R(2,1)+TransZ*lattice.R(2,2); // Trans is translation vector
                  r_new=std::sqrt(dot(dr_new,dr_new));
              
                  if(r_new>Rmax)   continue;

                  Ylm.evaluateV(-dr_new[0],-dr_new[1],-dr_new[2],ylm_v);
                  MultiRnl->evaluate(r_new,phi_r);
         
                  for(size_t ib=0; ib<BasisSetSize; ++ib)
                  {
                   
                     psi_new= ylm_v[ LM[ib] ]*phi_r[ NL[ib] ];
                     psi[ib]  += psi_new;
                  }
               }
          
        }
    };

}
#endif
