




// DO NOT EDIT !
// This file is generated using the MantaFlow preprocessor (prep generate).




#line 1 "/home/vishrut/Study/USC/Sem-1/CSCI 596: Scientific Computing and Visualization/project/mlflip/source/grid.h"
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

#ifndef _GRID_H
#define _GRID_H

#include "manta.h"
#include "vectorbase.h"
#include "interpol.h"
#include "interpolHigh.h"
#include "kernel.h"

namespace Manta {
class LevelsetGrid;
	
//! Base class for all grids
class GridBase : public PbClass {public:
	enum GridType { TypeNone = 0, TypeReal = 1, TypeInt = 2, TypeVec3 = 4, TypeMAC = 8, TypeLevelset = 16, TypeFlags = 32 };
		
	GridBase(FluidSolver* parent); static int _W_0 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { PbClass* obj = Pb::objFromPy(_self); if (obj) delete obj; try { PbArgs _args(_linargs, _kwds); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(0, "GridBase::GridBase" , !noTiming ); { ArgLocker _lock; FluidSolver* parent = _args.getPtr<FluidSolver >("parent",0,&_lock);  obj = new GridBase(parent); obj->registerObject(_self, &_args); _args.check(); } pbFinalizePlugin(obj->getParent(),"GridBase::GridBase" , !noTiming ); return 0; } catch(std::exception& e) { pbSetError("GridBase::GridBase",e.what()); return -1; } }
	
	//! Get the grids X dimension
	inline int getSizeX() const { return mSize.x; }
	//! Get the grids Y dimension
	inline int getSizeY() const { return mSize.y; }
	//! Get the grids Z dimension
	inline int getSizeZ() const { return mSize.z; }
	//! Get the grids dimensions
	inline Vec3i getSize() const { return mSize; }
	
	//! Get Stride in X dimension
	inline IndexInt getStrideX() const { return 1; }
	//! Get Stride in Y dimension
	inline IndexInt getStrideY() const { return mSize.x; }
	//! Get Stride in Z dimension
	inline IndexInt getStrideZ() const { return mStrideZ; }
	
	inline Real getDx() const { return mDx; }
	
	//! Check if indices are within bounds, otherwise error (should only be called when debugging)
	inline void checkIndex(int i, int j, int k) const;
	//! Check if indices are within bounds, otherwise error (should only be called when debugging)
	inline void checkIndex(IndexInt idx) const;
	//! Check if index is within given boundaries
	inline bool isInBounds(const Vec3i& p, int bnd) const;
	//! Check if index is within given boundaries
	inline bool isInBounds(const Vec3i& p) const;
	//! Check if index is within given boundaries
	inline bool isInBounds(const Vec3& p, int bnd = 0) const { return isInBounds(toVec3i(p), bnd); }
	//! Check if linear index is in the range of the array
	inline bool isInBounds(IndexInt idx) const;
	
	//! Get the type of grid
	inline GridType getType() const { return mType; }
	//! Check dimensionality
	inline bool is3D() const { return m3D; }
	
	//! Get index into the data
	inline IndexInt index(int i, int j, int k) const { DEBUG_ONLY(checkIndex(i,j,k)); return (IndexInt)i + (IndexInt)mSize.x * j + (IndexInt)mStrideZ * k; }
	//! Get index into the data
	inline IndexInt index(const Vec3i& pos) const    { DEBUG_ONLY(checkIndex(pos.x,pos.y,pos.z)); return (IndexInt)pos.x + (IndexInt)mSize.x * pos.y + (IndexInt)mStrideZ * pos.z; }

	//! grid4d compatibility functions 
	inline bool is4D() const { return false; }
	inline int getSizeT() const { return 1; }
	inline int getStrideT() const { return 0; }
	inline int index(int i, int j, int k, int unused) const { return index(i,j,k); }
	inline bool isInBounds(int i,int j, int k, int t, int bnd) const { if(t!=0) return false; return isInBounds( Vec3i(i,j,k), bnd ); }

protected:
	
