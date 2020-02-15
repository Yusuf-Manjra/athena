/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <vector>

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfIO/TrigDBJobOptionsLoader.h"
#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/HLTPrescalesSet.h"

using namespace std;

struct Config {
public:
   ~Config(){}
   Config(){}

   std::vector<std::string> knownParameters { "file", "smk", "l1psk", "hltpsk", "db", "write", "help", "h", "d", "detail" };

   // parameters
   // input
   std::vector<std::string> inputFiles {};
   unsigned int smk { 0 };
   unsigned int l1psk { 0 };
   unsigned int hltpsk { 0 };
   std::string  dbalias { "TRIGGERDBDEV2" };

   // output
   std::string  base { "" };

   // other
   bool         help { false };
   bool         detail { false };
   // to keep track of configuration errors
   vector<string> error;

   // parses the commandline 
   void parseProgramOptions(int argc, char* argv[]);

   // help
   void usage();

};


void Config::usage() {

  cout << "The program needs to be run with the following specifications:\n\n";
  cout << "TriggerMenuRW <options>\n";
  cout << "\n";
  cout << "[Input options]\n";
  cout << "  --file                file1 [file2 [file3 ...]]     ... one or multiple json files\n";
  cout << "  --smk                 smk                           ... smk \n";
  cout << "  --l1psk               l1psk                         ... the L1 prescale key \n";
  cout << "  --hltpsk              hltpsk                        ... the HLT prescale key \n";
  cout << "  --db                  dbalias                       ... dbalias (default " << dbalias << ") \n";
  cout << "[Output options]\n";
  cout << "  --write               [base]                        ... to write out json files, e.g. L1menu[_<base>].json. base is optional.\n";
  cout << "[Other options]\n";
  cout << "  -h|--help                                           ... this help\n";
  cout << "  -d|--detail                                         ... prints detailed job options\n";
  cout << "\n\n";
  cout << "Examples\n";
  cout << "  --file L1menu.json HLTMenu.json                     ... read L1Menu.json and HLTMenu.json and show some basic statistics\n";

}

void
Config::parseProgramOptions(int argc, char* argv[]) {

   std::string currentParameter("");
   std::string listofUnknownParameters = "";

   for(int i=1; i<argc; i++) {

      std::string currentWord(argv[i]);
      bool isParam = currentWord[0]=='-'; // string starts with a '-', so it is a parameter name

      // get the parameter name
      int firstChar = currentWord.find_first_not_of('-');
      string paramName = currentWord.substr(firstChar);

      // check if the parameter is known
      if ( isParam && std::find(knownParameters.begin(), knownParameters.end(), paramName) == knownParameters.end() ) {
         listofUnknownParameters += " " + currentWord;
         continue;
      }

      if(isParam) {
         currentParameter = "";
         // check the boolean parameters
         if(paramName == "h" || paramName == "help" ) { help = true; continue; }
         if(paramName == "d" || paramName == "detail" ) { detail = true; continue; }
         currentParameter = paramName;
         continue;
      }

      // now treat the parameter values

      // inputs
      if(currentParameter == "file") { 
         inputFiles.push_back(currentWord);
         continue; 
      }
      if(currentParameter == "smk") { 
         smk = stoul(currentWord);
         continue; 
      }
      if(currentParameter == "l1psk") { 
         l1psk = stoul(currentWord);
         continue; 
      }
      if(currentParameter == "hltpsk") { 
         hltpsk = stoul(currentWord);
         continue; 
      }
      if(currentParameter == "db") { 
         dbalias = currentWord;
         continue; 
      }

      // output
      if(currentParameter == "write") { 
         base = currentWord;
         continue; 
      }

   }

   // some sanity checks
   if ( inputFiles.size() == 0 and smk == 0 and l1psk == 0 and hltpsk == 0 ) {
      error.push_back("No input specified! Please provide either one of the following: input file(s), smk, l1psk, or hltpsk");
   }

   if ( listofUnknownParameters.size() > 0 ) {
      error.push_back( string("Unknown parameter(s):") + listofUnknownParameters);
   }

}




