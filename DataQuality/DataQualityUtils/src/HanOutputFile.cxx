/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanOutputFile.cxx,v 1.59 2009-05-18 15:37:12 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/HanOutputFile.h"

#include <TBufferJSON.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TEfficiency.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TImage.h>
#include <TIterator.h>
#include <TKey.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TString.h>
#include <TStyle.h>
#include <TText.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DataQualityInterfaces/HanUtils.h"
#include "TPluginManager.h"

#define BINLOEDGE(h, n) h->GetXaxis()->GetBinLowEdge(n)
#define BINWIDTH(h, n) h->GetXaxis()->GetBinWidth(n)

ClassImp(dqutils::HanOutputFile)

  namespace
{
  // class DisableMustClean {
  // public:
  //   inline DisableMustClean() : useRecursiveDelete(gROOT->MustClean()) { gROOT->SetMustClean(false); }
  //   inline ~DisableMustClean() { gROOT->SetMustClean(useRecursiveDelete); }
  // private:
  //   bool useRecursiveDelete;
  // };

  Double_t getScaleVal(std::string & display)
  {
    std::size_t found = display.find("ScaleRef");
    std::size_t found2 = display.find_first_of(',', found + 1);
    // has multiple entries? Do this later.
    try
    {
      return boost::lexical_cast<Double_t>(display.substr(found + 9, found2 - found - 9));
    }
    catch (boost::bad_lexical_cast const&)
    {
      std::cerr << "Unable to cast scaling value " << display.substr(found + 9, found2 - found - 9) << " to double"
                << std::endl;
      return 1.;
    }
  }
}  // end unnamed namespace

namespace dqutils
{

  // *********************************************************************
  // Public Methods
  // *********************************************************************

  std::vector<int> root_color_choices = { kBlue, kRed, kGray, kOrange, kViolet, kGreen + 1 };

  HanOutputFile::HanOutputFile() : m_file(0), m_style(0)
  {
    clearData();
    TPluginHandler* h;
    if ((h = gROOT->GetPluginManager()->FindHandler("TVirtualPS", "image")))
    {
      if (h->LoadPlugin() == -1) return;
      h->ExecPlugin(0);
    }
  }

  HanOutputFile::HanOutputFile(std::string fileName) : m_file(0), m_style(0)
  {
    clearData();
    setFile(fileName);
    TPluginHandler* h;
    if ((h = gROOT->GetPluginManager()->FindHandler("TVirtualPS", "image")))
    {
      if (h->LoadPlugin() == -1) return;
      h->ExecPlugin(0);
    }
  }

  HanOutputFile::~HanOutputFile()
  {
    //   bool useRecursiveDelete = gROOT->MustClean();
    //   gROOT->SetMustClean(false);

    clearData();

    //   gROOT->SetMustClean(useRecursiveDelete);
  }