	GridType mType;
	Vec3i mSize;
	Real mDx;
	bool m3D; 	// precomputed Z shift: to ensure 2D compatibility, always use this instead of sx*sy !
	IndexInt mStrideZ;  public: PbArgs _args; }
#define _C_GridBase
;

//! Grid class

template<class T> class Grid : public GridBase {public:
	//! init new grid, values are set to zero
	Grid(FluidSolver* parent, bool show = true); static int _W_1 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { PbClass* obj = Pb::objFromPy(_self); if (obj) delete obj; try { PbArgs _args(_linargs, _kwds); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(0, "Grid::Grid" , !noTiming ); { ArgLocker _lock; FluidSolver* parent = _args.getPtr<FluidSolver >("parent",0,&_lock); bool show = _args.getOpt<bool >("show",1,true,&_lock);  obj = new Grid(parent,show); obj->registerObject(_self, &_args); _args.check(); } pbFinalizePlugin(obj->getParent(),"Grid::Grid" , !noTiming ); return 0; } catch(std::exception& e) { pbSetError("Grid::Grid",e.what()); return -1; } }
	//! init new grid with an existing array
	Grid(FluidSolver* parent, T* data, bool show = true);
	//! create new & copy content from another grid
	Grid(const Grid<T>& a);
	//! return memory to solver
	virtual ~Grid();
	
	typedef T BASETYPE;
	typedef GridBase BASETYPE_GRID;
	
	void save(std::string name); static PyObject* _W_2 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::save" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; std::string name = _args.get<std::string >("name",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->save(name);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::save" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::save",e.what()); return 0; } }
	void load(std::string name); static PyObject* _W_3 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::load" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; std::string name = _args.get<std::string >("name",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->load(name);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::load" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::load",e.what()); return 0; } }

	//! set all cells to zero
	void clear(); static PyObject* _W_4 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::clear" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock;  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->clear();  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::clear" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::clear",e.what()); return 0; } }
	
	//! all kinds of access functions, use grid(), grid[] or grid.get()
	//! access data
	inline T get(int i,int j, int k) const         { return mData[index(i,j,k)]; }
	//! access data
	inline T& get(int i,int j, int k)              { return mData[index(i,j,k)]; }
	//! access data
	inline T get(IndexInt idx) const               { DEBUG_ONLY(checkIndex(idx)); return mData[idx]; }
	//! access data
	inline T get(const Vec3i& pos) const           { return mData[index(pos)]; }
	//! access data
	inline T& operator()(int i, int j, int k)      { return mData[index(i, j, k)]; }
	//! access data
	inline T operator()(int i, int j, int k) const { return mData[index(i, j, k)]; }
	//! access data
	inline T& operator()(IndexInt idx)             { DEBUG_ONLY(checkIndex(idx)); return mData[idx]; }
	//! access data
	inline T operator()(IndexInt idx) const        { DEBUG_ONLY(checkIndex(idx)); return mData[idx]; }
	//! access data
	inline T& operator()(const Vec3i& pos)         { return mData[index(pos)]; }
	//! access data
	inline T operator()(const Vec3i& pos) const    { return mData[index(pos)]; }
	//! access data
	inline T& operator[](IndexInt idx)             { DEBUG_ONLY(checkIndex(idx)); return mData[idx]; }
	//! access data
	inline const T operator[](IndexInt idx) const  { DEBUG_ONLY(checkIndex(idx)); return mData[idx]; }
	
	// interpolated access
	inline T    getInterpolated(const Vec3& pos) const { return interpol<T>(mData, mSize, mStrideZ, pos); }
	inline void setInterpolated(const Vec3& pos, const T& val, Grid<Real>& sumBuffer) const { setInterpol<T>(mData, mSize, mStrideZ, pos, val, &sumBuffer[0]); }
	// higher order interpolation (1=linear, 2=cubic)
	inline T getInterpolatedHi(const Vec3& pos, int order) const { 
		switch(order) {
		case 1:  return interpol     <T>(mData, mSize, mStrideZ, pos); 
		case 2:  return interpolCubic<T>(mData, mSize, mStrideZ, pos); 
		default: assertMsg(false, "Unknown interpolation order "<<order); }
	}
	
	// assignment / copy

	//! warning - do not use "=" for grids in python, this copies the reference! not the grid content...
	//Grid<T>& operator=(const Grid<T>& a);
	//! copy content from other grid (use this one instead of operator= !)
	Grid<T>& copyFrom(const Grid<T>& a, bool copyType=true ); static PyObject* _W_5 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::copyFrom" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<T>& a = *_args.getPtr<Grid<T> >("a",0,&_lock); bool copyType = _args.getOpt<bool >("copyType",1,true ,&_lock);  pbo->_args.copy(_args);  _retval = toPy(pbo->copyFrom(a,copyType));  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::copyFrom" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::copyFrom",e.what()); return 0; } } // old: { *this = a; }

	// helper functions to work with grids in scene files 

	//! add/subtract other grid
	void add(const Grid<T>& a); static PyObject* _W_6 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::add" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<T>& a = *_args.getPtr<Grid<T> >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->add(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::add" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::add",e.what()); return 0; } }
	void sub(const Grid<T>& a); static PyObject* _W_7 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::sub" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<T>& a = *_args.getPtr<Grid<T> >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->sub(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::sub" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::sub",e.what()); return 0; } }
	//! set all cells to constant value
	void setConst(const T& s); static PyObject* _W_8 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::setConst" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const T& s = *_args.getPtr<T >("s",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->setConst(s);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::setConst" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::setConst",e.what()); return 0; } }
	//! add constant to all grid cells
	void addConst(const T& s); static PyObject* _W_9 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::addConst" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const T& s = *_args.getPtr<T >("s",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->addConst(s);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::addConst" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::addConst",e.what()); return 0; } }
	//! add scaled other grid to current one (note, only "Real" factor, "T" type not supported here!)
	void addScaled(const Grid<T>& a, const T& factor); static PyObject* _W_10 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::addScaled" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<T>& a = *_args.getPtr<Grid<T> >("a",0,&_lock); const T& factor = *_args.getPtr<T >("factor",1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->addScaled(a,factor);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::addScaled" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::addScaled",e.what()); return 0; } } 
	//! multiply contents of grid
	void mult(const Grid<T>& a); static PyObject* _W_11 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::mult" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<T>& a = *_args.getPtr<Grid<T> >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->mult(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::mult" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::mult",e.what()); return 0; } }
	//! multiply each cell by a constant scalar value
	void multConst(const T& s); static PyObject* _W_12 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::multConst" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const T& s = *_args.getPtr<T >("s",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->multConst(s);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::multConst" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::multConst",e.what()); return 0; } }
	//! clamp content to range (for vec3, clamps each component separately)
	void clamp(const T& min, const T& max); static PyObject* _W_13 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::clamp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const T& min = *_args.getPtr<T >("min",0,&_lock); const T& max = *_args.getPtr<T >("max",1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->clamp(min,max);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::clamp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::clamp",e.what()); return 0; } }
	//! reduce small values to zero
	void stomp(const T& threshold); static PyObject* _W_14 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::stomp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const T& threshold = *_args.getPtr<T >("threshold",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->stomp(threshold);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::stomp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::stomp",e.what()); return 0; } }
	
	// common compound operators
	//! get absolute max value in grid 
	Real getMaxAbs() const ; static PyObject* _W_15 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getMaxAbs" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock;  pbo->_args.copy(_args);  _retval = toPy(pbo->getMaxAbs());  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getMaxAbs" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getMaxAbs",e.what()); return 0; } }
	//! get max value in grid 
	Real getMax() const ; static PyObject* _W_16 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getMax" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock;  pbo->_args.copy(_args);  _retval = toPy(pbo->getMax());  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getMax" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getMax",e.what()); return 0; } }
	//! get min value in grid 
	Real getMin() const ; static PyObject* _W_17 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getMin" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock;  pbo->_args.copy(_args);  _retval = toPy(pbo->getMin());  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getMin" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getMin",e.what()); return 0; } }
	//! calculate L1 norm of grid content
	Real getL1(int bnd=0); static PyObject* _W_18 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getL1" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int bnd = _args.getOpt<int >("bnd",0,0,&_lock);  pbo->_args.copy(_args);  _retval = toPy(pbo->getL1(bnd));  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getL1" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getL1",e.what()); return 0; } }
	//! calculate L2 norm of grid content
	Real getL2(int bnd=0); static PyObject* _W_19 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getL2" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int bnd = _args.getOpt<int >("bnd",0,0,&_lock);  pbo->_args.copy(_args);  _retval = toPy(pbo->getL2(bnd));  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getL2" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getL2",e.what()); return 0; } }

	//! set all boundary cells to constant value (Dirichlet)
	void setBound(T value, int boundaryWidth=1); static PyObject* _W_20 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::setBound" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; T value = _args.get<T >("value",0,&_lock); int boundaryWidth = _args.getOpt<int >("boundaryWidth",1,1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->setBound(value,boundaryWidth);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::setBound" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::setBound",e.what()); return 0; } }
	//! set all boundary cells to last inner value (Neumann)
	void setBoundNeumann(int boundaryWidth=1); static PyObject* _W_21 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::setBoundNeumann" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int boundaryWidth = _args.getOpt<int >("boundaryWidth",0,1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->setBoundNeumann(boundaryWidth);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::setBoundNeumann" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::setBoundNeumann",e.what()); return 0; } }

	//! get data pointer of grid
	std::string getDataPointer(); static PyObject* _W_22 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::getDataPointer" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock;  pbo->_args.copy(_args);  _retval = toPy(pbo->getDataPointer());  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::getDataPointer" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::getDataPointer",e.what()); return 0; } }
	
	//! debugging helper, print grid from python. skip boundary of width bnd
	void printGrid(int zSlice=-1, bool printIndex=false, int bnd=1); static PyObject* _W_23 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); Grid* pbo = dynamic_cast<Grid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "Grid::printGrid" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int zSlice = _args.getOpt<int >("zSlice",0,-1,&_lock); bool printIndex = _args.getOpt<bool >("printIndex",1,false,&_lock); int bnd = _args.getOpt<int >("bnd",2,1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->printGrid(zSlice,printIndex,bnd);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"Grid::printGrid" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("Grid::printGrid",e.what()); return 0; } } 

	// c++ only operators
	template<class S> Grid<T>& operator+=(const Grid<S>& a);
	template<class S> Grid<T>& operator+=(const S& a);
	template<class S> Grid<T>& operator-=(const Grid<S>& a);
	template<class S> Grid<T>& operator-=(const S& a);
	template<class S> Grid<T>& operator*=(const Grid<S>& a);
	template<class S> Grid<T>& operator*=(const S& a);
	template<class S> Grid<T>& operator/=(const Grid<S>& a);
	template<class S> Grid<T>& operator/=(const S& a);
	Grid<T>& safeDivide(const Grid<T>& a);    
	
	//! Swap data with another grid (no actual data is moved)
	void swap(Grid<T>& other);

	//! grid4d compatibility functions 
	inline T& operator()(int i, int j, int k, int unused)       { return mData[index(i, j, k)]; }
	inline T operator() (int i, int j, int k, int unused) const { return mData[index(i, j, k)]; }

protected: 	T* mData; 	bool externalData;		// True if mData is managed outside of the Fluidsolver
public: PbArgs _args; }
#define _C_Grid
;

// Python doesn't know about templates: explicit aliases needed




class FlagGrid;

//! Special function for staggered grids
class MACGrid : public Grid<Vec3> {public:
	MACGrid(FluidSolver* parent, bool show=true) :Grid<Vec3>(parent,show){ 
		mType = (GridType)(TypeMAC | TypeVec3); } static int _W_24 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { PbClass* obj = Pb::objFromPy(_self); if (obj) delete obj; try { PbArgs _args(_linargs, _kwds); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(0, "MACGrid::MACGrid" , !noTiming ); { ArgLocker _lock; FluidSolver* parent = _args.getPtr<FluidSolver >("parent",0,&_lock); bool show = _args.getOpt<bool >("show",1,true,&_lock);  obj = new MACGrid(parent,show); obj->registerObject(_self, &_args); _args.check(); } pbFinalizePlugin(obj->getParent(),"MACGrid::MACGrid" , !noTiming ); return 0; } catch(std::exception& e) { pbSetError("MACGrid::MACGrid",e.what()); return -1; } }

