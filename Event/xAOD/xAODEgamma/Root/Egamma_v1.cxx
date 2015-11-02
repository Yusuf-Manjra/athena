/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Egamma_v1$

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODEgamma/versions/Egamma_v1.h"
#include "EgammaAccessors_v1.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h" 
#include <stdexcept>

namespace xAOD {

  Egamma_v1::Egamma_v1()
    : IParticle(), m_p4(), m_p4Cached( false ) {
  }
  
  Egamma_v1::Egamma_v1(const Egamma_v1& eg) : IParticle(eg), m_p4(eg.m_p4), m_p4Cached(eg.m_p4Cached) {
  }

 Egamma_v1& Egamma_v1::operator=(const Egamma_v1& eg ){

    if (this != &eg){ // protect against invalid self-assignment      

      if (!this->container() && !this->hasStore() ) {      
	  makePrivateStore();
      }
      this->IParticle::operator=( eg );
      this->m_p4 = eg.m_p4;
      this->m_p4Cached = eg.m_p4Cached;
    }
    // by convention, always return *this
    return *this;
 }


  double Egamma_v1::pt() const {
    static Accessor< float > acc( "pt" );
    return acc( *this );
  }
  
  double Egamma_v1::eta() const {
    static Accessor<float > acc( "eta" );
    return acc( *this );
  }
  
  double Egamma_v1::phi() const {
    static Accessor< float > acc( "phi" );
    return acc( *this );
  }
  
  double Egamma_v1::m() const {
    static Accessor< float> acc( "m" );
    return acc( *this );
  }
  
  double Egamma_v1::e() const{
    return p4().E(); 
  }
  
  double Egamma_v1::rapidity() const {
    return p4().Rapidity();	
  }
  
  const Egamma_v1::FourMom_t& Egamma_v1::p4() const {
    if( ! m_p4Cached ) {
      m_p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
      m_p4Cached=true;
    }
    return m_p4;	
  }
  
  void Egamma_v1::setP4(float pt, float eta, float phi, float m){
    static Accessor< float > acc1( "pt" );
    acc1( *this ) = pt;
    static Accessor< float > acc2( "eta" );
    acc2( *this ) = eta;
    static Accessor< float > acc3( "phi" );
    acc3( *this ) = phi;
    static Accessor< float > acc4( "m" );
    acc4( *this ) = m;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }
  
  void Egamma_v1::setPt(float pt){
    static Accessor< float > acc( "pt" );
    acc( *this ) = pt;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }
  
