#ifndef HepMCFileReader_12_H
#define HepMCFileReader_12_H

/** \class HepMCFileReader_12
* 
*  This class is used by the implementation of DaqEventFactory present 
*  in this package to read in the full event raw data from a flat 
*  binary file. 
*  WARNING: If you want to use this class for other purposes you must 
*  always invoke the method initialize before starting using the interface
*  it exposes.
*
*  $Date: 2007/03/22 18:17:57 $
*  $Revision: 1.2 $
*  \author G. Bruno - CERN, EP Division
*/   
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "HepMC/GenEvent.h"
#include "IOMC/Input/interface/IO_Ascii_V12.h"
// #include "HepMC/IO_Ascii.h"

class HepMCFileReader_12 {
	
	protected:
	
	/// Constructor
	HepMCFileReader_12();
	void setInitialized(bool value);
	
	public:	
	/// Destructor
	virtual ~HepMCFileReader_12();	
	static HepMCFileReader_12 * instance();
	virtual void initialize(const std::string & filename);	
	bool isInitialized();

	
	virtual bool setEvent(int event);
	virtual bool readCurrentEvent();
	virtual bool printHepMcEvent() const;	
	HepMC::GenEvent* fillCurrentEventData();
	  //	virtual bool fillEventData(HepMC::GenEvent  * event);
	// this method prints the event information as 
	// obtained by the input file in HepEvt style
	void printEvent() const;
	// get all the 'integer' properties of a particle 
	// like mother, daughter, pid and status
	// 'j' is the number of the particle in the HepMc
	virtual void getStatsFromTuple(int &mo1, int &mo2, 
	int &da1, int &da2, 
	int &status, int &pid,
	int j) const;
	virtual void ReadStats();



	private:
	static HepMCFileReader_12 * instance_;
	// current  HepMC evt
	HepMC::GenEvent * evt;
	bool initialized_;
        HepMC::IO_Ascii_V12 * input_;
      	// # of particles in evt
	int  nParticles;

	//maps to convert HepMC::GenParticle to particles # and vice versa
	// -> needed for HepEvt like output
	std::vector<HepMC::GenParticle*> index_to_particle;  
	std::map<HepMC::GenParticle *,int> particle_to_index;    
	// find index to HepMC::GenParticle* p in map m
	int find_in_map(const std::map<HepMC::GenParticle*,int>& m,
	HepMC::GenParticle* p) const;
	

	
};

#endif // HepMCFileReader_12_H

