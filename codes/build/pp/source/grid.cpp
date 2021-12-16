




// DO NOT EDIT !
// This file is generated using the MantaFlow preprocessor (prep generate).




#line 1 "/home/vishrut/Study/USC/Sem-1/CSCI 596: Scientific Computing and Visualization/project/mlflip/source/grid.cpp"
/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * Apache License, Version 2.0 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Grid representation
 *
 ******************************************************************************/

#include "grid.h"
#include "levelset.h"
#include "kernel.h"
#include "commonkernels.h"
#include "mantaio.h"
#include <limits>
#include <queue>
#include <sstream>
#include <cstring>

using namespace std;
namespace Manta {

//******************************************************************************
// GridBase members

GridBase::GridBase (FluidSolver* parent) 
	: PbClass(parent), mType(TypeNone)
{
	checkParent();
	m3D = getParent()->is3D();
}

//******************************************************************************
// Grid<T> members

// helpers to set type
template<class T> inline GridBase::GridType typeList() { return GridBase::TypeNone; }
template<> inline GridBase::GridType typeList<Real>()  { return GridBase::TypeReal; }
template<> inline GridBase::GridType typeList<int>()   { return GridBase::TypeInt;  }
template<> inline GridBase::GridType typeList<Vec3>()  { return GridBase::TypeVec3; }

template<class T>
Grid<T>::Grid(FluidSolver* parent, bool show)
        : GridBase(parent), externalData(false)
{
	mType = typeList<T>();
	mSize = parent->getGridSize();
	mData = parent->getGridPointer<T>();
	
	mStrideZ = parent->is2D() ? 0 : (mSize.x * mSize.y);
	mDx = 1.0 / mSize.max();
	clear();
	setHidden(!show);
}

template<class T>
Grid<T>::Grid(FluidSolver* parent, T* data, bool show)
        : GridBase(parent), mData(data), externalData(true)
{
        mType = typeList<T>();
        mSize = parent->getGridSize();

        mStrideZ = parent->is2D() ? 0 : (mSize.x * mSize.y);
        mDx = 1.0 / mSize.max();

        setHidden(!show);
}

template<class T>
Grid<T>::Grid(const Grid<T>& a)
        : GridBase(a.getParent()), externalData(false) {
	mSize = a.mSize;
	mType = a.mType;
	mStrideZ = a.mStrideZ;
	mDx = a.mDx;
	FluidSolver *gp = a.getParent();
	mData = gp->getGridPointer<T>();
	memcpy(mData, a.mData, sizeof(T) * a.mSize.x * a.mSize.y * a.mSize.z);
}

template<class T>
Grid<T>::~Grid() {
    if(!externalData)  {
        mParent->freeGridPointer<T>(mData);
    }
}

template<class T>
void Grid<T>::clear() {
	memset(mData, 0, sizeof(T) * mSize.x * mSize.y * mSize.z);    
}

template<class T>
void Grid<T>::swap(Grid<T>& other) {
	if (other.getSizeX() != getSizeX() || other.getSizeY() != getSizeY() || other.getSizeZ() != getSizeZ())
		errMsg("Grid::swap(): Grid dimensions mismatch.");
	
        if(externalData || other.externalData)
            errMsg("Grid::swap(): Cannot swap if one grid stores externalData.");

	T* dswap = other.mData;
	other.mData = mData;
	mData = dswap;

}

template<class T>
void Grid<T>::load(string name) {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if (ext == ".raw")
		readGridRaw(name, this);
	else if (ext == ".uni")
		readGridUni(name, this);
	else if (ext == ".vol")
		readGridVol(name, this);
	else
		errMsg("file '" + name +"' filetype not supported");
}

template<class T>
void Grid<T>::save(string name) {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if (ext == ".raw")
		writeGridRaw(name, this);
	else if (ext == ".uni")
		writeGridUni(name, this);
	else if (ext == ".vol")
		writeGridVol(name, this);
#	if OPENVDB==1
	else if (ext == ".vdb")
		writeGridVDB(name, this);
#	endif // OPENVDB==1
	else if (ext == ".txt")
		writeGridTxt(name, this);
	else
		errMsg("file '" + name +"' filetype not supported");
}

//******************************************************************************
// Grid<T> operators

//! Kernel: Compute min value of Real grid

 struct CompMinReal : public KernelBase { CompMinReal(const Grid<Real>& val) :  KernelBase(&val,0) ,val(val) ,minVal(std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Real>& val ,Real& minVal)  {
	if (val[idx] < minVal)
		minVal = val[idx];
}    inline operator Real () { return minVal; } inline Real  & getRet() { return minVal; }  inline const Grid<Real>& getArg0() { return val; } typedef Grid<Real> type0; void runMessage() { debMsg("Executing kernel CompMinReal ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real minVal = std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,minVal); 
#pragma omp critical
{this->minVal = min(minVal, this->minVal); } }   } const Grid<Real>& val;  Real minVal;  };
#line 152 "grid.cpp"



//! Kernel: Compute max value of Real grid

 struct CompMaxReal : public KernelBase { CompMaxReal(const Grid<Real>& val) :  KernelBase(&val,0) ,val(val) ,maxVal(-std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Real>& val ,Real& maxVal)  {
	if (val[idx] > maxVal)
		maxVal = val[idx];
}    inline operator Real () { return maxVal; } inline Real  & getRet() { return maxVal; }  inline const Grid<Real>& getArg0() { return val; } typedef Grid<Real> type0; void runMessage() { debMsg("Executing kernel CompMaxReal ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real maxVal = -std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,maxVal); 
#pragma omp critical
{this->maxVal = max(maxVal, this->maxVal); } }   } const Grid<Real>& val;  Real maxVal;  };
#line 159 "grid.cpp"



//! Kernel: Compute min value of int grid

 struct CompMinInt : public KernelBase { CompMinInt(const Grid<int>& val) :  KernelBase(&val,0) ,val(val) ,minVal(std::numeric_limits<int>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<int>& val ,int& minVal)  {
	if (val[idx] < minVal)
		minVal = val[idx];
}    inline operator int () { return minVal; } inline int  & getRet() { return minVal; }  inline const Grid<int>& getArg0() { return val; } typedef Grid<int> type0; void runMessage() { debMsg("Executing kernel CompMinInt ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  int minVal = std::numeric_limits<int>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,minVal); 
#pragma omp critical
{this->minVal = min(minVal, this->minVal); } }   } const Grid<int>& val;  int minVal;  };
#line 166 "grid.cpp"



//! Kernel: Compute max value of int grid

 struct CompMaxInt : public KernelBase { CompMaxInt(const Grid<int>& val) :  KernelBase(&val,0) ,val(val) ,maxVal(-std::numeric_limits<int>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<int>& val ,int& maxVal)  {
	if (val[idx] > maxVal)
		maxVal = val[idx];
}    inline operator int () { return maxVal; } inline int  & getRet() { return maxVal; }  inline const Grid<int>& getArg0() { return val; } typedef Grid<int> type0; void runMessage() { debMsg("Executing kernel CompMaxInt ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  int maxVal = -std::numeric_limits<int>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,maxVal); 
#pragma omp critical
{this->maxVal = max(maxVal, this->maxVal); } }   } const Grid<int>& val;  int maxVal;  };
#line 173 "grid.cpp"



//! Kernel: Compute min norm of vec grid

 struct CompMinVec : public KernelBase { CompMinVec(const Grid<Vec3>& val) :  KernelBase(&val,0) ,val(val) ,minVal(std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Vec3>& val ,Real& minVal)  {
	const Real s = normSquare(val[idx]);
	if (s < minVal)
		minVal = s;
}    inline operator Real () { return minVal; } inline Real  & getRet() { return minVal; }  inline const Grid<Vec3>& getArg0() { return val; } typedef Grid<Vec3> type0; void runMessage() { debMsg("Executing kernel CompMinVec ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real minVal = std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,minVal); 
#pragma omp critical
{this->minVal = min(minVal, this->minVal); } }   } const Grid<Vec3>& val;  Real minVal;  };
#line 180 "grid.cpp"



//! Kernel: Compute max norm of vec grid