int main(int argc, char** argv) {

   Config cfg;
   cfg.parseProgramOptions(argc, argv);
   if(cfg.help) {
      cfg.usage();
      return 0;
   }
   if(cfg.error.size()!=0) {
      for(const string & e: cfg.error)
         cerr << e << endl;
      cfg.usage();
      return 1;
   }

   if( cfg.inputFiles.size()>0 ) {
      // load config from files

      // file loader
      TrigConf::JsonFileLoader fileLoader;

      for (const std::string & fn : cfg.inputFiles) {

         // check if the file is L1 or HLT
         std::string filetype = fileLoader.getFileType( fn );
         
         if(filetype == "l1menu") {
            TrigConf::L1Menu l1menu;
            fileLoader.loadFile( fn, l1menu);
            cout << "Loaded L1 menu file " << fn << endl;
            l1menu.printMenu(cfg.detail);
         } else if(filetype == "hltmenu" ) {
            TrigConf::HLTMenu hltmenu;
            fileLoader.loadFile( fn, hltmenu);
            cout << "Loaded HLT menu file " << fn << " with " << hltmenu.size() << " chains" << endl;
            hltmenu.printMenu(cfg.detail);
         } else if(filetype == "l1prescale" ) {
            TrigConf::L1PrescalesSet l1pss;
            fileLoader.loadFile( fn, l1pss);
            cout << "Loaded L1 prescales set file " << fn << " with " << l1pss.size() << " prescales" << endl;
         } else if(filetype == "hltprescale" ) {
            TrigConf::HLTPrescalesSet hltpss;
            fileLoader.loadFile( fn, hltpss);
            cout << "Loaded HLT prescales set file " << fn << " with " << hltpss.size() << " prescales" << endl;
            hltpss.printPrescaleSet(cfg.detail);
         } else {
            cerr << "File " << fn << " not recognized as being an L1 or HLT menu or prescale set" << endl;
         }

      }
   }

   if( cfg.smk != 0 ) {
      // load config from DB

      // db menu loader
      TrigConf::TrigDBMenuLoader dbloader(cfg.dbalias);

      TrigConf::L1Menu l1menu;
      TrigConf::HLTMenu hltmenu;
      
      dbloader.loadL1Menu( cfg.smk, l1menu );
      if (l1menu) {
         cout << "Loaded L1 menu with " << l1menu.size() << " items" <<  endl;
         l1menu.printMenu(cfg.detail);
      } else {
         cout << "Did not load an L1 menu" << endl;
      }

      dbloader.loadHLTMenu( cfg.smk, hltmenu );
      if (hltmenu) {
         cout << "Loaded HLT menu with " << hltmenu.size() << " chains" << endl;
      } else {
         cout << "Did not load an HLT menu" << endl;
      }

      // db job options loader
      TrigConf::TrigDBJobOptionsLoader jodbloader(cfg.dbalias);

      TrigConf::DataStructure jo;
      jodbloader.loadJobOptions( cfg.smk, jo );
      if (jo) {
         cout << "Loaded job options with " << jo.getObject("properties").getKeys().size() << " entries " << endl;
         if( cfg.detail ) {
            for( const auto alg : jo.getObject("properties").data()) {
               std::cout << alg.first << std::endl;
               for( const auto prop : alg.second ) {
                  std::cout << "      " << prop.first << " -> " << prop.second.data() << std::endl;
               }
            }
         }
      } else {
         cout << "Did not load job options" << endl;
      }
   }


   if( cfg.l1psk != 0 ) {
      // load L1 prescales set from DB
      TrigConf::TrigDBL1PrescalesSetLoader dbloader(cfg.dbalias);
      TrigConf::L1PrescalesSet l1pss;
      
      dbloader.loadL1Prescales( cfg.l1psk, l1pss );
      if (l1pss) {
         cout << "Loaded L1 prescales set with " << l1pss.size() << " prescales" <<  endl;
      } else {
         cout << "Did not load an L1 prescales set" << endl;
      }
   }


   if( cfg.hltpsk != 0 ) {
      // load L1 prescales set from DB
      TrigConf::TrigDBHLTPrescalesSetLoader dbloader(cfg.dbalias);
      TrigConf::HLTPrescalesSet hltpss;
      
      dbloader.loadHLTPrescales( cfg.hltpsk, hltpss );
      if (hltpss) {
         cout << "Loaded HLT prescales set with " << hltpss.size() << " prescales" <<  endl;
      } else {
         cout << "Did not load an HLT prescales set" << endl;
      }
   }

   return 0;
}
