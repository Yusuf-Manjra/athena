//
//  SingleHistogramDefinition.cpp
//  HDef
//
//  Created by sroe on 13/07/2015.
//  Copyright (c) 2015 sroe. All rights reserved.
//

#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <utility>
#include <limits>
SingleHistogramDefinition::SingleHistogramDefinition():
    name{},
    histoType{},
    title{},
    nBinsX{},
    nBinsY{},
    xAxis(std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN())),
    yAxis(std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN())),
    xTitle{},
    yTitle{},
    allTitles{},
    folder{},
    m_empty(true){
}

SingleHistogramDefinition::SingleHistogramDefinition(Titles_t thename, Titles_t thehistoType, Titles_t thetitle, NBins_t nbinsX,Var_t xLo, Var_t xHi,Titles_t xName, Titles_t yName,Titles_t thefolder):name(thename),histoType(thehistoType),title(thetitle),nBinsX(nbinsX),nBinsY(0),xTitle(xName),yTitle(yName),folder(thefolder){
    //should do sanity checks here
    xAxis=std::make_pair(xLo,xHi);
    yAxis=std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN());
    m_empty=false;
    const std::string s(";");
    allTitles=title+s+xTitle+s+yTitle;
}
SingleHistogramDefinition::SingleHistogramDefinition(Titles_t thename, Titles_t thehistoType, Titles_t thetitle, NBins_t nbinsX,  NBins_t nbinsY,Var_t xLo, Var_t xHi, Var_t yLo, Var_t yHi, Titles_t xName, Titles_t yName,Titles_t thefolder):name(thename),histoType(thehistoType),title(thetitle),nBinsX(nbinsX),nBinsY(nbinsY),xTitle(xName),yTitle(yName),folder(thefolder){
    //should do sanity checks here
    xAxis=std::make_pair(xLo,xHi);
    yAxis=std::make_pair(yLo,yHi);
    m_empty=false;
    const std::string s(";");
    allTitles=title+s+xTitle+s+yTitle;
}
bool SingleHistogramDefinition::empty() const {
    return m_empty;
}
std::string SingleHistogramDefinition::str() const {
    const std::string s(" ");
    return name + s + histoType+s+title+s+std::to_string(nBinsX)+s+std::to_string(nBinsY)+
      s+std::to_string(xAxis.first)+s+std::to_string(xAxis.second)+s+
      std::to_string(yAxis.first)+s+std::to_string(yAxis.second)+s+xTitle+s+yTitle;
}
bool SingleHistogramDefinition::validType() const{
    const std::string signature((histoType.substr(0,3)));
    return ((signature=="TH1") or (signature=="TH2") or (signature=="TPr"));
}

std::string SingleHistogramDefinition::stringIndex() const {
    return stringIndex(name,folder);
}

std::string SingleHistogramDefinition::stringIndex(const std::string & thisname, const std::string & thisfolder){
    if (thisfolder.empty()) return thisname;
    const std::string delimiter("/");
    std::string result(thisfolder);
    if ((thisfolder.substr(0,1)) == delimiter) result = thisfolder.substr(1,thisfolder.size()-2);// reduce "/myfolder" to "myfolder"
    size_t lastChar(result.size() - 1);
    if ((result.substr(lastChar,1) != delimiter)) result=result+delimiter; //add a slash: "myfolder" => "myfolder/"
    return result+thisname;
}