 struct CompMaxVec : public KernelBase { CompMaxVec(const Grid<Vec3>& val) :  KernelBase(&val,0) ,val(val) ,maxVal(-std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Vec3>& val ,Real& maxVal)  {
	const Real s = normSquare(val[idx]);
	if (s > maxVal)
		maxVal = s;
}    inline operator Real () { return maxVal; } inline Real  & getRet() { return maxVal; }  inline const Grid<Vec3>& getArg0() { return val; } typedef Grid<Vec3> type0; void runMessage() { debMsg("Executing kernel CompMaxVec ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real maxVal = -std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,maxVal); 
#pragma omp critical
{this->maxVal = max(maxVal, this->maxVal); } }   } const Grid<Vec3>& val;  Real maxVal;  };
#line 188 "grid.cpp"



template<class T> Grid<T>& Grid<T>::copyFrom (const Grid<T>& a, bool copyType ) {
	assertMsg (a.mSize.x == mSize.x && a.mSize.y == mSize.y && a.mSize.z == mSize.z, "different grid resolutions "<<a.mSize<<" vs "<<this->mSize );
	memcpy(mData, a.mData, sizeof(T) * mSize.x * mSize.y * mSize.z);
	if(copyType) mType = a.mType; // copy type marker
	return *this;
}
/*template<class T> Grid<T>& Grid<T>::operator= (const Grid<T>& a) {
	note: do not use , use copyFrom instead
}*/


template<class T> void Grid<T>::add(const Grid<T>& a)  { (*this) += a; }
template<class T> void Grid<T>::sub(const Grid<T>& a)  { (*this) -= a; }
template<class T> void Grid<T>::mult(const Grid<T>& a) { (*this) *= a; }
template<class T> void Grid<T>::addConst(const T& a)   { (*this) += a; }
template<class T> void Grid<T>::multConst(const T& a)  { (*this) *= a; }
template<class T> Grid<T>& Grid<T>::safeDivide(const Grid<T>& a) {
	gridSafeDiv<T>(*this, a);
	return *this;
}

template<class T> void Grid<T>::addScaled(const Grid<T>& a, const T& factor) { 
	gridScaledAdd<T,T>(*this, a, factor);
}
template<class T> void Grid<T>::setConst(const T& a) {
	gridSetConst<T>(*this, a);
}
template<class T> void Grid<T>::clamp(const T& min, const T& max) {
	gridClamp<T>(*this, min, max);
}
template<class T> void Grid<T>::stomp(const T& threshold) {
	gridStomp<T>(*this, threshold);
}

template<> Real Grid<Real>::getMax() const {
	return CompMaxReal (*this);
}
template<> Real Grid<Real>::getMin() const {
	return CompMinReal (*this);
}
template<> Real Grid<Real>::getMaxAbs() const {
	Real amin = CompMinReal (*this);
	Real amax = CompMaxReal (*this);
	return max( fabs(amin), fabs(amax));
}
template<> Real Grid<Vec3>::getMax() const {
	return sqrt(CompMaxVec (*this));
}
template<> Real Grid<Vec3>::getMin() const {
	return sqrt(CompMinVec (*this));
}
template<> Real Grid<Vec3>::getMaxAbs() const {
	return sqrt(CompMaxVec (*this));
}
template<> Real Grid<int>::getMax() const {
	return (Real) CompMaxInt (*this);
}
template<> Real Grid<int>::getMin() const {
	return (Real) CompMinInt (*this);
}
template<> Real Grid<int>::getMaxAbs() const {
	int amin = CompMinInt (*this);
	int amax = CompMaxInt (*this);
	return max( fabs((Real)amin), fabs((Real)amax));
}
template<class T> std::string Grid<T>::getDataPointer() {
	std::ostringstream out;
	out << mData ;
	return out.str();
}

// L1 / L2 functions

//! calculate L1 norm for whole grid with non-parallelized loop
template<class GRID>
Real loop_calcL1Grid (const GRID &grid, int bnd)
{
	double accu = 0.;
	FOR_IJKT_BND(grid, bnd) { accu += norm(grid(i,j,k,t)); }
	return (Real)accu;
}

//! calculate L2 norm for whole grid with non-parallelized loop
// note - kernels "could" be used here, but they can't be templated at the moment (also, that would
// mean the bnd parameter is fixed)
template<class GRID>
Real loop_calcL2Grid(const GRID &grid, int bnd)
{
	double accu = 0.;
	FOR_IJKT_BND(grid, bnd) {
		accu += normSquare(grid(i,j,k,t)); // supported for real and vec3,4 types
	}
	return (Real)sqrt(accu);
}

//! compute L1 norm of whole grid content (note, not parallel at the moment)
template<class T> Real Grid<T>::getL1(int bnd) {
	return loop_calcL1Grid<Grid<T> >(*this, bnd);
}
//! compute L2 norm of whole grid content (note, not parallel at the moment)
template<class T> Real Grid<T>::getL2(int bnd) {
	return loop_calcL2Grid<Grid<T> >(*this, bnd);
}


 struct knCountCells : public KernelBase { knCountCells(const FlagGrid& flags, int flag, int bnd, Grid<Real>* mask) :  KernelBase(&flags,0) ,flags(flags),flag(flag),bnd(bnd),mask(mask) ,cnt(0)  { runMessage(); run(); }  inline void op(int i, int j, int k, const FlagGrid& flags, int flag, int bnd, Grid<Real>* mask ,int& cnt)  {
	if(mask) (*mask)(i,j,k) = 0.;
	if(bnd>0 && (!flags.isInBounds(Vec3i(i,j,k)))) return;
	if(flags(i,j,k) & flag) {
		++cnt;
		if(mask) (*mask)(i,j,k) = 1.;
	}
}   inline operator int () { return cnt; } inline int  & getRet() { return cnt; }  inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline int& getArg1() { return flag; } typedef int type1;inline int& getArg2() { return bnd; } typedef int type2;inline Grid<Real>* getArg3() { return mask; } typedef Grid<Real> type3; void runMessage() { debMsg("Executing kernel knCountCells ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  int cnt = 0; 
#pragma omp for nowait  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,flag,bnd,mask,cnt); 
#pragma omp critical
{this->cnt += cnt; } } } else { const int k=0; 
#pragma omp parallel 
 {  int cnt = 0; 
#pragma omp for nowait  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,flag,bnd,mask,cnt); 
#pragma omp critical
{this->cnt += cnt; } } }  } const FlagGrid& flags; int flag; int bnd; Grid<Real>* mask;  int cnt;  };
#line 299 "grid.cpp"



//! count number of cells of a certain type flag (can contain multiple bits, checks if any one of them is set - not all!)
int FlagGrid::countCells(int flag, int bnd, Grid<Real>* mask) {
	return knCountCells(*this, flag, bnd, mask);
}

// compute maximal diference of two cells in the grid
// used for testing system

Real gridMaxDiff(Grid<Real>& g1, Grid<Real>& g2) {
	double maxVal = 0.;
	FOR_IJK(g1) {
		maxVal = std::max(maxVal, (double)fabs(g1(i, j, k) - g2(i, j, k)));
	}
	return maxVal;
} static PyObject* _W_0 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "gridMaxDiff" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Real>& g1 = *_args.getPtr<Grid<Real> >("g1",0,&_lock); Grid<Real>& g2 = *_args.getPtr<Grid<Real> >("g2",1,&_lock);   _retval = toPy(gridMaxDiff(g1,g2));  _args.check(); } pbFinalizePlugin(parent,"gridMaxDiff", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("gridMaxDiff",e.what()); return 0; } } static const Pb::Register _RP_gridMaxDiff ("","gridMaxDiff",_W_0);  extern "C" { void PbRegister_gridMaxDiff() { KEEP_UNUSED(_RP_gridMaxDiff); } } 

Real gridMaxDiffInt(Grid<int>& g1, Grid<int>& g2) {
	double maxVal = 0.;
	FOR_IJK(g1) {
		maxVal = std::max(maxVal, (double)fabs((double)g1(i, j, k) - g2(i, j, k)));
	}
	return maxVal;
} static PyObject* _W_1 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "gridMaxDiffInt" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<int>& g1 = *_args.getPtr<Grid<int> >("g1",0,&_lock); Grid<int>& g2 = *_args.getPtr<Grid<int> >("g2",1,&_lock);   _retval = toPy(gridMaxDiffInt(g1,g2));  _args.check(); } pbFinalizePlugin(parent,"gridMaxDiffInt", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("gridMaxDiffInt",e.what()); return 0; } } static const Pb::Register _RP_gridMaxDiffInt ("","gridMaxDiffInt",_W_1);  extern "C" { void PbRegister_gridMaxDiffInt() { KEEP_UNUSED(_RP_gridMaxDiffInt); } } 

Real gridMaxDiffVec3(Grid<Vec3>& g1, Grid<Vec3>& g2) {
	double maxVal = 0.;
	FOR_IJK(g1) {
		// accumulate differences with double precision
		// note - don't use norm here! should be as precise as possible...
		double d = 0.;
		for (int c = 0; c<3; ++c) {
			d += fabs((double)g1(i, j, k)[c] - (double)g2(i, j, k)[c]);
		}
		maxVal = std::max(maxVal, d);
		//maxVal = std::max(maxVal, (double)fabs( norm(g1(i,j,k)-g2(i,j,k)) ));
	}
	return maxVal;
} static PyObject* _W_2 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "gridMaxDiffVec3" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3>& g1 = *_args.getPtr<Grid<Vec3> >("g1",0,&_lock); Grid<Vec3>& g2 = *_args.getPtr<Grid<Vec3> >("g2",1,&_lock);   _retval = toPy(gridMaxDiffVec3(g1,g2));  _args.check(); } pbFinalizePlugin(parent,"gridMaxDiffVec3", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("gridMaxDiffVec3",e.what()); return 0; } } static const Pb::Register _RP_gridMaxDiffVec3 ("","gridMaxDiffVec3",_W_2);  extern "C" { void PbRegister_gridMaxDiffVec3() { KEEP_UNUSED(_RP_gridMaxDiffVec3); } } 


