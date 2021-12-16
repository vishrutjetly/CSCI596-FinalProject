




// DO NOT EDIT !
// This file is generated using the MantaFlow preprocessor (prep generate).




#line 1 "/home/vishrut/Study/USC/Sem-1/CSCI 596: Scientific Computing and Visualization/project/mlflip-mod/source/plugin/advection.cpp"
/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011-2015 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * Apache License, Version 2.0 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Plugins for advection
 *
 ******************************************************************************/

#include "vectorbase.h"
#include "grid.h"
#include "kernel.h"
#include <limits>

using namespace std;

namespace Manta { 


//! Semi-Lagrange interpolation kernel


template <class T>  struct SemiLagrange : public KernelBase { SemiLagrange(const FlagGrid& flags, const MACGrid& vel, Grid<T>& dst, const Grid<T>& src, Real dt, bool isLevelset, int orderSpace) :  KernelBase(&flags,1) ,flags(flags),vel(vel),dst(dst),src(src),dt(dt),isLevelset(isLevelset),orderSpace(orderSpace)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& vel, Grid<T>& dst, const Grid<T>& src, Real dt, bool isLevelset, int orderSpace )  {
	// traceback position
	Vec3 pos = Vec3(i+0.5f,j+0.5f,k+0.5f) - vel.getCentered(i,j,k) * dt;
	dst(i,j,k) = src.getInterpolatedHi(pos, orderSpace);
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return vel; } typedef MACGrid type1;inline Grid<T>& getArg2() { return dst; } typedef Grid<T> type2;inline const Grid<T>& getArg3() { return src; } typedef Grid<T> type3;inline Real& getArg4() { return dt; } typedef Real type4;inline bool& getArg5() { return isLevelset; } typedef bool type5;inline int& getArg6() { return orderSpace; } typedef int type6; void runMessage() { debMsg("Executing kernel SemiLagrange ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,src,dt,isLevelset,orderSpace);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,src,dt,isLevelset,orderSpace);  } }  } const FlagGrid& flags; const MACGrid& vel; Grid<T>& dst; const Grid<T>& src; Real dt; bool isLevelset; int orderSpace;   };
#line 27 "plugin/advection.cpp"



//! Semi-Lagrange interpolation kernel for MAC grids


 struct SemiLagrangeMAC : public KernelBase { SemiLagrangeMAC(const FlagGrid& flags, const MACGrid& vel, MACGrid& dst, const MACGrid& src, Real dt, int orderSpace) :  KernelBase(&flags,1) ,flags(flags),vel(vel),dst(dst),src(src),dt(dt),orderSpace(orderSpace)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& vel, MACGrid& dst, const MACGrid& src, Real dt, int orderSpace )  {
	// get currect velocity at MAC position
	// no need to shift xpos etc. as lookup field is also shifted
	Vec3 xpos = Vec3(i+0.5f,j+0.5f,k+0.5f) - vel.getAtMACX(i,j,k) * dt;
	Real vx = src.getInterpolatedComponentHi<0>(xpos, orderSpace);
	Vec3 ypos = Vec3(i+0.5f,j+0.5f,k+0.5f) - vel.getAtMACY(i,j,k) * dt;
	Real vy = src.getInterpolatedComponentHi<1>(ypos, orderSpace);
	Vec3 zpos = Vec3(i+0.5f,j+0.5f,k+0.5f) - vel.getAtMACZ(i,j,k) * dt;
	Real vz = src.getInterpolatedComponentHi<2>(zpos, orderSpace);
	
	dst(i,j,k) = Vec3(vx,vy,vz);
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return vel; } typedef MACGrid type1;inline MACGrid& getArg2() { return dst; } typedef MACGrid type2;inline const MACGrid& getArg3() { return src; } typedef MACGrid type3;inline Real& getArg4() { return dt; } typedef Real type4;inline int& getArg5() { return orderSpace; } typedef int type5; void runMessage() { debMsg("Executing kernel SemiLagrangeMAC ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,src,dt,orderSpace);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,src,dt,orderSpace);  } }  } const FlagGrid& flags; const MACGrid& vel; MACGrid& dst; const MACGrid& src; Real dt; int orderSpace;   };
#line 36 "plugin/advection.cpp"




//! Kernel: Correct based on forward and backward SL steps (for both centered & mac grids)



template <class T>  struct MacCormackCorrect : public KernelBase { MacCormackCorrect(const FlagGrid& flags, Grid<T>& dst, const Grid<T>& old, const Grid<T>& fwd, const Grid<T>& bwd, Real strength, bool isLevelSet, bool isMAC=false ) :  KernelBase(&flags,0) ,flags(flags),dst(dst),old(old),fwd(fwd),bwd(bwd),strength(strength),isLevelSet(isLevelSet),isMAC(isMAC)   { runMessage(); run(); }   inline void op(IndexInt idx, const FlagGrid& flags, Grid<T>& dst, const Grid<T>& old, const Grid<T>& fwd, const Grid<T>& bwd, Real strength, bool isLevelSet, bool isMAC=false  )  {
	dst[idx] = fwd[idx];

	if (flags.isFluid(idx)) {
		// only correct inside fluid region; note, strenth of correction can be modified here
		dst[idx] += strength * 0.5 * (old[idx] - bwd[idx]);
	}
}    inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline Grid<T>& getArg1() { return dst; } typedef Grid<T> type1;inline const Grid<T>& getArg2() { return old; } typedef Grid<T> type2;inline const Grid<T>& getArg3() { return fwd; } typedef Grid<T> type3;inline const Grid<T>& getArg4() { return bwd; } typedef Grid<T> type4;inline Real& getArg5() { return strength; } typedef Real type5;inline bool& getArg6() { return isLevelSet; } typedef bool type6;inline bool& getArg7() { return isMAC; } typedef bool type7; void runMessage() { debMsg("Executing kernel MacCormackCorrect ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,flags,dst,old,fwd,bwd,strength,isLevelSet,isMAC);  }   } const FlagGrid& flags; Grid<T>& dst; const Grid<T>& old; const Grid<T>& fwd; const Grid<T>& bwd; Real strength; bool isLevelSet; bool isMAC;   };
#line 54 "plugin/advection.cpp"



//! Kernel: Correct based on forward and backward SL steps (for both centered & mac grids)



template <class T>  struct MacCormackCorrectMAC : public KernelBase { MacCormackCorrectMAC(const FlagGrid& flags, Grid<T>& dst, const Grid<T>& old, const Grid<T>& fwd, const Grid<T>& bwd, Real strength, bool isLevelSet, bool isMAC=false ) :  KernelBase(&flags,0) ,flags(flags),dst(dst),old(old),fwd(fwd),bwd(bwd),strength(strength),isLevelSet(isLevelSet),isMAC(isMAC)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, Grid<T>& dst, const Grid<T>& old, const Grid<T>& fwd, const Grid<T>& bwd, Real strength, bool isLevelSet, bool isMAC=false  )  {
	bool skip[3] = { false, false, false };

	if (!flags.isFluid(i,j,k)) skip[0] = skip[1] = skip[2] = true;
	if(isMAC) {
		if( (i>0) && (!flags.isFluid(i-1,j,k) )) skip[0] = true; 
		if( (j>0) && (!flags.isFluid(i,j-1,k) )) skip[1] = true; 
		if( (k>0) && (!flags.isFluid(i,j,k-1) )) skip[2] = true; 
	}

	for(int c=0; c<3; ++c ) {
		if ( skip[c] ) {
			dst(i,j,k)[c] = fwd(i,j,k)[c];
		} else { 
			// perform actual correction with given strength
			dst(i,j,k)[c] = fwd(i,j,k)[c] + strength * 0.5 * (old(i,j,k)[c] - bwd(i,j,k)[c]);
		}
	}
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline Grid<T>& getArg1() { return dst; } typedef Grid<T> type1;inline const Grid<T>& getArg2() { return old; } typedef Grid<T> type2;inline const Grid<T>& getArg3() { return fwd; } typedef Grid<T> type3;inline const Grid<T>& getArg4() { return bwd; } typedef Grid<T> type4;inline Real& getArg5() { return strength; } typedef Real type5;inline bool& getArg6() { return isLevelSet; } typedef bool type6;inline bool& getArg7() { return isMAC; } typedef bool type7; void runMessage() { debMsg("Executing kernel MacCormackCorrectMAC ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,dst,old,fwd,bwd,strength,isLevelSet,isMAC);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,dst,old,fwd,bwd,strength,isLevelSet,isMAC);  } }  } const FlagGrid& flags; Grid<T>& dst; const Grid<T>& old; const Grid<T>& fwd; const Grid<T>& bwd; Real strength; bool isLevelSet; bool isMAC;   };
#line 67 "plugin/advection.cpp"



// Helper to collect min/max in a template
template<class T> inline void getMinMax(T& minv, T& maxv, const T& val) {
	if (val < minv) minv = val;
	if (val > maxv) maxv = val;
}
template<> inline void getMinMax<Vec3>(Vec3& minv, Vec3& maxv, const Vec3& val) {
	getMinMax(minv.x, maxv.x, val.x);
	getMinMax(minv.y, maxv.y, val.y);
	getMinMax(minv.z, maxv.z, val.z);
}

//! detect out of bounds value
template<class T> inline bool cmpMinMax(T& minv, T& maxv, const T& val) { 
	if (val < minv) return true;
	if (val > maxv) return true;
	return false;
}
template<> inline bool cmpMinMax<Vec3>(Vec3& minv, Vec3& maxv, const Vec3& val) { 
	return( cmpMinMax(minv.x, maxv.x, val.x) | cmpMinMax(minv.y, maxv.y, val.y) | cmpMinMax(minv.z, maxv.z, val.z));
}


#define checkFlag(x,y,z) (flags((x),(y),(z)) & (FlagGrid::TypeFluid|FlagGrid::TypeEmpty))

//! Helper function for clamping non-mac grids (those have specialized per component version below)
//  Note - 2 clamp modes, a sharper one (default, clampMode 1, also uses backward step), 
//         and a softer version (clampMode 2) that is recommended in Andy's paper
template<class T>
inline T doClampComponent(const Vec3i& gridSize, const FlagGrid& flags, T dst, const Grid<T>& orig, const T fwd, const Vec3& pos, const Vec3& vel, const int clampMode )
{
	T minv( std::numeric_limits<Real>::max()), maxv( -std::numeric_limits<Real>::max());
	bool haveFl = false;

	// forward (and optionally) backward
	Vec3i positions[2];
	int numPos = 1;
	positions[0] = toVec3i(pos - vel);
	if(clampMode==1) { numPos = 2;
	positions[1] = toVec3i(pos + vel); }

	for(int l=0; l<numPos; ++l) {
		Vec3i& currPos = positions[l];

		// clamp lookup to grid
		const int i0 = clamp(currPos.x, 0, gridSize.x-1); // note! gridsize already has -1 from call 
		const int j0 = clamp(currPos.y, 0, gridSize.y-1);
		const int k0 = clamp(currPos.z, 0, (orig.is3D() ? (gridSize.z-1) : 1) );
		const int i1 = i0+1, j1 = j0+1, k1= (orig.is3D() ? (k0+1) : k0);

		// find min/max around source pos
		if(checkFlag(i0,j0,k0)) { getMinMax(minv, maxv, orig(i0,j0,k0));  haveFl=true; }
		if(checkFlag(i1,j0,k0)) { getMinMax(minv, maxv, orig(i1,j0,k0));  haveFl=true; }
		if(checkFlag(i0,j1,k0)) { getMinMax(minv, maxv, orig(i0,j1,k0));  haveFl=true; }
		if(checkFlag(i1,j1,k0)) { getMinMax(minv, maxv, orig(i1,j1,k0));  haveFl=true; }

		if(orig.is3D()) {
		if(checkFlag(i0,j0,k1)) { getMinMax(minv, maxv, orig(i0,j0,k1)); haveFl=true; }
		if(checkFlag(i1,j0,k1)) { getMinMax(minv, maxv, orig(i1,j0,k1)); haveFl=true; }
		if(checkFlag(i0,j1,k1)) { getMinMax(minv, maxv, orig(i0,j1,k1)); haveFl=true; }
		if(checkFlag(i1,j1,k1)) { getMinMax(minv, maxv, orig(i1,j1,k1)); haveFl=true; } } 
	}

	if(!haveFl) return fwd;
	if(clampMode==1) {
		dst = clamp(dst, minv, maxv); // hard clamp
	} else {
		if(cmpMinMax(minv,maxv,dst)) dst = fwd; // recommended in paper, "softer"
	}
	return dst;
}
	
//! Helper function for clamping MAC grids, slight differences in flag checks
//  similar to scalar version, just uses single component c of vec3 values
//  for symmetry, reverts to first order near boundaries for clampMode 2
template<int c> 
inline Real doClampComponentMAC(const FlagGrid& flags, const Vec3i& gridSize, Real dst, const MACGrid& orig, Real fwd, const Vec3& pos, const Vec3& vel, const int clampMode )
{
	Real minv = std::numeric_limits<Real>::max(), maxv = -std::numeric_limits<Real>::max();
	//bool haveFl = false;

	// forward (and optionally) backward
	Vec3i positions[2];
	int numPos = 1;
	positions[0] = toVec3i(pos - vel);
	if(clampMode==1) { numPos = 2;
	positions[1] = toVec3i(pos + vel); }

	Vec3i oPos = toVec3i(pos); 
	Vec3i nbPos = oPos; nbPos[c] -= 1;
	if(clampMode==2 && (! (checkFlag(oPos.x,oPos.y,oPos.z) && checkFlag(nbPos.x,nbPos.y,nbPos.z)) )) return fwd; // replaces haveFl check

	for(int l=0; l<numPos; ++l) {
		Vec3i& currPos = positions[l];

		const int i0 = clamp(currPos.x, 0, gridSize.x-1); // note! gridsize already has -1 from call 
		const int j0 = clamp(currPos.y, 0, gridSize.y-1); // but we need a clamp to -2 for the +1 offset below	
		const int k0 = clamp(currPos.z, 0, (orig.is3D() ? (gridSize.z-1) : 0) );
		const int i1 = i0+1, j1 = j0+1, k1= (orig.is3D() ? (k0+1) : k0);

		// find min/max around source pos
		getMinMax(minv, maxv, orig(i0,j0,k0)[c]); 
		getMinMax(minv, maxv, orig(i1,j0,k0)[c]); 
		getMinMax(minv, maxv, orig(i0,j1,k0)[c]); 
		getMinMax(minv, maxv, orig(i1,j1,k0)[c]); 

		if(orig.is3D()) {
		getMinMax(minv, maxv, orig(i0,j0,k1)[c]); 
		getMinMax(minv, maxv, orig(i1,j0,k1)[c]); 
		getMinMax(minv, maxv, orig(i0,j1,k1)[c]); 
		getMinMax(minv, maxv, orig(i1,j1,k1)[c]); }
	}

	if(clampMode==1) {
		dst = clamp(dst, minv, maxv); // hard clamp
	} else {
		if(cmpMinMax(minv,maxv,dst)) dst = fwd; // recommended in paper, "softer"
	}
	return dst;
}

#undef checkFlag

//! Kernel: Clamp obtained value to min/max in source area, and reset values that point out of grid or into boundaries
//          (note - MAC grids are handled below)


template <class T>  struct MacCormackClamp : public KernelBase { MacCormackClamp(const FlagGrid& flags, const MACGrid& vel, Grid<T>& dst, const Grid<T>& orig, const Grid<T>& fwd, Real dt, const int clampMode) :  KernelBase(&flags,1) ,flags(flags),vel(vel),dst(dst),orig(orig),fwd(fwd),dt(dt),clampMode(clampMode)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& vel, Grid<T>& dst, const Grid<T>& orig, const Grid<T>& fwd, Real dt, const int clampMode )  {
	T     dval       = dst(i,j,k);
	Vec3i gridUpper  = flags.getSize() - 1;
	
	dval = doClampComponent<T>(gridUpper, flags, dval, orig, fwd(i,j,k), Vec3(i,j,k), vel.getCentered(i,j,k) * dt, clampMode );

	if(1 && clampMode==1) {
		// lookup forward/backward , round to closest NB
		Vec3i posFwd = toVec3i( Vec3(i,j,k) + Vec3(0.5,0.5,0.5) - vel.getCentered(i,j,k) * dt );
		Vec3i posBwd = toVec3i( Vec3(i,j,k) + Vec3(0.5,0.5,0.5) + vel.getCentered(i,j,k) * dt );
		
		// test if lookups point out of grid or into obstacle (note doClampComponent already checks sides, below is needed for valid flags access)
		if (posFwd.x < 0 || posFwd.y < 0 || posFwd.z < 0 ||
			posBwd.x < 0 || posBwd.y < 0 || posBwd.z < 0 ||
			posFwd.x > gridUpper.x || posFwd.y > gridUpper.y || ((posFwd.z > gridUpper.z)&&flags.is3D()) ||
			posBwd.x > gridUpper.x || posBwd.y > gridUpper.y || ((posBwd.z > gridUpper.z)&&flags.is3D()) ||
			flags.isObstacle(posFwd) || flags.isObstacle(posBwd) ) 
		{
			dval = fwd(i,j,k);
		}
	}
	// clampMode 2 handles flags in doClampComponent call

	dst(i,j,k) = dval;
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return vel; } typedef MACGrid type1;inline Grid<T>& getArg2() { return dst; } typedef Grid<T> type2;inline const Grid<T>& getArg3() { return orig; } typedef Grid<T> type3;inline const Grid<T>& getArg4() { return fwd; } typedef Grid<T> type4;inline Real& getArg5() { return dt; } typedef Real type5;inline const int& getArg6() { return clampMode; } typedef int type6; void runMessage() { debMsg("Executing kernel MacCormackClamp ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,orig,fwd,dt,clampMode);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,orig,fwd,dt,clampMode);  } }  } const FlagGrid& flags; const MACGrid& vel; Grid<T>& dst; const Grid<T>& orig; const Grid<T>& fwd; Real dt; const int clampMode;   };
#line 213 "plugin/advection.cpp"



//! Kernel: same as MacCormackClamp above, but specialized version for MAC grids


 struct MacCormackClampMAC : public KernelBase { MacCormackClampMAC(const FlagGrid& flags, const MACGrid& vel, MACGrid& dst, const MACGrid& orig, const MACGrid& fwd, Real dt, const int clampMode) :  KernelBase(&flags,1) ,flags(flags),vel(vel),dst(dst),orig(orig),fwd(fwd),dt(dt),clampMode(clampMode)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& vel, MACGrid& dst, const MACGrid& orig, const MACGrid& fwd, Real dt, const int clampMode )  {
	Vec3  pos(i,j,k);
	Vec3  dval       = dst(i,j,k);
	Vec3  dfwd       = fwd(i,j,k);
	Vec3i gridUpper  = flags.getSize() - 1;
	
	dval.x = doClampComponentMAC<0>(flags, gridUpper, dval.x, orig, dfwd.x, pos, vel.getAtMACX(i,j,k) * dt, clampMode );
	dval.y = doClampComponentMAC<1>(flags, gridUpper, dval.y, orig, dfwd.y, pos, vel.getAtMACY(i,j,k) * dt, clampMode );
	if(flags.is3D())
	dval.z = doClampComponentMAC<2>(flags, gridUpper, dval.z, orig, dfwd.z, pos, vel.getAtMACZ(i,j,k) * dt, clampMode );

	// note - the MAC version currently does not check whether source points were inside an obstacle! (unlike centered version)
	// this would need to be done for each face separately to stay symmetric...

	dst(i,j,k) = dval;
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return vel; } typedef MACGrid type1;inline MACGrid& getArg2() { return dst; } typedef MACGrid type2;inline const MACGrid& getArg3() { return orig; } typedef MACGrid type3;inline const MACGrid& getArg4() { return fwd; } typedef MACGrid type4;inline Real& getArg5() { return dt; } typedef Real type5;inline const int& getArg6() { return clampMode; } typedef int type6; void runMessage() { debMsg("Executing kernel MacCormackClampMAC ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,orig,fwd,dt,clampMode);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=1; j < _maxY; j++) for (int i=1; i < _maxX; i++) op(i,j,k,flags,vel,dst,orig,fwd,dt,clampMode);  } }  } const FlagGrid& flags; const MACGrid& vel; MACGrid& dst; const MACGrid& orig; const MACGrid& fwd; Real dt; const int clampMode;   };
#line 242 "plugin/advection.cpp"




//! template function for performing SL advection
//! (Note boundary width only needed for specialization for MAC grids below)
template<class GridType> 
void fnAdvectSemiLagrange(FluidSolver* parent, const FlagGrid& flags, const MACGrid& vel, GridType& orig, int order, Real strength, int orderSpace, bool openBounds, int bWidth, int clampMode) {
	typedef typename GridType::BASETYPE T;
	
	Real dt = parent->getDt();
	bool levelset = orig.getType() & GridBase::TypeLevelset;
	
	// forward step
	GridType fwd(parent);
	SemiLagrange<T> (flags, vel, fwd, orig, dt, levelset, orderSpace);
	
	if (order == 1) {
		orig.swap(fwd);
	}
	else if (order == 2) { // MacCormack
		GridType bwd(parent);
		GridType newGrid(parent);
	
		// bwd <- backwards step
		SemiLagrange<T> (flags, vel, bwd, fwd, -dt, levelset, orderSpace);
		
		// newGrid <- compute correction
		MacCormackCorrect<T> (flags, newGrid, orig, fwd, bwd, strength, levelset);
		
		// clamp values
		MacCormackClamp<T> (flags, vel, newGrid, orig, fwd, dt, clampMode);
		
		orig.swap(newGrid);
	}
}

// outflow functions

//! calculate local propagation velocity for cell (i,j,k)
Vec3 getBulkVel(const FlagGrid& flags, const MACGrid& vel, int i, int j, int k){
	Vec3 avg = Vec3(0,0,0);
	int count = 0;
	int size=1; // stencil size
	int nmax = (flags.is3D() ? size : 0);
	// average the neighboring fluid / outflow cell's velocity
	for (int n = -nmax; n<=nmax;n++){
		for (int m = -size; m<=size; m++){
			for (int l = -size; l<=size; l++){
				if (flags.isInBounds(Vec3i(i+l,j+m,k+n),0) && (flags.isFluid(i+l,j+m,k+n)||flags.isOutflow(i+l,j+m,k+n))){
					avg += vel(i+l,j+m,k+n);
					count++;
				}
			}
		}
	}
	return count>0 ? avg/count : avg;
}

//! extrapolate normal velocity components into outflow cell
 struct extrapolateVelConvectiveBC : public KernelBase { extrapolateVelConvectiveBC(const FlagGrid& flags, const MACGrid& vel, MACGrid& velDst, const MACGrid& velPrev, Real timeStep, int bWidth) :  KernelBase(&flags,0) ,flags(flags),vel(vel),velDst(velDst),velPrev(velPrev),timeStep(timeStep),bWidth(bWidth)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& vel, MACGrid& velDst, const MACGrid& velPrev, Real timeStep, int bWidth )  {
	if (flags.isOutflow(i,j,k)){
		Vec3 bulkVel = getBulkVel(flags,vel,i,j,k);
		bool done=false;
		int dim = flags.is3D() ? 3 : 2;
		Vec3i cur, low, up, flLow, flUp;
		cur = low = up = flLow = flUp = Vec3i(i,j,k);
		// iterate over each velocity component x, y, z
		for (int c = 0; c<dim; c++){
			Real factor = timeStep*max((Real)1.0,bulkVel[c]); // prevent the extrapolated velocity from exploding when bulk velocity below 1
			low[c] = flLow[c] = cur[c]-1;
			up[c] = flUp[c] = cur[c]+1;
			// iterate over bWidth to allow for extrapolation into more distant outflow cells
			for (int d = 0; d<bWidth+1; d++){
				if (cur[c]>d && flags.isFluid(flLow)) {	
					velDst(i,j,k)[c] = ((vel(i,j,k)[c] - velPrev(i,j,k)[c]) / factor) + vel(low)[c];
					done=true;
				}
				if (cur[c]<flags.getSize()[c]-d-1 && flags.isFluid(flUp)) {
					// check for cells equally far away from two fluid cells -> average value between both sides
					if (done) velDst(i,j,k)[c] = 0.5*(velDst(i,j,k)[c] + ((vel(i,j,k)[c] - velPrev(i,j,k)[c]) / factor) + vel(up)[c]);
					else velDst(i,j,k)[c] = ((vel(i,j,k)[c] - velPrev(i,j,k)[c]) / factor) + vel(up)[c];
					done=true;
				}
				flLow[c]=flLow[c]-1;
				flUp[c]=flUp[c]+1;
				if (done) break;
			}
			low = up = flLow = flUp = cur;
			done=false;
		}
	}
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return vel; } typedef MACGrid type1;inline MACGrid& getArg2() { return velDst; } typedef MACGrid type2;inline const MACGrid& getArg3() { return velPrev; } typedef MACGrid type3;inline Real& getArg4() { return timeStep; } typedef Real type4;inline int& getArg5() { return bWidth; } typedef int type5; void runMessage() { debMsg("Executing kernel extrapolateVelConvectiveBC ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,vel,velDst,velPrev,timeStep,bWidth);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,vel,velDst,velPrev,timeStep,bWidth);  } }  } const FlagGrid& flags; const MACGrid& vel; MACGrid& velDst; const MACGrid& velPrev; Real timeStep; int bWidth;   };
#line 316 "plugin/advection.cpp"



//! copy extrapolated velocity components
 struct copyChangedVels : public KernelBase { copyChangedVels(const FlagGrid& flags, const MACGrid& velDst, MACGrid& vel) :  KernelBase(&flags,0) ,flags(flags),velDst(velDst),vel(vel)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, const MACGrid& velDst, MACGrid& vel )  { if (flags.isOutflow(i,j,k)) vel(i, j, k) = velDst(i, j, k); }   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const MACGrid& getArg1() { return velDst; } typedef MACGrid type1;inline MACGrid& getArg2() { return vel; } typedef MACGrid type2; void runMessage() { debMsg("Executing kernel copyChangedVels ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,velDst,vel);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,velDst,vel);  } }  } const FlagGrid& flags; const MACGrid& velDst; MACGrid& vel;   };
#line 351 "plugin/advection.cpp"



//! extrapolate normal velocity components into open boundary cells (marked as outflow cells)
void applyOutflowBC(const FlagGrid& flags, MACGrid& vel, const MACGrid& velPrev, double timeStep, int bWidth=1) {
	MACGrid velDst(vel.getParent()); // do not overwrite vel while it is read
	extrapolateVelConvectiveBC(flags, vel, velDst, velPrev, max(1.0,timeStep*4), bWidth);
	copyChangedVels(flags,velDst,vel);
}

// advection helpers

//! prevent parts of the surface getting "stuck" in obstacle regions
 struct knResetPhiInObs : public KernelBase { knResetPhiInObs(const FlagGrid& flags, Grid<Real>& sdf) :  KernelBase(&flags,0) ,flags(flags),sdf(sdf)   { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, Grid<Real>& sdf )  {
	if( flags.isObstacle(i,j,k) && (sdf(i,j,k)<0.) ) {
		sdf(i,j,k) = 0.1;
	}
}   inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline Grid<Real>& getArg1() { return sdf; } typedef Grid<Real> type1; void runMessage() { debMsg("Executing kernel knResetPhiInObs ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,sdf);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,sdf);  } }  } const FlagGrid& flags; Grid<Real>& sdf;   };
#line 363 "plugin/advection.cpp"


void resetPhiInObs(const FlagGrid& flags, Grid<Real>& sdf) { knResetPhiInObs(flags, sdf); } static PyObject* _W_0 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "resetPhiInObs" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",0,&_lock); Grid<Real>& sdf = *_args.getPtr<Grid<Real> >("sdf",1,&_lock);   _retval = getPyNone(); resetPhiInObs(flags,sdf);  _args.check(); } pbFinalizePlugin(parent,"resetPhiInObs", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("resetPhiInObs",e.what()); return 0; } } static const Pb::Register _RP_resetPhiInObs ("","resetPhiInObs",_W_0);  extern "C" { void PbRegister_resetPhiInObs() { KEEP_UNUSED(_RP_resetPhiInObs); } } 

// advection main calls

//! template function for performing SL advection: specialized version for MAC grids
template<> 
void fnAdvectSemiLagrange<MACGrid>(FluidSolver* parent, const FlagGrid& flags, const MACGrid& vel, MACGrid& orig, int order, Real strength, int orderSpace, bool openBounds, int bWidth, int clampMode) {
	Real dt = parent->getDt();
	
	// forward step
	MACGrid fwd(parent);    
	SemiLagrangeMAC (flags, vel, fwd, orig, dt, orderSpace);
	
	if (orderSpace != 1) { debMsg("Warning higher order for MAC grids not yet implemented...",1); }

	if (order == 1) {
		if (openBounds) applyOutflowBC(flags, fwd, orig, dt, bWidth);
		orig.swap(fwd);
	}
	else if (order == 2) { // MacCormack 
		MACGrid bwd(parent);
		MACGrid newGrid(parent);
		
		// bwd <- backwards step
		SemiLagrangeMAC (flags, vel, bwd, fwd, -dt, orderSpace);
		
		// newGrid <- compute correction
		MacCormackCorrectMAC<Vec3> (flags, newGrid, orig, fwd, bwd, strength, false, true);
		
		// clamp values
		MacCormackClampMAC (flags, vel, newGrid, orig, fwd, dt, clampMode); 
		
		if (openBounds) applyOutflowBC(flags, newGrid, orig, dt, bWidth);
		orig.swap(newGrid);
	}
}


//! Perform semi-lagrangian advection of target Real- or Vec3 grid
//! Open boundary handling needs information about width of border
//! Clamping modes: 1 regular clamp leading to more overshoot and sharper results, 2 revert to 1st order slightly smoother less overshoot (enable when 1 gives artifacts)


void advectSemiLagrange(const FlagGrid* flags, const MACGrid* vel, GridBase* grid, int order = 1, Real strength = 1.0, int orderSpace = 1, bool openBounds = false, int boundaryWidth = 1, int clampMode = 2) {    
	assertMsg(order==1 || order==2, "AdvectSemiLagrange: Only order 1 (regular SL) and 2 (MacCormack) supported");
	
	// determine type of grid    
	if (grid->getType() & GridBase::TypeReal) {
		fnAdvectSemiLagrange< Grid<Real> >(flags->getParent(), *flags, *vel, *((Grid<Real>*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else if (grid->getType() & GridBase::TypeMAC) {    
		fnAdvectSemiLagrange< MACGrid >(flags->getParent(), *flags, *vel, *((MACGrid*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else if (grid->getType() & GridBase::TypeVec3) {    
		fnAdvectSemiLagrange< Grid<Vec3> >(flags->getParent(), *flags, *vel, *((Grid<Vec3>*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else
		errMsg("AdvectSemiLagrange: Grid Type is not supported (only Real, Vec3, MAC, Levelset)");    
} static PyObject* _W_1 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "advectSemiLagrange" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const FlagGrid* flags = _args.getPtr<FlagGrid >("flags",0,&_lock); const MACGrid* vel = _args.getPtr<MACGrid >("vel",1,&_lock); GridBase* grid = _args.getPtr<GridBase >("grid",2,&_lock); int order = _args.getOpt<int >("order",3,1,&_lock); Real strength = _args.getOpt<Real >("strength",4,1.0,&_lock); int orderSpace = _args.getOpt<int >("orderSpace",5,1,&_lock); bool openBounds = _args.getOpt<bool >("openBounds",6,false,&_lock); int boundaryWidth = _args.getOpt<int >("boundaryWidth",7,1,&_lock); int clampMode = _args.getOpt<int >("clampMode",8,2,&_lock);   _retval = getPyNone(); advectSemiLagrange(flags,vel,grid,order,strength,orderSpace,openBounds,boundaryWidth,clampMode);  _args.check(); } pbFinalizePlugin(parent,"advectSemiLagrange", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("advectSemiLagrange",e.what()); return 0; } } static const Pb::Register _RP_advectSemiLagrange ("","advectSemiLagrange",_W_1);  extern "C" { void PbRegister_advectSemiLagrange() { KEEP_UNUSED(_RP_advectSemiLagrange); } } 

} // end namespace DDF 



