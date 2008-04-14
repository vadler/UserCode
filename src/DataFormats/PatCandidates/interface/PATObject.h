#ifndef DataFormats_PatCandidates_PATObject_h
#define DataFormats_PatCandidates_PATObject_h

/** \class    pat::PATObject PATObject.h "DataFormats/PatCandidates/interface/PATObject.h"
 *
 *  \brief    Templated PAT object container
 *
 *  PATObject is the templated base PAT object that wraps around reco objects.
 *
 *  \author   Steven Lowette
 *
 *  \version  $Id$
 *
 */

#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/StringMap.h"
#include <vector>


namespace pat {

  template <class ObjectType>
  class PATObject : public ObjectType {

    public:

      /// default constructor
      PATObject();
      /// constructor from a base object (leaves invalid reference to original object!)
      PATObject(const ObjectType & obj);
      /// constructor from reference
      PATObject(const edm::RefToBase<ObjectType> & ref);
      /// destructor
      virtual ~PATObject() {}
      /// access to the original object; returns zero for null Ref and throws for unavailable collection
      const ObjectType * originalObject() const;
      /// reference to original object. Returns a null reference if not available
      const edm::RefToBase<ObjectType> & originalObjectRef() const;
      /// standard deviation on A (see CMS Note 2006/023)
      float resolutionA() const;
      /// standard deviation on B (see CMS Note 2006/023)
      float resolutionB() const;
      /// standard deviation on C (see CMS Note 2006/023)
      float resolutionC() const;
      /// standard deviation on D (see CMS Note 2006/023)
      float resolutionD() const;
      /// standard deviation on transverse energy
      float resolutionEt() const;
      /// standard deviation on pseudorapidity
      float resolutionEta() const;
      /// standard deviation on azimuthal angle
      float resolutionPhi() const;
      /// standard deviation on polar angle
      float resolutionTheta() const;
      /// covariance matrix elements
      const std::vector<float> & covMatrix() const;
      /// set standard deviation on A (see CMS Note 2006/023)
      void setResolutionA(float a);
      /// set standard deviation on B (see CMS Note 2006/023)
      void setResolutionB(float b);
      /// set standard deviation on C (see CMS Note 2006/023)
      void setResolutionC(float c);
      /// set standard deviation on D (see CMS Note 2006/023)
      void setResolutionD(float d);
      /// set standard deviation on transverse energy
      void setResolutionEt(float et);
      /// set standard deviation on pseudorapidity
      void setResolutionEta(float eta);
      /// set standard deviation on azimuthal angle
      void setResolutionPhi(float phi);
      /// set standard deviation on polar angle
      void setResolutionTheta(float theta);
      /// set covariance matrix elements
      void setCovMatrix(const std::vector<float> & c);

      /// return the matches to the L1 trigger primitives
      const reco::Particle * l1Primitives() const;
      /// return match to a particular L1 trigger primitive
      const reco::Particle * l1Primitive( size_t idx ) const;
      /// return match to a particular L1 trigger primitive given the L1 trigger name
      const reco::Particle * l1PrimitiveByTrigger( const std::string & ltn ) const;
      /// return the matches to the HLT trigger primitives
      const reco::Particle * hltPrimitives() const;
      /// return match to a particular HLT trigger primitive
      const reco::Particle * hltPrimitive( size_t idx) const;
      /// return match to a particular HLT trigger primitive given the HLT trigger and filter name
      const reco::Particle * hltPrimitiveByTrigger( const std::string & htn, const std::string & hfn ) const;
      /// add an L1 trigger primitive
      void addL1Primitive( const reco::Particle & lp, const std::string & ltn );
      /// add an HLT trigger primitive
      void addHLTPrimitive( const reco::Particle & hp, const std::string & htn, const std::string & hfn );
      /// sort L1 map
      /// should be done before using the trigger names' map after every adding new items
      void sortL1Map();
      /// sort HLT maps
      /// should be done before using the trigger/filter names' maps after every adding new items
      void sortHLTMaps();
      
    protected:
      /// reference back to the original object
      edm::RefToBase<ObjectType> refToOrig_;
      /// standard deviation on transverse energy
      float resEt_;
      /// standard deviation on pseudorapidity
      float resEta_;
      /// standard deviation on azimuthal angle
      float resPhi_;
      /// standard deviation on A (see CMS Note 2006/023)
      float resA_;
      /// standard deviation on B (see CMS Note 2006/023)
      float resB_;
      /// standard deviation on C (see CMS Note 2006/023)
      float resC_;
      /// standard deviation on D (see CMS Note 2006/023)
      float resD_;
      /// standard deviation on polar angle
      float resTheta_;
      /// covariance matrix elements
      std::vector<float> covM_;
      /// trigger info
      std::vector<reco::Particle> l1Primitives_;
      StringMap l1TriggerNames_;
      std::vector<reco::Particle> hltPrimitives_;
      StringMap hltTriggerNames_;
      StringMap hltFilterNames_;

  };


  template <class ObjectType> PATObject<ObjectType>::PATObject() :
    resEt_(0), resEta_(0), resPhi_(0), resA_(0), resB_(0), resC_(0), resD_(0), resTheta_(0) {
  }

