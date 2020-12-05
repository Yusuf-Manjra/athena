/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoCoreSim/TopoSteeringStructure.h"

#include "L1TopoCommon/StringUtils.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoCommon/Types.h"

#include "L1TopoConfig/LayoutConstraints.h"
#include "L1TopoConfig/L1TopoMenu.h"

#include "L1TopoInterfaces/DecisionAlg.h"
#include "L1TopoInterfaces/SortingAlg.h"

#include "L1TopoCoreSim/Connector.h"
#include "L1TopoCoreSim/InputConnector.h"
#include "L1TopoCoreSim/SortingConnector.h"
#include "L1TopoCoreSim/DecisionConnector.h"

#include "L1TopoHardware/L1TopoHardware.h"

#include "TrigConfData/L1Menu.h"

#include <set>
#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TCS;
using boost::lexical_cast;

namespace {
   uint32_t interpretGenericParam(const std::string& parvalue) {
      uint32_t val;
      try {
         val  = lexical_cast<uint32_t, string>(parvalue);
      }
      catch(const boost::bad_lexical_cast & bc) {
         if( parvalue.size()>=3 && parvalue[0]==':' and parvalue[parvalue.size()-1]==':' ) {

            auto x = L1TopoHWParameters::get().find(parvalue.substr(1,parvalue.size()-2));

            string parname = parvalue.substr(1,parvalue.size()-2);

            if( x != L1TopoHWParameters::get().end()) {
               val = x->second.value;
            } else {
               cout << "Hardware constrained parameters are" << endl;
               for(auto & x : L1TopoHWParameters::get())
                  cout << "   " << x.first << endl;

               TCS_EXCEPTION("Generic parameter value " << parvalue << " has the hardware contrained parameter format, but '" << parname << "' is not listed in L1TopoHardware.cxx");
            }
         } else {
            TCS_EXCEPTION("Generic parameter value " << parvalue << " is not a uint32_t and does not match the hardware contrained parameter specification ':<parname>:' ");
         }
      }
      return val;
   }
}


TCS::TopoSteeringStructure::TopoSteeringStructure() :
   m_parameters(2 * LayoutConstraints::maxComponents(), nullptr)
{}


TopoSteeringStructure::~TopoSteeringStructure() {
   for( Connector* c: m_connectors ) 
      delete c;
   for( ParameterSpace * ps : m_parameters )
      delete ps;
}


TCS::StatusCode
TopoSteeringStructure::reset() {
   for(Connector* conn: m_connectors)
      conn->reset();
   return TCS::StatusCode::SUCCESS;
}


void
TCS::TopoSteeringStructure::print(std::ostream & o) const {
   o << "Topo Steering Structure" << endl
     << "-----------------------" << endl;

   o << "Output summary:" << endl;
   for(auto conn: outputConnectors() ) {
      o << "  " << *conn.second << endl;
   }

   o << endl 
     << "Algorithm detail:" << endl;
   for(auto nc: outputConnectors() ) {
      DecisionConnector * conn = nc.second;
      unsigned int firstBit          = conn->decision().firstBit();
      unsigned int lastBit           = conn->numberOutputBits() + firstBit - 1;
      const ConfigurableAlg* alg     = conn->algorithm();
      const Connector* inputconn     = conn->inputConnectors().back();
      const ConfigurableAlg* sortalg = inputconn->algorithm();
      o << std::setw(2) << "bits " << firstBit << "-" << lastBit << ": " << conn->name() << " [input " << inputconn->name() <<"]" << endl;
      o << *alg << endl << endl;
      o << *sortalg << endl;
   }
}


void
TCS::TopoSteeringStructure::printParameters(std::ostream & o) const {
   unsigned int idx(0);
   for(const ParameterSpace* ps: m_parameters) {
      if(ps && ps->isInitialized())
         o << "pos " << std::setw(2) << idx << ": " << ps << endl;
      idx++;
   }
}


