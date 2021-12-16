




// DO NOT EDIT !
// This file is generated using the MantaFlow preprocessor (prep generate).




#line 1 "/home/vishrut/Study/USC/Sem-1/CSCI 596: Scientific Computing and Visualization/project/mlflip-mod/source/particle.cpp"
/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2013 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * Apache License, Version 2.0 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Particle data functionality
 *
 ******************************************************************************/

#include <fstream>
#include <cstring>
#if NO_ZLIB!=1
#include <zlib.h>
#endif
#include "particle.h"
#include "levelset.h"
#include "shapes.h"
#include "mantaio.h"

using namespace std;
namespace Manta {


ParticleBase::ParticleBase(FluidSolver* parent) : 
	PbClass(parent), mAllowCompress(true), mFreePdata(false) {
}

ParticleBase::~ParticleBase()
{
	// make sure data fields now parent system is deleted
	for(IndexInt i=0; i<(IndexInt)mPartData.size(); ++i)
		mPartData[i]->setParticleSys(NULL);
	
	if(mFreePdata) {
		for(IndexInt i=0; i<(IndexInt)mPartData.size(); ++i)
			delete mPartData[i];
	}
	
}

std::string ParticleBase::infoString() const { 
	return "ParticleSystem " + mName + " <no info>"; 
}

void ParticleBase::cloneParticleData(ParticleBase* nm) {
	// clone additional data , and make sure the copied particle system deletes it
	nm->mFreePdata = true;
	for(IndexInt i=0; i<(IndexInt)mPartData.size(); ++i) {
		ParticleDataBase* pdata = mPartData[i]->clone();
		nm->registerPdata(pdata);
	} 
}

void ParticleBase::deregister(ParticleDataBase* pdata) {
	bool done = false;
	// remove pointer from particle data list
	for(IndexInt i=0; i<(IndexInt)mPartData.size(); ++i) {
		if(mPartData[i] == pdata) {
			if(i<(IndexInt)mPartData.size()-1)
				mPartData[i] = mPartData[mPartData.size()-1];
			mPartData.pop_back();
			done = true;
		}
	} 
	if(!done)
		errMsg("Invalid pointer given, not registered!");
}

// create and attach a new pdata field to this particle system
PbClass* ParticleBase::create(PbType t, PbTypeVec T, const string& name) {        
#	if NOPYTHON!=1
	_args.add("nocheck",true);
	if (t.str() == "")
		errMsg("Specify particle data type to create");
	//debMsg( "Pdata creating '"<< t.str <<" with size "<< this->getSizeSlow(), 5 );
	
	PbClass* pyObj = PbClass::createPyObject(t.str() + T.str(), name, _args, this->getParent() );

	ParticleDataBase* pdata = dynamic_cast<ParticleDataBase*>(pyObj);
	if(!pdata) {
		errMsg("Unable to get particle data pointer from newly created object. Only create ParticleData type with a ParticleSys.creat() call, eg, PdataReal, PdataVec3 etc.");
		delete pyObj;
		return NULL;
	} else {
		this->registerPdata(pdata);
	}

	// directly init size of new pdata field:
	pdata->resize( this->getSizeSlow() );
#	else
	PbClass* pyObj = NULL;
#	endif
	return pyObj;
}

void ParticleBase::registerPdata(ParticleDataBase* pdata) {
	pdata->setParticleSys(this);
	mPartData.push_back(pdata);

	if( pdata->getType() == ParticleDataBase::TypeReal ) {
		ParticleDataImpl<Real>* pd = dynamic_cast< ParticleDataImpl<Real>* >(pdata);
		if(!pd) errMsg("Invalid pdata object posing as real!");
		this->registerPdataReal(pd);
	}
	else if( pdata->getType() == ParticleDataBase::TypeInt ) {
		ParticleDataImpl<int>* pd = dynamic_cast< ParticleDataImpl<int>* >(pdata);
		if(!pd) errMsg("Invalid pdata object posing as int!");
		this->registerPdataInt(pd);
	}
	else if( pdata->getType() == ParticleDataBase::TypeVec3 ) {
		ParticleDataImpl<Vec3>* pd = dynamic_cast< ParticleDataImpl<Vec3>* >(pdata);
		if(!pd) errMsg("Invalid pdata object posing as vec3!");
		this->registerPdataVec3(pd);
	}
}
void ParticleBase::registerPdataReal(ParticleDataImpl<Real>* pd) { mPdataReal.push_back(pd); }
void ParticleBase::registerPdataVec3(ParticleDataImpl<Vec3>* pd) { mPdataVec3.push_back(pd); }
void ParticleBase::registerPdataInt (ParticleDataImpl<int >* pd) { mPdataInt .push_back(pd); }

void ParticleBase::addAllPdata() {
	for(IndexInt i=0; i<(IndexInt)mPartData.size(); ++i) {
		mPartData[i]->addEntry();
	} 
}

 
BasicParticleSystem::BasicParticleSystem(FluidSolver* parent)
	   : ParticleSystem<BasicParticleData>(parent) {
	this->mAllowCompress = false;
}

// file io

void BasicParticleSystem::writeParticlesText(const string name) const {
	ofstream ofs(name.c_str());
	if (!ofs.good())
		errMsg("can't open file!");
	ofs << this->size()<<", pdata: "<< mPartData.size()<<" ("<<mPdataInt.size()<<","<<mPdataReal.size()<<","<<mPdataVec3.size()<<") \n";
	for(IndexInt i=0; i<this->size(); ++i) {
		ofs << i<<": "<< this->getPos(i) <<" , "<< this->getStatus(i) <<". "; 
		for(IndexInt pd=0; pd<(IndexInt)mPdataInt.size() ; ++pd) ofs << mPdataInt [pd]->get(i)<<" ";
		for(IndexInt pd=0; pd<(IndexInt)mPdataReal.size(); ++pd) ofs << mPdataReal[pd]->get(i)<<" ";
		for(IndexInt pd=0; pd<(IndexInt)mPdataVec3.size(); ++pd) ofs << mPdataVec3[pd]->get(i)<<" ";
		ofs << "\n"; 
	}
	ofs.close();
}
void BasicParticleSystem::writeParticlesBobj(const std::string& name, const ParticleDataImpl<int> *pT, const int exclude) const {
	writeBobjFile(name, this, pT, exclude);
}

void BasicParticleSystem::writeParticlesRawPositionsGz(const string name) const {
#	if NO_ZLIB!=1
	gzFile gzf = gzopen(name.c_str(), "wb1");
	if (!gzf) errMsg("can't open file "<<name);
	for(IndexInt i=0; i<this->size(); ++i) {
		Vector3D<float> p = toVec3f( this->getPos(i) );
		gzwrite(gzf, &p, sizeof(float)*3);
	}
	gzclose(gzf);
#	else
	cout << "file format not supported without zlib" << endl;
#	endif
}

void BasicParticleSystem::writeParticlesRawVelocityGz(const string name) const {
#	if NO_ZLIB!=1
	gzFile gzf = gzopen(name.c_str(), "wb1");
	if (!gzf) errMsg("can't open file "<<name);
	if( mPdataVec3.size() < 1 ) errMsg("no vec3 particle data channel found!");
	// note , assuming particle data vec3 0 is velocity! make optional...
	for(IndexInt i=0; i<this->size(); ++i) {		
		Vector3D<float> p = toVec3f( mPdataVec3[0]->get(i) );
		gzwrite(gzf, &p, sizeof(float)*3);
	}
	gzclose(gzf);
#	else
	cout << "file format not supported without zlib" << endl;
#	endif
}


void BasicParticleSystem::load(const string name) {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if ( ext == ".uni") 
		readParticlesUni(name, this );
	else if ( ext == ".raw") // raw = uni for now
		readParticlesUni(name, this );
	else 
		errMsg("particle '" + name +"' filetype not supported for loading");
}

void BasicParticleSystem::save(const string name) const {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if (ext == ".txt")
		this->writeParticlesText(name);
	else if (ext == ".uni")
		writeParticlesUni(name, this);
	else if (ext == ".raw") // raw = uni for now
		writeParticlesUni(name, this);

	// raw data formats, very basic for simple data transfer to other programs
	else if (ext == ".posgz") 
		this->writeParticlesRawPositionsGz(name);
	else if (ext == ".velgz") 
		this->writeParticlesRawVelocityGz(name);
	else
		errMsg("particle '" + name +"' filetype not supported for saving");
}

void BasicParticleSystem::printParts(IndexInt start, IndexInt stop, bool printIndex)
{
	std::ostringstream sstr;
	IndexInt s = (start>0 ? start : 0                      );
	IndexInt e = (stop>0  ? stop  : (IndexInt)mData.size() );
	s = Manta::clamp(s, (IndexInt)0, (IndexInt)mData.size());
	e = Manta::clamp(e, (IndexInt)0, (IndexInt)mData.size());

	for(IndexInt i=s; i<e; ++i) {
		if(printIndex) sstr << i<<": ";
		sstr<<mData[i].pos<<" "<<mData[i].flag<<"\n";
	} 
	debMsg( sstr.str() , 1 );
}

void BasicParticleSystem::readParticles(BasicParticleSystem* from) {
	// re-allocate all data
	this->resizeAll( from->size() ); 
	assertMsg (from->size() == this->size() , "particle size doesn't match");

	for(int i=0; i<this->size(); ++i) {
		(*this)[i].pos  = (*from)[i].pos;
		(*this)[i].flag = (*from)[i].flag;
	}
	Vec3i gridSize = from->getParent()->getGridSize();
	this->transformPositions( Vec3i(gridSize.x,gridSize.y,gridSize.z), this->size() );
}


Real appxSurfHeight(const Shape &shape, const BasicParticleSystem &pts, const Real th=10) {
	std::vector<Real> y;
	double avg_y = 0.0, height=0.0;
	int cnt = 0;
	for(int i=0; i<pts.size(); ++i)
		if(shape.isInside(pts[i].pos)) {
			y.push_back(pts[i].pos.y);
			avg_y += pts[i].pos.y;
			++cnt;
		}
	if(cnt) avg_y /= cnt;

	for(unsigned int i=0; i<y.size(); ++i) {
		if(std::fabs(avg_y-y[i])<th && height<y[i]) { height = y[i]; }
	}

	return height;
} static PyObject* _W_0 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "appxSurfHeight" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const Shape& shape = *_args.getPtr<Shape >("shape",0,&_lock); const BasicParticleSystem& pts = *_args.getPtr<BasicParticleSystem >("pts",1,&_lock); const Real th = _args.getOpt<Real >("th",2,10,&_lock);   _retval = toPy(appxSurfHeight(shape,pts,th));  _args.check(); } pbFinalizePlugin(parent,"appxSurfHeight", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("appxSurfHeight",e.what()); return 0; } } static const Pb::Register _RP_appxSurfHeight ("","appxSurfHeight",_W_0);  extern "C" { void PbRegister_appxSurfHeight() { KEEP_UNUSED(_RP_appxSurfHeight); } } 