  template <class ObjectType> PATObject<ObjectType>::PATObject(const ObjectType & obj) :
    ObjectType(obj),
    refToOrig_(),
    resEt_(0), resEta_(0), resPhi_(0), resA_(0), resB_(0), resC_(0), resD_(0),  resTheta_(0) {
  }

  template <class ObjectType> PATObject<ObjectType>::PATObject(const edm::RefToBase<ObjectType> & ref) :
    ObjectType(*ref),
    refToOrig_(ref),
    resEt_(0), resEta_(0), resPhi_(0), resA_(0), resB_(0), resC_(0), resD_(0),  resTheta_(0) {
  }

  template <class ObjectType> const ObjectType * PATObject<ObjectType>::originalObject() const {
    if (refToOrig_.isNull()) {
      // this object was not produced from a reference, so no link to the
      // original object exists -> return a 0-pointer
      return 0;
    // GIO: temporary disable the following: I have to find out why there is no "isAvailable()" in RefToBase...
    //     vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    //} else if (!refToOrig_.isAvailable()) {
    //  throw edm::Exception(edm::errors::ProductNotFound) << "The original collection from which this PAT object was made is not present any more in the event, hence you cannot access the originating object anymore.";
    //  return 0;
    //      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    } else {
      return refToOrig_.get();
    }
  }

  template <class ObjectType> 
  const edm::RefToBase<ObjectType> & PATObject<ObjectType>::originalObjectRef() const { return refToOrig_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionEt() const { return resEt_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionEta() const { return resEta_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionPhi() const { return resPhi_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionA() const { return resA_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionB() const { return resB_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionC() const { return resC_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionD() const { return resD_; }

  template <class ObjectType> 
  float PATObject<ObjectType>::resolutionTheta() const { return resTheta_; }

  template <class ObjectType> 
  const std::vector<float> & PATObject<ObjectType>::covMatrix() const { return covM_; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionEt(float et) { resEt_ = et; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionEta(float eta) { resEta_ = eta; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionPhi(float phi) { resPhi_ = phi; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionA(float a) { resA_ = a; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionB(float b) { resB_ = b; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionC(float c) { resC_ = c; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionD(float d) { resD_ = d; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setResolutionTheta(float theta) { resTheta_ = theta; }

  template <class ObjectType> 
  void PATObject<ObjectType>::setCovMatrix(const std::vector<float> & c) {
    //    covM_.clear();
    //    for (size_t i = 0; i < c.size(); i++) covM_.push_back(c[i]); 
    covM_ = c;
  }

  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::l1Primitives() const {
    return ( l1Primitives_.size() > 0 ? &l1Primitives_.front() : 0 );
  }

  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::l1Primitive( size_t idx ) const {
    return ( l1Primitives_.size() > idx ? &l1Primitives_[ idx ] : 0 );
  }
                  
  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::l1PrimitiveByTrigger( const std::string & ltn ) const {
    return ( l1TriggerNames_[ ltn ] == -1 ? 0 : l1Primitive( l1TriggerNames_[ ltn ] ) );
  }

  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::hltPrimitives() const {
    return ( hltPrimitives_.size() > 0 ? &hltPrimitives_.front() : 0 );
  }

  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::hltPrimitive( size_t idx ) const {
    return ( hltPrimitives_.size() > idx ? &hltPrimitives_[ idx ] : 0 );
  }
                  
  template <class ObjectType> 
  const reco::Particle * PATObject<ObjectType>::hltPrimitiveByTrigger( const std::string & htn, const std::string & hfn ) const {
    for ( size_t itn = 0; itn < hltTriggerNames_.size(); ++itn ) {
      if ( hltTriggerNames_.get( itn ).first == htn ) {
	for ( size_t ifn = 0; ifn < hltFilterNames_.size(); ++ifn ) {
	  if ( hltFilterNames_.get( ifn ).second == hltTriggerNames_.get( itn ).second && hltTriggerNames_.get( ifn ).first == hfn ) {
	    return &hltPrimitives_[ hltTriggerNames_.get( itn ).second ];
	  }
	}
      }
    }
    return 0;
  }

  template <class ObjectType> 
  void PATObject<ObjectType>::addL1Primitive( const reco::Particle & lp, const std::string & ltn ) {
    l1Primitives_.push_back( lp );
    l1TriggerNames_.add( ltn, l1Primitives_.size()-1 );
  }

  template <class ObjectType> 
  void PATObject<ObjectType>::addHLTPrimitive( const reco::Particle & hp, const std::string & htn, const std::string & hfn ) {
    hltPrimitives_.push_back( hp );
    hltTriggerNames_.add( htn, l1Primitives_.size()-1 );
    hltFilterNames_.add( hfn, l1Primitives_.size()-1 );
  }
  
  template <class ObjectType>
  void PATObject<ObjectType>::sortL1Map() {
    l1TriggerNames_.sort();
  }
  
  template <class ObjectType>
  void PATObject<ObjectType>::sortHLTMaps() {
    hltTriggerNames_.sort();
    hltFilterNames_.sort();
  }

}

#endif