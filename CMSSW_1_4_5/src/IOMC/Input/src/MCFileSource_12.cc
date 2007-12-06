/**
*  See header file for a description of this class.
*
*
*  $Date: 2006/04/19 14:52:03 $
*  $Revision: 1.8 $
*  \author Jo. Weng  - CERN, Ph Division & Uni Karlsruhe
*  \author F.Moortgat - CERN, Ph Division
*/


#include "IOMC/Input/interface/HepMCFileReader_12.h" 
#include "IOMC/Input/interface/MCFileSource_12.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <iostream>



using namespace edm;
using namespace std;

MCFileSource_12::MCFileSource_12( const ParameterSet & pset, InputSourceDescription const& desc ) :
ExternalInputSource(pset, desc),
reader_( HepMCFileReader_12::instance() ),
evt(0) {
	
	cout << "MCFileSource_12:Reading HepMC file: " << fileNames()[0] << endl;
	string fileName = fileNames()[0];
	// strip the file: 
	if ( ! fileName.find("file:")){
	  fileName.erase(0,5);
	}  
  
	reader_->initialize(fileName);  
	produces<HepMCProduct>();
}


MCFileSource_12::~MCFileSource_12(){
	clear();
}

void MCFileSource_12::clear() {

}


bool MCFileSource_12::produce(Event & e) {
	
	// no need to clean up GenEvent memory - now done in HepMCProduct
//	if (evt != 0) {
//	  delete evt;
//	  evt = 0;
//	}
	auto_ptr<HepMCProduct> bare_product(new HepMCProduct());  
	cout << "MCFileSource_12: Start Reading  " << endl;
	evt = reader_->fillCurrentEventData(); 
        if (evt == 0) return false;
	bare_product->addHepMCData(evt);
		
	e.put(bare_product);

        return true;
}