// particle data

ParticleDataBase::ParticleDataBase(FluidSolver* parent) : 
		PbClass(parent) , mpParticleSys(NULL) {
}

ParticleDataBase::~ParticleDataBase()
{
	// notify parent of deletion 
	if(mpParticleSys)
		mpParticleSys->deregister(this);
}


// actual data implementation

template<class T>
ParticleDataImpl<T>::ParticleDataImpl(FluidSolver* parent) : 
	ParticleDataBase(parent) , mpGridSource(NULL), mGridSourceMAC(false) {
}

template<class T>
ParticleDataImpl<T>::ParticleDataImpl(FluidSolver* parent, ParticleDataImpl<T>* other) : 
	ParticleDataBase(parent) , mpGridSource(NULL), mGridSourceMAC(false) {
	this->mData = other->mData;
	setName(other->getName());
}

template<class T>
ParticleDataImpl<T>::~ParticleDataImpl() {
}

template<class T>
IndexInt ParticleDataImpl<T>::getSizeSlow() const {
	return mData.size();
}
template<class T>
void ParticleDataImpl<T>::addEntry() {
	// add zero'ed entry
	T tmp = T(0.);
	// for debugging, force init:
	//tmp = T(0.02 * mData.size()); // increasing
	//tmp = T(1.); // constant 1
	return mData.push_back(tmp);
}
template<class T>
void ParticleDataImpl<T>::resize(IndexInt s) {
	mData.resize(s);
}
template<class T>
void ParticleDataImpl<T>::copyValueSlow(IndexInt from, IndexInt to) {
	this->copyValue(from,to);
}
template<class T>
ParticleDataBase* ParticleDataImpl<T>::clone() {
	ParticleDataImpl<T>* npd = new ParticleDataImpl<T>( getParent(), this );
	return npd;
}