Vec3 calcGridSizeFactorWithRange(const Vec3 &from_old, const Vec3 &to_old, const Vec3 &from_new, const Vec3 &to_new) {
	return (to_new - from_new)/(to_old - from_old);
} static PyObject* _W_3 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "calcGridSizeFactorWithRange" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Vec3& from_old = _args.get<Vec3 >("from_old",0,&_lock); const Vec3& to_old = _args.get<Vec3 >("to_old",1,&_lock); const Vec3& from_new = _args.get<Vec3 >("from_new",2,&_lock); const Vec3& to_new = _args.get<Vec3 >("to_new",3,&_lock);   _retval = toPy(calcGridSizeFactorWithRange(from_old,to_old,from_new,to_new));  _args.check(); } pbFinalizePlugin(parent,"calcGridSizeFactorWithRange", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("calcGridSizeFactorWithRange",e.what()); return 0; } } static const Pb::Register _RP_calcGridSizeFactorWithRange ("","calcGridSizeFactorWithRange",_W_3);  extern "C" { void PbRegister_calcGridSizeFactorWithRange() { KEEP_UNUSED(_RP_calcGridSizeFactorWithRange); } } 

// simple helper functions to copy (convert) mac to vec3 , and levelset to real grids
// (are assumed to be the same for running the test cases - in general they're not!)

void copyMacToVec3(MACGrid &source, Grid<Vec3>& target) {
	FOR_IJK(target) {
		target(i,j,k) = source(i,j,k);
	}
} static PyObject* _W_4 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "copyMacToVec3" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; MACGrid& source = *_args.getPtr<MACGrid >("source",0,&_lock); Grid<Vec3>& target = *_args.getPtr<Grid<Vec3> >("target",1,&_lock);   _retval = getPyNone(); copyMacToVec3(source,target);  _args.check(); } pbFinalizePlugin(parent,"copyMacToVec3", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("copyMacToVec3",e.what()); return 0; } } static const Pb::Register _RP_copyMacToVec3 ("","copyMacToVec3",_W_4);  extern "C" { void PbRegister_copyMacToVec3() { KEEP_UNUSED(_RP_copyMacToVec3); } } 

void convertMacToVec3(MACGrid &source , Grid<Vec3> &target) { debMsg("Deprecated - do not use convertMacToVec3... use copyMacToVec3 instead",1); copyMacToVec3(source,target); } static PyObject* _W_5 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "convertMacToVec3" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; MACGrid& source = *_args.getPtr<MACGrid >("source",0,&_lock); Grid<Vec3> & target = *_args.getPtr<Grid<Vec3>  >("target",1,&_lock);   _retval = getPyNone(); convertMacToVec3(source,target);  _args.check(); } pbFinalizePlugin(parent,"convertMacToVec3", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("convertMacToVec3",e.what()); return 0; } } static const Pb::Register _RP_convertMacToVec3 ("","convertMacToVec3",_W_5);  extern "C" { void PbRegister_convertMacToVec3() { KEEP_UNUSED(_RP_convertMacToVec3); } } 
void moveMacToCen(const MACGrid &source, Grid<Vec3>& target) { GetCentered(target, source); } static PyObject* _W_6 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "moveMacToCen" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const MACGrid& source = *_args.getPtr<MACGrid >("source",0,&_lock); Grid<Vec3>& target = *_args.getPtr<Grid<Vec3> >("target",1,&_lock);   _retval = getPyNone(); moveMacToCen(source,target);  _args.check(); } pbFinalizePlugin(parent,"moveMacToCen", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("moveMacToCen",e.what()); return 0; } } static const Pb::Register _RP_moveMacToCen ("","moveMacToCen",_W_6);  extern "C" { void PbRegister_moveMacToCen() { KEEP_UNUSED(_RP_moveMacToCen); } } 

//! vec3->mac grid conversion , but with full resampling 
void resampleVec3ToMac(Grid<Vec3>& source, MACGrid &target ) {
	FOR_IJK_BND(target,1) {
		target(i,j,k)[0] = 0.5*(source(i-1,j,k)[0]+source(i,j,k))[0];
		target(i,j,k)[1] = 0.5*(source(i,j-1,k)[1]+source(i,j,k))[1];
		if(target.is3D()) {
		target(i,j,k)[2] = 0.5*(source(i,j,k-1)[2]+source(i,j,k))[2]; }
	}
} static PyObject* _W_7 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "resampleVec3ToMac" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3>& source = *_args.getPtr<Grid<Vec3> >("source",0,&_lock); MACGrid& target = *_args.getPtr<MACGrid >("target",1,&_lock);   _retval = getPyNone(); resampleVec3ToMac(source,target);  _args.check(); } pbFinalizePlugin(parent,"resampleVec3ToMac", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("resampleVec3ToMac",e.what()); return 0; } } static const Pb::Register _RP_resampleVec3ToMac ("","resampleVec3ToMac",_W_7);  extern "C" { void PbRegister_resampleVec3ToMac() { KEEP_UNUSED(_RP_resampleVec3ToMac); } } 
//! mac->vec3 grid conversion , with full resampling 
void resampleMacToVec3(MACGrid &source, Grid<Vec3>& target ) {
	FOR_IJK_BND(target,1) {
		target(i,j,k) = source.getCentered(i,j,k);
	}
} static PyObject* _W_8 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "resampleMacToVec3" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; MACGrid& source = *_args.getPtr<MACGrid >("source",0,&_lock); Grid<Vec3>& target = *_args.getPtr<Grid<Vec3> >("target",1,&_lock);   _retval = getPyNone(); resampleMacToVec3(source,target);  _args.check(); } pbFinalizePlugin(parent,"resampleMacToVec3", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("resampleMacToVec3",e.what()); return 0; } } static const Pb::Register _RP_resampleMacToVec3 ("","resampleMacToVec3",_W_8);  extern "C" { void PbRegister_resampleMacToVec3() { KEEP_UNUSED(_RP_resampleMacToVec3); } } 


void copyLevelsetToReal(LevelsetGrid &source , Grid<Real> &target) {
	FOR_IJK(target) {
		target(i,j,k) = source(i,j,k);
	}
} static PyObject* _W_9 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "copyLevelsetToReal" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; LevelsetGrid& source = *_args.getPtr<LevelsetGrid >("source",0,&_lock); Grid<Real> & target = *_args.getPtr<Grid<Real>  >("target",1,&_lock);   _retval = getPyNone(); copyLevelsetToReal(source,target);  _args.check(); } pbFinalizePlugin(parent,"copyLevelsetToReal", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("copyLevelsetToReal",e.what()); return 0; } } static const Pb::Register _RP_copyLevelsetToReal ("","copyLevelsetToReal",_W_9);  extern "C" { void PbRegister_copyLevelsetToReal() { KEEP_UNUSED(_RP_copyLevelsetToReal); } } 

void copyVec3ToReal(Grid<Vec3> &source, Grid<Real> &targetX, Grid<Real> &targetY, Grid<Real> &targetZ) {
	FOR_IJK(source) {
		targetX(i,j,k) = source(i,j,k).x;
		targetY(i,j,k) = source(i,j,k).y;
		targetZ(i,j,k) = source(i,j,k).z;
	}
} static PyObject* _W_10 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "copyVec3ToReal" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3> & source = *_args.getPtr<Grid<Vec3>  >("source",0,&_lock); Grid<Real> & targetX = *_args.getPtr<Grid<Real>  >("targetX",1,&_lock); Grid<Real> & targetY = *_args.getPtr<Grid<Real>  >("targetY",2,&_lock); Grid<Real> & targetZ = *_args.getPtr<Grid<Real>  >("targetZ",3,&_lock);   _retval = getPyNone(); copyVec3ToReal(source,targetX,targetY,targetZ);  _args.check(); } pbFinalizePlugin(parent,"copyVec3ToReal", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("copyVec3ToReal",e.what()); return 0; } } static const Pb::Register _RP_copyVec3ToReal ("","copyVec3ToReal",_W_10);  extern "C" { void PbRegister_copyVec3ToReal() { KEEP_UNUSED(_RP_copyVec3ToReal); } } 


void copyRealToVec3(Grid<Real> &sourceX, Grid<Real> &sourceY, Grid<Real> &sourceZ, Grid<Vec3> &target) {
	FOR_IJK(target) {
		target(i,j,k).x = sourceX(i,j,k);
		target(i,j,k).y = sourceY(i,j,k);
		target(i,j,k).z = sourceZ(i,j,k);
	}
} static PyObject* _W_11 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "copyRealToVec3" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Real> & sourceX = *_args.getPtr<Grid<Real>  >("sourceX",0,&_lock); Grid<Real> & sourceY = *_args.getPtr<Grid<Real>  >("sourceY",1,&_lock); Grid<Real> & sourceZ = *_args.getPtr<Grid<Real>  >("sourceZ",2,&_lock); Grid<Vec3> & target = *_args.getPtr<Grid<Vec3>  >("target",3,&_lock);   _retval = getPyNone(); copyRealToVec3(sourceX,sourceY,sourceZ,target);  _args.check(); } pbFinalizePlugin(parent,"copyRealToVec3", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("copyRealToVec3",e.what()); return 0; } } static const Pb::Register _RP_copyRealToVec3 ("","copyRealToVec3",_W_11);  extern "C" { void PbRegister_copyRealToVec3() { KEEP_UNUSED(_RP_copyRealToVec3); } } 
void convertLevelsetToReal(LevelsetGrid &source , Grid<Real> &target) { debMsg("Deprecated - do not use convertLevelsetToReal... use copyLevelsetToReal instead",1); copyLevelsetToReal(source,target); } static PyObject* _W_12 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "convertLevelsetToReal" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; LevelsetGrid& source = *_args.getPtr<LevelsetGrid >("source",0,&_lock); Grid<Real> & target = *_args.getPtr<Grid<Real>  >("target",1,&_lock);   _retval = getPyNone(); convertLevelsetToReal(source,target);  _args.check(); } pbFinalizePlugin(parent,"convertLevelsetToReal", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("convertLevelsetToReal",e.what()); return 0; } } static const Pb::Register _RP_convertLevelsetToReal ("","convertLevelsetToReal",_W_12);  extern "C" { void PbRegister_convertLevelsetToReal() { KEEP_UNUSED(_RP_convertLevelsetToReal); } } 