  void Egamma_v1::setEta(float eta){
    static Accessor< float > acc( "eta" );
    acc( *this ) = eta;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  void Egamma_v1::setPhi(float phi){
    static Accessor< float > acc( "phi" );
    acc( *this ) = phi;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  void Egamma_v1::setM(float m){
    static Accessor< float > acc( "m" );
    acc( *this ) = m;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }

  Egamma_v1::EgammaCovMatrix_t Egamma_v1::covMatrix() const{
    static Accessor< std::vector<float> > acc( "EgammaCovarianceMatrix" );
    if(!acc.isAvailable( *this) ) { 
      EgammaCovMatrix_t dummy;
      dummy.setZero();
      return dummy;
    }
    const std::vector<float>& v = acc(*this);
    return Eigen::Map<const EgammaCovMatrix_t> (v.data());
  }

  void Egamma_v1::setCovMatrix(const Egamma_v1::EgammaCovMatrix_t& cov){
    //The internal storage is an std::vector
    static Accessor< std::vector < float > > acc( "EgammaCovarianceMatrix" );
    const std::vector<float> v(cov.data(),cov.data()+16);
    acc(*this)=v;
  }
  
  uint16_t Egamma_v1::author(uint16_t mask) const {
    static Accessor< uint16_t > acc( "author" );
    uint16_t author = acc( *this );
    return author & mask;
  }
  
  void Egamma_v1::addAuthor(uint16_t newAuthor) {
    static Accessor< uint16_t > acc( "author" );
    uint16_t author = acc( *this );
    acc( *this) = author | newAuthor;
  }
  
  void Egamma_v1::setAuthor(uint16_t newAuthor) {
    static Accessor< uint16_t > acc( "author" );
    acc( *this) = newAuthor;
  }
    
  /////////////// shower shapes
  bool Egamma_v1::showerShapeValue(float& value, const EgammaParameters::ShowerShapeType information)  const {
    xAOD::Egamma_v1::Accessor< float >* acc = showerShapeAccessorV1( information );
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) { 
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  float Egamma_v1::showerShapeValue(const EgammaParameters::ShowerShapeType information)  const {
    xAOD::Egamma_v1::Accessor< float >* acc = showerShapeAccessorV1( information );
    if(!acc ) throw std::runtime_error( "Unknown/Unavailable Shower Shape type requested" );
    return ( *acc )( *this );
  }
  
  bool Egamma_v1::setShowerShapeValue(float value, const EgammaParameters::ShowerShapeType information){
    xAOD::Egamma_v1::Accessor< float >* acc = showerShapeAccessorV1( information );
     if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;

  }

  ///////////////   Isolation value
  bool  Egamma_v1::isolation(float& value, const Iso::IsolationType information)  const {
    SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  float Egamma_v1::isolation( const Iso::IsolationType information)  const {
    SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation type requested" );
    return  ( *acc )( *this );
  }

  bool Egamma_v1::setIsolation(float value, const Iso::IsolationType information) {
    SG::AuxElement::Accessor< float >* acc = getIsolationAccessor( information );
    if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }

  /////////////// Isolation corrections

  bool Egamma_v1::isolationCaloCorrection(float& value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr, 
					  const Iso::IsolationCorrectionParameter param) const{
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr,param);
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
 }

  float Egamma_v1::isolationCaloCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr, 
					  const Iso::IsolationCorrectionParameter param) const{

    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr,param);
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation correction requested" );
    return  ( *acc )( *this );
  }

  bool Egamma_v1::setIsolationCaloCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr, 
					     const Iso::IsolationCorrectionParameter param){
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr,param);
    if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }


  bool Egamma_v1::isolationCaloCorrection(float& value,  Iso::IsolationType type , Iso::IsolationCaloCorrection corr) const{
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(type,corr);
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  float Egamma_v1::isolationCaloCorrection(Iso::IsolationType type, Iso::IsolationCaloCorrection corr) const{
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(type,corr);
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation correction requested" );
    return  ( *acc )( *this );
  }

  bool Egamma_v1::setIsolationCaloCorrection(float value, Iso::IsolationType type, Iso::IsolationCaloCorrection corr){
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(type,corr);
    if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }

  bool Egamma_v1::isolationTrackCorrection(float& value, const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection corr) const{
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr);
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }
  
  float Egamma_v1::isolationTrackCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection corr) const{
    
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr);
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation correction requested" );
    return  ( *acc )( *this );
  }
  
  bool Egamma_v1::setIsolationTrackCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationTrackCorrection corr){
    SG::AuxElement::Accessor< float >* acc = getIsolationCorrectionAccessor(flavour,corr);
    if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }

  bool Egamma_v1::isolationCorrectionBitset( std::bitset<32>& value, const Iso::IsolationFlavour flavour ) const{
   SG::AuxElement::Accessor< uint32_t >* acc = getIsolationCorrectionBitsetAccessor( flavour );
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value =  std::bitset<32>(( *acc )( *this ));
    return true;
  }

  std::bitset<32> Egamma_v1::isolationCorrectionBitset(const Iso::IsolationFlavour flavour ) const{
   SG::AuxElement::Accessor< uint32_t >* acc = getIsolationCorrectionBitsetAccessor( flavour );
    if( !acc ) throw std::runtime_error( "Unknown/Unavailable Isolation BitSet requested" );
    return   std::bitset<32>(( *acc )( *this ));
  }

  bool Egamma_v1::setIsolationCorrectionBitset(uint32_t value, const Iso::IsolationFlavour flavour ) {
   SG::AuxElement::Accessor< uint32_t >* acc = getIsolationCorrectionBitsetAccessor( flavour );
    if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }

  ////////////////////////////////////////////////////
  /// Implementation of the CaloCluster accessor functions
  size_t Egamma_v1::nCaloClusters() const {

     static SG::AuxElement::Accessor< Egamma_v1::CLELVec_t >
        clusterAcc( "caloClusterLinks" );

     if( clusterAcc.isAvailable( *this ) ) {
        return clusterAcc( *this ).size();
     }
     return 0;
  }

  const xAOD::CaloCluster* Egamma_v1::caloCluster( size_t index ) const {
    
     if( index >= nCaloClusters() ) {
        return 0;
     }
     const CLELVec_t& cls = caloClusterLinks();
     if( ! cls[ index ].isValid() ) {
        return 0;
     }
     return *( cls[ index ] );
  }

  const ElementLink< CaloClusterContainer >&
  Egamma_v1::caloClusterLink( size_t index ) const {

     if( index >= nCaloClusters() ) {
        static const ElementLink< CaloClusterContainer > dummy;
        return dummy;
     }
     return caloClusterLinks()[ index ];
  }

  AUXSTORE_OBJECT_SETTER_AND_GETTER( Egamma_v1, Egamma_v1::CLELVec_t,
                                     caloClusterLinks, setCaloClusterLinks )

  ///////////

  bool Egamma_v1::isGoodOQ(uint32_t mask) const {
    static Accessor< uint32_t > acc( "OQ" );
    uint32_t OQ = acc( *this );
    return (OQ & mask)==0;
  }

  uint32_t Egamma_v1::OQ() const{
    static Accessor< uint32_t > acc( "OQ" );
    return acc( *this) ;
  }
  
  void Egamma_v1::setOQ(uint32_t newOQ) {
    static Accessor< uint32_t > acc( "OQ" );
    acc( *this) = newOQ;
  }

  /////////// 

  ///Selectors / isEM and all that 

  ///First with enums (static accessor no lookup => faster but less flexible)
  bool Egamma_v1::passSelection(bool&  value, const xAOD::EgammaParameters::SelectionMenu menu ) const {
    SG::AuxElement::Accessor< char >* acc = selectionMenuAccessorV1( menu );
    if( !acc ) {
      return false;
    }
    if(!acc->isAvailable( *this) ) { 
      return  false;
    }
    value= (*acc)( *this );
    return true;
  }

  bool Egamma_v1::passSelection(const xAOD::EgammaParameters::SelectionMenu menu  ) const {
    SG::AuxElement::Accessor< char >* acc = selectionMenuAccessorV1( menu );
    if(!acc ) throw std::runtime_error( "Unknown/Unavailable bool selection menu requested" );
    return (*acc)( *this );
  }

  void Egamma_v1::setPassSelection(bool value, const xAOD::EgammaParameters::SelectionMenu menu){
    SG::AuxElement::Accessor< char >* acc = selectionMenuAccessorV1( menu );
    ( *acc )( *this )=value;
    return;
  }

  bool Egamma_v1::selectionisEM(unsigned int&  value, const xAOD::EgammaParameters::SelectionisEM isEM) const {
    SG::AuxElement::Accessor< unsigned int >* acc = selectionisEMAccessorV1( isEM );
    if(!acc ) {
      return false;
    }    
    if(!acc->isAvailable( *this) ) { 
      return  false;
    }
    value= (*acc)( *this );
    return true;
  }

  unsigned int Egamma_v1::selectionisEM(const xAOD::EgammaParameters::SelectionisEM isEM) const {
    SG::AuxElement::Accessor< unsigned int >* acc = selectionisEMAccessorV1( isEM );
    if(!acc ) throw std::runtime_error( "Unknown/Unavailable unsigned int isEM requested" );
    return (*acc)( *this );
  }

  void Egamma_v1::setSelectionisEM(unsigned int value, const xAOD::EgammaParameters::SelectionisEM isEM){
    SG::AuxElement::Accessor< unsigned int >* acc = selectionisEMAccessorV1( isEM );
    ( *acc )( *this )=value;
    return;
  }
  /////////////////////////////////////////////////////////////////////////
  ///Then with strings (full flexibility when adding new menus dynamically)
  bool Egamma_v1::passSelection(bool&  value, const std::string& menu ) const {
    SG::AuxElement::Accessor< char > acc( menu );
    if(!acc.isAvailable( *this) ) { 
      return  false;
    }
    value= (acc)( *this );
    return true;
  }

  bool Egamma_v1::passSelection(const std::string& menu ) const {
    SG::AuxElement::Accessor< char > acc( menu );
    return (acc)( *this );
  }

  void Egamma_v1::setPassSelection(bool value, const std::string& menu){
    SG::AuxElement::Accessor< char > acc( menu );
    ( acc )( *this )=value;
    return;
  }

  bool Egamma_v1::selectionisEM(unsigned int&  value, const std::string& isEM) const{
    SG::AuxElement::Accessor< unsigned int > acc( isEM );
    if(!acc.isAvailable( *this) ) { 
      return  false;
    }
    value= (acc)( *this );
    return true;
  }

  unsigned int Egamma_v1::selectionisEM(const std::string& isEM) const{
    SG::AuxElement::Accessor< unsigned int > acc( isEM );
    return (acc)( *this );
  }

  void Egamma_v1::setSelectionisEM(unsigned int value, const std::string& isEM){
    SG::AuxElement::Accessor< unsigned int > acc( isEM );
    ( acc )( *this )=value;
    return;
  }

  bool Egamma_v1::likelihoodValue(float&  value, const std::string& LHValue/*=std::string("LHValue")*/) const{
    SG::AuxElement::Accessor<float> acc( LHValue );
    if(!acc.isAvailable( *this) ) { 
      return  false;
    }
    value= (acc)( *this );
    return true;
  }

  float Egamma_v1::likelihoodValue(const std::string& LHValue/*=std::string("LHValue")*/) const{
    SG::AuxElement::Accessor< float > acc( LHValue );
    return (acc)( *this );
  }

  void Egamma_v1::setLikelihoodValue(float value, const std::string& LHValue/*=std::string("LHValue")*/){
    SG::AuxElement::Accessor<float> acc( LHValue);
    ( acc )( *this )=value;
    return;
  }
 
} // namespace xAOD

//  LocalWords:  const el hasStore makePrivateStore