	void clearCell(const FlagGrid& flag, const int type); static PyObject* _W_25 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); MACGrid* pbo = dynamic_cast<MACGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "MACGrid::clearCell" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const FlagGrid& flag = *_args.getPtr<FlagGrid >("flag",0,&_lock); const int type = _args.get<int >("type",1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->clearCell(flag,type);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"MACGrid::clearCell" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("MACGrid::clearCell",e.what()); return 0; } }

        MACGrid(FluidSolver* parent, Vec3* data, bool show=true) : Grid<Vec3>(parent, data, show) { 
		mType = (GridType)(TypeMAC | TypeVec3); }
	
	// specialized functions for interpolating MAC information
	inline Vec3 getCentered(int i, int j, int k) const;
	inline Vec3 getCentered(const Vec3i& pos) const { return getCentered(pos.x, pos.y, pos.z); }
	inline Vec3 getAtMACX(int i, int j, int k) const;
	inline Vec3 getAtMACY(int i, int j, int k) const;
	inline Vec3 getAtMACZ(int i, int j, int k) const;
	// interpolation
	inline Vec3 getInterpolated(const Vec3& pos) const { return interpolMAC(mData, mSize, mStrideZ, pos); }
	inline void setInterpolated(const Vec3& pos, const Vec3& val, Vec3* tmp) const { return setInterpolMAC(mData, mSize, mStrideZ, pos, val, tmp); }
	inline Vec3 getInterpolatedHi(const Vec3& pos, int order) const { 
		switch(order) {
		case 1:  return interpolMAC     (mData, mSize, mStrideZ, pos); 
		case 2:  return interpolCubicMAC(mData, mSize, mStrideZ, pos); 
		default: assertMsg(false, "Unknown interpolation order "<<order); }
	}
	// specials for mac grid:
	template<int comp> inline Real getInterpolatedComponent(Vec3 pos) const { return interpolComponent<comp>(mData, mSize, mStrideZ, pos); }
	template<int comp> inline Real getInterpolatedComponentHi(const Vec3& pos, int order) const { 
		switch(order) {
		case 1:  return interpolComponent<comp>(mData, mSize, mStrideZ, pos); 
		case 2:  return interpolCubicMAC(mData, mSize, mStrideZ, pos)[comp];  // warning - not yet optimized
		default: assertMsg(false, "Unknown interpolation order "<<order); }
	}

	//! set all boundary cells of a MAC grid to certain value (Dirchlet). Respects staggered grid locations
	//! optionally, only set normal components
	void setBoundMAC(Vec3 value, int boundaryWidth, bool normalOnly=false); static PyObject* _W_26 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); MACGrid* pbo = dynamic_cast<MACGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "MACGrid::setBoundMAC" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; Vec3 value = _args.get<Vec3 >("value",0,&_lock); int boundaryWidth = _args.get<int >("boundaryWidth",1,&_lock); bool normalOnly = _args.getOpt<bool >("normalOnly",2,false,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->setBoundMAC(value,boundaryWidth,normalOnly);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"MACGrid::setBoundMAC" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("MACGrid::setBoundMAC",e.what()); return 0; } }
	 protected: public: PbArgs _args; }