template<class T>
void ParticleDataImpl<T>::setSource(Grid<T>* grid, bool isMAC ) {
	mpGridSource = grid;
	mGridSourceMAC = isMAC;
	if(isMAC) assertMsg( dynamic_cast<MACGrid*>(grid) != NULL , "Given grid is not a valid MAC grid");
}

template<class T>
void ParticleDataImpl<T>::initNewValue(IndexInt idx, Vec3 pos) {
	if(!mpGridSource)
		mData[idx] = 0; 
	else {
		mData[idx] = mpGridSource->getInterpolated(pos);
	}
}
// special handling needed for velocities
template<>
void ParticleDataImpl<Vec3>::initNewValue(IndexInt idx, Vec3 pos) {
	if(!mpGridSource)
		mData[idx] = 0;
	else {
		if(!mGridSourceMAC)
			mData[idx] = mpGridSource->getInterpolated(pos);
		else
			mData[idx] = ((MACGrid*)mpGridSource)->getInterpolated(pos);
	}
}

template<typename T>
void ParticleDataImpl<T>::load(string name) {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if ( ext == ".uni") 
		readPdataUni<T>(name, this);
	else if ( ext == ".raw") // raw = uni for now 
		readPdataUni<T>(name, this);
	else 
		errMsg("particle data '" + name +"' filetype not supported for loading");
}

template<typename T>
void ParticleDataImpl<T>::save(string name) {
	if (name.find_last_of('.') == string::npos)
		errMsg("file '" + name + "' does not have an extension");
	string ext = name.substr(name.find_last_of('.'));
	if (ext == ".uni")
		writePdataUni<T>(name, this);
	else if (ext == ".raw") // raw = uni for now
		writePdataUni<T>(name, this);
	else
		errMsg("particle data '" + name +"' filetype not supported for saving");
}

// specializations

template<>
ParticleDataBase::PdataType ParticleDataImpl<Real>::getType() const {
	return ParticleDataBase::TypeReal;
} 
template<>
ParticleDataBase::PdataType ParticleDataImpl<int>::getType() const {
	return ParticleDataBase::TypeInt;
} 
template<>
ParticleDataBase::PdataType ParticleDataImpl<Vec3>::getType() const {
	return ParticleDataBase::TypeVec3;
}

