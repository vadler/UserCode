#ifndef DataFormats_PatCandidates_TriggerObjectStandAlone_h
#define DataFormats_PatCandidates_TriggerObjectStandAlone_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerObjectStandAlone
//
// $Id: TriggerObjectStandAlone.h,v 1.12 2010/12/20 20:05:52 vadler Exp $
//
/**
  \class    pat::TriggerObjectStandAlone TriggerObjectStandAlone.h "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
  \brief    Analysis-level trigger object class (stand-alone)

   TriggerObjectStandAlone implements a container for trigger objects' information within the 'pat' namespace.
   These Trigger objects keep also information on filters and paths to be saved independently or embedded into PAT objects.
   The TriggerObjectStandAlone is also the data format used in the PAT trigger object matching.
   For detailed information, consult
   https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePATTrigger#TriggerObjectStandAlone

  \author   Volker Adler
  \version  $Id: TriggerObjectStandAlone.h,v 1.12 2010/12/20 20:05:52 vadler Exp $
*/


#include "DataFormats/PatCandidates/interface/TriggerObject.h"


namespace pat {

  class TriggerObjectStandAlone : public TriggerObject {

      /// Data Members
      /// Keeping the old names of the data members for backward compatibility,
      /// although they refer only to HLT objects.

      /// Vector of labels of all HLT filters or names od L1 conditions the trigger objects has been used in
      std::vector< std::string > filterLabels_;
      /// Vector of names of all HLT paths or L1 algorithms the trigger objects has been used in
      std::vector< std::string > pathNames_;
      /// Vector alligned with 'pathNames_' of boolean indicating the usage of the trigger object
      /// An element is true, if the corresponding path succeeded and the trigger object was used in the last filter (HLT
      /// or the corresponding algorithm succeeded as well as the corresponding condition (L1).
      /// The vector is empty for data (size 0), if the according information is not available in data.
      std::vector< bool > pathLastFilterAccepted_;

      /// Constants

      /// Constant defining the wild-card used in 'hasAnyName()'
      static const char wildcard_ = '*';

      /// Private methods

      /// Checks a string vector for occurence of a certain string, incl. wild-card mechanism
      bool hasAnyName( const std::string & name, const std::vector< std::string > & nameVec ) const;
      /// Adds a new HLT filter label or L1 condition name
      void addFilterOrCondition( const std::string & name ) { if ( ! hasFilterOrCondition( name ) ) filterLabels_.push_back( name ); };
      /// Adds a new HLT path or L1 algorithm name
      void addPathOrAlgorithm( const std::string & name, bool firing = true );
      /// Gets all HLT filter labels or L1 condition names
      std::vector< std::string > filtersOrConditions() const { return filterLabels_; };
      /// Gets all HLT path or L1 algorithm names
      std::vector< std::string > pathsOrAlgorithms( bool firing = true ) const;
      /// Checks, if a certain HLT filter label or L1 condition name is assigned
      bool hasFilterOrCondition( const std::string & name ) const;
      /// Checks, if a certain HLT path or L1 algorithm name is assigned
      bool hasPathOrAlgorithm( const std::string & name, bool firing = true ) const;
      /// Checks, if the usage indicator vector has been filled
      bool hasFiring() const { return ( pathLastFilterAccepted_.size() > 0 && pathLastFilterAccepted_.size() == pathNames_.size() ); };
      /// Prints warnings, if an HLT objects functions is used with an L1 or inconsistent objects
      void checkHlt() const;
      /// Prints warnings, if an L1 objects functions is used with an HLT or inconsistent objects
      void checkL1() const;

    public:

      /// Constructors and Destructor

      /// Default constructor
      TriggerObjectStandAlone() : TriggerObject() {};
      /// Constructor from pat::TriggerObject
      TriggerObjectStandAlone( const TriggerObject & trigObj ) : TriggerObject( trigObj ) {};
      /// Constructor from trigger::TriggerObject
      TriggerObjectStandAlone( const trigger::TriggerObject & trigObj ) : TriggerObject( trigObj ) {};
      /// Constructor from reco::Candidate
      TriggerObjectStandAlone( const reco::LeafCandidate & leafCand ) : TriggerObject( leafCand ) {};
      /// Constructors from Lorentz-vectors and (optional) PDG ID
      TriggerObjectStandAlone( const reco::Particle::LorentzVector & vec, int id = 0 ) : TriggerObject( vec, id ) {};
      TriggerObjectStandAlone( const reco::Particle::PolarLorentzVector & vec, int id = 0 ) : TriggerObject( vec, id ) {};