#define _C_MACGrid
;

//! Special functions for FlagGrid
class FlagGrid : public Grid<int> {public:
	FlagGrid(FluidSolver* parent, int dim=3, bool show=true) :Grid<int>(parent,show){ 
		mType = (GridType)(TypeFlags | TypeInt); } static int _W_27 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { PbClass* obj = Pb::objFromPy(_self); if (obj) delete obj; try { PbArgs _args(_linargs, _kwds); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(0, "FlagGrid::FlagGrid" , !noTiming ); { ArgLocker _lock; FluidSolver* parent = _args.getPtr<FluidSolver >("parent",0,&_lock); int dim = _args.getOpt<int >("dim",1,3,&_lock); bool show = _args.getOpt<bool >("show",2,true,&_lock);  obj = new FlagGrid(parent,dim,show); obj->registerObject(_self, &_args); _args.check(); } pbFinalizePlugin(obj->getParent(),"FlagGrid::FlagGrid" , !noTiming ); return 0; } catch(std::exception& e) { pbSetError("FlagGrid::FlagGrid",e.what()); return -1; } }
	FlagGrid(FluidSolver* parent, int* data, int dim = 3, bool show=true) : Grid<int>(parent, data, show) { 
            mType = (GridType)(TypeFlags | TypeInt); }	

	//! types of cells, in/outflow can be combined, e.g., TypeFluid|TypeInflow
	enum CellType { 
		TypeNone     = 0,
		TypeFluid    = 1,
		TypeObstacle = 2,
		TypeEmpty    = 4,
		TypeInflow   = 8,
		TypeOutflow  = 16,
		TypeOpen     = 32,
		TypeStick    = 64,
		// internal use only, for fast marching
		TypeReserved = 256,
		// 2^10 - 2^14 reserved for moving obstacles
	};