// note, we need a flag value for functions such as advection
// ideally, this value should never be modified
int ParticleIndexData::flag = 0; 
Vec3 ParticleIndexData::pos = Vec3(0.,0.,0.); 

template <class T, class S>  struct knPdataAdd : public KernelBase { knPdataAdd(ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other )  { me[idx] += other[idx]; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<S>& getArg1() { return other; } typedef ParticleDataImpl<S> type1; void runMessage() { debMsg("Executing kernel knPdataAdd ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<S>& other;   };
#line 400 "particle.cpp"


template <class T, class S>  struct knPdataSub : public KernelBase { knPdataSub(ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other )  { me[idx] -= other[idx]; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<S>& getArg1() { return other; } typedef ParticleDataImpl<S> type1; void runMessage() { debMsg("Executing kernel knPdataSub ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<S>& other;   };
#line 401 "particle.cpp"


template <class T, class S>  struct knPdataMult : public KernelBase { knPdataMult(ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other )  { me[idx] *= other[idx]; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<S>& getArg1() { return other; } typedef ParticleDataImpl<S> type1; void runMessage() { debMsg("Executing kernel knPdataMult ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<S>& other;   };
#line 402 "particle.cpp"


template <class T, class S>  struct knPdataDiv : public KernelBase { knPdataDiv(ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<S>& other )  { me[idx] /= other[idx]; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<S>& getArg1() { return other; } typedef ParticleDataImpl<S> type1; void runMessage() { debMsg("Executing kernel knPdataDiv ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<S>& other;   };
#line 403 "particle.cpp"


template <class T>  struct knPdataSafeDiv : public KernelBase { knPdataSafeDiv(ParticleDataImpl<T>& me, const ParticleDataImpl<T>& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<T>& other )  { me[idx] = safeDivide(me[idx], other[idx]); }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<T>& getArg1() { return other; } typedef ParticleDataImpl<T> type1; void runMessage() { debMsg("Executing kernel knPdataSafeDiv ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<T>& other;   };
#line 404 "particle.cpp"



template <class T, class S>  struct knPdataSetScalar : public KernelBase { knPdataSetScalar(ParticleDataImpl<T>& me, const S& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const S& other )  { me[idx]  = other; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel knPdataSetScalar ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const S& other;   };
#line 406 "particle.cpp"


template <class T, class S>  struct knPdataAddScalar : public KernelBase { knPdataAddScalar(ParticleDataImpl<T>& me, const S& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const S& other )  { me[idx] += other; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel knPdataAddScalar ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const S& other;   };
#line 407 "particle.cpp"


template <class T, class S>  struct knPdataMultScalar : public KernelBase { knPdataMultScalar(ParticleDataImpl<T>& me, const S& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const S& other )  { me[idx] *= other; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel knPdataMultScalar ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const S& other;   };
#line 408 "particle.cpp"


template <class T, class S>  struct knPdataInvMultScalar : public KernelBase { knPdataInvMultScalar(ParticleDataImpl<T>& me, const S& other) :  KernelBase(me.size()) ,me(me),other(other)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const S& other )  { me[idx] = safeDivide(other, me[idx]); }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const S& getArg1() { return other; } typedef S type1; void runMessage() { debMsg("Executing kernel knPdataInvMultScalar ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other);  }   } ParticleDataImpl<T>& me; const S& other;   };
#line 409 "particle.cpp"


template <class T, class S>  struct knPdataScaledAdd : public KernelBase { knPdataScaledAdd(ParticleDataImpl<T>& me, const ParticleDataImpl<T>& other, const S& factor) :  KernelBase(me.size()) ,me(me),other(other),factor(factor)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const ParticleDataImpl<T>& other, const S& factor )  { me[idx] += factor * other[idx]; }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<T>& getArg1() { return other; } typedef ParticleDataImpl<T> type1;inline const S& getArg2() { return factor; } typedef S type2; void runMessage() { debMsg("Executing kernel knPdataScaledAdd ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other,factor);  }   } ParticleDataImpl<T>& me; const ParticleDataImpl<T>& other; const S& factor;   };
#line 410 "particle.cpp"



template <class T>  struct knPdataClamp : public KernelBase { knPdataClamp(ParticleDataImpl<T>& me, const T vmin, const T vmax) :  KernelBase(me.size()) ,me(me),vmin(vmin),vmax(vmax)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const T vmin, const T vmax )  { me[idx] = clamp(me[idx], vmin, vmax); }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const T& getArg1() { return vmin; } typedef T type1;inline const T& getArg2() { return vmax; } typedef T type2; void runMessage() { debMsg("Executing kernel knPdataClamp ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,vmin,vmax);  }   } ParticleDataImpl<T>& me; const T vmin; const T vmax;   };
#line 412 "particle.cpp"


template <class T>  struct knPdataClampMin : public KernelBase { knPdataClampMin(ParticleDataImpl<T>& me, const T vmin) :  KernelBase(me.size()) ,me(me),vmin(vmin)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const T vmin )  { me[idx] = std::max(vmin, me[idx]); }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const T& getArg1() { return vmin; } typedef T type1; void runMessage() { debMsg("Executing kernel knPdataClampMin ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,vmin);  }   } ParticleDataImpl<T>& me; const T vmin;   };
#line 413 "particle.cpp"


template <class T>  struct knPdataClampMax : public KernelBase { knPdataClampMax(ParticleDataImpl<T>& me, const T vmax) :  KernelBase(me.size()) ,me(me),vmax(vmax)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T>& me, const T vmax )  { me[idx] = std::min(vmax, me[idx]); }    inline ParticleDataImpl<T>& getArg0() { return me; } typedef ParticleDataImpl<T> type0;inline const T& getArg1() { return vmax; } typedef T type1; void runMessage() { debMsg("Executing kernel knPdataClampMax ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,vmax);  }   } ParticleDataImpl<T>& me; const T vmax;   };
#line 414 "particle.cpp"


 struct knPdataClampMinVec3 : public KernelBase { knPdataClampMinVec3(ParticleDataImpl<Vec3>& me, const Real vmin) :  KernelBase(me.size()) ,me(me),vmin(vmin)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<Vec3>& me, const Real vmin )  {
	me[idx].x = std::max(vmin, me[idx].x);
	me[idx].y = std::max(vmin, me[idx].y);
	me[idx].z = std::max(vmin, me[idx].z);
}    inline ParticleDataImpl<Vec3>& getArg0() { return me; } typedef ParticleDataImpl<Vec3> type0;inline const Real& getArg1() { return vmin; } typedef Real type1; void runMessage() { debMsg("Executing kernel knPdataClampMinVec3 ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,vmin);  }   } ParticleDataImpl<Vec3>& me; const Real vmin;   };
#line 415 "particle.cpp"


 struct knPdataClampMaxVec3 : public KernelBase { knPdataClampMaxVec3(ParticleDataImpl<Vec3>& me, const Real vmax) :  KernelBase(me.size()) ,me(me),vmax(vmax)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<Vec3>& me, const Real vmax )  {
	me[idx].x = std::min(vmax, me[idx].x);
	me[idx].y = std::min(vmax, me[idx].y);
	me[idx].z = std::min(vmax, me[idx].z);
}    inline ParticleDataImpl<Vec3>& getArg0() { return me; } typedef ParticleDataImpl<Vec3> type0;inline const Real& getArg1() { return vmax; } typedef Real type1; void runMessage() { debMsg("Executing kernel knPdataClampMaxVec3 ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,vmax);  }   } ParticleDataImpl<Vec3>& me; const Real vmax;   };
#line 420 "particle.cpp"



// python operators


template<typename T>
ParticleDataImpl<T>& ParticleDataImpl<T>::copyFrom(const ParticleDataImpl<T>& a) {
	assertMsg (a.mData.size() == mData.size() , "different pdata size "<<a.mData.size()<<" vs "<<this->mData.size() );
	mData = a.mData;
	return *this; 
}

template<typename T>
void ParticleDataImpl<T>::setConst(const T& s) {
	knPdataSetScalar<T,T> op( *this, s );
}

template<typename T>
void ParticleDataImpl<T>::setConstRange(const T& s, const int begin, const int end) {
	for(int i=begin; i<end; ++i) (*this)[i] = s;
}

// special set by flag
template <class T, class S>  struct knPdataSetScalarIntFlag : public KernelBase { knPdataSetScalarIntFlag(ParticleDataImpl<T> &me, const S &other, const ParticleDataImpl<int> &t, const int itype) :  KernelBase(me.size()) ,me(me),other(other),t(t),itype(itype)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<T> &me, const S &other, const ParticleDataImpl<int> &t, const int itype )  {
	if(t[idx]&itype) me[idx] = other;
}    inline ParticleDataImpl<T> & getArg0() { return me; } typedef ParticleDataImpl<T>  type0;inline const S& getArg1() { return other; } typedef S type1;inline const ParticleDataImpl<int> & getArg2() { return t; } typedef ParticleDataImpl<int>  type2;inline const int& getArg3() { return itype; } typedef int type3; void runMessage() { debMsg("Executing kernel knPdataSetScalarIntFlag ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,me,other,t,itype);  }   } ParticleDataImpl<T> & me; const S& other; const ParticleDataImpl<int> & t; const int itype;   };
#line 447 "particle.cpp"


template<typename T>
void ParticleDataImpl<T>::setConstIntFlag(const T &s, const ParticleDataImpl<int> &t, const int itype) {
	knPdataSetScalarIntFlag<T,T> op(*this, s, t, itype);
}

template<typename T>
void ParticleDataImpl<T>::setValuesRangeSelected(const ParticleDataImpl<T> &s, const ParticleDataImpl<int> &t, const int select, const int begin) {
	IndexInt j=begin;
	for(IndexInt i=0; i<s.size(); ++i) {
		if(t[i]==select) {
			assertMsg(j<size(), "the capacity is not enough");
			(*this)[j] = s[i];
			++j;
		}
	}
}

template<typename T>
void ParticleDataImpl<T>::add(const ParticleDataImpl<T>& a) {
	knPdataAdd<T,T> op( *this, a );
}
template<typename T>
void ParticleDataImpl<T>::sub(const ParticleDataImpl<T>& a) {
	knPdataSub<T,T> op( *this, a );
}

template<typename T>
void ParticleDataImpl<T>::addConst(const T& s) {
	knPdataAddScalar<T,T> op( *this, s );
}

template<typename T>
void ParticleDataImpl<T>::addScaled(const ParticleDataImpl<T>& a, const T& factor) {
	knPdataScaledAdd<T,T> op( *this, a, factor );
}

template<typename T>
void ParticleDataImpl<T>::mult(const ParticleDataImpl<T>& a) {
	knPdataMult<T,T> op( *this, a );
}

template<typename T>
void ParticleDataImpl<T>::safeDiv(const ParticleDataImpl<T>& a) {
	knPdataSafeDiv<T> op( *this, a );
}

template<typename T>
void ParticleDataImpl<T>::multConst(const T& s) {
	knPdataMultScalar<T,T> op( *this, s );
}


template<typename T>
void ParticleDataImpl<T>::invMultConst(const T& s) {
	knPdataInvMultScalar<T,T> op( *this, s );
}

template<typename T>
void ParticleDataImpl<T>::clamp(const Real vmin, const Real vmax) {
	knPdataClamp<T> op( *this, vmin, vmax );
}

template<typename T>
void ParticleDataImpl<T>::clampMin(const Real vmin) {
	knPdataClampMin<T> op( *this, vmin );
}
template<typename T>
void ParticleDataImpl<T>::clampMax(const Real vmax) {
	knPdataClampMax<T> op( *this, vmax );
}

template<>
void ParticleDataImpl<Vec3>::clampMin(const Real vmin) {
	knPdataClampMinVec3 op( *this, vmin );
}
template<>
void ParticleDataImpl<Vec3>::clampMax(const Real vmax) {
	knPdataClampMaxVec3 op( *this, vmax );
}

template<typename T>  struct KnPtsSum : public KernelBase { KnPtsSum(const ParticleDataImpl<T>& val, const ParticleDataImpl<int> *t, const int itype) :  KernelBase(val.size()) ,val(val),t(t),itype(itype) ,result(T(0.))  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<T>& val, const ParticleDataImpl<int> *t, const int itype ,T& result)  { if(t && !((*t)[idx]&itype)) return; result += val[idx]; }    inline operator T () { return result; } inline T  & getRet() { return result; }  inline const ParticleDataImpl<T>& getArg0() { return val; } typedef ParticleDataImpl<T> type0;inline const ParticleDataImpl<int> * getArg1() { return t; } typedef ParticleDataImpl<int>  type1;inline const int& getArg2() { return itype; } typedef int type2; void runMessage() { debMsg("Executing kernel KnPtsSum ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  T result = T(0.); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,t,itype,result); 
#pragma omp critical
{this->result += result; } }   } const ParticleDataImpl<T>& val; const ParticleDataImpl<int> * t; const int itype;  T result;  };
#line 530 "particle.cpp"


template<typename T>  struct KnPtsSumSquare : public KernelBase { KnPtsSumSquare(const ParticleDataImpl<T>& val) :  KernelBase(val.size()) ,val(val) ,result(0.)  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<T>& val ,Real& result)  { result += normSquare(val[idx]); }    inline operator Real () { return result; } inline Real  & getRet() { return result; }  inline const ParticleDataImpl<T>& getArg0() { return val; } typedef ParticleDataImpl<T> type0; void runMessage() { debMsg("Executing kernel KnPtsSumSquare ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real result = 0.; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,result); 
#pragma omp critical
{this->result += result; } }   } const ParticleDataImpl<T>& val;  Real result;  };
#line 531 "particle.cpp"


template<typename T>  struct KnPtsSumMagnitude : public KernelBase { KnPtsSumMagnitude(const ParticleDataImpl<T>& val) :  KernelBase(val.size()) ,val(val) ,result(0.)  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<T>& val ,Real& result)  { result += norm(val[idx]); }    inline operator Real () { return result; } inline Real  & getRet() { return result; }  inline const ParticleDataImpl<T>& getArg0() { return val; } typedef ParticleDataImpl<T> type0; void runMessage() { debMsg("Executing kernel KnPtsSumMagnitude ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real result = 0.; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,result); 
#pragma omp critical
{this->result += result; } }   } const ParticleDataImpl<T>& val;  Real result;  };
#line 532 "particle.cpp"



template<typename T>
T ParticleDataImpl<T>::sum(const ParticleDataImpl<int> *t, const int itype) const {
	return KnPtsSum<T>(*this, t, itype);
}
template<typename T>
Real ParticleDataImpl<T>::sumSquare() const {
	return KnPtsSumSquare<T>(*this);
}
template<typename T>
Real ParticleDataImpl<T>::sumMagnitude() const {
	return KnPtsSumMagnitude<T>(*this);
}

template<typename T>

 struct CompPdata_Min : public KernelBase { CompPdata_Min(const ParticleDataImpl<T>& val) :  KernelBase(val.size()) ,val(val) ,minVal(std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<T>& val ,Real& minVal)  {
	if (val[idx] < minVal) minVal = val[idx];
}    inline operator Real () { return minVal; } inline Real  & getRet() { return minVal; }  inline const ParticleDataImpl<T>& getArg0() { return val; } typedef ParticleDataImpl<T> type0; void runMessage() { debMsg("Executing kernel CompPdata_Min ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real minVal = std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,minVal); 
#pragma omp critical
{this->minVal = min(minVal, this->minVal); } }   } const ParticleDataImpl<T>& val;  Real minVal;  };
#line 549 "particle.cpp"



template<typename T>

 struct CompPdata_Max : public KernelBase { CompPdata_Max(const ParticleDataImpl<T>& val) :  KernelBase(val.size()) ,val(val) ,maxVal(-std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<T>& val ,Real& maxVal)  {
	if (val[idx] > maxVal) maxVal = val[idx];
}    inline operator Real () { return maxVal; } inline Real  & getRet() { return maxVal; }  inline const ParticleDataImpl<T>& getArg0() { return val; } typedef ParticleDataImpl<T> type0; void runMessage() { debMsg("Executing kernel CompPdata_Max ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real maxVal = -std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,maxVal); 
#pragma omp critical
{this->maxVal = max(maxVal, this->maxVal); } }   } const ParticleDataImpl<T>& val;  Real maxVal;  };
#line 555 "particle.cpp"



template<typename T>
Real ParticleDataImpl<T>::getMin() const {
	return CompPdata_Min<T>(*this);
}

template<typename T>
Real ParticleDataImpl<T>::getMaxAbs() const {
	Real amin = CompPdata_Min<T>(*this);
	Real amax = CompPdata_Max<T>(*this);
	return max( fabs(amin), fabs(amax));
}

template<typename T>
Real ParticleDataImpl<T>::getMax() const {
	return CompPdata_Max<T>(*this);

} 

template<typename T>
void ParticleDataImpl<T>::printPdata(IndexInt start, IndexInt stop, bool printIndex)
{
	std::ostringstream sstr;
	IndexInt s = (start>0 ? start : 0                      );
	IndexInt e = (stop>0  ? stop  : (IndexInt)mData.size() );
	s = Manta::clamp(s, (IndexInt)0, (IndexInt)mData.size());
	e = Manta::clamp(e, (IndexInt)0, (IndexInt)mData.size());

	for(IndexInt i=s; i<e; ++i) {
		if(printIndex) sstr << i<<": ";
		sstr<<mData[i]<<" "<<"\n";
	} 
	debMsg( sstr.str() , 1 );
}

// specials for vec3
// work on length values, ie, always positive (in contrast to scalar versions above)


 struct CompPdata_MinVec3 : public KernelBase { CompPdata_MinVec3(const ParticleDataImpl<Vec3>& val) :  KernelBase(val.size()) ,val(val) ,minVal(std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<Vec3>& val ,Real& minVal)  {
	const Real s = normSquare(val[idx]);
	if (s < minVal) minVal = s;
}    inline operator Real () { return minVal; } inline Real  & getRet() { return minVal; }  inline const ParticleDataImpl<Vec3>& getArg0() { return val; } typedef ParticleDataImpl<Vec3> type0; void runMessage() { debMsg("Executing kernel CompPdata_MinVec3 ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real minVal = std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,minVal); 
#pragma omp critical
{this->minVal = min(minVal, this->minVal); } }   } const ParticleDataImpl<Vec3>& val;  Real minVal;  };
#line 597 "particle.cpp"




 struct CompPdata_MaxVec3 : public KernelBase { CompPdata_MaxVec3(const ParticleDataImpl<Vec3>& val) :  KernelBase(val.size()) ,val(val) ,maxVal(-std::numeric_limits<Real>::max())  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<Vec3>& val ,Real& maxVal)  {
	const Real s = normSquare(val[idx]);
	if (s > maxVal) maxVal = s;
}    inline operator Real () { return maxVal; } inline Real  & getRet() { return maxVal; }  inline const ParticleDataImpl<Vec3>& getArg0() { return val; } typedef ParticleDataImpl<Vec3> type0; void runMessage() { debMsg("Executing kernel CompPdata_MaxVec3 ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  Real maxVal = -std::numeric_limits<Real>::max(); 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,val,maxVal); 
#pragma omp critical
{this->maxVal = max(maxVal, this->maxVal); } }   } const ParticleDataImpl<Vec3>& val;  Real maxVal;  };
#line 603 "particle.cpp"



template<>
Real ParticleDataImpl<Vec3>::getMin() const {
	return sqrt(CompPdata_MinVec3(*this));
}

template<>
Real ParticleDataImpl<Vec3>::getMaxAbs() const {
	return sqrt(CompPdata_MaxVec3(*this));  // no minimum necessary here
}

template<>
Real ParticleDataImpl<Vec3>::getMax() const {
	return sqrt(CompPdata_MaxVec3(*this));
}

// utilities


 struct knCountParticles : public KernelBase { knCountParticles(const ParticleDataImpl<int> &t, const int flag) :  KernelBase(t.size()) ,t(t),flag(flag) ,result(0)  { runMessage(); run(); }   inline void op(IndexInt idx, const ParticleDataImpl<int> &t, const int flag ,int& result)  { if(t[idx]&flag) { ++result; } }    inline operator int () { return result; } inline int  & getRet() { return result; }  inline const ParticleDataImpl<int> & getArg0() { return t; } typedef ParticleDataImpl<int>  type0;inline const int& getArg1() { return flag; } typedef int type1; void runMessage() { debMsg("Executing kernel knCountParticles ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  int result = 0; 
#pragma omp for nowait  
  for (IndexInt i = 0; i < _sz; i++) op(i,t,flag,result); 
#pragma omp critical
{this->result += result; } }   } const ParticleDataImpl<int> & t; const int flag;  int result;  };
#line 626 "particle.cpp"


int countParticles(const ParticleDataImpl<int> &t, const int flag) { return knCountParticles(t, flag); } static PyObject* _W_1 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "countParticles" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; const ParticleDataImpl<int> & t = *_args.getPtr<ParticleDataImpl<int>  >("t",0,&_lock); const int flag = _args.get<int >("flag",1,&_lock);   _retval = toPy(countParticles(t,flag));  _args.check(); } pbFinalizePlugin(parent,"countParticles", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("countParticles",e.what()); return 0; } } static const Pb::Register _RP_countParticles ("","countParticles",_W_1);  extern "C" { void PbRegister_countParticles() { KEEP_UNUSED(_RP_countParticles); } } 



 struct knGetGridIdx : public KernelBase { knGetGridIdx(ParticleDataImpl<int> &gidx, const BasicParticleSystem &parts, const GridBase &grid) :  KernelBase(gidx.size()) ,gidx(gidx),parts(parts),grid(grid)   { runMessage(); run(); }   inline void op(IndexInt idx, ParticleDataImpl<int> &gidx, const BasicParticleSystem &parts, const GridBase &grid )  {
	const Vec3i p = toVec3i(parts.getPos(idx));
	gidx[idx] = (grid.isInBounds(p)) ? grid.index(p) : -1;
}    inline ParticleDataImpl<int> & getArg0() { return gidx; } typedef ParticleDataImpl<int>  type0;inline const BasicParticleSystem& getArg1() { return parts; } typedef BasicParticleSystem type1;inline const GridBase& getArg2() { return grid; } typedef GridBase type2; void runMessage() { debMsg("Executing kernel knGetGridIdx ", 3); debMsg("Kernel range" <<  " size "<<  size  << " "   , 4); }; void run() {   const IndexInt _sz = size; 
#pragma omp parallel 
 {  
#pragma omp for  
  for (IndexInt i = 0; i < _sz; i++) op(i,gidx,parts,grid);  }   } ParticleDataImpl<int> & gidx; const BasicParticleSystem& parts; const GridBase& grid;   };
#line 631 "particle.cpp"


void getGridIdx(ParticleDataImpl<int> &gidx, const BasicParticleSystem &parts, const GridBase &grid) { knGetGridIdx(gidx, parts, grid); } static PyObject* _W_2 (PyObject* _self, PyObject* _linargs, PyObject* _kwds) { try { PbArgs _args(_linargs, _kwds); FluidSolver *parent = _args.obtainParent(); bool noTiming = _args.getOpt<bool>("notiming", -1, 0); pbPreparePlugin(parent, "getGridIdx" , !noTiming ); PyObject *_retval = 0; { ArgLocker _lock; ParticleDataImpl<int> & gidx = *_args.getPtr<ParticleDataImpl<int>  >("gidx",0,&_lock); const BasicParticleSystem& parts = *_args.getPtr<BasicParticleSystem >("parts",1,&_lock); const GridBase& grid = *_args.getPtr<GridBase >("grid",2,&_lock);   _retval = getPyNone(); getGridIdx(gidx,parts,grid);  _args.check(); } pbFinalizePlugin(parent,"getGridIdx", !noTiming ); return _retval; } catch(std::exception& e) { pbSetError("getGridIdx",e.what()); return 0; } } static const Pb::Register _RP_getGridIdx ("","getGridIdx",_W_2);  extern "C" { void PbRegister_getGridIdx() { KEEP_UNUSED(_RP_getGridIdx); } } 

// explicit instantiation
template class ParticleDataImpl<int>;
template class ParticleDataImpl<Real>;
template class ParticleDataImpl<Vec3>;


} // namespace