TCS::StatusCode
TCS::TopoSteeringStructure::setupFromMenu(const TrigConf::L1Menu& l1menu, bool debug, bool legacy) {

   if(debug)
      cout << "/***************************************************************************/" << endl
           << "           L1Topo steering structure: configuring from L1 Topo Menu          " << endl
           << "/***************************************************************************/" << endl;

   //   set<TCS::inputTOBType_t> neededInputs; // Stores inputs for DecisionConnectors
   vector<string> storedConn; // Stores decision connectors in order to avoid double counting
   vector<vector<string>> confAlgorithms; // Stores algorithm name/category that have been configured in L1Menu to be used for setting the parameters
   // Loop over boards in L1Menu and skip the ones that are not TOPO. Use Run-2 boards if legacy flag is on
   for (const string & boardName : l1menu.boardNames() ){
     
     auto & l1board = l1menu.board(boardName);

     if (l1board.type() != "TOPO") continue;
     if (l1board.legacy() != legacy) continue;

     // Access the connectors in the boards

     for (const string & connName : l1board.connectorNames() ){

       auto & l1conn = l1menu.connector(connName);

       // All topo decision algorithms are configured in boards with electrical connection to CTP - Add optical connectors when adding multiplicity algorithms
       // Look at all Topo algorithms in each connector, and get inputs from each algorithm to configure SortingConnectors
       if ( ! (l1conn.connectorType() == TrigConf::L1Connector::ConnectorType::ELECTRICAL ) ) continue;

       for( size_t fpga : { 0, 1} ) {
	 for( size_t clock : { 0, 1} ) {
	   for( auto & tl : l1conn.triggerLines(fpga, clock) ) {
	     
	     const string & tlName = tl.name();
	     auto & algo = l1menu.algorithmFromTriggerline(tlName);

	     // One algorithm can have multiple trigger lines. Check the connector/algorithm has not been stored already
	     auto it = find(storedConn.begin(), storedConn.end(), algo.name());
	     if (it == storedConn.end()) { // Algorithm/Connector does not exist: create and store it

	       storedConn.push_back(algo.name());
	       vector<string> inputNames;
	       for( auto & input : algo.inputs() ) {
		 if( sortingConnector(input) == 0 ) { // if connector does not exist, create it
		   if(debug) 
		     cout << "L1TopoSteering: Decision algo( " << algo.name() << " ) input is not defined: " << input << ". Creating sortingConnector" << endl;
		   
		   auto & sortAlgo = l1menu.algorithm(input, algo.category());
		   if(!(sortAlgo.type() == TrigConf::L1TopoAlgorithm::AlgorithmType::SORTING ) )
		     TCS_EXCEPTION("L1TopoSteering: Decision algo " << algo.name() << ") has as input " << input << " which is not associated to a sorting algorithm");

		   //		   TCS::inputTOBType_t en = inputType(sortAlgo.inputs().at(0));
		   //		   neededInputs.insert(en);

		   // Create connector
		   SortingConnector * sortConn = new SortingConnector(sortAlgo.inputs().at(0), sortAlgo.klass()+"/"+input, sortAlgo.outputs().at(0));
		   if(debug)
		     cout << "Adding sorting connector " << "[" << *sortConn << "]" << endl;
		   addSortingConnector( sortConn );
		   confAlgorithms.push_back({sortAlgo.name(), sortAlgo.category()});

		 } // if connector does not exist
		 
		 inputNames.push_back(input);

	       } // loop over inputs

	       DecisionConnector * conn = new DecisionConnector(algo.name(), inputNames, algo.klass()+"/"+algo.name(), algo.outputs());
	       conn->m_decision.setNBits( algo.outputs().size() );

	       if(tl.name() != "UNDEF")
		 conn->m_triggers.push_back(tl);

	       if(debug)
		 cout << "Adding decision connector " << "[" << *conn << "]" << endl;
	       addDecisionConnector( conn );
	       confAlgorithms.push_back({algo.name(), algo.category()});

	     } else { // Connector already exists: look for it and add the trigger line
	       for(auto out : algo.outputs()){
		 auto c = m_outputLookup.find(out);
		 if (c != m_outputLookup.end()){
		   auto conn = c->second;
		   if(tl.name() != "UNDEF")
		     conn->m_triggers.push_back(tl);
		   break;
		 }
	       }
	     }

	   } // Trigger Line

	 } // Clock

       } // FPGA
       
     } // Connector in l1board

   } // Board in l1menu

   /*
   if(debug)
     cout << "... building sorting connectors" << endl;

   std::string categories[] = { "TOPO" };

   for( auto & category : categories ){
   
     for( auto & name : l1menu.topoAlgorithmNames(category)) {
       
       auto & algo = l1menu.algorithm(name, category);
       
       if(!(algo.type() == TrigConf::L1TopoAlgorithm::AlgorithmType::SORTING ) ) continue;
       
       TCS::inputTOBType_t en = inputType(algo.inputs().at(0));
       neededInputs.insert(en);
       
       // create connector
       SortingConnector * conn = new SortingConnector(algo.inputs().at(0), algo.klass()+"/"+name, algo.outputs().at(0));
       if(debug)
	 cout << "Adding sorting connector " << "[" << *conn << "]" << endl;
       addSortingConnector( conn );
     }

   }
     

   
   if(debug)
     cout << "... building output connectors" << endl;

   // All topo decision algorithms are configured in boards with electrical connection to CTP
   // Create all DecisionConnectors from L1Connectors of type ELECTRICAL
   std::vector<std::string> storedConn;
   for ( const string & connName : l1menu.connectorNames() ){

     auto & l1conn = l1menu.connector(connName);

     if ( ! (l1conn.type() == TrigConf::L1Connector::ConnectorType::ELECTRICAL ) ) continue;
     if ( l1conn.isLegacy() ) continue;

     for( size_t fpga : { 0 ,1 } ) {
       for( size_t clock : { 0 ,1 } ) {
	 for( auto & tl : l1conn.triggerLines(fpga, clock) ) {
	   
	   const string & tlName = tl.name();
	   auto & algo = l1menu.algorithmFromTriggerline(tlName);
	   
	   // One algorithm can have multiple trigger lines. Check the connector/algorithm has not been stored already
	   auto it = std::find(storedConn.begin(), storedConn.end(), algo.name());
	   if (it == storedConn.end()) { // Algorithm/Connector do not exist: create and store it

	     storedConn.push_back(algo.name());

	     std::vector<std::string> inputNames;
	     for( auto & input : algo.inputs() ) {
	       if( sortingConnector(input) == 0 ) {
		 TCS_EXCEPTION("L1TopoSteering: Decision algo ( " << algo.name() << " ) input is not defined: " << input);
	       }
	       inputNames.push_back(input);
	     }

	     DecisionConnector * conn = new DecisionConnector(algo.name(), inputNames, algo.klass()+"/"+algo.name(), algo.outputs());
	     conn->m_decision.setNBits( algo.outputs().size() );
	   
	     if(tl.name() != "UNDEF")
	       conn->m_triggers.push_back(tl);

	     if(debug)
	       cout << "Adding decision connector " << "[" << *conn << "]" << endl;
	     addDecisionConnector( conn );

	   } else { // Connector already exists: look for it and add the trigger line
	     for(auto out : algo.outputs()){
	       auto c = m_outputLookup.find(out);
	       if (c != m_outputLookup.end()){
		 auto conn = c->second;
		 if(tl.name() != "UNDEF")
		   conn->m_triggers.push_back(tl);
		 break;
	       }
	     }
	   }
	 } // Trigger Line
       } // Clock
     } // FPGA
   } // L1Connectors

   */

   if(debug)
     cout << "... building input connectors" << endl;
   for(auto sortConn : m_sortedLookup) {
     const string & in = sortConn.second->inputNames()[0]; // name of input
     
     if( m_inputLookup.count(in) > 0 ) continue; // InputConnector already exists

     InputConnector * conn = new InputConnector(in);
     m_connectors.push_back(conn);
     m_inputLookup[in] = conn;
     if(debug)
       cout << "Adding input connector " << "[" << *conn << "]" << endl;
   }

   // link the connector objects together
   TCS::StatusCode sc = linkConnectors();

   // instantiate the algorithms from the algorithm names in the connectors
   if(debug)
     cout << "... instantiating algorithms" << endl;
   sc &= instantiateAlgorithms(debug);


   // set algorithm parameters
   if(debug)
     cout << "... setting algorithm parameters" << endl;

   for ( auto & confAlgo : confAlgorithms ) {
     
     auto & l1algo = l1menu.algorithm(confAlgo.at(0), confAlgo.at(1));
       
     ConfigurableAlg * alg = AlgFactory::instance().algorithm(l1algo.name());
     alg->setAlgoId( l1algo.algId() );
     
     if(debug)
       cout << "Algorithm " << alg->name() << " has algoId " << alg->algoId() <<  endl << " (reading parameters)" << endl;
     
     if(alg->isDecisionAlg())
       ((DecisionAlg *) alg)->setNumberOutputBits(l1algo.outputs().size());

     // create ParameterSpace for this algorithm
     ParameterSpace * ps = new ParameterSpace(alg->name());

     for(auto & pe : l1algo.parameters()) {
	 
       auto & pname = pe.name();
       uint32_t val = pe.value();
       uint32_t sel = pe.selection();

       if(debug)
	 cout << " parameter " << ": " << setw(20) << left << pname << " value = " << setw(3) << left << val << " (selection " << sel << ")" << endl;
       ps->addParameter( pname, val, sel);
	 
     }
       
     for(auto & gen : l1algo.generics().getKeys()) {
	 
       auto pe = l1algo.generics().getObject(gen);
       string pname = gen;
       uint32_t val = interpretGenericParam(pe.getAttribute("value"));
       if (pname == "NumResultBits"){
	 if(val != l1algo.outputs().size()) {
	   TCS_EXCEPTION("Algorithm " << pname << " parameter OutputBits (" << val << ") is different from output size (" << l1algo.outputs().size() << ")");
	 }
	 continue; // ignore this, because it is defined through the output list
       }
       if(debug)
	 cout << " fixed parameter : " << setw(20) << left << pname << " value = " << setw(3) << left << val << endl;
       ps->addParameter( pname, val );
	 
     }
       
       
     if(debug)
       cout << " (setting parameters)";
     alg->setParameters( *ps );
       
     if(debug)
       cout << " --> (parameters set)";
       
     if( alg->isDecisionAlg() ) {
       if( m_parameters[alg->algoId()] != nullptr ) {
	 //	   TCS_EXCEPTION("Decision algorithm " << alg->name() << " has algoId " << alg->algoId() << " which is already used");
       }
       m_parameters[alg->algoId()] = ps;
     } else if (alg->isSortingAlg() ) {
       if( m_parameters[alg->algoId() + LayoutConstraints::maxComponents()] != nullptr ) {
	 //	   TCS_EXCEPTION("Sorting algorithm " << alg->name() << " has algoId " << alg->algoId() << " which is already used");
       }
       m_parameters[alg->algoId() + LayoutConstraints::maxComponents()] = ps;
     } else {
       // newed parameters usued so delete to avoid memory leak
       delete ps;
       ps=0;
     }

     if(debug)
       cout << " --> (parameters stored)" << endl;
   }
   
   m_isConfigured = true;
   
   if(debug)
     cout << "... L1TopoSteering successfully configured" << endl;
   
   return sc;
}


