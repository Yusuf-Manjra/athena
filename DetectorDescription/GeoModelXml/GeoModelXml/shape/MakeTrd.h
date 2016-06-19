/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Automatically generated from scripts in /home/hessey/prog/gmx2geo/makeshape
//    Handle tube shape elements.
//
#ifndef GEO_MODEL_XML_MAKE_TRD_H
#define GEO_MODEL_XML_MAKE_TRD_H

#include "GeoModelXml/Element2GeoItem.h"

class MakeTrd: public Element2GeoItem {
public:
    MakeTrd();
    const RCBase * make(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // GEO_MODEL_XML_MAKE_TRD_H
