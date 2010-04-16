#ifndef DataFormats_PatCandidates_TriggerAlgorithm_h
#define DataFormats_PatCandidates_TriggerAlgorithm_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerAlgorithm
//
// $Id$
//
/**
  \class    pat::TriggerAlgorithm TriggerAlgorithm.h "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"
  \brief    Analysis-level L1 trigger algorithm class

   TriggerAlgorithm implements a container for L1 trigger algorithms' information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id$
*/


#include <string>
#include <vector>

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/RefVectorIterator.h"


namespace pat {

  class TriggerAlgorithm {

      /// data members
      std::string name_;
      bool        tech_;
      unsigned    bit_;    // in trigger table
      unsigned    prescale_;
      bool        run_;
      bool        accept_;
      bool        error_;

    public:

      /// constructors and desctructor
      TriggerAlgorithm();
      TriggerAlgorithm( const std::string & name );
      TriggerAlgorithm( const std::string & name, bool tech, unsigned bit, unsigned prescale, bool run, bool accept, bool error );
      virtual ~TriggerAlgorithm() {};

      /// setters & getters
      void setName( const std::string & name ) { name_     = name; };
      void setTechTrigger( bool tech )         { tech_     = tech; };
      void setBit( unsigned bit )              { bit_    = bit; };
      void setPrescale( unsigned prescale )    { prescale_ = prescale; };
      void setRun( bool run )                  { run_      = run; };
      void setAccept( bool accept )            { accept_   = accept; };
      void setError( bool error )              { error_    = error; };
      std::string name() const          { return name_; };
      bool        isTechTrigger() const { return tech_; };
      unsigned    bit() const           { return bit_; };
      unsigned    prescale() const      { return prescale_; };
      bool        wasRun() const        { return run_; };
      bool        wasAccept() const     { return accept_; };
      bool        wasError() const      { return error_; };

  };


  /// collection of TriggerAlgorithm
  typedef std::vector< TriggerAlgorithm >                      TriggerAlgorithmCollection;
  /// persistent reference to an item in a TriggerAlgorithmCollection
  typedef edm::Ref< TriggerAlgorithmCollection >               TriggerAlgorithmRef;
  /// persistent reference to a TriggerAlgorithmCollection product
  typedef edm::RefProd< TriggerAlgorithmCollection >           TriggerAlgorithmRefProd;
  /// vector of persistent references to items in the same TriggerAlgorithmCollection
  typedef edm::RefVector< TriggerAlgorithmCollection >         TriggerAlgorithmRefVector;
  /// const iterator over vector of persistent references to items in the same TriggerAlgorithmCollection
  typedef edm::RefVectorIterator< TriggerAlgorithmCollection > TriggerAlgorithmRefVectorIterator;

}


#endif