TCS::StatusCode
TopoSteeringStructure::addSortingConnector(SortingConnector * conn) {
   m_connectors.push_back(conn);
   for( const string & output : conn->outputNames() )
      m_sortedLookup[output] = conn;
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TopoSteeringStructure::addDecisionConnector(DecisionConnector * conn) {
   m_connectors.push_back(conn); 
   for( const string & output : conn->outputNames() )
     m_outputLookup[output] = conn;     
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TopoSteeringStructure::linkConnectors() {

   for(TCS::Connector * conn: m_connectors)
      for(const std::string & inconn: conn->inputNames())
         conn->inputConnectors().push_back( connector(inconn) );

   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteeringStructure::instantiateAlgorithms(bool debug) {

   for(TCS::Connector* conn: m_connectors) {

      if(conn->isInputConnector()) continue;

      // for each connector instantiate the algorithm and add to connector
      const std::string & alg = conn->algorithmName();
       
      // split into name and type
      std::string algType(alg, 0, alg.find('/'));
      std::string algName(alg, alg.find('/')+1);

      ConfigurableAlg * algInstance = TCS::AlgFactory::instance().algorithm(algName);
      if(algInstance==0) {
         if(debug)
            cout << "Instantiating " << alg << endl;
         algInstance = TCS::AlgFactory::create(algType, algName);
      } else {
         if(algInstance->className() != algType) {
            TCS_EXCEPTION("L1 TopoSteering: duplicate algorithm names:  algorithm " << algName << " has already been instantiated but with different type");
         }
      }
      conn->setAlgorithm(algInstance);
   }
   return TCS::StatusCode::SUCCESS;
}


TCS::Connector *
TCS::TopoSteeringStructure::connector(const std::string & connectorName) const {
   for( TCS::Connector* conn: m_connectors ) {
      if( conn->name() == connectorName )
         return conn;
   }
   TCS_EXCEPTION("L1Topo Steering: can not find connector of name " << connectorName << ". Need to abort!");
   return 0;
}


SortingConnector* 
TopoSteeringStructure::sortingConnector(const std::string & connectorName) const {
   SortingConnector * sc = nullptr;
   for( TCS::Connector* conn: m_connectors ) {
      if( conn->name() == connectorName ) {
         sc = dynamic_cast<SortingConnector*>(conn);
         if(sc==nullptr) {
            TCS_EXCEPTION("TopoSteeringStructure: connector of name " << connectorName << " exists, but is not a SortingConnector. Need to abort!");            
         }
      }
   }
   
   return sc;
}


TCS::DecisionConnector *
TCS::TopoSteeringStructure::outputConnector(const std::string & output) const {
   auto c = m_outputLookup.find(output);
   if( c != m_outputLookup.end() )
      return c->second;
   TCS_EXCEPTION("L1Topo Steering: can not find output connector of that produces " << output << ". Need to abort!");
   return 0;
}