	void andOp(const Grid<int>& a) { (*this) &= a; } static PyObject* _W_28 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::andOp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<int>& a = *_args.getPtr<Grid<int> >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->andOp(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::andOp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::andOp",e.what()); return 0; } }
	void OrOp(const Grid<int>& a) { (*this) |= a; } static PyObject* _W_29 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::OrOp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const Grid<int>& a = *_args.getPtr<Grid<int> >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->OrOp(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::OrOp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::OrOp",e.what()); return 0; } }
	void andScalarOp(const int a) { (*this) &= a; } static PyObject* _W_30 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::andScalarOp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const int a = _args.get<int >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->andScalarOp(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::andScalarOp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::andScalarOp",e.what()); return 0; } }
	void OrScalarOp(const int a) { (*this) |= a; } static PyObject* _W_31 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::OrScalarOp" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const int a = _args.get<int >("a",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->OrScalarOp(a);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::OrScalarOp" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::OrScalarOp",e.what()); return 0; } }

	Grid<int>& operator&=(const Grid<int>& a);
	Grid<int>& operator|=(const Grid<int>& a);
	Grid<int>& operator&=(const int a);
	Grid<int>& operator|=(const int a);
		
	//! access for particles
	inline int getAt(const Vec3& pos) const { return mData[index((int)pos.x, (int)pos.y, (int)pos.z)]; }
			
	//! check for different flag types
	inline bool isObstacle(IndexInt idx) const { return get(idx) & TypeObstacle; }
	inline bool isObstacle(int i, int j, int k) const { return get(i,j,k) & TypeObstacle; }
	inline bool isObstacle(const Vec3i& pos) const { return get(pos) & TypeObstacle; }
	inline bool isObstacle(const Vec3& pos) const { return getAt(pos) & TypeObstacle; }
	inline bool isFluid(IndexInt idx) const { return get(idx) & TypeFluid; }
	inline bool isFluid(int i, int j, int k) const { return get(i,j,k) & TypeFluid; }
	inline bool isFluid(const Vec3i& pos) const { return get(pos) & TypeFluid; }
	inline bool isFluid(const Vec3& pos) const { return getAt(pos) & TypeFluid; }
	inline bool isInflow(IndexInt idx) const { return get(idx) & TypeInflow; }
	inline bool isInflow(int i, int j, int k) const { return get(i,j,k) & TypeInflow; }
	inline bool isInflow(const Vec3i& pos) const { return get(pos) & TypeInflow; }
	inline bool isInflow(const Vec3& pos) const { return getAt(pos) & TypeInflow; }
	inline bool isEmpty(IndexInt idx) const { return get(idx) & TypeEmpty; }
	inline bool isEmpty(int i, int j, int k) const { return get(i,j,k) & TypeEmpty; }
	inline bool isEmpty(const Vec3i& pos) const { return get(pos) & TypeEmpty; }
	inline bool isEmpty(const Vec3& pos) const { return getAt(pos) & TypeEmpty; }
	inline bool isOutflow(IndexInt idx) const { return get(idx) & TypeOutflow; }
	inline bool isOutflow(int i, int j, int k) const { return get(i, j, k) & TypeOutflow; }
	inline bool isOutflow(const Vec3i& pos) const { return get(pos) & TypeOutflow; }
	inline bool isOutflow(const Vec3& pos) const { return getAt(pos) & TypeOutflow; }
	inline bool isOpen(IndexInt idx) const { return get(idx) & TypeOpen; }
	inline bool isOpen(int i, int j, int k) const { return get(i, j, k) & TypeOpen; }
	inline bool isOpen(const Vec3i& pos) const { return get(pos) & TypeOpen; }
	inline bool isOpen(const Vec3& pos) const { return getAt(pos) & TypeOpen; }
	inline bool isStick(IndexInt idx) const { return get(idx) & TypeStick; }
	inline bool isStick(int i, int j, int k) const { return get(i,j,k) & TypeStick; }
	inline bool isStick(const Vec3i& pos) const { return get(pos) & TypeStick; }
	inline bool isStick(const Vec3& pos) const { return getAt(pos) & TypeStick; }

	
	void InitMinXWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	void InitMaxXWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	void InitMinYWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	void InitMaxYWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	void InitMinZWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	void InitMaxZWall(const int &boundaryWidth, Grid<Real>& phiWalls);
	// Python callables
	




void initDomain( const int &boundaryWidth = 0, const std::string &wall = "xXyYzZ", const std::string &open = "      ", const std::string &inflow = "      ", const std::string &outflow = "      ", Grid<Real>* phiWalls = 0x00 ); static PyObject* _W_32 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::initDomain" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const int& boundaryWidth = _args.getOpt<int >("boundaryWidth",0,0,&_lock); const std::string& wall = _args.getOpt<std::string >("wall",1,"xXyYzZ",&_lock); const std::string& open = _args.getOpt<std::string >("open",2,"      ",&_lock); const std::string& inflow = _args.getOpt<std::string >("inflow",3,"      ",&_lock); const std::string& outflow = _args.getOpt<std::string >("outflow",4,"      ",&_lock); Grid<Real>* phiWalls = _args.getPtrOpt<Grid<Real> >("phiWalls",5,0x00 ,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->initDomain(boundaryWidth,wall,open,inflow,outflow,phiWalls);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::initDomain" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::initDomain",e.what()); return 0; } }
	void initBoundaries( const int &boundaryWidth, const int *types );

	void minifyFrom(const FlagGrid &flags, const Vec3i &scale); static PyObject* _W_33 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::minifyFrom" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const FlagGrid& flags = *_args.getPtr<FlagGrid >("flags",0,&_lock); const Vec3i& scale = _args.get<Vec3i >("scale",1,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->minifyFrom(flags,scale);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::minifyFrom" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::minifyFrom",e.what()); return 0; } }
	//! set fluid flags inside levelset (liquids)
	void updateFromLevelset(const LevelsetGrid& levelset); static PyObject* _W_34 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::updateFromLevelset" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const LevelsetGrid& levelset = *_args.getPtr<LevelsetGrid >("levelset",0,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->updateFromLevelset(levelset);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::updateFromLevelset" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::updateFromLevelset",e.what()); return 0; } }
	void updateFromLevelsetNonMatched(const LevelsetGrid& levelset, const int orderSpace=1);

	//! set all cells (except obs/in/outflow) to type (fluid by default)
	void fillGrid(int type=TypeFluid); static PyObject* _W_35 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::fillGrid" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int type = _args.getOpt<int >("type",0,TypeFluid,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->fillGrid(type);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::fillGrid" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::fillGrid",e.what()); return 0; } }
	void extendRegion(const int region, const int exclude, const int depth); static PyObject* _W_36 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::extendRegion" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; const int region = _args.get<int >("region",0,&_lock); const int exclude = _args.get<int >("exclude",1,&_lock); const int depth = _args.get<int >("depth",2,&_lock);  pbo->_args.copy(_args);  _retval = getPyNone(); pbo->extendRegion(region,exclude,depth);  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::extendRegion" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::extendRegion",e.what()); return 0; } }
 	//! count no. of cells matching flags via "AND"
	//! warning for large grids! only regular int returned (due to python interface)
	//! optionally creates mask in RealGrid (1 where flag matches, 0 otherwise)
	int countCells(int flag, int bnd=0, Grid<Real>* mask=NULL); static PyObject* _W_37 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FlagGrid* pbo = dynamic_cast<FlagGrid*>(Pb::objFromPy(_self)); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(pbo->getParent(), "FlagGrid::countCells" , !noTiming); PyObject *_retval = 0; { ArgLocker _lock; int flag = _args.get<int >("flag",0,&_lock); int bnd = _args.getOpt<int >("bnd",1,0,&_lock); Grid<Real>* mask = _args.getPtrOpt<Grid<Real> >("mask",2,NULL,&_lock);  pbo->_args.copy(_args);  _retval = toPy(pbo->countCells(flag,bnd,mask));  pbo->_args.check(); } pbFinalizePlugin(pbo->getParent(),"FlagGrid::countCells" , !noTiming); return _retval; } catch(std::exception& e) { pbSetError("FlagGrid::countCells",e.what()); return 0; } } public: PbArgs _args; }
#define _C_FlagGrid
;

//! helper to compute grid conversion factor between local coordinates of two grids
inline Vec3 calcGridSizeFactor(Vec3i s1, Vec3i s2) {
	return Vec3( Real(s1[0])/s2[0], Real(s1[1])/s2[1], Real(s1[2])/s2[2] );
}
Vec3 calcGridSizeFactorWithRange(const Vec3 &from_old, const Vec3 &to_old, const Vec3 &from_new, const Vec3 &to_new);

// prototypes for grid plugins
void copyMacToVec3(MACGrid &source, Grid<Vec3>& target);
void convertMacToVec3(MACGrid &source, Grid<Vec3>& target);
void resampleVec3ToMac(Grid<Vec3>& source, MACGrid &target);
void resampleMacToVec3 (MACGrid &source, Grid<Vec3>& target );

void getComponent(const Grid<Vec3>& source, Grid<Real>& target, int component);
void setComponent(const Grid<Real>& source, Grid<Vec3>& target, int component);




//******************************************************************************
// Implementation of inline functions

inline void GridBase::checkIndex(int i, int j, int k) const {
	//if (i<0 || j<0  || i>=mSize.x || j>=mSize.y || (is3D() && (k<0|| k>= mSize.z))) {
	if (i<0 || j<0  || i>=mSize.x || j>=mSize.y || k<0|| k>= mSize.z ) {
		std::ostringstream s;
		s << "Grid " << mName << " dim " << mSize << " : index " << i << "," << j << "," << k << " out of bound ";
		errMsg(s.str());
	}
}

inline void GridBase::checkIndex(IndexInt idx) const {
	if (idx<0 || idx >= mSize.x * mSize.y * mSize.z) {
		std::ostringstream s;
		s << "Grid " << mName << " dim " << mSize << " : index " << idx << " out of bound ";
		errMsg(s.str());
	}
}

inline bool GridBase::isInBounds(const Vec3i& p) const {
	return (p.x >= 0 && p.y >= 0 && p.z >= 0 && p.x < mSize.x && p.y < mSize.y && p.z < mSize.z); 
}

inline bool GridBase::isInBounds(const Vec3i& p, int bnd) const {
	bool ret = (p.x >= bnd && p.y >= bnd && p.x < mSize.x-bnd && p.y < mSize.y-bnd);
	if(this->is3D()) {
		ret &= (p.z >= bnd && p.z < mSize.z-bnd); 
	} else {
		ret &= (p.z == 0);
	}
	return ret;
}
//! Check if linear index is in the range of the array
inline bool GridBase::isInBounds(IndexInt idx) const {
	if (idx<0 || idx >= mSize.x * mSize.y * mSize.z) {
		return false;
	}
	return true;
}

inline void MACGrid::clearCell(const FlagGrid &flag, const int type) {
	FOR_IJK(flag) {
		if(flag(i,j,k)&type) {
			(*this)(i, j, k).x = (*this)(i, j, k).y = 0;
			if(getSizeX()<i+1) (*this)(i+1, j, k).x = 0;
			if(getSizeY()<j+1) (*this)(i, j+1, k).y = 0;
			if(is3D()) {
				(*this)(i, j, k).z = 0;
				if(getSizeZ()<k+1) (*this)(i, j, k+1).z = 0;
			}
		}
	}
}

inline Vec3 MACGrid::getCentered(int i, int j, int k) const {
	DEBUG_ONLY(checkIndex(i+1,j+1,k));
	const IndexInt idx = index(i,j,k);
	Vec3 v = Vec3(0.5* (mData[idx].x + mData[idx+1].x),
				  0.5* (mData[idx].y + mData[idx+mSize.x].y),
				  0.);
	if( this->is3D() ) {
		DEBUG_ONLY(checkIndex(idx+mStrideZ));
		v[2] =    0.5* (mData[idx].z + mData[idx+mStrideZ].z);
	}
	return v;
}

inline Vec3 MACGrid::getAtMACX(int i, int j, int k) const {
	DEBUG_ONLY(checkIndex(i-1,j+1,k));
	const IndexInt idx = index(i,j,k);
	Vec3 v =  Vec3(   (mData[idx].x),
				0.25* (mData[idx].y + mData[idx-1].y + mData[idx+mSize.x].y + mData[idx+mSize.x-1].y),
				0.);
	if( this->is3D() ) {
		DEBUG_ONLY(checkIndex(idx+mStrideZ-1));
		v[2] = 0.25* (mData[idx].z + mData[idx-1].z + mData[idx+mStrideZ].z + mData[idx+mStrideZ-1].z);
	}
	return v;
}

inline Vec3 MACGrid::getAtMACY(int i, int j, int k) const {
	DEBUG_ONLY(checkIndex(i+1,j-1,k));
	const IndexInt idx = index(i,j,k);
	Vec3 v =  Vec3(0.25* (mData[idx].x + mData[idx-mSize.x].x + mData[idx+1].x + mData[idx+1-mSize.x].x),
						 (mData[idx].y),   0. );
	if( this->is3D() ) {
		DEBUG_ONLY(checkIndex(idx+mStrideZ-mSize.x));
		v[2] = 0.25* (mData[idx].z + mData[idx-mSize.x].z + mData[idx+mStrideZ].z + mData[idx+mStrideZ-mSize.x].z);
	}
	return v;
}

inline Vec3 MACGrid::getAtMACZ(int i, int j, int k) const {
	const IndexInt idx = index(i,j,k);
	DEBUG_ONLY(checkIndex(idx-mStrideZ));
	DEBUG_ONLY(checkIndex(idx+mSize.x-mStrideZ));
	Vec3 v =  Vec3(0.25* (mData[idx].x + mData[idx-mStrideZ].x + mData[idx+1].x + mData[idx+1-mStrideZ].x),
				   0.25* (mData[idx].y + mData[idx-mStrideZ].y + mData[idx+mSize.x].y + mData[idx+mSize.x-mStrideZ].y),
						 (mData[idx].z) );
	return v;
}

template<typename T> inline void stomp(T &v, const T &th) { if(v<th) v=0; }
template<> inline void stomp<Vec3>(Vec3 &v, const Vec3 &th) { if(v[0]<th[0]) v[0]=0; if(v[1]<th[1]) v[1]=0; if(v[2]<th[2]) v[2]=0; }

template <class T, class S>  struct gridAnd : public KernelBase { gridAnd(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] &= other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridAnd ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 493 "grid.h"


template <class T, class S>  struct gridOr : public KernelBase { gridOr(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] |= other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridOr ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 494 "grid.h"


template <class T, class S>  struct gridAdd : public KernelBase { gridAdd(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] += other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridAdd ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 495 "grid.h"


template <class T, class S>  struct gridSub : public KernelBase { gridSub(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] -= other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridSub ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 496 "grid.h"


template <class T, class S>  struct gridMult : public KernelBase { gridMult(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] *= other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridMult ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 497 "grid.h"


template <class T, class S>  struct gridDiv : public KernelBase { gridDiv(Grid<T>& me, const Grid<S>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<S>& other )  { me[idx] /= other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<S>& getArg1() { return other; } typedef Grid<S> type1; void runMessage() { debMsg("Executing kernel gridDiv ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<S>& other;   };
#line 498 "grid.h"


template <class T, class S>  struct gridAndScalar : public KernelBase { gridAndScalar(Grid<T>& me, const S& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const S& other )  { me[idx] &= other; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel gridAndScalar ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const S& other;   };
#line 499 "grid.h"


template <class T, class S>  struct gridOrScalar : public KernelBase { gridOrScalar(Grid<T>& me, const S& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const S& other )  { me[idx] |= other; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel gridOrScalar ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const S& other;   };
#line 500 "grid.h"


template <class T, class S>  struct gridAddScalar : public KernelBase { gridAddScalar(Grid<T>& me, const S& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const S& other )  { me[idx] += other; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel gridAddScalar ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const S& other;   };
#line 501 "grid.h"


template <class T, class S>  struct gridMultScalar : public KernelBase { gridMultScalar(Grid<T>& me, const S& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const S& other )  { me[idx] *= other; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel gridMultScalar ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const S& other;   };
#line 502 "grid.h"


template <class T, class S>  struct gridScaledAdd : public KernelBase { gridScaledAdd(Grid<T>& me, const Grid<T>& other, const S& factor) :  KernelBase(&me,0) ,me(me),other(other),factor(factor)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<T>& other, const S& factor )  { me[idx] += factor*other[idx]; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<T>& getArg1() { return other; } typedef Grid<T> type1;inline const S& getArg2() { return factor; } typedef S type2; void runMessage() { debMsg("Executing kernel gridScaledAdd ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other,factor);  }   } Grid<T>& me; const Grid<T>& other; const S& factor;   };
#line 503 "grid.h"



template <class T>  struct gridSetConst : public KernelBase { gridSetConst(Grid<T>& me, const T& value) :  KernelBase(&me,0) ,me(me),value(value)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const T& value )  { me[idx] = value; }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const T& getArg1() { return value; } typedef T type1; void runMessage() { debMsg("Executing kernel gridSetConst ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,value);  }   } Grid<T>& me; const T& value;   };
#line 505 "grid.h"


template <class T>  struct gridSafeDiv : public KernelBase { gridSafeDiv(Grid<T>& me, const Grid<T>& other) :  KernelBase(&me,0) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const Grid<T>& other )  { me[idx] = safeDivide(me[idx], other[idx]); }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const Grid<T>& getArg1() { return other; } typedef Grid<T> type1; void runMessage() { debMsg("Executing kernel gridSafeDiv ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } Grid<T>& me; const Grid<T>& other;   };
#line 506 "grid.h"


template <class T>  struct gridClamp : public KernelBase { gridClamp(Grid<T>& me, const T& min, const T& max) :  KernelBase(&me,0) ,me(me),min(min),max(max)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const T& min, const T& max )  { me[idx] = clamp(me[idx], min, max); }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const T& getArg1() { return min; } typedef T type1;inline const T& getArg2() { return max; } typedef T type2; void runMessage() { debMsg("Executing kernel gridClamp ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,min,max);  }   } Grid<T>& me; const T& min; const T& max;   };
#line 507 "grid.h"


template <class T>  struct gridStomp : public KernelBase { gridStomp(Grid<T>& me, const T& threshold) :  KernelBase(&me,0) ,me(me),threshold(threshold)   { runMessage(); run(); }   inline void op(IndexInt idx, Grid<T>& me, const T& threshold )  { stomp(me[idx], threshold); }    inline Grid<T>& getArg0() { return me; } typedef Grid<T> type0;inline const T& getArg1() { return threshold; } typedef T type1; void runMessage() { debMsg("Executing kernel gridStomp ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,threshold);  }   } Grid<T>& me; const T& threshold;   };
#line 508 "grid.h"



inline Grid<int>& FlagGrid::operator&=(const Grid<int>& a) { gridAnd<int,int>(*this, a);       return *this; }
inline Grid<int>& FlagGrid::operator|=(const Grid<int>& a) { gridOr<int,int>(*this, a);        return *this; }
inline Grid<int>& FlagGrid::operator&=(const int a)        { gridAndScalar<int,int>(*this, a); return *this; }
inline Grid<int>& FlagGrid::operator|=(const int a)        { gridOrScalar<int,int>(*this, a);  return *this; }
template<class T> template<class S> Grid<T>& Grid<T>::operator+=(const Grid<S>& a) {
	gridAdd<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator+=(const S& a) {
	gridAddScalar<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator-=(const Grid<S>& a) {
	gridSub<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator-=(const S& a) {
	gridAddScalar<T,S>(*this, -a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator*=(const Grid<S>& a) {
	gridMult<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator*=(const S& a) {
	gridMultScalar<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator/=(const Grid<S>& a) {
	gridDiv<T,S>(*this, a);
	return *this;
}
template<class T> template<class S> Grid<T>& Grid<T>::operator/=(const S& a) {
	S rez((S)1.0 / a);
	gridMultScalar<T,S>(*this, rez);
	return *this;
}

//******************************************************************************
// Other helper functions

// compute gradient of a scalar grid
inline Vec3 getGradient(const Grid<Real>& data, int i, int j, int k) {
	Vec3 v;

	if (i > data.getSizeX()-2) i= data.getSizeX()-2;
	if (j > data.getSizeY()-2) j= data.getSizeY()-2;
	if (i < 1) i = 1;
	if (j < 1) j = 1;
	v = Vec3( data(i+1,j  ,k  ) - data(i-1,j  ,k  ) ,
			  data(i  ,j+1,k  ) - data(i  ,j-1,k  ) , 0. );

	if(data.is3D()) {
		if (k > data.getSizeZ()-2) k= data.getSizeZ()-2;
		if (k < 1) k = 1;
		v[2]= data(i  ,j  ,k+1) - data(i  ,j  ,k-1);
	} 

	return v;
}

// interpolate grid from one size to another size

template <class S>  struct knInterpolateGridTempl : public KernelBase { knInterpolateGridTempl(Grid<S>& target, const Grid<S>& source, const Vec3& sourceFactor, const Vec3 offset, const int orderSpace=1) :  KernelBase(&target,0) ,target(target),source(source),sourceFactor(sourceFactor),offset(offset),orderSpace(orderSpace)   { runMessage(); run(); }  inline void op(int i, int j, int k, Grid<S>& target, const Grid<S>& source, const Vec3& sourceFactor, const Vec3 offset, const int orderSpace=1 )  {
	Vec3 pos = Vec3(i,j,k) * sourceFactor + offset;
	if(!source.is3D()) pos[2] = 0; // allow 2d -> 3d
	target(i,j,k) = source.getInterpolatedHi(pos, orderSpace);
}   inline Grid<S>& getArg0() { return target; } typedef Grid<S> type0;inline const Grid<S>& getArg1() { return source; } typedef Grid<S> type1;inline const Vec3& getArg2() { return sourceFactor; } typedef Vec3 type2;inline const Vec3& getArg3() { return offset; } typedef Vec3 type3;inline const int& getArg4() { return orderSpace; } typedef int type4; void runMessage() { debMsg("Executing kernel knInterpolateGridTempl ", 3); debMsg("Kernel range" <<  " x "<<  maxX  << " y "<< maxY  << " z "<< minZ<<" - "<< maxZ  << " "   , 4); }; void run() {  const int _maxX = maxX; const int _maxY = maxY; if (maxZ > 1) { 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int k=minZ; k < maxZ; k++) for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,target,source,sourceFactor,offset,orderSpace);  } } else { const int k=0; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (int j=0; j < _maxY; j++) for (int i=0; i < _maxX; i++) op(i,j,k,target,source,sourceFactor,offset,orderSpace);  } }  } Grid<S>& target; const Grid<S>& source; const Vec3& sourceFactor; const Vec3 offset; const int orderSpace;   };
#line 573 "grid.h"

 
// template glue code - choose interpolation based on template arguments
template<class GRID>
void interpolGridTempl( GRID& target, GRID& source ) {
	errMsg("interpolGridTempl - Only valid for specific instantiations");
}


} //namespace
#endif