      /// Destructor
      virtual ~TriggerObjectStandAlone() {};

      /// Methods

      /// Adds a new HLT filter label
      void addFilterLabel( const std::string & filterLabel );
      /// Adds a new L1 condition name
      void addConditionName( const std::string & conditionName );
      /// Adds a new HLT path name
      void addPathName( const std::string & pathName, bool pathLastFilterAccepted = true );
      /// Adds a new L1 algorithm name
      void addAlgorithmName( const std::string & algorithmName, bool algoCondAccepted = true );
      /// Gets all HLT filter labels
      std::vector< std::string > filterLabels() const;
      /// Gets all L1 condition names
      std::vector< std::string > conditionNames() const;
      /// Gets all HLT path names
      std::vector< std::string > pathNames( bool pathLastFilterAccepted = true ) const;
      /// Gets all L1 algorithm names
      std::vector< std::string > algorithmNames( bool algoCondAccepted = true ) const;
      /// Gets the pat::TriggerObject (parent class)
      TriggerObject triggerObject();
      /// Checks, if a certain HLT filter label is assigned
      bool hasFilterLabel( const std::string & filterLabel ) const;
      /// Checks, if a certain L1 condition name is assigned
      bool hasConditionName( const std::string & conditionName ) const;
      /// Checks, if a certain HLT path name is assigned
      bool hasPathName( const std::string & pathName, bool pathLastFilterAccepted = true ) const;
      /// Checks, if a certain L1 algorithm name is assigned
      bool hasAlgorithmName( const std::string & algorithmName, bool algoCondAccepted = true ) const;
      /// Checks, if a certain label of original collection is assigned (method overrides)
      virtual bool hasCollection( const std::string & collName ) const;
      virtual bool hasCollection( const edm::InputTag & collName ) const { return hasCollection( collName.encode() ); };
      /// Checks, if the usage indicator vector has been filled
      bool hasPathLastFilterAccepted() const;
      bool hasAlgoCondAccepted() const;

      /// Special methods for the cut string parser
      /// - argument types usable in the cut string parser
      /// - short names for readable configuration files

      /// Calls 'hasFilterLabel(...)'
      bool filter( const std::string & filterLabel ) const { return hasFilterLabel( filterLabel ); };
      /// Calls 'hasConditionName(...)'
      bool cond( const std::string & conditionName ) const { return hasConditionName( conditionName ); };
      /// Calls 'hasPathName(...)'
      bool path( const std::string & pathName, unsigned pathLastFilterAccepted = 1 ) const { return hasPathName( pathName, bool( pathLastFilterAccepted ) ); };
      /// Calls 'hasAlgorithmName(...)'
      bool algo( const std::string & algorithmName, unsigned algoCondAccepted = 1 ) const { return hasAlgorithmName( algorithmName, bool( algoCondAccepted ) ); };
      /// Calls 'hasCollection(...)' (method override)
      virtual bool coll( const std::string & collName ) const { return hasCollection( collName ); };

  };


  /// Collection of TriggerObjectStandAlone
  typedef std::vector< TriggerObjectStandAlone >                      TriggerObjectStandAloneCollection;
  /// Persistent reference to an item in a TriggerObjectStandAloneCollection
  typedef edm::Ref< TriggerObjectStandAloneCollection >               TriggerObjectStandAloneRef;
  /// Persistent reference to a TriggerObjectStandAloneCollection product
  typedef edm::RefProd< TriggerObjectStandAloneCollection >           TriggerObjectStandAloneRefProd;
  /// Vector of persistent references to items in the same TriggerObjectStandAloneCollection
  typedef edm::RefVector< TriggerObjectStandAloneCollection >         TriggerObjectStandAloneRefVector;
  /// Const iterator over vector of persistent references to items in the same TriggerObjectStandAloneCollection
  typedef edm::RefVectorIterator< TriggerObjectStandAloneCollection > TriggerObjectStandAloneRefVectorIterator;
  /// Association of TriggerObjectStandAlones to store matches to Candidates
  typedef edm::Association< TriggerObjectStandAloneCollection >       TriggerObjectStandAloneMatch;

}


#endif