template<class T> void Grid<T>::printGrid(int zSlice, bool printIndex, int bnd) {
	std::ostringstream out;
	out << std::endl;
	FOR_IJK_BND(*this,bnd) {
		IndexInt idx = (*this).index(i,j,k);
		if((zSlice>=0 && k==zSlice) || (zSlice<0)) { 
			out << " ";
			if(printIndex &&  this->is3D()) out << "  "<<i<<","<<j<<","<<k <<":";
			if(printIndex && !this->is3D()) out << "  "<<i<<","<<j<<":";
			out << (*this)[idx]; 
			if(i==(*this).getSizeX()-1 -bnd) out << std::endl; 
		}
	}
	out << endl; debMsg("Printing " << this->getName() << out.str().c_str() , 1);
}

//! helper to swap components of a grid (eg for data import)
void swapComponents(Grid<Vec3>& vel, int c1=0, int c2=1, int c3=2) {
	FOR_IJK(vel) {
		Vec3 v = vel(i,j,k);
		vel(i,j,k)[0] = v[c1];
		vel(i,j,k)[1] = v[c2];
		vel(i,j,k)[2] = v[c3];
	}
} static PyObject* _W_13 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "swapComponents" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3>& vel = *_args.getPtr<Grid<Vec3> >("vel",0,&_lock); int c1 = _args.getOpt<int >("c1",1,0,&_lock); int c2 = _args.getOpt<int >("c2",2,1,&_lock); int c3 = _args.getOpt<int >("c3",3,2,&_lock);   _retval = getPyNone(); swapComponents(vel,c1,c2,c3);  _args.check(); } pbFinalizePlugin(parent,"swapComponents", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("swapComponents",e.what()); return 0; } } static const Pb::Register _RP_swapComponents ("","swapComponents",_W_13);  extern "C" { void PbRegister_swapComponents() { KEEP_UNUSED(_RP_swapComponents); } } 

// helper functions for UV grid data (stored grid coordinates as Vec3 values, and uv weight in entry zero)

// make uv weight accesible in python
Real getUvWeight(Grid<Vec3> &uv) { return uv[0][0]; } static PyObject* _W_14 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getUvWeight" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3> & uv = *_args.getPtr<Grid<Vec3>  >("uv",0,&_lock);   _retval = toPy(getUvWeight(uv));  _args.check(); } pbFinalizePlugin(parent,"getUvWeight", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getUvWeight",e.what()); return 0; } } static const Pb::Register _RP_getUvWeight ("","getUvWeight",_W_14);  extern "C" { void PbRegister_getUvWeight() { KEEP_UNUSED(_RP_getUvWeight); } } 

// note - right now the UV grids have 0 values at the border after advection... could be fixed with an extrapolation step...

// compute normalized modulo interval
static inline Real computeUvGridTime(Real t, Real resetTime) {
	return fmod( (t / resetTime), (Real)1. );
}
// create ramp function in 0..1 range with half frequency
static inline Real computeUvRamp(Real t) {
	Real uvWeight = 2. * t; 
	if (uvWeight>1.) uvWeight=2.-uvWeight;
	return uvWeight;
}

 struct knResetUvGrid : public KernelBase { knResetUvGrid(Grid<Vec3>& target) :  KernelBase(&target,0) ,target(target)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<Vec3>& target )  { target(i,j,k) = Vec3((Real)i,(Real)j,(Real)k); }   inline Grid<Vec3>& getArg0() { return target; } typedef Grid<Vec3> type0; void runMessage() { debMsg("Executing kernel knResetUvGrid ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,target);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,target);  } }  } Grid<Vec3>& target;   };
#line 449 "grid.cpp"




void resetUvGrid(Grid<Vec3> &target) {
	knResetUvGrid reset(target); // note, llvm complains about anonymous declaration here... ?
} static PyObject* _W_15 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "resetUvGrid" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3> & target = *_args.getPtr<Grid<Vec3>  >("target",0,&_lock);   _retval = getPyNone(); resetUvGrid(target);  _args.check(); } pbFinalizePlugin(parent,"resetUvGrid", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("resetUvGrid",e.what()); return 0; } } static const Pb::Register _RP_resetUvGrid ("","resetUvGrid",_W_15);  extern "C" { void PbRegister_resetUvGrid() { KEEP_UNUSED(_RP_resetUvGrid); } } 

void updateUvWeight(Real resetTime, int index, int numUvs, Grid<Vec3> &uv) {
	const Real t   = uv.getParent()->getTime();
	Real  timeOff  = resetTime/(Real)numUvs;

	Real lastt = computeUvGridTime(t +(Real)index*timeOff - uv.getParent()->getDt(), resetTime);
	Real currt = computeUvGridTime(t +(Real)index*timeOff                          , resetTime);
	Real uvWeight = computeUvRamp(currt);

	// normalize the uvw weights , note: this is a bit wasteful...
	Real uvWTotal = 0.;
	for(int i=0; i<numUvs; ++i) {
		uvWTotal += computeUvRamp( computeUvGridTime(t +(Real)i*timeOff , resetTime) );
	}
	if(uvWTotal<=VECTOR_EPSILON) { uvWeight =  uvWTotal = 1.; }
	else                           uvWeight /= uvWTotal;

	// check for reset
	if( currt < lastt ) 
		knResetUvGrid reset( uv );

	// write new weight value to grid
	uv[0] = Vec3( uvWeight, 0.,0.);

	// print info about uv weights?
	debMsg("Uv grid "<<index<<"/"<<numUvs<< " t="<<currt<<" w="<<uvWeight<<", reset:"<<(int)(currt<lastt) , 2);
} static PyObject* _W_16 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "updateUvWeight" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Real resetTime = _args.get<Real >("resetTime",0,&_lock); int index = _args.get<int >("index",1,&_lock); int numUvs = _args.get<int >("numUvs",2,&_lock); Grid<Vec3> & uv = *_args.getPtr<Grid<Vec3>  >("uv",3,&_lock);   _retval = getPyNone(); updateUvWeight(resetTime,index,numUvs,uv);  _args.check(); } pbFinalizePlugin(parent,"updateUvWeight", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("updateUvWeight",e.what()); return 0; } } static const Pb::Register _RP_updateUvWeight ("","updateUvWeight",_W_16);  extern "C" { void PbRegister_updateUvWeight() { KEEP_UNUSED(_RP_updateUvWeight); } } 

