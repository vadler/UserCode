#ifndef Input_MCFileSource_12_h
#define Input_MCFileSource_12_h

/** \class MCFileSource_12
 *
 * Reads in HepMC events
 * Joanna Weng & Filip Moortgat 08/2005 
 ***************************************/

#include "FWCore/Framework/interface/ExternalInputSource.h"
#include "IOMC/Input/interface/HepMCFileReader_12.h"
#include <map>
#include <string>

class HepMCFileReader_12;

namespace edm
{
  class MCFileSource_12 : public ExternalInputSource {
  public:
    MCFileSource_12(const ParameterSet &, const InputSourceDescription &);
   virtual ~MCFileSource_12();
// the following cannot be used anymore since an explicit InputSourceDescription is needed ?? FM
/*
    MCFileSource_12(const std::string& processName);
    /// Specify the file to be read. FIXME: should be done by the "configuration"
    MCFileSource_12(const std::string& filename, const std::string& processName);
*/

  private:
   
   virtual bool produce(Event &e);
    void clear();
    
    HepMCFileReader_12 * reader_;
    
    HepMC::GenEvent  *evt;
    	
  };
} 

#endif