  void HanOutputFile::getAllGroupDirs(DirMap_t& dirmap, TDirectory* dir, std::string dirName)
  {
    if (dir == 0) return;

    if (dirName != "")
    {  // Not a file
      std::string name(dir->GetName());
      if (name == "Config" || name == "Results")
      {
        delete dir;
        return;
      }

      std::string::size_type i = name.find_last_of('_');
      if (i == (name.size() - 1))
      {
        delete dir;
        return;
      }

      DirMap_t::value_type dirmapVal(dirName, dir);
      dirmap.insert(dirmapVal);
    }
    else
    {
      DirMap_t::value_type dirmapVal("<top_level>", dir);
      dirmap.insert(dirmapVal);
    }

    dir->cd();

    TIter next(dir->GetListOfKeys());
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next())) != 0)
    {
      // don't delete TDirectories
      TObject* obj = key->ReadObj();
      TDirectory* subdir = dynamic_cast<TDirectory*>(obj);
      if (subdir != 0)
      {
        std::string subdirName(subdir->GetName());
        std::string fName("");
        if (dirName != "")
        {
          fName += dirName;
          fName += "/";
        }
        fName += subdirName;
        getAllGroupDirs(dirmap, subdir, fName);
      }
      else
      {
        delete obj;
      }
    }
  }

  void HanOutputFile::getAllGroupDirs_V2(DirStrMap_t& dirstrmap, TObject* obj, std::string objName)
  {
    if (obj == nullptr) return;
    TDirectory* dir{};
    TString obj_type = obj->ClassName();

    if (objName != "")
    {  // Not a file
      if (obj_type == "TDirectoryFile" || obj_type == "TDirectory" || obj_type == "TFile")
      {
        dir = (TDirectory*)obj;
        std::string name(dir->GetName());
        if (name == "Config" || name == "Results")
        {
          delete dir;
          return;
        }

        std::string::size_type i = name.find_last_of('_');  // If this dir is a histgram info
        if (i == (name.size() - 1))
        {
          delete dir;
          return;
        }
        DirStrMap_t::value_type dirstrmapVal(objName, obj);
        dirstrmap.insert(dirstrmapVal);
      }
      else
      {
        DirStrMap_t::value_type dirstrmapVal(objName, obj);
        dirstrmap.insert(dirstrmapVal);
      }
    }
    else
    {  // If the object is a file
      DirStrMap_t::value_type dirstrmapVal("<top_level>", obj);
      dirstrmap.insert(dirstrmapVal);
    }

    if (obj_type == "TDirectoryFile" || obj_type == "TDirectory" || obj_type == "TFile")
    {
      dir = (TDirectory*)obj;
      dir->cd();
      TIter next(dir->GetListOfKeys());
      TKey* key;
      while ((key = dynamic_cast<TKey*>(next())) != 0)
      {
        // don't delete TDirectories
        std::string fName("");
        TObject* obj_in_dir = key->ReadObj();
        TString obj_in_dir_type = obj_in_dir->ClassName();
        // Check if this is node (not a histogram)
        if (obj_in_dir_type == "TDirectoryFile" || obj_in_dir_type == "TDirectory" || obj_in_dir_type == "TFile" ||
            obj_in_dir_type == "TObjString")
        {
          std::string obj_in_dirName;
          obj_in_dirName = key->GetName();  // If we will read name of the string, it will actually be a content of
          // the string, so that's why we read name of the key
          if (objName != "")
          {
            fName += objName;
            fName += "/";
          }
          fName += obj_in_dirName;  //?
          if (obj_in_dirName != "Config" && obj_in_dirName != "Results" && obj_in_dirName != "Version_name")
          {  // We don't save 'Config' and 'Results' in dirstrmap structure. And we don't store version flag in
             // dirstrmap object
            std::string::size_type i =
              obj_in_dirName.find_last_of('_');  // if it's an object, that stores info about histogram
            if (i != (obj_in_dirName.size() - 1))
            {  // We don't store it in a dirstrmap object
              getAllGroupDirs_V2(dirstrmap, obj_in_dir,
                fName);  // Everything else we store in dirstrmap as it is for getAllGroupDirs method
            }
          }
        }
        else
        {  // in case if it isn't a node but a histogram
          delete obj_in_dir;
        }
      }
    }
  }

  void HanOutputFile::getAllAssessments(AssMap_t& dirmap, TDirectory* dir)
  {
    dqi::DisableMustClean disabled;

    dir->cd();
    TIter next(dir->GetListOfKeys());
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next())) != 0)
    {
      TObject* obj = key->ReadObj();
      if (dynamic_cast<TH1*>(obj) || dynamic_cast<TGraph*>(obj) || dynamic_cast<TEfficiency*>(obj))
      {
        const char* path(dir->GetPath());
        std::string assName(obj->GetName());
        AssMap_t::value_type AssmapVal(assName, path);
        dirmap.insert(AssmapVal);
      }
      delete obj;
    }
  }

  void HanOutputFile::printDQGroupJSON(nlohmann::json j, std::string location, const char* path_to_file)
  {
    // Parse our JSON and get all nested Assessments
    nlohmann::json valuestring;
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
    {
      std::string sName = location;
      std::string keyname = it.key();
      if (keyname != "Config" && keyname != "Results")
      {  // We are now  at subAssessments (not at Results and Config nodes)
        std::string::size_type i = keyname.find_last_of('_');  // if it's an object, that stores info about histogram
        if (i != (keyname.size() - 1))
        {  // We don't store it in a dirstrmap object
          if (location != "")
          {                // Path to the TObjString
            sName += "/";  // increment the path for the nested assesment
          }
          sName += keyname;
          std::cout << "name: " << sName << ", path: " << path_to_file << sName << "\n";
          valuestring = it.value();                            // JSON content inside the subAssessment
          printDQGroupJSON(valuestring, sName, path_to_file);  // Recursively print Results/Status, Config/name of this
          // subAssessment and looking for subsubAssessment
        }
      }
    }
  }

  std::string HanOutputFile::getStringName(std::string location, int file_version)
  {
    std::string stringName("Undefined");
    if (file_version == 1)
    {
      // bool success = gROOT->cd(location.c_str() );
      //  if( !success ) {
      if (gROOT->cd(location.c_str()) == 0)
      {
        // std::cout << "Directory \"" << location << "\" is not in han output file\n";
        return "Undefined";
      }
      TIter mylist(gDirectory->GetListOfKeys());
      TKey* key;
      while ((key = dynamic_cast<TKey*>(mylist.Next())) != 0)
      {
        TObject* obj = key->ReadObj();
        stringName = (obj->GetName());
        delete obj;
      }
      return stringName;
    }
    else if (file_version == 2)
    {
      // Split path to TDirectories part and JSON part
      // All JSON strings are Results or Config
      std::size_t split_point = 0;
      std::string JSON_name("");    // Results or Config
      std::string path_inTDir("");  // Path befor Results
      std::string path_inJSON("");  // Path after Results
      if ((split_point = location.find("/Results/")) != std::string::npos)
      {
        JSON_name = "Results";
        path_inTDir = location.substr(0, split_point);
        path_inJSON = location.substr(split_point + 8);  // 8 - is the length of "/Results"
      }
      else if ((split_point = location.find("/Config/")) != std::string::npos)
      {
        JSON_name = "Config";
        path_inTDir = location.substr(0, split_point);
        path_inJSON = location.substr(split_point + 7);  // 7 - is the length of "/Config"
      }
      // Go to TDirectory path
      if (gROOT->cd(path_inTDir.c_str()) == 0)
      {
        return "Undefined";
      }
      // Extract JSON object
      TObjString* JSON_obj = dynamic_cast<TObjString*>(gDirectory->GetKey(JSON_name.c_str())->ReadObj());
      if (not JSON_obj)
      {
        std::cerr << "HanOutputFile::getStringName : dynamic cast failed\n";
        return "Null";
      }
      std::string JSON_str = (JSON_obj->GetName());
      nlohmann::json j = nlohmann::json::parse(JSON_str);
      nlohmann::json::json_pointer JSON_ptr(path_inJSON);
      delete JSON_obj;  // Maybe should not be used
      try
      {
        auto val = j.at(JSON_ptr);
        if (!(val.is_null()))
          stringName = val.get<std::string>();
        else
          return "Null";
      }
      catch (...)
      {
        // Exception. In case if json_pointer (used above) doesn't exist
        return "Null";
      }
    }
    return stringName;
  }

  bool HanOutputFile::containsDir(std::string dirname, std::string maindir)
  {
    while (dirname.size() > 0 && dirname[dirname.size() - 1] == '/')
    {
      dirname = dirname.substr(0, dirname.size() - 2);
    }
    while (dirname.size() > 0 && dirname[0] == '/')
    {
      dirname = dirname.substr(1, dirname.size());
    }
    std::size_t found = dirname.find_first_of("/", 1);
    std::string str = dirname.substr(0, found);
    gROOT->cd(maindir.c_str());
    TKey* key = gDirectory->FindKey(str.c_str());
    bool status = false;
    TObject* obj(0);
    TDirectory* dirobj(0);
    if (key != 0)
    {
      obj = key->ReadObj();
      dirobj = dynamic_cast<TDirectory*>(obj);
    }
    if (dirobj != 0)
    {
      if (found != std::string::npos)
      {
        maindir = maindir + "/" + str;
        dirname = dirname.substr(found + 1, dirname.size());
        status = containsDir(dirname, maindir);
      }
      else
      {
        status = true;
      }
    }
    delete obj;
    return status;
  }

  std::optional<std::string> HanOutputFile::containsKeyInJSON(
    std::string pathInJSON, std::string jsonName, std::string path_to_JSON)
  {
    gROOT->cd(path_to_JSON.c_str());
    TKey* key = gDirectory->FindKey(jsonName.c_str());
    TObject* obj(0);
    if (key == 0)
    {
      return {};  // the JSON with this name is absent
    }
    else
    {
      obj = key->ReadObj();
    }
    std::string content = obj->GetName();  // In ATLAS DQM root files GetName() actually returns the content rather
                                           // than the name of a string
    if (pathInJSON == "")                  // If we should check just the existense of the JSON string
    {
      delete obj;
      return "JSON exists";
    }
    if (pathInJSON == "/")  // When we need the whole JSON string
    {
      delete obj;
      return content;
    }
    nlohmann::ordered_json j = nlohmann::ordered_json::parse(content);
    nlohmann::ordered_json::json_pointer p1(pathInJSON);
    std::string return_string;
    try
    {
      auto val1 = j.at(p1);
      if (val1.type() == nlohmann::json::value_t::string)
      {
        val1.get_to (return_string);
      }
      else if (val1.type() == nlohmann::json::value_t::object)
      {
        return_string = val1.dump();
      }
      else
      {
        std::cout << "Warning: Strange part of JSON" << std::endl;
        delete obj;
        return {};
      }
    }
    catch (...)
    {
      // std::cout<<"   the path not exists\n";
      // std::cout<<"J\n";
      delete obj;
      return {};
    }
    delete obj;
    return return_string;
  }

  double HanOutputFile::getNEntries(std::string location, std::string histname)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::getNEntries(): "
                << "No input file is open\n";
      return 0.0;
    }

    double Nentries = 0.0;
    m_file->cd(location.c_str());
    // gdirectory->cd(location.c_str() );
    TH1* h(0);
    gDirectory->GetObject(histname.c_str(), h);
    if (h != 0)
    {
      Nentries = h->GetEntries();
      delete h;
    }
    TGraph* g(0);
    gDirectory->GetObject(histname.c_str(), g);
    if (g != 0)
    {
      Nentries = g->GetN();
      delete g;
    }
    TEfficiency* e(0);
    gDirectory->GetObject(histname.c_str(), e);
    if (e != 0)
    {
      Nentries = e->GetCopyTotalHisto()->GetEntries();
      delete e;
    }

    return Nentries;
  }

  double HanOutputFile::getNEntries(const TObject* obj)
  {
    if (const TH1* h = dynamic_cast<const TH1*>(obj))
    {
      return h->GetEntries();
    }
    else if (const TGraph* g = dynamic_cast<const TGraph*>(obj))
    {
      return g->GetN();
    }
    else if (const TEfficiency* e = dynamic_cast<const TEfficiency*>(obj))
    {
      return e->GetCopyTotalHisto()->GetEntries();
    }
    else
    {
      std::cerr << "HanOutputFile::getNEntries(): "
                << "provided object is not a histogram or graph\n";
      return 0.0;
    }
  }

  std::string HanOutputFile::getInfo(std::string location, int file_version)
  {
    dqi::DisableMustClean disabled;
    std::string value("");
    if (file_version == 1)
    {
      gROOT->cd(location.c_str());
      TIter mylist(gDirectory->GetListOfKeys());
      TKey* key;
      while ((key = dynamic_cast<TKey*>(mylist.Next())) != 0)
      {
        TObject* obj = key->ReadObj();
        TDirectory* subdir = dynamic_cast<TDirectory*>(obj);
        if (subdir != 0)
        {
          std::string name_subdir = subdir->GetName();
          gROOT->cd((location + "/" + name_subdir).c_str());
          TIter mylist1(gDirectory->GetListOfKeys());
          TKey* key1;
          while ((key1 = dynamic_cast<TKey*>(mylist1.Next())) != 0)
          {
            TObject* obj1 = key1->ReadObj();
            TDirectory* subsubdir = dynamic_cast<TDirectory*>(obj1);
            if (subsubdir != 0)
            {
              std::string name_subsubdir = obj1->GetName();
              gROOT->cd((location + "/" + name_subdir + "/" + name_subsubdir).c_str());
              TIter mylist2(gDirectory->GetListOfKeys());
              TKey* key2;
              while ((key2 = dynamic_cast<TKey*>(mylist2.Next())) != 0)
              {
                TObject* obj2 = key2->ReadObj();
                TDirectory* finaldir = dynamic_cast<TDirectory*>(obj2);
                if (finaldir != 0)
                {
                  std::string name_finaldir = obj2->GetName();
                  gROOT->cd((location + "/" + name_subdir + "/" + name_subsubdir + "/" + name_finaldir).c_str());
                  TIter mylist3(gDirectory->GetListOfKeys());
                  TKey* key3;
                  while ((key3 = dynamic_cast<TKey*>(mylist3.Next())) != 0)
                  {
                    TObject* obj3 = key3->ReadObj();
                    std::string value_info = obj3->GetName();
                    value += (name_subsubdir + name_finaldir + ":  " + value_info + "  ");
                    delete obj3;
                  }
                  gROOT->cd((location + "/" + name_subdir + "/" + name_subsubdir).c_str());
                }
                else if (name_subsubdir != "name" && name_subsubdir != "Status" && name_subsubdir != "display")
                {
                  // else if(name_subsubdir!="name" && name_subsubdir!="Status"){
                  std::string value_info = obj2->GetName();
                  value += (name_subsubdir + ":  " + value_info + "  ");
                }
                delete obj2;
              }
              gROOT->cd((location + "/" + name_subdir).c_str());
            }
            else if (name_subdir != "name" && name_subdir != "Status")
            {
              std::string value_info = obj1->GetName();
              value += (name_subdir + ":  " + value_info + "  ");
            }
            delete obj1;
          }
          gROOT->cd((location).c_str());
        }
        delete obj;
      }
    }
    else if (file_version == 2)
    {
      // Separate Tdirectory from the JSON file. JSON File starts with Results or Config Node
      std::string JSON_name("");    // Results or Config
      std::string TDir_path("");    // Path befor Results
      std::string path_inJSON("");  // Path after Results
      std::size_t split_point = 0;
      if ((split_point = location.rfind("/Results")) != std::string::npos)
      {
        JSON_name = "Results";
        TDir_path = location.substr(0, split_point);
        path_inJSON = location.substr(split_point + 8);  // 8 - is the length of "/Results"
      }
      else if ((split_point = location.rfind("/Config")) != std::string::npos)
      {
        JSON_name = "Config";
        TDir_path = location.substr(0, split_point);
        path_inJSON = location.substr(split_point + 7);  // 7 - is the length of "/Config"
      }
      // Go to TDirectory path
      if (gROOT->cd(TDir_path.c_str()) == 0)
      {  // Go to Tdirectory, that contains JSON
        return "Undefined";
      }
      // Extract JSON object
      TObjString* JSON_obj = dynamic_cast<TObjString*>(gDirectory->GetKey(JSON_name.c_str())->ReadObj());
      if (not JSON_obj)
      {
        std::cerr << "HanOutputFile::getInfo : dynamic cast failed\n";
        return "Null";
      }
      std::string JSON_str = (JSON_obj->GetName());
      nlohmann::ordered_json j = nlohmann::ordered_json::parse(JSON_str);
      nlohmann::ordered_json json_in_j;
      if (path_inJSON != "")
      {
        nlohmann::ordered_json::json_pointer JSON_ptr(path_inJSON);
        try
        {
          json_in_j = j.at(JSON_ptr);
        }
        catch (...)
        {
          // Exception. In case if json_pointer (used above) doesn't exist
          std::cout << "Wrong path: " << location << "\n";
          return "Null";
        }
      }
      else
      {
        json_in_j = j;
      }
      value = processJSON_ingetInfo(json_in_j);
    }
    return value;
  }

  std::string HanOutputFile::getInfo(std::string JSON_str)
  {
    dqi::DisableMustClean disabled;
    std::string value("");
    nlohmann::ordered_json j = nlohmann::ordered_json::parse(JSON_str);
    value = processJSON_ingetInfo(j);
    return value;
  }

  std::string HanOutputFile::processJSON_ingetInfo(nlohmann::ordered_json j)
  {
    std::string value("");
    for (nlohmann::ordered_json::iterator it = j.begin(); it != j.end(); ++it)
    {                       // Search in Results/Config node
      auto key = it.key();  // subdir name analog
      auto val = it.value();
      if (strcmp(val.type_name(), "object") == 0)
      {  // subsubdir analog
        for (nlohmann::ordered_json::iterator it1 = val.begin(); it1 != val.end(); ++it1)
        {
          auto key1 = it1.key();  // subsubdir name analog
          auto val1 = it1.value();
          if (strcmp(val1.type_name(), "object") == 0)
          {  // finaldir analog
            for (nlohmann::ordered_json::iterator it2 = val1.begin(); it2 != val1.end(); ++it2)
            {
              auto key2 = it2.key();    // finaldir name analog
              auto val2 = it2.value();  // leaf
              value += (key1 + key2 + ":  " + val2.get<std::string>() +
                        "  ");  //.get<std::string is needed to get rid of quotes in cout
            }
          }
          else if (key1 != "name" && key1 != "Status" && key1 != "display")
          {
            value += (key1 + ":  " + val1.get<std::string>() +
                      "  ");  //.get<std::string is needed to get rid of quotes in cout
          }
        }
      }
      else if (key != "name" && key != "Status")
      {
        if (val.is_null())
        {
          value += (key + ":  null  ");
        }
        else
        {
          value +=
            (key + ":  " + val.get<std::string>() + "  ");  //.get<std::string is needed to get rid of quotes in cout
        }
      }
    }
    return value;
  }

  std::string HanOutputFile::getIndentation(std::string pathName, std::string leadingSpace)
  {
    std::string space = leadingSpace;
    std::string::size_type i = pathName.find_first_of('/');
    if (i != std::string::npos)
    {
      std::string subPath(pathName, i + 1, std::string::npos);
      space += "  ";
      return getIndentation(subPath, space);
    }
    return space;
  }

  bool HanOutputFile::setFile(std::string fileName)
  {
    clearData();
    if (fileName == "") return false;
    m_file = TFile::Open(fileName.c_str());
    if (m_file != 0) return true;

    return false;
  }

  int HanOutputFile::getFileVersion()
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::getFileVersion(): "
                << "No input file is open\n";
      return 0;
    }

    m_file->cd("HanMetadata_");

    if (gDirectory->FindKey("File"))
    {
      gDirectory->cd("File");
      gDirectory->cd("Version_name");
      TIter next(gDirectory->GetListOfKeys());
      TKey* key;
      while ((key = (TKey*)next()))
      {
        TString key_name = key->GetName();
        if (key_name == "V.2.3")
        {
          return 2;
        }
        else
        {
          return 1;
        }
      }
    }
    else
    {
      return 1;
    }
    return 0;
  }

  void HanOutputFile::printAllDQGroups()
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::printAllGroupDirs(): "
                << "No input file is open\n";
      return;
    }
    int file_version = getFileVersion();
    if (file_version == 1)
    {
      if (m_indirMap.size() == 0)
      {
        getAllGroupDirs(m_indirMap, m_file, "");
      }

      DirMap_t::const_iterator idirend = m_indirMap.end();
      for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
      {
        std::string idirName = idir->first;
        const char* path(idir->second->GetPath());
        std::cout << "name: " << idirName << ", path: " << path << "\n";
      }
    }
    else if (file_version == 2)
    {
      if (m_indirstrMap.size() == 0)
      {
        getAllGroupDirs_V2(m_indirstrMap, m_file, "");
      }
      const char* path_to_file = m_file->GetPath();
      DirStrMap_t::const_iterator idirend = m_indirstrMap.end();
      for (DirStrMap_t::const_iterator idir = m_indirstrMap.begin(); idir != idirend; ++idir)
      {
        std::string idirName = idir->first;  // Actually it's a path
        TDirectory* dirobj = dynamic_cast<TDirectory*>(idir->second);
        if (dirobj != 0)
        {  // If the object is a TDirectory type
          std::string pathname(dirobj->GetPath());
          std::cout << "name: " << idirName << ", path: " << pathname << "\n";
        }
        else
        {
          std::cout << "name: " << idirName << ", path: " << path_to_file << idirName << "\n";
          TObjString* strobj = dynamic_cast<TObjString*>(idir->second);
          if (not strobj)
          {
            std::cerr << "HanOutputFile::printAllGroupDirs(): dynamic cast failed\n";
            continue;
          }
          std::string content = strobj->GetName();  // In ATLAS DQM root files GetName() actually returns the
          // content rather than the name of a string
          nlohmann::json j = nlohmann::json::parse(content);  // Get JSON object from TObjString
          printDQGroupJSON(j, idirName, path_to_file);        // Recursive parsing and print of the nested Assessment in
          // the form of TOBJString
        }
      }
    }
  }

  void HanOutputFile::printAllDQAssessments() { streamAllDQAssessments(std::cout, false); }

  void HanOutputFile::printHistoAssessments() { streamHistoAssessments(std::cout, false); }

  std::string HanOutputFile::stringListSystemPaths(std::string location)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::stringListSystemPaths(): "
                << "No input file is open\n";
      return "";
    }

    if (m_indirMap.size() == 0)
    {
      getAllGroupDirs(m_indirMap, m_file, "");
    }

    std::string result("");

    DirMap_t::const_iterator idirend = m_indirMap.end();
    for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
    {
      DirToAssMap_t::const_iterator aMapIter = m_assessMap.find(idir->first);
      if (aMapIter == m_assessMap.end())
      {
        AssMap_t* aMap = new AssMap_t();
        DirToAssMap_t::value_type aMapVal(idir->first, aMap);
        aMapIter = m_assessMap.insert(aMapVal).first;
        getAllAssessments(*aMap, idir->second);
      }

      AssMap_t::const_iterator ias = aMapIter->second->begin();
      if (ias != aMapIter->second->end())
      {
        std::string hisPath = ias->second;
        std::string::size_type sepi = hisPath.find(':');
        if (sepi != std::string::npos)
        {
          hisPath = std::string(hisPath, sepi + 1, std::string::npos);
        }
        std::string completeDir(location);
        completeDir += hisPath;
        result += completeDir;
        result += " ";
      }
    }
    return result;
  }

  std::string HanOutputFile::stringAllDQAssessments()
  {
    std::ostringstream result;
    streamAllDQAssessments(result, true);
    return result.str();
  }

  std::string HanOutputFile::stringHistoAssessments()
  {
    std::ostringstream result;
    streamHistoAssessments(result, true);
    return result.str();
  }

  std::string HanOutputFile::stringAllHistograms()
  {
    std::ostringstream result;
    streamAllHistograms(result, true);
    return result.str();
  }

  void HanOutputFile::streamAllDQAssessments(std::ostream& o, bool streamAll)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::streamAllDQAssessments(): "
                << "No input file is open\n";
      return;
    }

    int file_version = getFileVersion();

    if ((file_version == 1) || (file_version == 2))
    {
      if (m_indirMap.size() == 0)
      {
        getAllGroupDirs(m_indirMap, m_file, "");
      }

      DirMap_t::const_iterator idirend = m_indirMap.end();
      for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
      {
        std::string idirName = idir->first;
        std::string pathname(idir->second->GetPath());
        std::string::size_type i = pathname.find_last_of('/');
        if (i != (pathname.size() - 1))
        {
          pathname += "/";
        }
        std::string pathnameS = pathname + "Results/Status";
        std::string pathnameA = pathname + "Config/name";
        std::string idirStatus = getStringName(pathnameS, file_version);
        std::string idirAlg = getStringName(pathnameA, file_version);
        std::string indent = (idirName == "<top_level>") ? "" : getIndentation(idirName, "  ");
        if (!streamAll)
        {
          std::string::size_type idirNamei = idirName.find_last_of('/');
          if (idirNamei != std::string::npos)
          {
            idirName = std::string(idirName, idirNamei + 1, std::string::npos);
          }
        }
        std::string formattedName(indent);
        formattedName += idirName;

        // left-align this text
        const std::ios_base::fmtflags savedFlags = o.setf(std::ios_base::left, std::ios_base::adjustfield);
        // set the width of the next item to be printed

        o.width(40);
        o << formattedName << "  ";
        // return to normal right-alignment
        o.setf(std::ios_base::right, std::ios_base::adjustfield);
        o.width(9);
        o << idirStatus << "  ";
        o.width(30);
        o << idirAlg;
        if (streamAll)
        {
          o << "  dir\n";
        }
        else
        {
          o << "\n";
        }
        o.flags(savedFlags);

        DirToAssMap_t::const_iterator aMapIter = m_assessMap.find(idir->first);
        if (aMapIter == m_assessMap.end())
        {
          AssMap_t* aMap = new AssMap_t();
          DirToAssMap_t::value_type aMapVal(idir->first, aMap);
          aMapIter = m_assessMap.insert(aMapVal).first;
          getAllAssessments(*aMap, idir->second);
        }

        AssMap_t::const_iterator aend = aMapIter->second->end();
        std::string info1("");
        std::string info2("");
        for (AssMap_t::const_iterator ias = aMapIter->second->begin(); ias != aend; ++ias)
        {
          std::string hisName = ias->first;
          std::string hisPath = ias->second;
          std::string Path1 = hisPath + "/" + hisName + "_/Results";
          std::string Path2 = hisPath + "/" + hisName + "_/Config";
          if (streamAll)
          {
            info1 = getInfo(Path1, file_version);

            info2 = getInfo(Path2, file_version);
          }
          std::string formattedHistName(indent);
          std::string status = getStringName(Path1 + "/Status", file_version);
          std::string algo = getStringName(Path2 + "/name", file_version);
          formattedHistName += "  ";
          formattedHistName += hisName;
          // left-align this text
          const std::ios_base::fmtflags savedFlags = o.setf(std::ios_base::left, std::ios_base::adjustfield);
          // set the width of the next item to be printed

          o.width(40);
          o << formattedHistName << "  ";
          // return to normal right-alignment
          o.setf(std::ios_base::right, std::ios_base::adjustfield);
          o.width(9);
          o << status << "  ";
          o.width(30);
          o << algo;
          if (streamAll)
          {
            // Get information about hsitogram
            gDirectory->cd(pathname.c_str());
            TObject* h;
            // TH1 *h;
            gDirectory->GetObject(hisName.c_str(), h);
            std::string hisTitle = h->GetTitle();
            // Print information about histograms
            o << "  ass  entries:  " << getNEntries(h) << "  ";
            TH1* h1;
            if ((h1 = dynamic_cast<TH1*>(h)) && h1->GetDimension() == 1)
            {
              o << " Underflow: " << h1->GetBinContent(0) << " Overflow: " << h1->GetBinContent(h1->GetNbinsX() + 1)
                << " ";
            }
            if (info1 != "" && info2 != "")
            {
              o << "Config  "
                << "Config  " << info2 << "  Results  "
                << "Results  " << info1 << " title " << hisTitle << "\n";
            }
            else if (info1 != "")
            {
              o << "Results  Results  " << info1 << " title " << hisTitle << "\n";
            }
            else if (info2 != "")
            {
              o << "Config  Config  " << info2 << " title " << hisTitle << "\n";
            }
            else
            {
              o << " title " << hisTitle << "\n";
            }
          }
          else
          {
            o << "\n";
          }
          o.flags(savedFlags);
        }
      }
    }
  }

  void HanOutputFile::streamHistoAssessments(std::ostream& o, bool streamAll)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::streamHistoAssessments(): "
                << "No input file is open\n";
      return;
    }

    if (m_indirMap.size() == 0)
    {
      getAllGroupDirs(m_indirMap, m_file, "");
    }

    DirMap_t::const_iterator idirend = m_indirMap.end();
    for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
    {
      std::string idirName = idir->first;
      std::string pathname(idir->second->GetPath());
      std::string::size_type i = pathname.find_last_of('/');
      if (i != (pathname.size() - 1))
      {
        pathname += "/";
      }
      std::string idirStatus = "Undefined";
      std::string idirAlg = "Undefined";
      std::string indent = (idirName == "<top_level>") ? "" : getIndentation(idirName, "  ");
      if (!streamAll)
      {
        std::string::size_type idirNamei = idirName.find_last_of('/');
        if (idirNamei != std::string::npos)
        {
          idirName = std::string(idirName, idirNamei + 1, std::string::npos);
        }
      }
      std::string formattedName;
      formattedName += idirName;

      DirToAssMap_t::const_iterator aMapIter = m_assessMap.find(idir->first);
      if (aMapIter == m_assessMap.end())
      {
        AssMap_t* aMap = new AssMap_t();
        DirToAssMap_t::value_type aMapVal(idir->first, aMap);
        aMapIter = m_assessMap.insert(aMapVal).first;
        getAllAssessments(*aMap, idir->second);
      }

      AssMap_t::const_iterator aend = aMapIter->second->end();
      for (AssMap_t::const_iterator ias = aMapIter->second->begin(); ias != aend; ++ias)
      {
        std::string hisName = ias->first;
        std::string hisPath = ias->second;
        std::string formattedHistName;
        gDirectory->cd(pathname.c_str());
        // TH1 *h;
        TObject* h;
        gDirectory->GetObject(hisName.c_str(), h);
        std::string hisTitle(h->GetTitle());
        formattedHistName += "";
        formattedHistName += hisName;
        // left-align this text
        const std::ios_base::fmtflags savedFlags = o.setf(std::ios_base::left, std::ios_base::adjustfield);
        o.width(40);

        if (streamAll)
        {
          o << hisPath << "/" << formattedHistName << "  "
            << "\n";
        }
        else
        {
          o << "\n";
        }
        o.flags(savedFlags);
      }
    }
  }

  void HanOutputFile::streamAllHistograms(std::ostream& o, bool streamAll)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::streamAllDQAssessments(): "
                << "No input file is open\n";
      return;
    }

    if (m_indirMap.size() == 0)
    {
      getAllGroupDirs(m_indirMap, m_file, "");
    }

    DirMap_t::const_iterator idirend = m_indirMap.end();
    for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
    {
      std::string idirName = idir->first;
      std::string pathname(idir->second->GetPath());
      std::string::size_type i = pathname.find_last_of('/');
      if (i != (pathname.size() - 1))
      {
        pathname += "/";
      }
      std::string idirStatus = "Undefined";
      std::string idirAlg = "Undefined";
      std::string indent = (idirName == "<top_level>") ? "" : getIndentation(idirName, "  ");
      if (!streamAll)
      {
        std::string::size_type idirNamei = idirName.find_last_of('/');
        if (idirNamei != std::string::npos)
        {
          idirName = std::string(idirName, idirNamei + 1, std::string::npos);
        }
      }
      std::string formattedName(indent);
      formattedName += idirName;

      // left-align this text
      const std::ios_base::fmtflags savedFlags = o.setf(std::ios_base::left, std::ios_base::adjustfield);
      // set the width of the next item to be printed
      o.width(40);
      o << formattedName << "  ";
      // return to normal right-alignment
      o.setf(std::ios_base::right, std::ios_base::adjustfield);
      o.width(9);
      o << idirStatus << "  ";
      o.width(30);
      o << idirAlg;
      if (streamAll)
      {
        o << "  dir\n";
      }
      else
      {
        o << "\n";
      }
      o.flags(savedFlags);

      DirToAssMap_t::const_iterator aMapIter = m_assessMap.find(idir->first);
      if (aMapIter == m_assessMap.end())
      {
        AssMap_t* aMap = new AssMap_t();
        DirToAssMap_t::value_type aMapVal(idir->first, aMap);
        aMapIter = m_assessMap.insert(aMapVal).first;
        getAllAssessments(*aMap, idir->second);
      }

      AssMap_t::const_iterator aend = aMapIter->second->end();
      for (AssMap_t::const_iterator ias = aMapIter->second->begin(); ias != aend; ++ias)
      {
        std::string hisName = ias->first;
        std::string hisPath = ias->second;
        // std::string Path1 = hisPath + "/"+ hisName + "_/Results";
        // std::string info1 = getInfo(Path1);
        std::string info1, info2;
        // std::string Path2 = hisPath + "/"+ hisName + "_/Config";
        // std::string info2 = getInfo(Path2);
        std::string formattedHistName(indent);
        std::string status = "Undefined";
        std::string algo = "Undefined";
        gDirectory->cd(pathname.c_str());
        // TH1 *h;
        TObject* h;
        gDirectory->GetObject(hisName.c_str(), h);
        std::string hisTitle(h->GetTitle());
        formattedHistName += "  ";
        formattedHistName += hisName;
        // left-align this text
        const std::ios_base::fmtflags savedFlags = o.setf(std::ios_base::left, std::ios_base::adjustfield);
        // set the width of the next item to be printed
        o.width(40);
        o << formattedHistName << "  ";
        // return to normal right-alignment
        o.setf(std::ios_base::right, std::ios_base::adjustfield);
        o.width(9);
        o << status << "  ";
        o.width(30);
        o << algo;
        if (streamAll)
        {
          o << "  ass  entries:  " << getNEntries(h) << "  ";
          if (info1 != "" && info2 != "")
          {
            o << "Config  "
              << "Config  " << info2 << "  Results  "
              << "Results  " << info1 << " title " << hisTitle << "\n";
          }
          else if (info1 != "")
          {
            o << "Results  Results  " << info1 << " title  " << hisTitle << "\n";
          }
          else if (info2 != "")
          {
            o << "Config  Config  " << info2 << " title " << hisTitle << "\n";
          }
          else
          {
            o << " title  " << hisTitle << "\n";
          }
        }
        else
        {
          o << "\n";
        }
        o.flags(savedFlags);
      }
    }
  }

  int HanOutputFile::saveAllHistograms(std::string location, bool drawRefs, std::string run_min_LB, int cnvsType)
  {
    if (m_file == 0)
    {
      std::cerr << "HanOutputFile::saveAllHistograms(): "
                << "No input file is open\n";
      return 0;
    }

    if (m_indirMap.size() == 0)
    {
      getAllGroupDirs(m_indirMap, m_file, "");
    }

    int nSaved = 0;

    DirMap_t::const_iterator idirend = m_indirMap.end();
    for (DirMap_t::const_iterator idir = m_indirMap.begin(); idir != idirend; ++idir)
    {
      DirToAssMap_t::const_iterator aMapIter = m_assessMap.find(idir->first);
      if (aMapIter == m_assessMap.end())
      {
        AssMap_t* aMap = new AssMap_t();
        DirToAssMap_t::value_type aMapVal(idir->first, aMap);
        aMapIter = m_assessMap.insert(aMapVal).first;
        getAllAssessments(*aMap, idir->second);
      }
      AssMap_t::const_iterator aend = aMapIter->second->end();
      for (AssMap_t::const_iterator ias = aMapIter->second->begin(); ias != aend; ++ias)
      {
        std::string hisName = ias->first;
        std::string hisPath = ias->second;
        std::string::size_type sepi = hisPath.find(':');
        if (sepi != std::string::npos)
        {
          hisPath = std::string(hisPath, sepi + 1, std::string::npos);
        }
        std::string completeDir(location);
        completeDir += hisPath;
        completeDir += "/";
        std::cout << "Saving " << completeDir << " " << hisName << "\n" << std::flush;
        try
        {
          bool isSaved = saveHistogramToFile(
            hisName, completeDir, idir->second, drawRefs, run_min_LB, (hisPath + "/" + hisName), cnvsType);
          if (isSaved) ++nSaved;
        }
        catch (std::exception& e)
        {
          std::cerr << "Exception caught: " << e.what() << std::endl;
        }
      }
    }
    return nSaved;
  }

  void getImageBuffer(TImage* img, TCanvas* myC, char** x, int* y)
  {
    img->FromPad(myC);
    img->GetImageBuffer(x, y, TImage::kPng);
  }

  bool HanOutputFile::saveHistogramToFile(std::string nameHis, std::string location, TDirectory* groupDir,
    bool drawRefs, std::string run_min_LB, std::string pathName, int cnvsType)
  {
    std::pair<std::string, std::string> pngAndJson =
      getHistogram(nameHis, groupDir, drawRefs, run_min_LB, pathName, cnvsType);
    // std::string tosave = getHistogramPNG(nameHis, groupDir, drawRefs, run_min_LB, pathName);
    if (pngAndJson.first == "" && pngAndJson.second == "")
    {
      return false;
    }
    std::string namePNG = nameHis;
    std::string nameJSON = nameHis;

    namePNG += ".png";
    nameJSON += ".json";

    std::string::size_type i = location.find_last_of('/');
    if (i != (location.size() - 1))
    {
      location += '/';
    }

    namePNG = location + namePNG;
    nameJSON = location + nameJSON;
    return saveFile(cnvsType, namePNG, pngAndJson.first, nameJSON, pngAndJson.second);
  }

  std::string HanOutputFile::getHistogramPNG(
    std::string nameHis, TDirectory* groupDir, bool drawRefs, std::string run_min_LB, std::string pathName)
  {
    int cnvsType = 1;
    return getHistogram(nameHis, groupDir, drawRefs, run_min_LB, pathName, cnvsType).first;
  }

  std::pair<std::string, std::string> HanOutputFile::getHistogramJSON(
    std::string nameHis, TDirectory* groupDir, bool drawRefs, std::string run_min_LB, std::string pathName)
  {
    int cnvsType = 2;
    return getHistogram(nameHis, groupDir, drawRefs, run_min_LB, pathName, cnvsType);
  }

  std::pair<std::string, std::string> HanOutputFile::getHistogram(std::string nameHis, TDirectory* groupDir,
    bool drawRefs, std::string run_min_LB, std::string pathName, int cnvsType)
  {
    dqi::DisableMustClean disabled;
    groupDir->cd();

    int iMarkerStyle = 20;
    gStyle->SetFrameBorderMode(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetTitleColor(0);
    gStyle->SetStatColor(0);
    gStyle->SetFillColor(1);
    gStyle->SetPalette(1, 0);
    gStyle->SetTitleFontSize(0.06);
    gStyle->SetTitleH(0.06);
    gStyle->SetMarkerStyle(iMarkerStyle);
    gStyle->SetOptStat(111100);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatX(0.99);
    gStyle->SetStatY(0.99);
    gStyle->SetStatW(0.2);
    gStyle->SetStatH(0.1);

    char* x;
    int y;
    std::string json;
    TImage* img = TImage::Create();

    gROOT->SetBatch();
    std::string pathname(groupDir->GetPath());
    std::string display = "";
    bool WasCollectionReference = false;
    int file_version = getFileVersion();
    bool LookForDisplay;
    if (file_version == 1)
    {
      LookForDisplay = containsDir("Config/annotations/display", (pathname + "/" + nameHis + "_"));
      if (LookForDisplay)
      {
        display = getStringName(pathname + "/" + nameHis + "_/Config/annotations/display", file_version);
      }
    }
    else if (file_version == 2)
    {
      std::optional<std::string> JSON_content;
      LookForDisplay = containsDir((nameHis + "_"), pathname);
      if (LookForDisplay)
      {
        JSON_content = containsKeyInJSON("/annotations/display", "Config", (pathname + "/" + nameHis + "_"));
        if (JSON_content)
        {
          LookForDisplay = true;
        }
        else
        {
          LookForDisplay = false;
        }
      }
      if (LookForDisplay)
      {
        display = JSON_content.value();
      }
    }
    // Plot overflows?
    bool PlotOverflows = (display.find("PlotUnderOverflow") != std::string::npos);
    // Look for Draw Options
    std::size_t found = display.find("Draw=");
    std::string drawopt = "";
    while (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      if (found1 != std::string::npos)
      {
        drawopt += boost::algorithm::to_lower_copy(display.substr(found + 5, found1 - found - 5));
      }
      else
      {
        drawopt += boost::algorithm::to_lower_copy(display.substr(found + 5, display.size()));
      }
      found = display.find("Draw=", found + 1);
    }
    // Look for DrawRef Options
    found = display.find("DrawRef=");
    std::string drawrefopt = "";
    while (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      if (found1 != std::string::npos)
      {
        drawrefopt += boost::algorithm::to_lower_copy(display.substr(found + 8, found1 - found - 8));
      }
      else
      {
        drawrefopt += boost::algorithm::to_lower_copy(display.substr(found + 8, display.size()));
      }
      found = display.find("DrawRef=", found + 1);
    }
    if (drawrefopt == "")
    {
      drawrefopt = drawopt;
    }
    // Look for DrawRef2D Options
    found = display.find("DrawRef2D=");
    std::string drawrefopt2D = "";
    while (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      if (found1 != std::string::npos)
      {
        drawrefopt2D += boost::algorithm::to_lower_copy(display.substr(found + 10, found1 - found - 10));
      }
      else
      {
        drawrefopt2D += boost::algorithm::to_lower_copy(display.substr(found + 10, display.size()));
      }
      found = display.find("DrawRef2D=", found + 1);
    }

    // should we rename "Data" ?
    found = display.find("DataName");
    std::string datatitle;
    if (found == std::string::npos)
    {
      datatitle = "Data";
    }
    while (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      if (found1 != std::string::npos)
      {
        datatitle += display.substr(found + 9, found1 - found - 9);
      }
      else
      {
        datatitle += display.substr(found + 9, display.size());
      }
      found = display.find("DataName", found + 1);
    }
    groupDir->cd();
    TKey* hkey = groupDir->FindKey(nameHis.c_str());
    if (hkey == 0)
    {
      std::cerr << "Did not find TKey for \"" << nameHis << "\", will not save this histogram.\n";
      return std::pair<std::string, std::string>{ "", "" };
    }
    TLegend* legend(0);
    TObject* hobj = hkey->ReadObj();
    TObject* ref(0);
    TH1* hRef(0);
    TEfficiency* eRef(0);
    TH2* h2Ref(0);
    std::vector<TH1*> hRefs;
    std::vector<TEfficiency*> eRefs;
    bool hasPlotted(false);
    TH1* h = dynamic_cast<TH1*>(hobj);
    TH2* h2 = dynamic_cast<TH2*>(h);
    TGraph* g = dynamic_cast<TGraph*>(hobj);
    TEfficiency* e = dynamic_cast<TEfficiency*>(hobj);

    std::string name = nameHis;
    /*  name+=".png";
    std::string::size_type i = location.find_last_of( '/' );
    if( i != (location.size()-1) ) {
      location+="/";
    }
    name=location + name; */
    std::string AlgoName("");
    AlgoName = getStringName(pathname + "/" + nameHis + "_/Config/name", file_version);
    int ww = 550;
    int wh = 490;
    found = display.find("TCanvas", found + 1);
    if (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      ww = std::atoi((display.substr(found + 8, found1 - found - 8)).c_str());
      found = display.find_first_of(')', found1 + 1);
      wh = std::atoi((display.substr(found1 + 1, found - found1 - 1)).c_str());
    }
    if (h != 0)
    {
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);

      // if(  h->GetMinimum() >= 0) {
      //       gPad->SetLogy(display.find("LogY")!=std::string::npos );
      //     }
      //     if( BINLOEDGE(h, 1)  > 0) {
      //       gPad->SetLogx(display.find("LogX")!=std::string::npos );
      //     }
      gPad->SetGridx(display.find("SetGridx") != std::string::npos);
      gPad->SetGridy(display.find("SetGridy") != std::string::npos);
      std::size_t found = display.find("SetPalette");
      if (found != std::string::npos)
      {
        std::size_t found1 = display.find_first_of('(', found + 1);
        std::size_t found2 = display.find_first_of(",)", found + 1);
        std::string cn = display.substr(found1 + 1, found2 - found1 - 1);
        int n1 = std::strtol(cn.c_str(), NULL, 0);
        gStyle->SetPalette((Int_t)n1);
      }
      found = display.find("SetGridStyle");
      if (found != std::string::npos)
      {
        std::size_t found1 = display.find_first_of('(', found + 1);
        std::size_t found2 = display.find_first_of(",)", found + 1);
        std::string cn = display.substr(found1 + 1, found2 - found1 - 1);
        int n1 = std::strtol(cn.c_str(), NULL, 0);
        gStyle->SetGridStyle((Style_t)n1);
      }

      /******************* for plotting fit function on top of histogram ******************/
      found = display.find("gaus");
      if (found != std::string::npos)
      {
        Double_t minstat = 0.;
        std::size_t fpos1, fpos2, fpos;
        fpos = display.find("MinStat");
        if (fpos != std::string::npos)
        {
          fpos1 = display.find('(', fpos + 1);
          if (fpos1 != std::string::npos)
          {
            fpos2 = display.find(')', fpos1 + 1);
            if (fpos2 != std::string::npos)
            {
              std::string s_minstat = display.substr(fpos1 + 1, fpos2 - fpos1 - 1);
              minstat = std::strtod(s_minstat.c_str(), NULL);
            }
          }
        }
        std::string fitopt("");
        fpos = display.find("FitOption");
        if (fpos != std::string::npos)
        {
          fpos1 = display.find('(', fpos + 1);
          if (fpos1 != std::string::npos)
          {
            fpos2 = display.find(')', fpos1 + 1);
            if (fpos2 != std::string::npos)
            {
              fitopt = display.substr(fpos1 + 1, fpos2 - fpos1 - 1);
            }
          }
        }
        // plot double gaus
        std::size_t found1 = display.find("doublegaus");
        if (found1 != std::string::npos)
        {
          std::size_t found2 = display.find('(', found1 + 1);
          if (found2 != std::string::npos)
          {
            std::size_t found3 = display.find(')', found2 + 1);
            if (found3 != std::string::npos)
            {
              std::string range = display.substr(found2 + 1, found3 - found2 - 1);
              Double_t xmin = std::strtod(range.c_str(), NULL);
              std::size_t found4 = display.find(',', found2 + 1);
              if (found4 != std::string::npos)
              {
                range = display.substr(found4 + 1, found3 - found4 - 1);
                Double_t xmax = std::strtod(range.c_str(), NULL);
                TF1* f1 = new TF1("f1", "gaus", xmin, xmax);
                h->Fit(f1, "q");
                Double_t par[6];
                f1->GetParameters(par);
                TF1* func = new TF1("func", "gaus(0)+gaus(3)", xmin, xmax);
                func->SetParameters(par);
                func->SetParameter(3, h->GetBinContent(h->GetMaximumBin()));
                func->SetParameter(4, h->GetMean());
                func->SetParameter(5, par[2]);
                func->SetLineColor(kRed);
                func->SetLineWidth(2);
                if (h->GetEffectiveEntries() > minstat)
                {
                  h->Fit(func, ("rq" + fitopt).c_str());
                }
                delete f1;
                delete func;
              }
            }
          }
        }
        else
        {
          // draw gaus+pol1
          std::size_t found1 = display.find("gauspluspol1");
          if (found1 != std::string::npos)
          {
            std::size_t found2 = display.find('(', found1 + 1);
            if (found2 != std::string::npos)
            {
              std::size_t found3 = display.find(')', found2 + 1);
              if (found3 != std::string::npos)
              {
                std::string range = display.substr(found2 + 1, found3 - found2 - 1);
                Double_t xmin = std::strtod(range.c_str(), NULL);
                std::size_t found4 = display.find(',', found2 + 1);
                if (found4 != std::string::npos)
                {
                  range = display.substr(found4 + 1, found3 - found4 - 1);
                  Double_t xmax = std::strtod(range.c_str(), NULL);
                  TF1* func = new TF1("func", "gaus(0)+pol1(3)", xmin, xmax);
                  func->SetLineColor(kRed);
                  func->SetLineWidth(2);
                  func->SetParameters(h->GetBinContent(h->GetMaximumBin()), h->GetMean(), h->GetRMS());
                  if (h->GetEffectiveEntries() > minstat)
                  {
                    h->Fit(func, ("rq" + fitopt).c_str());
                  }
                  delete func;
                }
              }
            }
          }
          else
          {
            // draw gaus+expo
            found1 = display.find("gausplusexpo");
            if (found1 != std::string::npos)
            {
              std::size_t found2 = display.find('(', found1 + 1);
              if (found2 != std::string::npos)
              {
                std::size_t found3 = display.find(')', found2 + 1);
                if (found3 != std::string::npos)
                {
                  std::string range = display.substr(found2 + 1, found3 - found2 - 1);
                  Double_t xmin = std::strtod(range.c_str(), NULL);
                  std::size_t found4 = display.find(',', found2 + 1);
                  if (found4 != std::string::npos)
                  {
                    range = display.substr(found4 + 1, found3 - found4 - 1);
                    Double_t xmax = std::strtod(range.c_str(), NULL);

                    TF1* func = new TF1("func", "gaus(0)+expo(3)", xmin, xmax);
                    func->SetLineColor(kRed);
                    func->SetLineWidth(2);
                    func->SetParameters(h->GetBinContent(h->GetMaximumBin()), h->GetMean(), h->GetRMS());
                    if (h->GetEffectiveEntries() > minstat)
                    {
                      h->Fit(func, ("rq" + fitopt).c_str());
                    }
                    delete func;
                  }
                }
              }
            }
            else
            {
              // the last case: single gaus
              std::size_t found2 = display.find('(', found + 1);
              if (found2 != std::string::npos)
              {
                std::size_t found3 = display.find(')', found2 + 1);
                if (found3 != std::string::npos)
                {
                  std::string range = display.substr(found2 + 1, found3 - found2 - 1);
                  Double_t xmin = std::strtod(range.c_str(), NULL);
                  std::size_t found4 = display.find(',', found2 + 1);
                  if (found4 != std::string::npos)
                  {
                    range = display.substr(found4 + 1, found3 - found4 - 1);
                    Double_t xmax = std::strtod(range.c_str(), NULL);
                    TF1* func = new TF1("func", "gaus", xmin, xmax);
                    func->SetLineColor(kRed);
                    func->SetLineWidth(2);
                    if (h->GetEffectiveEntries() > minstat)
                    {
                      h->Fit(func, ("rq" + fitopt).c_str());
                    }
                    delete func;
                  }
                }
              }
            }
          }
        }
      }
      if (h2 != 0)
      {
        formatTH2(myC.get(), h2);
        myC->cd();
        if (h2->GetMinimum() >= 0 && h2->GetMaximum() > 0.)
        {
          gPad->SetLogy(display.find("LogY") != std::string::npos);
          gPad->SetLogz(display.find("LogZ") != std::string::npos);
        }
        if (BINLOEDGE(h2, 1) > 0)
        {
          gPad->SetLogx(display.find("LogX") != std::string::npos);
        }
        if (h->GetXaxis()->GetXmin() >= h->GetXaxis()->GetXmax())
        {
          std::cerr << "HanOutputFile::saveHistogramToFile(): "
                    << "Inconsistent x-axis settings:  min=" << h->GetXaxis()->GetXmin() << ", "
                    << "max=" << h->GetXaxis()->GetXmax() << ", "
                    << "Will not save this histogram.\n";
          return std::pair<std::string, std::string>{ "", "" };
        }
        if (h->GetYaxis()->GetXmin() >= h->GetYaxis()->GetXmax())
        {
          std::cerr << "HanOutputFile::saveHistogramToFile(): "
                    << "Inconsistent y-axis settings:  min=" << h->GetYaxis()->GetXmin() << ", "
                    << "max=" << h->GetYaxis()->GetXmax() << ", "
                    << "Will not save this histogram.\n";
          return std::pair<std::string, std::string>{ "", "" };
        }
        axisOption(display, h2);
        if (drawopt == "")
        {
          drawopt = "COLZ";
        }
        if (drawRefs)
        {
          if (file_version == 1)
          {
            groupDir->cd((nameHis + "_/Results").c_str());
            gDirectory->GetObject("Reference;1", ref);
          }
          else if (file_version == 2)
          {
            if (groupDir->GetDirectory((nameHis + "_").c_str()) != 0)
            {
              groupDir->cd((nameHis + "_").c_str());
              gDirectory->GetObject("Reference;1", ref);
            }
          }
          h2Ref = dynamic_cast<TH2*>(ref);
          TCollection* colln = dynamic_cast<TCollection*>(ref);
          if (colln)
          {
            h2Ref = dynamic_cast<TH2*>(colln->MakeIterator()->Next());
          }
          if (h2Ref && (drawrefopt2D != ""))
          {
            formatTH2(myC.get(), h2Ref);
            h2Ref->Draw(drawrefopt2D.c_str());
          }
        }
        h2->Draw(("SAME" + drawopt).c_str());
        displayExtra(myC.get(), display);
        if (drawopt.find("lego") == std::string::npos)
        {
          myC->RedrawAxis();
        }
        if (h2Ref) ratioplot2D(myC.get(), h2, h2Ref, display);

        polynomial(myC.get(), display, h2);
        TLatex t;
        t.SetNDC();
        t.SetTextSize(0.03);
        t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
        TLatex tt;
        tt.SetNDC();
        tt.SetTextSize(0.03);
        tt.DrawLatex(0.02, 0.01, pathName.c_str());
        convertToGraphics(cnvsType, myC.get(), json, img, &x, &y);
      }
      else if (h != 0)
      {
        formatTH1(myC.get(), h);
        if (display.find("StatBox") != std::string::npos)
        {
          h->SetStats(kTRUE);
        }
        if (h->GetXaxis()->GetXmin() >= h->GetXaxis()->GetXmax())
        {
          std::cerr << "HanOutputFile::saveHistogramToFile(): "
                    << "Inconsistent x-axis settings:  min=" << h->GetXaxis()->GetXmin() << ", "
                    << "max=" << h->GetXaxis()->GetXmax() << ", "
                    << "Will not save this histogram.\n";
          return std::pair<std::string, std::string>{ "", "" };
        }
        h->SetLineColor(kBlack);
        h->SetMarkerColor(1);
        // h->SetMarkerStyle(iMarkerStyle);
        // h->SetMarkerSize(0.8);
        h->SetFillStyle(0);
        h->SetLineWidth(2);
        myC->cd();
        if (drawRefs)
        {
          if (file_version == 1)
          {
            groupDir->cd((nameHis + "_/Results").c_str());
            gDirectory->GetObject("Reference;1", ref);
          }
          else if (file_version == 2)
          {
            if (groupDir->GetDirectory((nameHis + "_").c_str()) != 0)
            {
              groupDir->cd((nameHis + "_").c_str());
              gDirectory->GetObject("Reference;1", ref);
            }
          }
          hRef = dynamic_cast<TH1*>(ref);
          if (hRef)
          {
            hRefs.push_back(hRef);
          }
          else
          {
            TCollection* colln = dynamic_cast<TCollection*>(ref);
            if (colln)
            {
              WasCollectionReference = true;
              TIterator* icolln = colln->MakeIterator();
              TObject* ref2;
              while ((ref2 = icolln->Next()))
              {
                hRef = dynamic_cast<TH1*>(ref2);
                if (hRef)
                {
                  if (hRef->GetDimension() == h->GetDimension())
                  {
                    hRefs.push_back(hRef);
                  }
                }
                else
                  std::cout << "hRef cast failed!!!" << std::endl;
              }
            }
          }
          groupDir->cd();
        }

        if (hRefs.size() > 0)
        {
          legend = new TLegend(0.55, 0.77, 0.87, 0.87);
          legend->SetTextFont(62);
          legend->SetMargin(0.15);
          legend->SetFillStyle(0);
          legend->SetBorderSize(0);
          legend->AddEntry(h, datatitle.c_str());
          int itrcolor(0);
          for (auto hRef : hRefs)
          {
            int local_color = root_color_choices[itrcolor];
            itrcolor++;
            formatTH1(myC.get(), hRef);
            TProfile* pRef = dynamic_cast<TProfile*>(hRef);
            if (pRef != 0)
            {
              hRef->SetMarkerColor(local_color);
              // hRef->SetMarkerStyle(iMarkerStyle);
              // hRef->SetMarkerSize(0.8);
              hRef->SetLineColor(local_color);
              hRef->SetLineWidth(2);
              double ymin = (hRef->GetMinimum() < h->GetMinimum()) ? hRef->GetMinimum() : h->GetMinimum();
              double ymax = (hRef->GetMaximum() > h->GetMaximum()) ? hRef->GetMaximum() : h->GetMaximum();
              double xmin, xmax;
              if (PlotOverflows)
              {
                xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1) ? BINLOEDGE(hRef, 1) - BINWIDTH(hRef, 1)
                                                             : BINLOEDGE(h, 1) - BINWIDTH(h, 1));
                xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX()) >
                         BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()))
                         ? BINLOEDGE(hRef, hRef->GetNbinsX()) + 2.0 * BINWIDTH(hRef, hRef->GetNbinsX())
                         : BINLOEDGE(h, h->GetNbinsX()) + 2.0 * BINWIDTH(h, h->GetNbinsX());
              }
              else
              {
                xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1)) ? BINLOEDGE(hRef, 1) : BINLOEDGE(h, 1);
                xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) > BINLOEDGE(h, h->GetNbinsX())
                          ? BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX())
                          : BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()));
              }
              // 	  double y_av = (ymax + ymin)/2;
              // 	  double y_halv = (ymax-ymin)*0.6;
              bool isLogY = (display.find("LogY") != std::string::npos);
              if (isLogY)
              {
                if (ymax <= 0.) ymax = 5.0;
                if (ymin > 0.)
                {
                  double lymax = log(ymax);
                  double lymin = log(ymin);
                  h->SetAxisRange(exp(lymin - (lymax - lymin) * 0.05), exp(lymax + (lymax - lymin) * 0.05),
                    "Y");  // leave 5% gap on above and below
                }
                else
                {
                  std::cerr << "ymin is <0. and LogY requested for histogram \"" << pathname + "/" + nameHis
                            << "\", ymin=" << ymin << std::endl;
                }
              }
              else
              {
                double yMargin = (ymax - ymin) * 0.05;
                h->SetAxisRange(ymin - yMargin, ymax + yMargin, "Y");
              }
              h->GetXaxis()->SetRangeUser(xmin, xmax);
              hRef->GetXaxis()->SetRangeUser(xmin, xmax);
              axisOption(display, h);
              if (h->GetMinimum() >= 0. && hRef->GetMinimum() >= 0. && h->GetMaximum() > 0. && hRef->GetMaximum() > 0.)
              {
                gPad->SetLogy(display.find("LogY") != std::string::npos);
              }
              if (BINLOEDGE(h, 1) > 0 && BINLOEDGE(hRef, 1) > 0)
              {
                gPad->SetLogx(display.find("LogX") != std::string::npos);
              }
              if (!hasPlotted)
              {
                h->Draw(drawopt.c_str());
                hasPlotted = true;
              }
              hRef->Draw(("SAME" + drawrefopt).c_str());
            }
            else
            {
              double scale = 1.0;
              if (display.find("ScaleRef") != std::string::npos)
              {
                scale = getScaleVal(display);
              }
              else if (h->Integral("width") > 0.0 && hRef->Integral("width") > 0.0 &&
                       (AlgoName.find("BinContentComp") == std::string::npos) &&
                       (display.find("NoNorm") == std::string::npos))
              {
                scale = h->Integral("width") / hRef->Integral("width");
              }
              hRef->Scale(scale);
              // hRef->SetMarkerStyle(iMarkerStyle);
              // hRef->SetMarkerSize(0.8);
              hRef->SetMarkerColor(local_color);
              // hRef->SetFillColor(local_color);
              hRef->SetLineColor(local_color);
              double ymin = (hRef->GetMinimum() < h->GetMinimum()) ? hRef->GetMinimum() : h->GetMinimum();
              double ymax = (hRef->GetMaximum() > h->GetMaximum()) ? hRef->GetMaximum() : h->GetMaximum();
              double xmin, xmax;
              if (PlotOverflows)
              {
                xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1)) ? BINLOEDGE(hRef, 1) - BINWIDTH(hRef, 1)
                                                              : BINLOEDGE(h, 1) - BINWIDTH(h, 1);
                xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX()) >
                         BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()))
                         ? BINLOEDGE(hRef, hRef->GetNbinsX()) + 2.0 * BINWIDTH(hRef, hRef->GetNbinsX())
                         : BINLOEDGE(h, h->GetNbinsX()) + 2.0 * BINWIDTH(h, h->GetNbinsX());
              }
              else
              {
                xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1)) ? BINLOEDGE(hRef, 1) : BINLOEDGE(h, 1);
                xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX()) >
                         BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()))
                         ? BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX())
                         : BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX());
              }

              // 	  double y_av = (ymax + ymin)/2;
              // 	  double y_halv = (ymax-ymin)*0.6;
              bool isLogY = (display.find("LogY") != std::string::npos);
              //	  if ( ymin == 0.0 &&  display.find("LogY")!=std::string::npos ){

              if (isLogY)
              {
                if (ymax <= 0.) ymax = 5.0;
                if (ymin > 0.)
                {
                  double lymax = log(ymax);
                  double lymin = log(ymin);
                  h->SetAxisRange(exp(lymin - (lymax - lymin) * 0.05), exp(lymax + (lymax - lymin) * 0.05), "Y");
                  // leave 5% gap on above and below
                }
                else
                {
                  std::cerr << "ymin is <=0. and LogY requested for histogram \"" << pathname + "/" + nameHis
                            << "\", ymin=" << ymin << std::endl;
                }
              }
              else
              {
                double yDiff = ymax - ymin;
                h->SetAxisRange(ymin - yDiff * 0.05, ymax + yDiff * 0.05, "Y");  // leave 5% gap above and below
              }

              h->GetXaxis()->SetRangeUser(xmin, xmax);
              hRef->GetXaxis()->SetRangeUser(xmin, xmax);
              myC->cd();
              if (h->GetMinimum() >= 0 && hRef->GetMinimum() >= 0 && h->GetMaximum() > 0. && hRef->GetMaximum() > 0.)
              {
                gPad->SetLogy(display.find("LogY") != std::string::npos);
              }
              if (BINLOEDGE(h, 1) > 0 && BINLOEDGE(hRef, 1) > 0)
              {
                gPad->SetLogx(display.find("LogX") != std::string::npos);
              }
              axisOption(display, h);
              if (!hasPlotted)
              {
                h->Draw(drawopt.c_str());
                hasPlotted = true;
              }
              hRef->Draw(("SAME" + drawrefopt).c_str());
            }
            if (WasCollectionReference)
            {
              legend->AddEntry(hRef, hRef->GetName());
            }
            else
            {
              std::string refInfo("");
              refInfo = getStringName(pathname + "/" + nameHis + "_/Config/annotations/refInfo", file_version);
              legend->AddEntry(hRef, refInfo != "Undefined" ? refInfo.c_str() : "Reference");
            }
          }
          h->Draw(("SAME" + drawopt).c_str());
          legend->Draw();
        }
        else
        {
          myC->cd();
          if (h->GetMinimum() >= 0)
          {
            gPad->SetLogy(display.find("LogY") != std::string::npos);
          }
          if (BINLOEDGE(h, 1) > 0)
          {
            gPad->SetLogx(display.find("LogX") != std::string::npos);
          }
          axisOption(display, h);
          h->Draw(drawopt.c_str());
        }
        myC->cd();
        displayExtra(myC.get(), display);
        myC->RedrawAxis();

        if (hRef)
        {
          ratioplot(myC.get(), h, hRef, display);  // RatioPad
        }
        myC->cd();                          // might be unnecessary
        polynomial(myC.get(), display, h);  // draw polynome for TH1

        TLatex t;
        t.SetNDC();
        t.SetTextSize(0.03);
        t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
        TLatex tt;
        tt.SetNDC();
        tt.SetTextSize(0.03);
        tt.DrawLatex(0.02, 0.01, pathName.c_str());

        convertToGraphics(cnvsType, myC.get(), json, img, &x, &y);
      }
      // delete myC;
      gStyle->Reset();
    }
    if (g)
    {
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);
      myC->cd();
      if (g->GetMinimum() >= 0. && g->GetMaximum() > 0.)
      {
        gPad->SetLogy(display.find("LogY") != std::string::npos);
      }
      //       if( BINLOEDGE(h2, 1)  > 0) {
      // 	gPad->SetLogx(display.find("LogX")!=std::string::npos );
      //       }
      //     gPad->SetLogz(display.find("LogZ")!=std::string::npos );
      formatTGraph(myC.get(), g);
      //       axisOption(display,g);
      g->Draw((std::string("AP") + drawopt).c_str());
      displayExtra(myC.get(), display);
      TLatex t;
      t.SetNDC();
      t.SetTextSize(0.03);
      t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
      TLatex tt;
      tt.SetNDC();
      tt.SetTextSize(0.03);
      tt.DrawLatex(0.02, 0.01, pathName.c_str());
      // myC->SaveAs( name.c_str() );

      convertToGraphics(cnvsType, myC.get(), json, img, &x, &y);

      gStyle->Reset();
    }
    if (e != 0)
    {
      hasPlotted = false;
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);
      formatTEfficiency(myC.get(), e);
      if (drawopt == "")
      {
        if (e->GetDimension() == 1)
        {
          drawopt = "AP";
        }
        else
        {
          drawopt = "COLZ";
        }
      }
      if (drawRefs)
      {
        if (file_version == 1)
        {
          groupDir->cd((nameHis + "_/Results").c_str());
          gDirectory->GetObject("Reference;1", ref);
        }
        else if (file_version == 2)
        {
          if (groupDir->cd((nameHis + "_").c_str()))
          {
            gDirectory->GetObject("Reference;1", ref);
          }
        }
        eRef = dynamic_cast<TEfficiency*>(ref);
        if (eRef)
        {
          eRefs.push_back(eRef);
        }
        else
        {
          TCollection* colln = dynamic_cast<TCollection*>(ref);
          if (colln)
          {
            WasCollectionReference = true;
            TIterator* icolln = colln->MakeIterator();
            TObject* ref2;
            while ((ref2 = icolln->Next()))
            {
              eRef = dynamic_cast<TEfficiency*>(ref2);
              if (eRef)
              {
                if (eRef->GetDimension() == e->GetDimension())
                {
                  eRefs.push_back(eRef);
                }
              }
              else
                std::cout << "eRef cast failed!!!" << std::endl;
            }
          }
        }
        groupDir->cd();
      }
      if (eRefs.size() > 0)
      {
        legend = new TLegend(0.55, 0.77, 0.87, 0.87);
        legend->SetTextFont(62);
        legend->SetMargin(0.15);
        legend->SetFillStyle(0);
        legend->SetBorderSize(0);
        legend->AddEntry(e, datatitle.c_str());
        int itrcolor(0);
        for (auto eRef : eRefs)
        {
          myC->cd();
          e->Draw("");
          eRef->Draw("");
          gPad->Update();

          int local_color = root_color_choices[itrcolor];
          itrcolor++;

          formatTEfficiency(myC.get(), eRef);
          eRef->SetMarkerColor(local_color);
          eRef->SetLineColor(local_color);

          if (!hasPlotted)
          {
            e->Draw(drawopt.c_str());
            hasPlotted = true;
          }
          eRef->Draw("SAME");
          myC->Update();

          if (WasCollectionReference)
          {
            legend->AddEntry(eRef, eRef->GetName());
          }
          else
          {
            std::string refInfo("");
            refInfo = getStringName(pathname + "/" + nameHis + "_/Config/annotations/refInfo", file_version);
            legend->AddEntry(eRef, refInfo != "Undefined" ? refInfo.c_str() : "Reference");
          }
        }
        legend->Draw();
      }
      else
      {
        myC->cd();
        e->Draw(drawopt.c_str());
      }
      myC->cd();
      displayExtra(myC.get(), display);
      TLatex t;
      t.SetNDC();
      t.SetTextSize(0.03);
      t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
      TLatex tt;
      tt.SetNDC();
      tt.SetTextSize(0.03);
      tt.DrawLatex(0.02, 0.01, pathName.c_str());
      convertToGraphics(cnvsType, myC.get(), json, img, &x, &y);
      gStyle->Reset();
    }
    std::string rv;
    if (cnvsType & GENERATE_PNG)
    {
      rv.assign(x, y);
    };
    std::pair<std::string, std::string> rvPair{ rv, json };

    delete img;
    delete hobj;
    delete hRef;
    delete legend;
    return rvPair;
  }

  bool HanOutputFile::saveHistogramToFileSuperimposed(std::string nameHis, std::string location, TDirectory* groupDir1,
    TDirectory* groupDir2, bool drawRefs, std::string run_min_LB, std::string pathName, int cnvsType)
  {
    dqi::DisableMustClean disabled;
    groupDir1->cd();
    gStyle->SetFrameBorderMode(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetTitleColor(0);
    gStyle->SetStatColor(0);
    gStyle->SetFillColor(1);
    gStyle->SetPalette(1, 0);
    gStyle->SetTitleFontSize(0.06);
    gStyle->SetTitleH(0.06);
    gStyle->SetMarkerStyle(20);
    gStyle->SetOptStat(111100);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatX(0.99);
    gStyle->SetStatY(0.99);
    gStyle->SetStatW(0.2);
    gStyle->SetStatH(0.1);

    gROOT->SetBatch();
    std::string pathname(groupDir1->GetPath());
    std::string display = "";
    int file_version = getFileVersion();
    bool LookForDisplay;
    if (file_version == 1)
    {
      LookForDisplay = containsDir("Config/annotations/display", (pathname + "/" + nameHis + "_"));
      if (LookForDisplay)
      {
        display = getStringName(pathname + "/" + nameHis + "_/Config/annotations/display", file_version);
      }
    }
    else if (file_version == 2)
    {
      std::optional<std::string> JSON_content;
      LookForDisplay = containsDir((nameHis + "_"), pathname);
      if (LookForDisplay)
      {
        JSON_content = containsKeyInJSON("/annotations/display", "Config", (pathname + "/" + nameHis + "_"));
        if (JSON_content)
        {
          LookForDisplay = true;
        }
        else
        {
          LookForDisplay = false;
        }
      }
      if (LookForDisplay)
      {
        display = JSON_content.value();
      }
    }

    // Look for Draw Options
    std::size_t found = display.find("Draw");
    std::string drawopt = "";
    while (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      if (found1 != std::string::npos)
      {
        drawopt += boost::algorithm::to_lower_copy(display.substr(found + 5, found1 - found - 5));
      }
      else
      {
        drawopt += boost::algorithm::to_lower_copy(display.substr(found + 5, display.size()));
      }
      found = display.find("Draw", found + 1);
    }

    groupDir1->cd();
    TKey* hkey = groupDir1->FindKey(nameHis.c_str());
    groupDir2->cd();
    TKey* hkey2 = groupDir2->FindKey(nameHis.c_str());
    if (hkey == 0 || hkey2 == 0)
    {
      std::cerr << "Did not find TKey for \"" << nameHis << "\", will not save this histogram.\n";
      return false;
    }
    groupDir1->cd();
    TLegend* legend(0);
    TObject* hobj = hkey->ReadObj();
    TObject* hobj2 = hkey2->ReadObj();
    TH1* hRef(0);
    TH1 *h(0), *hist2(0);
    TH2 *h2(0), *h2_2(0), *h2Diff(0);
    TGraph *g(0), *g2(0);
    TEfficiency *e(0), *e2(0);

    std::string json;
    std::string nameJSON = nameHis;
    std::string namePNG = nameHis;
    namePNG += ".png";
    nameJSON += ".json";
    std::string::size_type i = location.find_last_of('/');
    if (i != (location.size() - 1))
    {
      location += "/";
    }
    namePNG = location + namePNG;
    nameJSON = location + nameJSON;
    std::string AlgoName = getStringName(pathname + "/" + nameHis + "_/Config/name", file_version);
    int ww = 550;
    int wh = 490;
    found = display.find("TCanvas", found + 1);
    if (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of(',', found + 1);
      ww = std::atoi((display.substr(found + 8, found1 - found - 8)).c_str());
      found = display.find_first_of(')', found1 + 1);
      wh = std::atoi((display.substr(found1 + 1, found - found1 - 1)).c_str());
    }

    if ((h = dynamic_cast<TH1*>(hobj)) != 0 && (hist2 = dynamic_cast<TH1*>(hobj2)) != 0)
    {
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);
      setupCanvas(drawopt, display);
      attachFits(h, drawopt, display);
      /******************* for plotting fit function on top of histogram ******************/

      /*************************************************************************************************************/
      std::string tmpdraw(drawopt);
      if ((h2 = dynamic_cast<TH2*>(h)) != 0 && ((h2_2 = dynamic_cast<TH2*>(hist2)) != 0))
      {
        // if(tmpdraw=="")
        h2Diff = (TH2*)h2->Clone("difference");
        //       tmpdraw="box";//we don't want colz for overlaid histos
        //       h2->SetLineColor(2);
        //       h2->SetMarkerColor(2);
        //       if(!drawH2(myC,h2,tmpdraw,display))return false;
        //       h2_2->SetMarkerColor(4);
        //       //h2_2->SetFillColor(4);
        //       h2_2->SetLineWidth(2);
        //       h2_2->SetLineColor(4);
        //       tmpdraw=" box same";
        //       if(!drawH2(myC,h2_2,tmpdraw,display))return false;
        h2Diff->Add(h2, h2_2, 1.0, -1.0);
        h2Diff->SetLineColor(2);
        h2Diff->SetMarkerColor(2);
        if (!drawH2(myC.get(), h2Diff, tmpdraw, display)) return false;
        TLatex t;
        t.SetNDC();
        t.SetTextSize(0.03);
        t.DrawLatex(0.02, 0.04, (run_min_LB + " difference").c_str());
        TLatex tt;
        tt.SetNDC();
        tt.SetTextSize(0.03);
        tt.DrawLatex(0.02, 0.01, pathName.c_str());

        convertToGraphics(cnvsType, myC.get(), namePNG, nameJSON);
      }
      else if (h != 0 && hist2 != 0)
      {
        // Petronel
        double scale = 1.0;
        if (display.find("ScaleRef") != std::string::npos)
        {
          scale = getScaleVal(display);
        }
        else if (h->Integral("width") > 0.0 && hist2->Integral("width") > 0.0 &&
                 (AlgoName.find("BinContentComp") == std::string::npos) &&
                 (display.find("NoNorm") == std::string::npos))
        {
          scale = h->Integral("width") / hist2->Integral("width");
        }
        hist2->Scale(scale);
        double ymin = (hist2->GetMinimum() < h->GetMinimum()) ? hist2->GetMinimum() : h->GetMinimum();
        double ymax = (hist2->GetMaximum() > h->GetMaximum()) ? hist2->GetMaximum() : h->GetMaximum();
        double yMargin = (ymax - ymin) * 0.05;
        h->SetAxisRange(ymin - yMargin, ymax + yMargin, "Y");

        h->SetMarkerColor(1);
        h->SetFillStyle(0);
        h->SetLineWidth(2);
        hist2->SetMarkerColor(4);
        hist2->SetLineColor(4);
        hist2->SetFillStyle(0);
        hist2->SetLineWidth(2);
        if (drawRefs)
        {
          if (file_version == 1)
          {
            groupDir1->cd((nameHis + "_/Results").c_str());
          }
          else
          {
            groupDir1->cd((nameHis + "_").c_str());
          }
          gDirectory->GetObject("Reference;1", hRef);
          groupDir1->cd();
        }
        if (!drawH1(myC.get(), h, hRef, tmpdraw, display, AlgoName)) return false;
        tmpdraw += "same";
        if (!drawH1(myC.get(), hist2, 0, tmpdraw, display, AlgoName)) return false;
        legend = new TLegend(0.55, 0.77, 0.87, 0.87);
        legend->SetTextFont(62);
        legend->SetMargin(0.15);
        legend->SetFillStyle(0);
        legend->SetBorderSize(0);

        std::size_t foundN1 = run_min_LB.find_first_of("-");
        std::size_t foundN2 = run_min_LB.find_first_of(',');

        legend->AddEntry(h, ("Run " + run_min_LB.substr(5, foundN1 - 5)).c_str());
        legend->AddEntry(hist2, ("Run " + run_min_LB.substr(foundN1 + 1, foundN2 - foundN1 - 1)).c_str());
        if (hRef)
        {
          legend->AddEntry(hRef, "Reference");
        }
        legend->Draw();

        TLatex t;
        t.SetNDC();
        t.SetTextSize(0.03);
        t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
        TLatex tt;
        tt.SetNDC();
        tt.SetTextSize(0.03);
        tt.DrawLatex(0.02, 0.01, pathName.c_str());

        convertToGraphics(cnvsType, myC.get(), namePNG, nameJSON);

      }  // end histogram drawing
      delete h2Diff;
      gStyle->Reset();
    }

    /*************************************************************************************************************/
    if (((g = dynamic_cast<TGraph*>(hobj)) != 0) && ((g2 = dynamic_cast<TGraph*>(hobj2)) != 0))
    {
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);
      myC->cd();
      if (g->GetMinimum() >= 0. && g2->GetMinimum() >= 0. && g->GetMaximum() > 0. && g2->GetMaximum() > 0.)
      {
        gPad->SetLogy(display.find("LogY") != std::string::npos);
      }
      formatTGraph(myC.get(), g);
      formatTGraph(myC.get(), g2);
      g->Draw((std::string("AP") + drawopt).c_str());
      displayExtra(myC.get(), display);
      g2->SetMarkerColor(2);
      g2->SetLineColor(2);
      g2->Draw((std::string("P") + drawopt + " same").c_str());
      TLatex t;
      t.SetNDC();
      t.SetTextSize(0.03);
      t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
      TLatex tt;
      tt.SetNDC();
      tt.SetTextSize(0.03);
      tt.DrawLatex(0.02, 0.01, pathName.c_str());

      convertToGraphics(cnvsType, myC.get(), namePNG, nameJSON);

      gStyle->Reset();
    }

    if (((e = dynamic_cast<TEfficiency*>(hobj)) != 0) && ((e2 = dynamic_cast<TEfficiency*>(hobj2)) != 0))
    {
      auto myC = std::make_unique<TCanvas>(nameHis.c_str(), "myC", ww, wh);
      myC->cd();

      formatTEfficiency(myC.get(), e);
      formatTEfficiency(myC.get(), e2);
      e->Draw((std::string("AP") + drawopt).c_str());
      displayExtra(myC.get(), display);
      e2->SetMarkerColor(2);
      e2->SetLineColor(2);
      e2->Draw((std::string("P") + drawopt + " same").c_str());
      TLatex t;
      t.SetNDC();
      t.SetTextSize(0.03);
      t.DrawLatex(0.02, 0.04, run_min_LB.c_str());
      TLatex tt;
      tt.SetNDC();
      tt.SetTextSize(0.03);
      tt.DrawLatex(0.02, 0.01, pathName.c_str());

      convertToGraphics(cnvsType, myC.get(), namePNG, nameJSON);

      gStyle->Reset();
    }

    delete hobj;
    delete hobj2;
    delete hRef;
    delete legend;
    return true;
  }

  bool HanOutputFile::drawH2(TCanvas* myC, TH2* h2, std::string& drawop, std::string& display)
  {
    std::string drawopt(drawop);
    myC->cd();
    if (h2->GetMinimum() >= 0 && h2->GetMaximum() > 0)
    {
      gPad->SetLogy(display.find("LogY") != std::string::npos);
      gPad->SetLogz(display.find("LogZ") != std::string::npos);
    }
    else
    {
      gPad->SetLogy(false);
    }
    if (BINLOEDGE(h2, 1) > 0)
    {
      gPad->SetLogx(display.find("LogX") != std::string::npos);
    }
    else
    {
      gPad->SetLogx(false);
    }
    formatTH2(myC, h2);
    if (h2->GetXaxis()->GetXmin() >= h2->GetXaxis()->GetXmax())
    {
      std::cerr << "HanOutputFile::saveHistogramToFile(): "
                << "Inconsistent x-axis settings:  min=" << h2->GetXaxis()->GetXmin() << ", "
                << "max=" << h2->GetXaxis()->GetXmax() << ", "
                << "Will not save this histogram.\n";
      return false;
    }
    if (h2->GetYaxis()->GetXmin() >= h2->GetYaxis()->GetXmax())
    {
      std::cerr << "HanOutputFile::saveHistogramToFile(): "
                << "Inconsistent y-axis settings:  min=" << h2->GetYaxis()->GetXmin() << ", "
                << "max=" << h2->GetYaxis()->GetXmax() << ", "
                << "Will not save this histogram.\n";
      return false;
    }
    axisOption(display, h2);
    if (drawopt == "")
    {
      drawopt = "COLZ";
    }
    h2->Draw(drawopt.c_str());
    displayExtra(myC, display);
    //  std::cout<<"drawh2 drawopt="<<drawopt<<",display="<<display<<std::endl;
    //  if (drawopt.find("lego") == std::string::npos) {
    myC->RedrawAxis();
    //}
    return true;
  }

  void HanOutputFile::setupCanvas(std::string& drawopt, std::string& display)
  {
    gPad->SetGridx(display.find("SetGridx") != std::string::npos);
    gPad->SetGridy(display.find("SetGridy") != std::string::npos);
    std::size_t found = display.find("SetPalette");
    if (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of('(', found + 1);
      std::size_t found2 = display.find_first_of(",)", found + 1);
      std::string cn = display.substr(found1 + 1, found2 - found1 - 1);
      int n1 = std::strtol(cn.c_str(), NULL, 0);
      gStyle->SetPalette((Int_t)n1);
    }
    found = display.find("SetGridStyle");
    if (found != std::string::npos)
    {
      std::size_t found1 = display.find_first_of('(', found + 1);
      std::size_t found2 = display.find_first_of(",)", found + 1);
      std::string cn = display.substr(found1 + 1, found2 - found1 - 1);
      int n1 = std::strtol(cn.c_str(), NULL, 0);
      gStyle->SetGridStyle((Style_t)n1);
    }
    if (!drawopt.empty())
    {
      // do any modifications coming from drawopt
    }
  }

  void HanOutputFile::attachFits(TH1* h, std::string& drawopt, std::string& display)
  {
    size_t found = display.find("gaus");
    if (found != std::string::npos)
    {
      Double_t minstat = 0.;
      std::size_t fpos1, fpos2, fpos;
      fpos = display.find("MinStat");
      if (fpos != std::string::npos)
      {
        fpos1 = display.find('(', fpos + 1);
        if (fpos1 != std::string::npos)
        {
          fpos2 = display.find(')', fpos1 + 1);
          if (fpos2 != std::string::npos)
          {
            std::string s_minstat = display.substr(fpos1 + 1, fpos2 - fpos1 - 1);
            minstat = std::strtod(s_minstat.c_str(), NULL);
          }
        }
      }
      std::string fitopt("");
      fpos = display.find("FitOption");
      if (fpos != std::string::npos)
      {
        fpos1 = display.find('(', fpos + 1);
        if (fpos1 != std::string::npos)
        {
          fpos2 = display.find(')', fpos1 + 1);
          if (fpos2 != std::string::npos)
          {
            fitopt = display.substr(fpos1 + 1, fpos2 - fpos1 - 1);
          }
        }
      }
      // plot double gaus
      std::size_t found1 = display.find("doublegaus");
      if (found1 != std::string::npos)
      {
        std::size_t found2 = display.find('(', found1 + 1);
        if (found2 != std::string::npos)
        {
          std::size_t found3 = display.find(')', found2 + 1);
          if (found3 != std::string::npos)
          {
            std::string range = display.substr(found2 + 1, found3 - found2 - 1);
            Double_t xmin = std::strtod(range.c_str(), NULL);
            std::size_t found4 = display.find(',', found2 + 1);
            if (found4 != std::string::npos)
            {
              range = display.substr(found4 + 1, found3 - found4 - 1);
              Double_t xmax = std::strtod(range.c_str(), NULL);
              TF1* f1 = new TF1("f1", "gaus", xmin, xmax);
              h->Fit(f1, "q");
              Double_t par[6];
              f1->GetParameters(par);
              TF1* func = new TF1("func", "gaus(0)+gaus(3)", xmin, xmax);
              func->SetParameters(par);
              func->SetParameter(3, h->GetBinContent(h->GetMaximumBin()));
              func->SetParameter(4, h->GetMean());
              func->SetParameter(5, par[2]);
              func->SetLineColor(kRed);
              func->SetLineWidth(2);
              if (h->GetEffectiveEntries() > minstat)
              {
                h->Fit(func, ("rq" + fitopt).c_str());
              }
              delete f1;
              delete func;
            }
          }
        }
      }
      else
      {
        // draw gaus+pol1
        std::size_t found1 = display.find("gauspluspol1");
        if (found1 != std::string::npos)
        {
          std::size_t found2 = display.find('(', found1 + 1);
          if (found2 != std::string::npos)
          {
            std::size_t found3 = display.find(')', found2 + 1);
            if (found3 != std::string::npos)
            {
              std::string range = display.substr(found2 + 1, found3 - found2 - 1);
              Double_t xmin = std::strtod(range.c_str(), NULL);
              std::size_t found4 = display.find(',', found2 + 1);
              if (found4 != std::string::npos)
              {
                range = display.substr(found4 + 1, found3 - found4 - 1);
                Double_t xmax = std::strtod(range.c_str(), NULL);
                TF1* func = new TF1("func", "gaus(0)+pol1(3)", xmin, xmax);
                func->SetLineColor(kRed);
                func->SetLineWidth(2);
                func->SetParameters(h->GetBinContent(h->GetMaximumBin()), h->GetMean(), h->GetRMS());
                if (h->GetEffectiveEntries() > minstat)
                {
                  h->Fit(func, ("rq" + fitopt).c_str());
                }
                delete func;
              }
            }
          }
        }
        else
        {
          // draw gaus+expo
          found1 = display.find("gausplusexpo");
          if (found1 != std::string::npos)
          {
            std::size_t found2 = display.find('(', found1 + 1);
            if (found2 != std::string::npos)
            {
              std::size_t found3 = display.find(')', found2 + 1);
              if (found3 != std::string::npos)
              {
                std::string range = display.substr(found2 + 1, found3 - found2 - 1);
                Double_t xmin = std::strtod(range.c_str(), NULL);
                std::size_t found4 = display.find(',', found2 + 1);
                if (found4 != std::string::npos)
                {
                  range = display.substr(found4 + 1, found3 - found4 - 1);
                  Double_t xmax = std::strtod(range.c_str(), NULL);

                  TF1* func = new TF1("func", "gaus(0)+expo(3)", xmin, xmax);
                  func->SetLineColor(kRed);
                  func->SetLineWidth(2);
                  func->SetParameters(h->GetBinContent(h->GetMaximumBin()), h->GetMean(), h->GetRMS());
                  if (h->GetEffectiveEntries() > minstat)
                  {
                    h->Fit(func, ("rq" + fitopt).c_str());
                  }
                  delete func;
                }
              }
            }
          }
          else
          {
            // the last case: single gaus
            std::size_t found2 = display.find('(', found + 1);
            if (found2 != std::string::npos)
            {
              std::size_t found3 = display.find(')', found2 + 1);
              if (found3 != std::string::npos)
              {
                std::string range = display.substr(found2 + 1, found3 - found2 - 1);
                Double_t xmin = std::strtod(range.c_str(), NULL);
                std::size_t found4 = display.find(',', found2 + 1);
                if (found4 != std::string::npos)
                {
                  range = display.substr(found4 + 1, found3 - found4 - 1);
                  Double_t xmax = std::strtod(range.c_str(), NULL);
                  TF1* func = new TF1("func", "gaus", xmin, xmax);
                  func->SetLineColor(kRed);
                  func->SetLineWidth(2);
                  if (h->GetEffectiveEntries() > minstat)
                  {
                    h->Fit(func, ("rq" + fitopt).c_str());
                  }
                  delete func;
                }
              }
            }
          }
        }
      }
    }
    if (!drawopt.empty())
    {
      // do drawopt related stuff here
    }
  }

  bool HanOutputFile::drawH1(
    TCanvas* myC, TH1* h, TH1* hRef, std::string& drawopt, std::string& display, std::string& AlgoName)
  {
    formatTH1(myC, h);
    if (display.find("StatBox") != std::string::npos)
    {
      h->SetStats(kTRUE);
    }
    if (h->GetXaxis()->GetXmin() >= h->GetXaxis()->GetXmax())
    {
      std::cerr << "HanOutputFile::saveHistogramToFile(): "
                << "Inconsistent x-axis settings:  min=" << h->GetXaxis()->GetXmin() << ", "
                << "max=" << h->GetXaxis()->GetXmax() << ", "
                << "Will not save this histogram.\n";
      return false;
    }
    myC->cd();
    if (hRef != 0)
    {
      drawReference(myC, hRef, h, drawopt, display, AlgoName);
    }
    else
    {
      myC->cd();
      if (h->GetMinimum() >= 0 && h->GetMaximum() > 0.)
      {
        gPad->SetLogy(display.find("LogY") != std::string::npos);
      }
      else
      {
        gPad->SetLogy(false);
      }
      if (BINLOEDGE(h, 1) > 0)
      {
        gPad->SetLogx(display.find("LogX") != std::string::npos);
      }
      else
      {
        gPad->SetLogx(false);
      }
      axisOption(display, h);
      h->Draw(drawopt.c_str());
    }
    myC->cd();
    displayExtra(myC, display);
    myC->RedrawAxis();
    return true;
  }

  bool HanOutputFile::drawReference(
    TCanvas* myC, TH1* hRef, TH1* h, std::string& drawopt, std::string& display, std::string& AlgoName)
  {
    formatTH1(myC, hRef);
    TProfile* pRef = dynamic_cast<TProfile*>(hRef);
    if (pRef != 0)
    {  // profile reference
      hRef->SetMarkerColor(2);
      hRef->SetLineColor(2);
      hRef->SetLineWidth(2);
      double ymin = (hRef->GetMinimum() < h->GetMinimum()) ? hRef->GetMinimum() : h->GetMinimum();
      double ymax = (hRef->GetMaximum() > h->GetMaximum()) ? hRef->GetMaximum() : h->GetMaximum();
      // double xmin = ( BINLOEDGE(hRef, 1) <  BINLOEDGE(h, 1)) ?   BINLOEDGE(hRef, 1)-BINWIDTH(hRef, 1) : BINLOEDGE(h,
      // 1)-BINWIDTH(h, 1); double xmax = ( BINLOEDGE(hRef, hRef->GetNbinsX()) +  BINWIDTH(hRef, hRef->GetNbinsX()) >
      // BINLOEDGE(h, h->GetNbinsX()) +  BINWIDTH(h, h->GetNbinsX()) ) ?
      //   BINLOEDGE(hRef, hRef->GetNbinsX()) +  2.0*BINWIDTH(hRef, hRef->GetNbinsX()):  BINLOEDGE(h, h->GetNbinsX())
      //   +  2.0*BINWIDTH(h, h->GetNbinsX()) ;
      double xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1)) ? BINLOEDGE(hRef, 1) : BINLOEDGE(h, 1);
      double xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX()) >
                      BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()))
                      ? BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX())
                      : BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX());
      // 	  double y_av = (ymax + ymin)/2;
      // 	  double y_halv = (ymax-ymin)*0.6;
      bool isLogY = (display.find("LogY") != std::string::npos);
      if (isLogY)
      {
        if (ymax <= 0.0) ymax = 5.0;
        if (ymin > 0.)
        {
          double lymax = log(ymax);
          double lymin = log(ymin);
          h->SetAxisRange(exp(lymin - (lymax - lymin) * 0.05), exp(lymax + (lymax - lymin) * 0.05), "Y");
          // leave 5% gap on above and below
        }
        else
        {
          std::cerr << "ymin is <0. and LogY requested for histogram \"" << h->GetName() << " "
                    << h->GetDirectory()->GetPath() << "\", ymin=" << ymin << std::endl;
        }
      }
      else
      {
        double yMargin = (ymax - ymin) * 0.05;
        h->SetAxisRange(ymin - yMargin, ymax + yMargin, "Y");
      }
      // h->SetAxisRange(xmin,xmax,"X");
      h->GetXaxis()->SetRangeUser(xmin, xmax);
      axisOption(display, h);
      if (h->GetMinimum() >= 0 && hRef->GetMinimum() >= 0 && h->GetMaximum() > 0 && hRef->GetMaximum() > 0)
      {
        gPad->SetLogy(display.find("LogY") != std::string::npos);
      }
      if (BINLOEDGE(h, 1) > 0 && BINLOEDGE(hRef, 1) > 0)
      {
        gPad->SetLogx(display.find("LogX") != std::string::npos);
      }
      h->Draw(drawopt.c_str());
      hRef->Draw(("SAME" + drawopt).c_str());
      h->Draw(("SAME" + drawopt).c_str());
    }
    else
    {  // ordinary reference
      double scale = 1.0;
      if (display.find("ScaleRef") != std::string::npos)
      {
        scale = getScaleVal(display);
      }
      else if (h->Integral("width") > 0.0 && hRef->Integral("width") > 0.0 &&
               (AlgoName.find("BinContentComp") == std::string::npos) && (display.find("NoNorm") == std::string::npos))
      {
        scale = h->Integral("width") / hRef->Integral("width");
      }
      hRef->Scale(scale);
      hRef->SetMarkerColor(15);
      hRef->SetFillColor(15);
      hRef->SetLineColor(15);
      double ymin = (hRef->GetMinimum() < h->GetMinimum()) ? hRef->GetMinimum() : h->GetMinimum();
      double ymax = (hRef->GetMaximum() > h->GetMaximum()) ? hRef->GetMaximum() : h->GetMaximum();
      // double xmin = ( BINLOEDGE(hRef, 1) <  BINLOEDGE(h, 1)) ?   BINLOEDGE(hRef, 1)-BINWIDTH(hRef, 1) : BINLOEDGE(h,
      // 1)-BINWIDTH(h, 1); double xmax = ( BINLOEDGE(hRef, hRef->GetNbinsX()) +  BINWIDTH(hRef, hRef->GetNbinsX()) >
      // BINLOEDGE(h, h->GetNbinsX()) +  BINWIDTH(h, h->GetNbinsX()) ) ?  BINLOEDGE(hRef, hRef->GetNbinsX())
      // +  2.0*BINWIDTH(hRef, hRef->GetNbinsX()):  BINLOEDGE(h, h->GetNbinsX()) +  2.0*BINWIDTH(h, h->GetNbinsX()) ;
      double xmin = (BINLOEDGE(hRef, 1) < BINLOEDGE(h, 1)) ? BINLOEDGE(hRef, 1) : BINLOEDGE(h, 1);
      double xmax = (BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX()) >
                      BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX()))
                      ? BINLOEDGE(hRef, hRef->GetNbinsX()) + BINWIDTH(hRef, hRef->GetNbinsX())
                      : BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX());
      // 	  double y_av = (ymax + ymin)/2;
      // 	  double y_halv = (ymax-ymin)*0.6;
      bool isLogY = (display.find("LogY") != std::string::npos);
      //	  if ( ymin == 0.0 &&  display.find("LogY")!=std::string::npos ){

      if (isLogY)
      {
        if (ymax <= 0.0) ymax = 5.0;
        if (ymin > 0.)
        {
          double lymax = log(ymax);
          double lymin = log(ymin);
          h->SetAxisRange(exp(lymin - (lymax - lymin) * 0.05), exp(lymax + (lymax - lymin) * 0.05), "Y");
          // leave 5% gap on above and below
        }
        else
        {
          std::cerr << "ymin is <=0. and LogY requested for histogram \"" << h->GetName() << " "
                    << h->GetDirectory()->GetPath() << "\", ymin=" << ymin << std::endl;
        }
      }
      else
      {
        double yDiff = ymax - ymin;
        h->SetAxisRange(ymin - yDiff * 0.05, ymax + yDiff * 0.05, "Y");  // leave 5% gap above and below
      }

      // h->SetAxisRange(xmin,xmax,"X");
      h->GetXaxis()->SetRangeUser(xmin, xmax);
      myC->cd();
      if (h->GetMinimum() >= 0. && hRef->GetMinimum() >= 0. && h->GetMaximum() > 0. && hRef->GetMaximum() > 0.)
      {
        gPad->SetLogy(display.find("LogY") != std::string::npos);
      }
      if (BINLOEDGE(h, 1) > 0 && BINLOEDGE(hRef, 1) > 0)
      {
        gPad->SetLogx(display.find("LogX") != std::string::npos);
      }
      axisOption(display, h);
      h->Draw(drawopt.c_str());
      hRef->Draw(("SAME" + drawopt).c_str());
      h->Draw(("SAME" + drawopt).c_str());
    }
    return true;
  }

  // bool HanOutputFile::drawGraph(TCanvas* canv,TGraph* hist,std::string &drawopt,std::string &display){
  //   return false;
  // }

  void HanOutputFile::axisOption(std::string str, TH1* h)
  {
    std::size_t found = str.find("AxisRange");
    while (found != std::string::npos)
    {
      // std::string coordinates, cx1,cy1 ="";
      // std::size_t found1 = str.find_first_of(')',found+1);
      // std::size_t found2 = str.find_first_of("\'",found+1);
      // if (found2!=std::string::npos){
      std::string coordinates, cx1, cy1 = "";
      std::size_t found1 = str.find_first_of(')', found + 1);
      std::size_t found2 = str.find_first_of("\'", found + 1);
      if (found2 != std::string::npos)
      {
        found2 = str.find_first_of("\'", found2 + 1);
        if (found1 < found2)
        {
          found1 = str.find_first_of(')', found2 + 1);
        }
        /*      }
              if (found1!=std::string::npos){
          coordinates = str.substr(found+10,found1-found-10);
          found1 = coordinates.find_first_of(',');
          if (found1!=std::string::npos){
            cx1 = coordinates.substr(0,found1);
            double x1=std::strtod(cx1.c_str(),NULL);
            found2 =  coordinates.find_first_of(',',found1+1);
            if (found2!=std::string::npos){
              cy1 = coordinates.substr(found1+1,found2-found1-1);
              double y1=std::strtod(cy1.c_str(),NULL);
              std::string txt =  coordinates.substr(found2+2,coordinates.size() );
              txt =  txt.substr(0,txt.size()-1 );
              if (txt == "X" && x1 < y1)
                {
            h->SetAxisRange(x1,y1,"X");
                }
              if (txt == "Y" && x1 < y1)
                {
            h->SetAxisRange(x1,y1,"Y");
                }
              if (txt == "Z" && x1 < y1)
                {
            h->SetAxisRange(x1,y1,"Z");
                }
            }
          }
              }
              found=str.find("AxisRange",found+1);
            }
        */
      }
      if (found1 != std::string::npos)
      {
        coordinates = str.substr(found + 10, found1 - found - 10);
        found1 = coordinates.find_first_of(',');
        if (found1 != std::string::npos)
        {
          cx1 = coordinates.substr(0, found1);
          double x1 = std::strtod(cx1.c_str(), NULL);
          found2 = coordinates.find_first_of(',', found1 + 1);
          if (found2 != std::string::npos)
          {
            cy1 = coordinates.substr(found1 + 1, found2 - found1 - 1);
            double y1 = std::strtod(cy1.c_str(), NULL);
            std::string txt = coordinates.substr(found2 + 2, coordinates.size());
            txt.pop_back();
            if (txt == "X" && x1 < y1)
            {
              h->GetXaxis()->SetRangeUser(x1, y1);
            }
            if (txt == "Y" && x1 < y1)
            {
              h->SetAxisRange(x1, y1, "Y");
            }
            if (txt == "Z" && x1 < y1)
            {
              h->SetAxisRange(x1, y1, "Z");
            }
          }
          else
          {
            std::string txt = coordinates.substr(found1 + 2, coordinates.size());
            txt.pop_back();
            if (txt[1] == 'M')
            {
              if (txt == "XMax")
              {
                double xmin = BINLOEDGE(h, 1);
                h->GetXaxis()->SetRangeUser(xmin, x1);
              }
              if (txt == "XMin")
              {
                double xmax = BINLOEDGE(h, h->GetNbinsX()) + BINWIDTH(h, h->GetNbinsX());
                h->GetXaxis()->SetRangeUser(x1, xmax);
              }
              if (txt == "YMax")
              {
                double ymin = h->GetMinimum();
                h->SetAxisRange(ymin, x1, "Y");
              }
              if (txt == "YMin")
              {
                double ymax = h->GetMaximum();
                h->SetAxisRange(x1, ymax, "Y");
              }
            }
          }
        }
      }
      found = str.find("AxisRange", found + 1);
    }
  }

  //-----------------------------
  void HanOutputFile::ratioplot(TCanvas* myC_upperpad, TH1* h, TH1* hRef, std::string display)
  {
    // this method creates two pads under a main canvas, upperpad with input canvas displayed, lower with ratio plot
    // Then it clears the input canvas and draws this newly created in input
    // I dont know if it is the best aproach,I used this method to minimize the changes on the main code
    if (display.find("RatioPad") == std::string::npos) return;
    unsigned int ww = myC_upperpad->GetWw();
    unsigned int wh = myC_upperpad->GetWh();
    std::string padname = "PAD";
    std::string padname_ratio = "PAD_main";
    auto myC_ratiopad = std::make_unique<TCanvas>(padname_ratio.c_str(), "myC_ratiopad", ww, wh);
    auto myC_main = std::make_unique<TCanvas>(padname.c_str(), "myC_main", ww, wh);
    myC_main->cd();
    // producing ratio histogram and plotting it on to myC_ratiopad
    myC_ratiopad->cd();
    myC_ratiopad->SetTopMargin(0);
    myC_ratiopad->SetLogx(display.find("LogX") != std::string::npos);
    // TH1F *clonehist=(TH1F*)h->Clone();
    // clonehist->Divide(hRef);

    ///////
    /// Fixing Error Bars
    //////
    TProfile* phRef = dynamic_cast<TProfile*>(hRef);
    TProfile* ph = dynamic_cast<TProfile*>(h);
    TH1F* clonehist;
    TH1F* clonehistref;
    // transform if profiles
    if (ph != 0)
    {
      clonehist = (TH1F*)ph->ProjectionX();
    }
    else
    {
      clonehist = (TH1F*)h->Clone();
      clonehist->Sumw2();
    }
    if (phRef != 0)
    {
      clonehistref = (TH1F*)phRef->ProjectionX();
    }
    else
    {
      clonehistref = (TH1F*)hRef->Clone();
      clonehistref->Sumw2();
    }
    if (!clonehist or !clonehistref)
    {
      return;
    }
    clonehist->Divide(clonehistref);
    /// Error Bars fixed
    //////

    formatTH1(myC_ratiopad.get(), clonehist);
    clonehist->SetTitle("");

    // extract delta value from string that holds the draw options
    double delta = 0.75;
    if (display.find("delta(") != std::string::npos)
    {
      delta = std::stod(display.substr(display.find("delta(") + 6));
    }
    clonehist->SetAxisRange(1. - delta, 1. + delta, "Y");

    clonehist->GetYaxis()->SetNdivisions(3, true);
    clonehist->SetMarkerStyle(1);
    clonehist->Draw("E");
    clonehist->GetXaxis()->SetTitleSize(0.11);
    clonehist->GetXaxis()->SetLabelSize(0.11);
    clonehist->GetYaxis()->SetTitleSize(0.11);
    clonehist->GetYaxis()->SetLabelSize(0.11);
    myC_main->cd();
    TPad* lowerPad = new TPad("lowerPad", "lowerPad", .005, .060, .995, .250);
    lowerPad->SetTopMargin(0);
    lowerPad->SetFillStyle(0);
    lowerPad->Draw();
    TPad* upperPad = new TPad("upperPad", "upperPad", .005, .250, .995, .995);
    upperPad->SetBottomMargin(0);
    upperPad->SetFillStyle(0);
    upperPad->Draw();

    lowerPad->cd();
    myC_ratiopad->DrawClonePad();
    // Draw y=1 lineon ratio plot
    TLine* line = new TLine;
    line->SetLineColor(kRed);
    line->SetLineWidth(1);
    // method belove might be a problem when axis range changed
    double xmin = clonehist->GetXaxis()->GetXmin();
    double xmax = clonehist->GetXaxis()->GetXmax();
    // double xmin = BINLOEDGE(clonehist, 1)-BINWIDTH(clonehist, 1);
    // double xmax = BINLOEDGE(clonehist, clonehist->GetNbinsX() ) +  2.0*BINWIDTH(clonehist, clonehist->GetNbinsX() ) ;
    line->DrawLine(xmin, 1, xmax, 1);
    upperPad->cd();
    myC_upperpad->SetBottomMargin(0);
    myC_upperpad->SetFillStyle(0);
    h->GetXaxis()->SetLabelSize(0.);
    h->GetXaxis()->SetTitleSize(0.);
    myC_upperpad->DrawClonePad();
    myC_upperpad->cd();
    myC_upperpad->Clear();
    myC_main->DrawClonePad();
  }

  void HanOutputFile::ratioplot2D(TCanvas* canvas_top, TH2* h2, TH2* h2Ref, std::string display)
  {
    if (display.find("Ref2DRatio") == std::string::npos && display.find("Ref2DSignif") == std::string::npos) return;

    auto canvas_bot =
      std::make_unique<TCanvas>("canvas_bottom", "canvas_bottom", canvas_top->GetWw(), canvas_top->GetWh());
    auto canvas_all = std::make_unique<TCanvas>("canvas_all", "canvas_all", canvas_top->GetWw(), canvas_top->GetWh());

    canvas_bot->cd();
    canvas_bot->SetTopMargin(0);

    h2Ref->Scale(h2->Integral() / h2Ref->Integral());

    TH2* comparison = (TH2*)(h2->Clone());
    comparison->Divide(h2, h2Ref, 1.0, 1.0);
    comparison->SetTitle("");
    formatTH2(canvas_bot.get(), comparison);

    if (display.find("Ref2DRatio") != std::string::npos)
    {
      comparison->GetZaxis()->SetTitle("ratio to ref.");
      comparison->SetAxisRange(0.0, 2.0, "Z");
    }

    else if (display.find("Ref2DSignif") != std::string::npos)
    {
      comparison->GetZaxis()->SetTitle("difference to ref. (#sigma)");
      comparison->SetAxisRange(-4.5, 4.5, "Z");

      double value_a = 0;
      double value_b = 0;
      double sigma_a = 0;
      double sigma_b = 0;
      double signif = 0;

      for (int binx = 0; binx <= comparison->GetNbinsX(); binx++)
      {
        for (int biny = 0; biny <= comparison->GetNbinsY(); biny++)
        {
          value_a = h2->GetBinContent(binx, biny);
          value_b = h2Ref->GetBinContent(binx, biny);

          sigma_a = h2->GetBinError(binx, biny);
          sigma_b = h2Ref->GetBinError(binx, biny);

          if (sigma_a == 0 && sigma_b == 0)
            signif = 0;
          else
            signif = (value_a - value_b) / sqrt((sigma_a * sigma_a + sigma_b * sigma_b));

          comparison->SetBinContent(binx, biny, signif);
        }
      }
    }

    comparison->Draw("colz");

    canvas_all->cd();
    TPad* pad_bot = new TPad("pad_bot", "pad_bot", 0.005, 0.060, 0.995, 0.550);
    TPad* pad_top = new TPad("pad_top", "pad_top", 0.005, 0.550, 0.995, 0.995);

    pad_bot->SetTopMargin(0);
    pad_top->SetBottomMargin(0);

    pad_bot->SetFillStyle(0);
    pad_top->SetFillStyle(0);

    pad_bot->Draw();
    pad_top->Draw();

    pad_bot->cd();
    canvas_bot->DrawClonePad();

    pad_top->cd();
    canvas_top->SetBottomMargin(0);
    canvas_top->SetFillStyle(0);
    h2->GetXaxis()->SetLabelSize(0.);
    h2->GetXaxis()->SetTitleSize(0.);
    canvas_top->DrawClonePad();
    canvas_top->cd();
    canvas_top->Clear();

    canvas_all->DrawClonePad();
  }

  //-----------------------------
  void HanOutputFile::polynomial(TCanvas* c, std::string str, TH1* h)
  {
    double xmin = h->GetXaxis()->GetXmin();
    double xmax = h->GetXaxis()->GetXmax();
    std::size_t found = str.find("polynomial(");
    while (found != std::string::npos)
    {
      std::size_t endpos = str.find_first_of(')', found + 1);
      std::cout << "found;" << found << " endpos;" << endpos << "count "
                << " \n";
      std::string inp_str = str.substr(found + 11, endpos - found - 11);
      std::size_t found1 = 0;
      std::size_t found2 = inp_str.find_first_of(',', found1);
      TF1* func = new TF1("func", "pol9", xmin, xmax);
      for (int j = 0; j < 10; j++)
      {
        std::string value_str = inp_str.substr(found1, found2 - found1);
        double value_double = std::strtod(value_str.c_str(), NULL);
        func->SetParameter(j, value_double);
        if (found2 == std::string::npos)
        {
          break;
        }
        found1 = found2 + 1;
        found2 = inp_str.find_first_of(',', found1);
      }
      func->SetNpx(1000);
      c->cd();
      func->Draw("SAME");
      found = str.find("polynomial(", found + 1);
    }
  }

  void HanOutputFile::displayExtra(TCanvas* c, std::string str)
  {
    std::size_t found = str.find("TLine");
    while (found != std::string::npos)
    {
      std::size_t found1 = str.find_first_of(')', found + 1);
      if (found1 != std::string::npos)
      {
        std::string coordinates = str.substr(found + 6, found1 - found - 6);
        bool NDC = false;
        if (found1 < str.size() - 3 && str.substr(found1 + 1, 3) == "NDC")
        {
          NDC = true;
        }
        found1 = coordinates.find_first_of(',');
        if (found1 != std::string::npos)
        {
          std::string cx1 = coordinates.substr(0, found1);
          double x1 = std::strtod(cx1.c_str(), NULL);
          std::size_t found2 = coordinates.find_first_of(',', found1 + 1);
          if (found2 != std::string::npos)
          {
            std::string cy1 = coordinates.substr(found1 + 1, found2 - found1 - 1);
            double y1 = std::strtod(cy1.c_str(), NULL);
            found1 = coordinates.find_first_of(',', found2 + 1);
            if (found1 != std::string::npos)
            {
              std::string cx2 = coordinates.substr(found2 + 1, found1 - found2 - 1);
              double x2 = std::strtod(cx2.c_str(), NULL);
              std::string cy2 = coordinates.substr(found1 + 1, coordinates.size());
              double y2 = std::strtod(cy2.c_str(), NULL);
              c->cd();
              TLine* L = new TLine;
              if (NDC)
              {
                if (x1 <= 1.0 && x1 >= 0.0 && x2 <= 1.0 && x2 >= 0.0 && y1 <= 1.0 && y1 >= 0.0 && y2 <= 1.0 &&
                    y2 >= 0.0)
                {
                  L->DrawLineNDC(x1, y1, x2, y2);
                }
              }
              else
              {
                L->DrawLine(x1, y1, x2, y2);
              }
            }
          }
        }
      }
      found = str.find("TLine", found + 1);
    }

    found = str.find("TText");
    while (found != std::string::npos)
    {
      std::string coordinates, cx1, cy1 = "";
      std::size_t found1 = str.find_first_of(')', found + 1);
      std::size_t found2 = str.find_first_of('\'', found + 1);
      if (found2 != std::string::npos)
      {
        found2 = str.find_first_of('\"', found2 + 1);
        if (found2 != std::string::npos && found1 < found2)
        {
          found1 = str.find_first_of(')', found2 + 1);
        }
      }
      if (found1 != std::string::npos)
      {
        coordinates = str.substr(found + 6, found1 - found - 6);
        bool NDC = false;
        if (found1 < str.size() - 3 && str.substr(found1 + 1, 3) == "NDC")
        {
          NDC = true;
        }
        found1 = coordinates.find_first_of(',');
        if (found1 != std::string::npos)
        {
          cx1 = coordinates.substr(0, found1);
          double x1 = std::strtod(cx1.c_str(), NULL);
          found2 = coordinates.find_first_of(',', found1 + 1);
          if (found2 != std::string::npos)
          {
            cy1 = coordinates.substr(found1 + 1, found2 - found1 - 1);
            double y1 = std::strtod(cy1.c_str(), NULL);
            std::string txt = coordinates.substr(found2 + 2, coordinates.size());
            txt.pop_back();
            c->cd();
            TText* T = new TText;
            if (NDC)
            {
              if (x1 <= 1.0 && x1 >= 0.0 && y1 <= 1.0 && y1 >= 0.0)
              {
                T->DrawTextNDC(x1, y1, txt.c_str());
              }
            }
            else
            {
              T->DrawText(x1, y1, txt.c_str());
            }
          }
        }
      }
      found = str.find("TText", found + 1);
    }

    found = str.find("TDota");
    while (found != std::string::npos)
    {
      std::size_t found1 = str.find_first_of(')', found + 1);
      if (found1 != std::string::npos)
      {
        std::string coordinates = str.substr(found + 6, found1 - found - 6);
        bool NDC = false;
        if (found1 < str.size() - 3 && str.substr(found1 + 1, 3) == "NDC")
        {
          NDC = true;
        }
        found1 = coordinates.find_first_of(',');
        if (found1 != std::string::npos)
        {
          std::string cx1 = coordinates.substr(0, found1);
          double x1 = std::strtod(cx1.c_str(), NULL);
          std::size_t found2 = coordinates.find_first_of(',', found1 + 1);
          if (found2 != std::string::npos)
          {
            std::string cy1 = coordinates.substr(found1 + 1, found2 - found1 - 1);
            double y1 = std::strtod(cy1.c_str(), NULL);
            found1 = coordinates.find_first_of(',', found2 + 1);
            if (found1 != std::string::npos)
            {
              std::string cx2 = coordinates.substr(found2 + 1, found1 - found2 - 1);
              double x2 = std::strtod(cx2.c_str(), NULL);
              std::string cy2 = coordinates.substr(found1 + 1, coordinates.size());
              double y2 = std::strtod(cy2.c_str(), NULL);
              c->cd();
              TLine* L = new TLine;
              L->SetLineStyle(2);
              if (NDC)
              {
                if (x1 <= 1.0 && x1 >= 0.0 && x2 <= 1.0 && x2 >= 0.0 && y1 <= 1.0 && y1 >= 0.0 && y2 <= 1.0 &&
                    y2 >= 0.0)
                {
                  L->DrawLineNDC(x1, y1, x2, y2);
                }
              }
              else
              {
                L->DrawLine(x1, y1, x2, y2);
              }
            }
          }
        }
      }
      found = str.find("TDota", found + 1);
    }

    found = str.find("TSize");
    while (found != std::string::npos)
    {
      std::string coordinates, cx1, cy1, txtsize = "";
      std::size_t found1 = str.find_first_of(')', found + 1);
      std::size_t found2 = str.find_first_of('\'', found + 1);
      if (found2 != std::string::npos)
      {
        found2 = str.find_first_of('\"', found2 + 1);
        if (found2 != std::string::npos && found1 < found2)
        {
          found1 = str.find_first_of(')', found2 + 1);
        }
      }
      if (found1 != std::string::npos)
      {
        coordinates = str.substr(found + 6, found1 - found - 6);
        bool NDC = false;
        if (found1 < str.size() - 3 && str.substr(found1 + 1, 3) == "NDC")
        {
          NDC = true;
        }
        found1 = coordinates.find_first_of(',');
        if (found1 != std::string::npos)
        {
          cx1 = coordinates.substr(0, found1);
          double x1 = std::strtod(cx1.c_str(), NULL);
          found2 = coordinates.find_first_of(',', found1 + 1);
          if (found2 != std::string::npos)
          {
            cy1 = coordinates.substr(found1 + 1, found2 - found1 - 1);
            double y1 = std::strtod(cy1.c_str(), NULL);
            std::size_t found3 = coordinates.find_first_of(',', found2 + 1);
            if (found3 != std::string::npos)
            {
              txtsize = coordinates.substr(found2 + 1, found3 - found2 - 1);
              double size = std::strtod(txtsize.c_str(), NULL);
              std::string txt = coordinates.substr(found3 + 2, coordinates.size());
              txt.pop_back();
              c->cd();
              TText* T = new TText;
              T->SetTextSize(size / 100);
              if (NDC)
              {
                if (x1 <= 1.0 && x1 >= 0.0 && y1 <= 1.0 && y1 >= 0.0)
                {
                  T->DrawTextNDC(x1, y1, txt.c_str());
                }
              }
              else
              {
                T->DrawText(x1, y1, txt.c_str());
              }
            }
          }
        }
      }
      found = str.find("TSize", found + 1);
    }
  }

  void HanOutputFile::formatTH1(TCanvas* c, TH1* h) const
  {
    if (c == 0 || h == 0) return;

    c->SetLeftMargin(0.15);
    c->SetRightMargin(0.13);
    c->SetBottomMargin(0.15);
    c->SetTopMargin(0.12);

    h->SetStats(kFALSE);
    h->SetLabelSize(0.04, "X");
    h->SetLabelSize(0.04, "Y");
    h->SetLabelFont(62, "X");
    h->SetLabelFont(62, "Y");
    h->SetTitleSize(0.04, "X");
    h->SetTitleSize(0.04, "Y");
    h->GetXaxis()->SetTitleFont(62);
    h->GetXaxis()->SetTitleSize(0.04);
    h->GetYaxis()->SetTitleFont(62);
    h->GetYaxis()->SetTitleSize(0.04);
    h->SetMarkerStyle(20);
    h->SetMarkerSize(0.8);

    h->SetTitleOffset(1.5, "y");
    h->SetTitleOffset(0.9, "x");

    h->SetNdivisions(504, "X");
    h->SetNdivisions(504, "Y");
  }

  void HanOutputFile::formatTH2(TCanvas* c, TH2* h) const
  {
    if (c == 0 || h == 0) return;

    c->SetLeftMargin(0.15);
    c->SetRightMargin(0.13);
    c->SetBottomMargin(0.15);
    c->SetTopMargin(0.12);

    h->SetStats(kFALSE);

    h->SetLabelSize(0.04, "X");
    h->SetLabelSize(0.04, "Y");
    h->SetLabelSize(0.04, "Z");
    h->SetLabelFont(62, "X");
    h->SetLabelFont(62, "Y");
    h->SetLabelFont(62, "Z");
    h->SetTitleSize(0.04, "X");
    h->SetTitleSize(0.04, "Y");
    h->SetTitleSize(0.04, "Z");
    h->GetXaxis()->SetTitleFont(62);
    h->GetXaxis()->SetTitleSize(0.04);
    h->GetYaxis()->SetTitleFont(62);
    h->GetYaxis()->SetTitleSize(0.04);

    h->SetTitleOffset(1.5, "y");
    h->SetTitleOffset(0.9, "x");

    h->SetNdivisions(504, "X");
    h->SetNdivisions(504, "Y");
  }

  void HanOutputFile::formatTGraph(TCanvas* c, TGraph* g) const
  {
    if (c == 0 || g == 0) return;

    c->SetLeftMargin(0.15);
    c->SetRightMargin(0.13);
    c->SetBottomMargin(0.15);
    c->SetTopMargin(0.12);

    g->SetMarkerStyle(20);
  }

  void HanOutputFile::formatTEfficiency(TCanvas* c, TEfficiency* e) const
  {
    if (c == 0 || e == 0) return;
    c->SetLeftMargin(0.15);
    c->SetRightMargin(0.13);
    c->SetBottomMargin(0.15);
    c->SetTopMargin(0.12);
  }

  // *********************************************************************
  // Protected Methods
  // *********************************************************************

  void HanOutputFile::clearData()
  {
    dqi::DisableMustClean disabled;
    //   bool useRecursiveDelete = gROOT->MustClean();
    //   gROOT->SetMustClean(false);

    delete m_file;
    delete m_style;
    m_file = 0;
    m_style = 0;
    m_indirMap.clear();

    DirToAssMap_t::const_iterator assessMapEnd = m_assessMap.end();
    for (DirToAssMap_t::const_iterator i = m_assessMap.begin(); i != assessMapEnd; ++i)
    {
      delete i->second;
    }
    m_assessMap.clear();

    //   gROOT->SetMustClean(useRecursiveDelete);
  }

  bool HanOutputFile::writeToFile(std::string fname, std::string content)
  {
    std::ofstream outfile(fname);
    if (!outfile.is_open())
    {
      std::cerr << "Error writing file to " << fname << std::endl;
      return false;
    }
    outfile << content;
    outfile.close();
    return true;
  }

  void HanOutputFile::convertToGraphics(int cnvsType, TCanvas* myC, std::string& json, TImage* img, char** x, int* y)
  {
    if (cnvsType & GENERATE_PNG)
    {
      if (img) getImageBuffer(img, myC, x, y);
    }
    if (cnvsType & GENERATE_JSON)
    {
      json = TBufferJSON::ConvertToJSON(myC);
    }
  }

  void HanOutputFile::convertToGraphics(int cnvsType, TCanvas* myC, std::string namePNG, std::string nameJSON)
  {
    if (cnvsType & GENERATE_PNG)
    {
      myC->SaveAs(namePNG.c_str());
    }
    if (cnvsType & GENERATE_JSON)
    {
      std::string json = std::string(TBufferJSON::ConvertToJSON(myC));
      writeToFile(nameJSON, json);
    }
  }

  bool HanOutputFile::saveFile(
    int cnvsType, std::string pngfName, std::string pngContent, std::string jsonfName, std::string jsonfContent)
  {
    bool png = false;
    bool json = false;
    if (cnvsType & GENERATE_PNG)
    {
      png = writeToFile(pngfName, pngContent);
    }
    if (cnvsType & GENERATE_JSON)
    {
      json = writeToFile(jsonfName, jsonfContent);
    }
    return (png || json);
  }

}  // namespace dqutils