template <class T>  struct knSetBoundary : public KernelBase { knSetBoundary(Grid<T>& grid, T value, int w) :  KernelBase(&grid,0) ,grid(grid),value(value),w(w)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<T>& grid, T value, int w )  { 
	bool bnd = (i<=w || i>=grid.getSizeX()-1-w || j<=w || j>=grid.getSizeY()-1-w || (grid.is3D() && (k<=w || k>=grid.getSizeZ()-1-w)));
	if (bnd) 
		grid(i,j,k) = value;
}   inline Grid<T>& getArg0() { return grid; } typedef Grid<T> type0;inline T& getArg1() { return value; } typedef T type1;inline int& getArg2() { return w; } typedef int type2; void runMessage() { debMsg("Executing kernel knSetBoundary ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } }  } Grid<T>& grid; T value; int w;   };
#line 483 "grid.cpp"



template<class T> void Grid<T>::setBound(T value, int boundaryWidth) {
	knSetBoundary<T>( *this, value, boundaryWidth );
}


template <class T>  struct knSetBoundaryNeumann : public KernelBase { knSetBoundaryNeumann(Grid<T>& grid, int w) :  KernelBase(&grid,0) ,grid(grid),w(w)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<T>& grid, int w )  { 
	bool set = false;
	int  si=i, sj=j, sk=k;
	if( i<=w) {
		si = w+1; set=true;
	}
	if( i>=grid.getSizeX()-1-w){
		si = grid.getSizeX()-1-w-1; set=true;
	}
	if( j<=w){
		sj = w+1; set=true;
	}
	if( j>=grid.getSizeY()-1-w){
		sj = grid.getSizeY()-1-w-1; set=true;
	}
	if( grid.is3D() ){
		 if( k<=w ) {
			sk = w+1; set=true;
		 }
		 if( k>=grid.getSizeZ()-1-w ) {
			sk = grid.getSizeZ()-1-w-1; set=true;
		 }
	}
	if(set)
		grid(i,j,k) = grid(si, sj, sk);
}   inline Grid<T>& getArg0() { return grid; } typedef Grid<T> type0;inline int& getArg1() { return w; } typedef int type1; void runMessage() { debMsg("Executing kernel knSetBoundaryNeumann ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,w);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,w);  } }  } Grid<T>& grid; int w;   };
#line 494 "grid.cpp"



template<class T> void Grid<T>::setBoundNeumann(int boundaryWidth) {
	knSetBoundaryNeumann<T>( *this, boundaryWidth );
}

//! kernel to set velocity components of mac grid to value for a boundary of w cells
 struct knSetBoundaryMAC : public KernelBase { knSetBoundaryMAC(Grid<Vec3>& grid, Vec3 value, int w) :  KernelBase(&grid,0) ,grid(grid),value(value),w(w)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<Vec3>& grid, Vec3 value, int w )  { 
	if (i<=w   || i>=grid.getSizeX()  -w || j<=w-1 || j>=grid.getSizeY()-1-w || (grid.is3D() && (k<=w-1 || k>=grid.getSizeZ()-1-w)))
		grid(i,j,k).x = value.x;
	if (i<=w-1 || i>=grid.getSizeX()-1-w || j<=w   || j>=grid.getSizeY()  -w || (grid.is3D() && (k<=w-1 || k>=grid.getSizeZ()-1-w)))
		grid(i,j,k).y = value.y;
	if (i<=w-1 || i>=grid.getSizeX()-1-w || j<=w-1 || j>=grid.getSizeY()-1-w || (grid.is3D() && (k<=w   || k>=grid.getSizeZ()  -w)))
		grid(i,j,k).z = value.z;
}   inline Grid<Vec3>& getArg0() { return grid; } typedef Grid<Vec3> type0;inline Vec3& getArg1() { return value; } typedef Vec3 type1;inline int& getArg2() { return w; } typedef int type2; void runMessage() { debMsg("Executing kernel knSetBoundaryMAC ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } }  } Grid<Vec3>& grid; Vec3 value; int w;   };
#line 526 "grid.cpp"

 

//! only set normal velocity components of mac grid to value for a boundary of w cells
 struct knSetBoundaryMACNorm : public KernelBase { knSetBoundaryMACNorm(Grid<Vec3>& grid, Vec3 value, int w) :  KernelBase(&grid,0) ,grid(grid),value(value),w(w)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<Vec3>& grid, Vec3 value, int w )  { 
	if (i<=w   || i>=grid.getSizeX()  -w ) grid(i,j,k).x = value.x;
	if (j<=w   || j>=grid.getSizeY()  -w ) grid(i,j,k).y = value.y;
	if ( (grid.is3D() && (k<=w   || k>=grid.getSizeZ()  -w))) grid(i,j,k).z = value.z;
}   inline Grid<Vec3>& getArg0() { return grid; } typedef Grid<Vec3> type0;inline Vec3& getArg1() { return value; } typedef Vec3 type1;inline int& getArg2() { return w; } typedef int type2; void runMessage() { debMsg("Executing kernel knSetBoundaryMACNorm ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,grid,value,w);  } }  } Grid<Vec3>& grid; Vec3 value; int w;   };
#line 536 "grid.cpp"

 

//! set velocity components of mac grid to value for a boundary of w cells (optionally only normal values)
void MACGrid::setBoundMAC(Vec3 value, int boundaryWidth, bool normalOnly) { 
	if(!normalOnly) knSetBoundaryMAC    ( *this, value, boundaryWidth ); 
	else            knSetBoundaryMACNorm( *this, value, boundaryWidth ); 
}

//! various sums of grid data

 struct knGridTotalSum : public KernelBase { knGridTotalSum(const Grid<Real>& a, const FlagGrid* flags) :  KernelBase(&a,0) ,a(a),flags(flags) ,result(0.0)  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Real>& a, const FlagGrid* flags ,double& result)  {
	if(flags) { if(flags->isFluid(idx)) result += a[idx]; }
	else      { result += a[idx]; }
}    inline operator double () { return result; } inline double  & getRet() { return result; }  inline const Grid<Real>& getArg0() { return a; } typedef Grid<Real> type0;inline const FlagGrid* getArg1() { return flags; } typedef FlagGrid type1; void runMessage() { debMsg("Executing kernel knGridTotalSum ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  double result = 0.0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,a,flags,result); 
#pragma omp critical
{this->result += result; } }   } const Grid<Real>& a; const FlagGrid* flags;  double result;  };
#line 550 "grid.cpp"



 struct knGridTotalMagSum : public KernelBase { knGridTotalMagSum(const Grid<Real>& a, const FlagGrid* flags) :  KernelBase(&a,0) ,a(a),flags(flags) ,result(0.0)  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Real>& a, const FlagGrid* flags ,double& result)  {
	if(flags) { if(flags->isFluid(idx)) result += std::fabs(a[idx]); }
	else      { result += std::fabs(a[idx]); }
}    inline operator double () { return result; } inline double  & getRet() { return result; }  inline const Grid<Real>& getArg0() { return a; } typedef Grid<Real> type0;inline const FlagGrid* getArg1() { return flags; } typedef FlagGrid type1; void runMessage() { debMsg("Executing kernel knGridTotalMagSum ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  double result = 0.0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,a,flags,result); 
#pragma omp critical
{this->result += result; } }   } const Grid<Real>& a; const FlagGrid* flags;  double result;  };
#line 555 "grid.cpp"



 struct knGridTotalVecMagSum : public KernelBase { knGridTotalVecMagSum(const Grid<Vec3>& a, const FlagGrid* flags) :  KernelBase(&a,0) ,a(a),flags(flags) ,result(0.0)  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Vec3>& a, const FlagGrid* flags ,double& result)  {
	if(flags) { if(flags->isFluid(idx)) result += norm(a[idx]); }
	else      { result += norm(a[idx]); }
}    inline operator double () { return result; } inline double  & getRet() { return result; }  inline const Grid<Vec3>& getArg0() { return a; } typedef Grid<Vec3> type0;inline const FlagGrid* getArg1() { return flags; } typedef FlagGrid type1; void runMessage() { debMsg("Executing kernel knGridTotalVecMagSum ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  double result = 0.0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,a,flags,result); 
#pragma omp critical
{this->result += result; } }   } const Grid<Vec3>& a; const FlagGrid* flags;  double result;  };
#line 560 "grid.cpp"



 struct knGridTotalSqrSum : public KernelBase { knGridTotalSqrSum(const Grid<Vec3>& a, const FlagGrid* flags) :  KernelBase(&a,0) ,a(a),flags(flags) ,result(0.0)  { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Vec3>& a, const FlagGrid* flags ,double& result)  {
	if(flags) { if(flags->isFluid(idx)) result += normSquare(a[idx]); }
	else      { result += normSquare(a[idx]); }
}    inline operator double () { return result; } inline double  & getRet() { return result; }  inline const Grid<Vec3>& getArg0() { return a; } typedef Grid<Vec3> type0;inline const FlagGrid* getArg1() { return flags; } typedef FlagGrid type1; void runMessage() { debMsg("Executing kernel knGridTotalSqrSum ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  double result = 0.0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,a,flags,result); 
#pragma omp critical
{this->result += result; } }   } const Grid<Vec3>& a; const FlagGrid* flags;  double result;  };
#line 565 "grid.cpp"


Real getGridTotalSum(const Grid<Real>& a, const FlagGrid* flags=NULL) { return knGridTotalSum(a, flags); } static PyObject* _W_17 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridTotalSum" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Real>& a = *_args.getPtr<Grid<Real> >("a",0,&_lock); const FlagGrid* flags = _args.getPtrOpt<FlagGrid >("flags",1,NULL,&_lock);   _retval = toPy(getGridTotalSum(a,flags));  _args.check(); } pbFinalizePlugin(parent,"getGridTotalSum", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridTotalSum",e.what()); return 0; } } static const Pb::Register _RP_getGridTotalSum ("","getGridTotalSum",_W_17);  extern "C" { void PbRegister_getGridTotalSum() { KEEP_UNUSED(_RP_getGridTotalSum); } } 
Real getGridTotalMagSum(const Grid<Real>& a, const FlagGrid* flags=NULL) { return knGridTotalMagSum(a, flags); } static PyObject* _W_18 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridTotalMagSum" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Real>& a = *_args.getPtr<Grid<Real> >("a",0,&_lock); const FlagGrid* flags = _args.getPtrOpt<FlagGrid >("flags",1,NULL,&_lock);   _retval = toPy(getGridTotalMagSum(a,flags));  _args.check(); } pbFinalizePlugin(parent,"getGridTotalMagSum", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridTotalMagSum",e.what()); return 0; } } static const Pb::Register _RP_getGridTotalMagSum ("","getGridTotalMagSum",_W_18);  extern "C" { void PbRegister_getGridTotalMagSum() { KEEP_UNUSED(_RP_getGridTotalMagSum); } } 
Real getGridTotalVecMagSum(const Grid<Vec3>& a, const FlagGrid* flags=NULL) { return knGridTotalVecMagSum(a, flags); } static PyObject* _W_19 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridTotalVecMagSum" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Vec3>& a = *_args.getPtr<Grid<Vec3> >("a",0,&_lock); const FlagGrid* flags = _args.getPtrOpt<FlagGrid >("flags",1,NULL,&_lock);   _retval = toPy(getGridTotalVecMagSum(a,flags));  _args.check(); } pbFinalizePlugin(parent,"getGridTotalVecMagSum", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridTotalVecMagSum",e.what()); return 0; } } static const Pb::Register _RP_getGridTotalVecMagSum ("","getGridTotalVecMagSum",_W_19);  extern "C" { void PbRegister_getGridTotalVecMagSum() { KEEP_UNUSED(_RP_getGridTotalVecMagSum); } } 
Real getGridTotalSqrSum(const Grid<Vec3>& a, const FlagGrid* flags=NULL) { return knGridTotalSqrSum(a, flags); } static PyObject* _W_20 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridTotalSqrSum" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Vec3>& a = *_args.getPtr<Grid<Vec3> >("a",0,&_lock); const FlagGrid* flags = _args.getPtrOpt<FlagGrid >("flags",1,NULL,&_lock);   _retval = toPy(getGridTotalSqrSum(a,flags));  _args.check(); } pbFinalizePlugin(parent,"getGridTotalSqrSum", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridTotalSqrSum",e.what()); return 0; } } static const Pb::Register _RP_getGridTotalSqrSum ("","getGridTotalSqrSum",_W_20);  extern "C" { void PbRegister_getGridTotalSqrSum() { KEEP_UNUSED(_RP_getGridTotalSqrSum); } } 


 struct knCountFluidCells : public KernelBase { knCountFluidCells(const FlagGrid& flags) :  KernelBase(&flags,0) ,flags(flags) ,numEmpty(0)  { runMessage(); run(); }   inline void op(IndexInt idx, const FlagGrid& flags ,int& numEmpty)  { if (flags.isFluid(idx) ) numEmpty++; }    inline operator int () { return numEmpty; } inline int  & getRet() { return numEmpty; }  inline const FlagGrid& getArg0() { return flags; } typedef FlagGrid type0; void runMessage() { debMsg("Executing kernel knCountFluidCells ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  int numEmpty = 0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,flags,numEmpty); 
#pragma omp critical
{this->numEmpty += numEmpty; } }   } const FlagGrid& flags;  int numEmpty;  };
#line 575 "grid.cpp"


int getCountFluidCells(const FlagGrid& flags) { return knCountFluidCells(flags); } static PyObject* _W_21 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getCountFluidCells" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",0,&_lock);   _retval = toPy(getCountFluidCells(flags));  _args.check(); } pbFinalizePlugin(parent,"getCountFluidCells", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getCountFluidCells",e.what()); return 0; } } static const Pb::Register _RP_getCountFluidCells ("","getCountFluidCells",_W_21);  extern "C" { void PbRegister_getCountFluidCells() { KEEP_UNUSED(_RP_getCountFluidCells); } } 

//! averaged value for all cells (if flags are given, only for fluid cells)

Real getGridAvg(Grid<Real>& source, FlagGrid* flags=NULL) {
	double sum = knGridTotalSum(source, flags);

	double cells;
	if(flags) { cells = knCountFluidCells(*flags); }
	else      { cells = source.getSizeX()*source.getSizeY()*source.getSizeZ(); }

	if(cells>0.) sum *= 1./cells;
	else         sum = -1.;
	return sum;
} static PyObject* _W_22 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridAvg" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Real>& source = *_args.getPtr<Grid<Real> >("source",0,&_lock); FlagGrid* flags = _args.getPtrOpt<FlagGrid >("flags",1,NULL,&_lock);   _retval = toPy(getGridAvg(source,flags));  _args.check(); } pbFinalizePlugin(parent,"getGridAvg", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridAvg",e.what()); return 0; } } static const Pb::Register _RP_getGridAvg ("","getGridAvg",_W_22);  extern "C" { void PbRegister_getGridAvg() { KEEP_UNUSED(_RP_getGridAvg); } } 

//! transfer data between real and vec3 grids

 struct knGetComponent : public KernelBase { knGetComponent(const Grid<Vec3>& source, Grid<Real>& target, int component) :  KernelBase(&source,0) ,source(source),target(target),component(component)   { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Vec3>& source, Grid<Real>& target, int component )  {
	target[idx] = source[idx][component]; 
}    inline const Grid<Vec3>& getArg0() { return source; } typedef Grid<Vec3> type0;inline Grid<Real>& getArg1() { return target; } typedef Grid<Real> type1;inline int& getArg2() { return component; } typedef int type2; void runMessage() { debMsg("Executing kernel knGetComponent ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,source,target,component);  }   } const Grid<Vec3>& source; Grid<Real>& target; int component;   };
#line 594 "grid.cpp"


void getComponent(const Grid<Vec3>& source, Grid<Real>& target, int component) { knGetComponent(source, target, component); } static PyObject* _W_23 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getComponent" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Vec3>& source = *_args.getPtr<Grid<Vec3> >("source",0,&_lock); Grid<Real>& target = *_args.getPtr<Grid<Real> >("target",1,&_lock); int component = _args.get<int >("component",2,&_lock);   _retval = getPyNone(); getComponent(source,target,component);  _args.check(); } pbFinalizePlugin(parent,"getComponent", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getComponent",e.what()); return 0; } } static const Pb::Register _RP_getComponent ("","getComponent",_W_23);  extern "C" { void PbRegister_getComponent() { KEEP_UNUSED(_RP_getComponent); } } 

 struct knSetComponent : public KernelBase { knSetComponent(const Grid<Real>& source, Grid<Vec3>& target, int component) :  KernelBase(&source,0) ,source(source),target(target),component(component)   { runMessage(); run(); }   inline void op(IndexInt idx, const Grid<Real>& source, Grid<Vec3>& target, int component )  {
	target[idx][component] = source[idx]; 
}    inline const Grid<Real>& getArg0() { return source; } typedef Grid<Real> type0;inline Grid<Vec3>& getArg1() { return target; } typedef Grid<Vec3> type1;inline int& getArg2() { return component; } typedef int type2; void runMessage() { debMsg("Executing kernel knSetComponent ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,source,target,component);  }   } const Grid<Real>& source; Grid<Vec3>& target; int component;   };
#line 599 "grid.cpp"


void setComponent(const Grid<Real>& source, Grid<Vec3>& target, int component) { knSetComponent(source, target, component); } static PyObject* _W_24 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "setComponent" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Grid<Real>& source = *_args.getPtr<Grid<Real> >("source",0,&_lock); Grid<Vec3>& target = *_args.getPtr<Grid<Vec3> >("target",1,&_lock); int component = _args.get<int >("component",2,&_lock);   _retval = getPyNone(); setComponent(source,target,component);  _args.check(); } pbFinalizePlugin(parent,"setComponent", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("setComponent",e.what()); return 0; } } static const Pb::Register _RP_setComponent ("","setComponent",_W_24);  extern "C" { void PbRegister_setComponent() { KEEP_UNUSED(_RP_setComponent); } } 

//******************************************************************************
// Specialization classes

void FlagGrid::InitMinXWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(i - w - .5, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::InitMaxXWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(mSize.x-i-1.5-w, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::InitMinYWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(j - w - .5, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::InitMaxYWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(mSize.y-j-1.5-w, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::InitMinZWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(k - w - .5, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::InitMaxZWall(const int &boundaryWidth, Grid<Real>& phiWalls) {
	const int w = boundaryWidth;
	FOR_IJK(phiWalls) {
		phiWalls(i,j,k) = std::min(mSize.z-k-1.5-w, (double)phiWalls(i,j,k));
	}
}

void FlagGrid::initDomain(const int &boundaryWidth,
			  const string &wallIn,
			  const string &openIn,
			  const string &inflowIn,
			  const string &outflowIn,
			  Grid<Real>* phiWalls) {
	int  types[6] = {0};
	bool set  [6] = {false};
	// make sure we have at least 6 entries
	string wall    = wallIn;    wall.append("      ");
	string open    = openIn;    open.append("      ");
	string inflow  = inflowIn;  inflow.append("      ");
	string outflow = outflowIn; outflow.append("      ");

	if(phiWalls) phiWalls->setConst(1000000000);

	for (char i = 0; i<6; ++i) {
		//min x-direction
		if(!set[0]) {
			if(open[i]=='x')         {types[0] = TypeOpen;set[0] = true;}
			else if(inflow[i]=='x')  {types[0] = TypeInflow;set[0] = true;}
			else if(outflow[i]=='x') {types[0] = TypeOutflow;set[0] = true;}
			else if(wall[i]=='x') {
				types[0]    = TypeObstacle;
				if(phiWalls) InitMinXWall(boundaryWidth, *phiWalls);
				set[0] = true;
			}			
		}
		//max x-direction
		if(!set[1]) {
			if(open[i]=='X')         {types[1] = TypeOpen;set[1] = true;}
			else if(inflow[i]=='X')  {types[1] = TypeInflow;set[1] = true;}
			else if(outflow[i]=='X') {types[1] = TypeOutflow;set[1] = true;}
			else if(wall[i]=='X')  {
				types[1]    = TypeObstacle;
				if(phiWalls) InitMaxXWall(boundaryWidth, *phiWalls);
				set[1] = true;
			}			
		}
		//min y-direction
		if(!set[2]) {
			if(open[i]=='y')         {types[2] = TypeOpen;set[2] = true;}
			else if(inflow[i]=='y')  {types[2] = TypeInflow;set[2] = true;}
			else if(outflow[i]=='y') {types[2] = TypeOutflow;set[2] = true;}
			else if(wall[i]=='y') {
				types[2]    = TypeObstacle;
				if(phiWalls) InitMinYWall(boundaryWidth, *phiWalls);
				set[2] = true;
			}			
		}
		//max y-direction
		if(!set[3]) {
			if(open[i]=='Y')         {types[3] = TypeOpen;set[3] = true;}
			else if(inflow[i]=='Y')  {types[3] = TypeInflow;set[3] = true;}
			else if(outflow[i]=='Y') {types[3] = TypeOutflow;set[3] = true;}
			else if(wall[i]=='Y') {
				types[3]    = TypeObstacle;
				if(phiWalls) InitMaxYWall(boundaryWidth, *phiWalls);
				set[3] = true;
			}			
		}
		if(this->is3D()) {
		//min z-direction
			if(!set[4]) {
				if(open[i]=='z')         {types[4] = TypeOpen;set[4] = true;}
				else if(inflow[i]=='z')  {types[4] = TypeInflow;set[4] = true;}
				else if(outflow[i]=='z') {types[4] = TypeOutflow;set[4] = true;}
				else if(wall[i]=='z') {
					types[4]    = TypeObstacle;
					if(phiWalls) InitMinZWall(boundaryWidth, *phiWalls);
					set[4] = true;
				}				
			}
			//max z-direction
			if(!set[5]) {
				if(open[i]=='Z')         {types[5] = TypeOpen;set[5] = true;}
				else if(inflow[i]=='Z')  {types[5] = TypeInflow;set[5] = true;}
				else if(outflow[i]=='Z') {types[5] = TypeOutflow;set[5] = true;}
				else if(wall[i]=='Z') {
					types[5]    = TypeObstacle;
					if(phiWalls) InitMaxZWall(boundaryWidth, *phiWalls);
					set[5] = true;
				}				
			}
		}
	}

	setConst(TypeEmpty); 
	initBoundaries(boundaryWidth, types); 
}

void FlagGrid::initBoundaries(const int &boundaryWidth, const int *types) {
	const int w = boundaryWidth;
	FOR_IJK(*this) {
		bool bnd = (i <= w);
		if (bnd) mData[index(i,j,k)] = types[0];
		bnd = (i >= mSize.x-1-w);
		if (bnd) mData[index(i,j,k)] = types[1];
		bnd = (j <= w);
		if (bnd) mData[index(i,j,k)] = types[2];
		bnd = (j >= mSize.y-1-w);
		if (bnd) mData[index(i,j,k)] = types[3];
		if(is3D()) {
			bnd = (k <= w);
			if (bnd) mData[index(i,j,k)] = types[4];
			bnd = (k >= mSize.z-1-w);
			if (bnd) mData[index(i,j,k)] = types[5];
		}
	}
}

void FlagGrid::minifyFrom(const FlagGrid &flags, const Vec3i &scale) {
	FOR_IJK(*this) {
		if(isObstacle(i,j,k)) continue;
		bool fluidP = false;
		for(int si=0; si<scale.x; ++si) {
			for(int sj=0; sj<scale.y; ++sj) {
				for(int sk=0; sk<(is3D() ? scale.z : 1); ++sk) {
					fluidP |= flags.isFluid(i*scale.x+si, j*scale.y+sj, k*scale.z+sk);
				}
			}
		}

		(*this)(i,j,k) &= ~(TypeEmpty | TypeFluid); // clear empty/fluid flags
		(*this)(i,j,k) |= (fluidP) ? TypeFluid : TypeEmpty; // set resepctive flag
	}
}

void FlagGrid::updateFromLevelset(const LevelsetGrid& levelset) {
	if(getSize()!=levelset.getSize()) return updateFromLevelsetNonMatched(levelset);
	FOR_IDX(*this) {
		if (!isObstacle(idx) && !isOutflow(idx)) {
			const Real phi = levelset[idx];
			if (phi <= levelset.invalidTimeValue()) continue;
			
			mData[idx] &= ~(TypeEmpty | TypeFluid); // clear empty/fluid flags
			mData[idx] |= (phi <= 0) ? TypeFluid : TypeEmpty; // set resepctive flag
		}
	}
}



 struct knUpdateFromLevelsetNonMatched : public KernelBase { knUpdateFromLevelsetNonMatched(FlagGrid &flags, const LevelsetGrid &levelset, const Vec3 &factor, const Vec3 &offset, const int orderSpace) :  KernelBase(&flags,0) ,flags(flags),levelset(levelset),factor(factor),offset(offset),orderSpace(orderSpace)   { runMessage(); run(); }  inline void op(int i, int j, int k, FlagGrid &flags, const LevelsetGrid &levelset, const Vec3 &factor, const Vec3 &offset, const int orderSpace )  {
	if(flags.isObstacle(i,j,k)) return;
	Vec3 pos = Vec3(i,j,k)*factor + offset;
	if(!flags.is3D()) pos[2] = 0; // allow 2d -> 3d
	const Real phi = levelset.getInterpolatedHi(pos, orderSpace);
	if(phi <= levelset.invalidTimeValue()) return;

	flags(i,j,k) &= ~(FlagGrid::TypeEmpty | FlagGrid::TypeFluid); // clear empty/fluid flags
	flags(i,j,k) |= (phi <= 0) ? FlagGrid::TypeFluid : FlagGrid::TypeEmpty; // set resepctive flag
}   inline FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const LevelsetGrid& getArg1() { return levelset; } typedef LevelsetGrid type1;inline const Vec3& getArg2() { return factor; } typedef Vec3 type2;inline const Vec3& getArg3() { return offset; } typedef Vec3 type3;inline const int& getArg4() { return orderSpace; } typedef int type4; void runMessage() { debMsg("Executing kernel knUpdateFromLevelsetNonMatched ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,levelset,factor,offset,orderSpace);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,flags,levelset,factor,offset,orderSpace);  } }  } FlagGrid& flags; const LevelsetGrid& levelset; const Vec3& factor; const Vec3& offset; const int orderSpace;   };
#line 792 "grid.cpp"



void FlagGrid::updateFromLevelsetNonMatched(const LevelsetGrid &levelset, const int orderSpace) {
	const Vec3 sourceFactor = calcGridSizeFactor(levelset.getSize(), getSize());
	const Vec3 offset       = sourceFactor*0.5;
	knUpdateFromLevelsetNonMatched(*this, levelset, sourceFactor, offset, orderSpace);
}

void FlagGrid::fillGrid(int type) {
	FOR_IDX(*this) {
		if ((mData[idx] & TypeObstacle)==0 && (mData[idx] & TypeInflow)==0&& (mData[idx] & TypeOutflow)==0&& (mData[idx] & TypeOpen)==0)
			mData[idx] = (mData[idx] & ~(TypeEmpty | TypeFluid)) | type;
	}
}

void FlagGrid::extendRegion(const int region, const int exclude, const int depth) {
	const int I=getSizeX()-1, J=getSizeY()-1, K=getSizeZ()-1;
	for(int i_depth=0; i_depth<depth; ++i_depth) {
		std::vector<int> update;
		FOR_IJK(*this) {
			if(get(i, j, k) & exclude) continue;
			if((i>0 && (get(i-1, j, k)&region)) || (i<I && (get(i+1, j, k)&region)) ||
			   (j>0 && (get(i, j-1, k)&region)) || (j<J && (get(i, j+1, k)&region)) ||
			   (is3D() && ((k>0 && (get(i, j, k-1)&region)) || (k<K && (get(i, j, k+1)&region)))))
				update.push_back(index(i, j, k));
		}

		for(std::vector<int>::const_iterator it=update.begin(); it!=update.end(); ++it) {
			mData[*it] = region;
		}
	}
}

void floodFillDfs(Grid<int> &r, const FlagGrid &flags, const IndexInt idx, const int c, const int type) {
	r(idx) = c;
	if((flags(idx-flags.getStrideX()) & type) && !r[idx-flags.getStrideX()]) floodFillDfs(r, flags, idx-flags.getStrideX(), c, type);
	if((flags(idx+flags.getStrideX()) & type) && !r[idx+flags.getStrideX()]) floodFillDfs(r, flags, idx+flags.getStrideX(), c, type);
	if((flags(idx-flags.getStrideY()) & type) && !r[idx-flags.getStrideY()]) floodFillDfs(r, flags, idx-flags.getStrideY(), c, type);
	if((flags(idx+flags.getStrideY()) & type) && !r[idx+flags.getStrideY()]) floodFillDfs(r, flags, idx+flags.getStrideY(), c, type);
	if(!flags.is3D()) return;
	if((flags(idx-flags.getStrideZ()) & type) && !r[idx-flags.getStrideZ()]) floodFillDfs(r, flags, idx-flags.getStrideZ(), c, type);
	if((flags(idx+flags.getStrideZ()) & type) && !r[idx+flags.getStrideZ()]) floodFillDfs(r, flags, idx+flags.getStrideZ(), c, type);
}

int getRegionsDfs(Grid<int> &r, const FlagGrid &flags, const int ctype) {
	r.clear();
	int n_regions = 0;

	FOR_IDX(flags) {
		if((flags(idx) & ctype) && !r(idx)) floodFillDfs(r, flags, idx, ++n_regions, ctype);
	}
	return n_regions;
} static PyObject* _W_25 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getRegionsDfs" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<int> & r = *_args.getPtr<Grid<int>  >("r",0,&_lock); const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",1,&_lock); const int ctype = _args.get<int >("ctype",2,&_lock);   _retval = toPy(getRegionsDfs(r,flags,ctype));  _args.check(); } pbFinalizePlugin(parent,"getRegionsDfs", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getRegionsDfs",e.what()); return 0; } } static const Pb::Register _RP_getRegionsDfs ("","getRegionsDfs",_W_25);  extern "C" { void PbRegister_getRegionsDfs() { KEEP_UNUSED(_RP_getRegionsDfs); } } 

int getRegions(Grid<int> &r, const FlagGrid &flags, const int ctype) {
	r.clear();
	int n_regions = 0;
	const IndexInt maxIdx = flags.getSizeX()*flags.getSizeY()*flags.getSizeZ();
	const IndexInt strides[] = {
		-flags.getStrideX(), flags.getStrideX(),
		-flags.getStrideY(), flags.getStrideY(),
		-flags.getStrideZ(), flags.getStrideZ()
	};
	const int K = 2*(flags.is3D() ? 3 : 2);

	FOR_IDX(flags) {
		if((flags(idx) & ctype) && !r(idx)) {
			++n_regions;
			std::queue<IndexInt> q;
			q.push(idx);
			while(!q.empty()) {
				const IndexInt it = q.front(); q.pop();
				r(it) = n_regions;
				for(int k=0; k<K; ++k) {
					const IndexInt nidx = it + strides[k];
					if((nidx<0) || (nidx>=maxIdx) || (!(flags(nidx) & ctype)) || r(nidx)) continue;
					r(nidx) = n_regions;
					q.push(nidx);
				}
			}
		}
	}
	return n_regions;
} static PyObject* _W_26 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getRegions" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<int> & r = *_args.getPtr<Grid<int>  >("r",0,&_lock); const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",1,&_lock); const int ctype = _args.get<int >("ctype",2,&_lock);   _retval = toPy(getRegions(r,flags,ctype));  _args.check(); } pbFinalizePlugin(parent,"getRegions", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getRegions",e.what()); return 0; } } static const Pb::Register _RP_getRegions ("","getRegions",_W_26);  extern "C" { void PbRegister_getRegions() { KEEP_UNUSED(_RP_getRegions); } } 

int getRegionalCounts(Grid<int> &r, const FlagGrid &flags, const int ctype) {
	const int n_regions = getRegions(r, flags, ctype);
	std::vector<int> cnt(n_regions+1, 0);
	FOR_IDX(flags) {
		if(r[idx]>0) ++(cnt[r[idx]]);
	}
	FOR_IDX(flags) {
		r[idx] = cnt[r[idx]];
	}
	return n_regions;
} static PyObject* _W_27 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getRegionalCounts" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<int> & r = *_args.getPtr<Grid<int>  >("r",0,&_lock); const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",1,&_lock); const int ctype = _args.get<int >("ctype",2,&_lock);   _retval = toPy(getRegionalCounts(r,flags,ctype));  _args.check(); } pbFinalizePlugin(parent,"getRegionalCounts", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getRegionalCounts",e.what()); return 0; } } static const Pb::Register _RP_getRegionalCounts ("","getRegionalCounts",_W_27);  extern "C" { void PbRegister_getRegionalCounts() { KEEP_UNUSED(_RP_getRegionalCounts); } } 

bool isIsolatedFluidCell(const IndexInt idx, const FlagGrid &flags) {
	if(!flags.isFluid(idx)) return false;
	if(flags.isFluid(idx-flags.getStrideX())) return false;
	if(flags.isFluid(idx+flags.getStrideX())) return false;
	if(flags.isFluid(idx-flags.getStrideY())) return false;
	if(flags.isFluid(idx+flags.getStrideY())) return false;
	if(!flags.is3D()) return true;
	if(flags.isFluid(idx-flags.getStrideZ())) return false;
	if(flags.isFluid(idx+flags.getStrideZ())) return false;
	return true;
}


 struct knMarkIsolatedFluidCell : public KernelBase { knMarkIsolatedFluidCell(FlagGrid &flags, const int mark) :  KernelBase(&flags,0) ,flags(flags),mark(mark)   { runMessage(); run(); }   inline void op(IndexInt idx, FlagGrid &flags, const int mark )  {
	if(isIsolatedFluidCell(idx, flags)) flags[idx] = mark;
}    inline FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const int& getArg1() { return mark; } typedef int type1; void runMessage() { debMsg("Executing kernel knMarkIsolatedFluidCell ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,flags,mark);  }   } FlagGrid& flags; const int mark;   };
#line 911 "grid.cpp"




void markIsolatedFluidCell(FlagGrid &flags, const int mark) {
	knMarkIsolatedFluidCell(flags, mark);
} static PyObject* _W_28 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "markIsolatedFluidCell" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",0,&_lock); const int mark = _args.get<int >("mark",1,&_lock);   _retval = getPyNone(); markIsolatedFluidCell(flags,mark);  _args.check(); } pbFinalizePlugin(parent,"markIsolatedFluidCell", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("markIsolatedFluidCell",e.what()); return 0; } } static const Pb::Register _RP_markIsolatedFluidCell ("","markIsolatedFluidCell",_W_28);  extern "C" { void PbRegister_markIsolatedFluidCell() { KEEP_UNUSED(_RP_markIsolatedFluidCell); } } 


 struct knMarkSmallRegions : public KernelBase { knMarkSmallRegions(FlagGrid &flags, const Grid<int> &rcnt, const int mark, const int exclude, const int th) :  KernelBase(&flags,0) ,flags(flags),rcnt(rcnt),mark(mark),exclude(exclude),th(th)   { runMessage(); run(); }   inline void op(IndexInt idx, FlagGrid &flags, const Grid<int> &rcnt, const int mark, const int exclude, const int th )  {
	if(flags[idx] & exclude) return;
	if(rcnt[idx] <= th) flags[idx] = mark;
}    inline FlagGrid& getArg0() { return flags; } typedef FlagGrid type0;inline const Grid<int> & getArg1() { return rcnt; } typedef Grid<int>  type1;inline const int& getArg2() { return mark; } typedef int type2;inline const int& getArg3() { return exclude; } typedef int type3;inline const int& getArg4() { return th; } typedef int type4; void runMessage() { debMsg("Executing kernel knMarkSmallRegions ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,flags,rcnt,mark,exclude,th);  }   } FlagGrid& flags; const Grid<int> & rcnt; const int mark; const int exclude; const int th;   };
#line 921 "grid.cpp"




void markSmallRegions(FlagGrid &flags, const Grid<int> &rcnt, const int mark, const int exclude, const int th=1) {
	knMarkSmallRegions(flags, rcnt, mark, exclude, th);
} static PyObject* _W_29 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "markSmallRegions" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",0,&_lock); const Grid<int> & rcnt = *_args.getPtr<Grid<int>  >("rcnt",1,&_lock); const int mark = _args.get<int >("mark",2,&_lock); const int exclude = _args.get<int >("exclude",3,&_lock); const int th = _args.getOpt<int >("th",4,1,&_lock);   _retval = getPyNone(); markSmallRegions(flags,rcnt,mark,exclude,th);  _args.check(); } pbFinalizePlugin(parent,"markSmallRegions", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("markSmallRegions",e.what()); return 0; } } static const Pb::Register _RP_markSmallRegions ("","markSmallRegions",_W_29);  extern "C" { void PbRegister_markSmallRegions() { KEEP_UNUSED(_RP_markSmallRegions); } } 

void getGradientGrid(Grid<Vec3> &gradient, const Grid<Real> &grid) {
	GradientOp(gradient, grid);
} static PyObject* _W_30 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGradientGrid" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Vec3> & gradient = *_args.getPtr<Grid<Vec3>  >("gradient",0,&_lock); const Grid<Real> & grid = *_args.getPtr<Grid<Real>  >("grid",1,&_lock);   _retval = getPyNone(); getGradientGrid(gradient,grid);  _args.check(); } pbFinalizePlugin(parent,"getGradientGrid", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGradientGrid",e.what()); return 0; } } static const Pb::Register _RP_getGradientGrid ("","getGradientGrid",_W_30);  extern "C" { void PbRegister_getGradientGrid() { KEEP_UNUSED(_RP_getGradientGrid); } } 

void getDivergenceMAC(Grid<Real> &divergence, const MACGrid &grid) {
	DivergenceOpMAC(divergence, grid);
} static PyObject* _W_31 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getDivergenceMAC" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Real> & divergence = *_args.getPtr<Grid<Real>  >("divergence",0,&_lock); const MACGrid& grid = *_args.getPtr<MACGrid >("grid",1,&_lock);   _retval = getPyNone(); getDivergenceMAC(divergence,grid);  _args.check(); } pbFinalizePlugin(parent,"getDivergenceMAC", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getDivergenceMAC",e.what()); return 0; } } static const Pb::Register _RP_getDivergenceMAC ("","getDivergenceMAC",_W_31);  extern "C" { void PbRegister_getDivergenceMAC() { KEEP_UNUSED(_RP_getDivergenceMAC); } } 

void getLaplacian(Grid<Real> &laplacian, const Grid<Real> &grid) {
	LaplaceOp(laplacian, grid);
} static PyObject* _W_32 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getLaplacian" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; Grid<Real> & laplacian = *_args.getPtr<Grid<Real>  >("laplacian",0,&_lock); const Grid<Real> & grid = *_args.getPtr<Grid<Real>  >("grid",1,&_lock);   _retval = getPyNone(); getLaplacian(laplacian,grid);  _args.check(); } pbFinalizePlugin(parent,"getLaplacian", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getLaplacian",e.what()); return 0; } } static const Pb::Register _RP_getLaplacian ("","getLaplacian",_W_32);  extern "C" { void PbRegister_getLaplacian() { KEEP_UNUSED(_RP_getLaplacian); } } 

// explicit instantiation
template class Grid<int>;
template class Grid<Real>;
template class Grid<Vec3>;

} //namespace


