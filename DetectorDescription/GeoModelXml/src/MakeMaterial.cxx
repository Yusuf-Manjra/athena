/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeMaterial.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"
#include <CLHEP/Units/SystemOfUnits.h>

using namespace xercesc;
using namespace std;
using namespace CLHEP;

MakeMaterial::MakeMaterial() {}

const RCBase * MakeMaterial::make(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil) const {
char *name;
char *density;
char *densitySF;
double rho;
char *fracString;
double fraction;
char *qString;
const XMLCh *ref = translate("ref");
const XMLCh *idref;
DOMDocument *doc = element->getOwnerDocument();
char *toRelease;
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
//
//    Get material density scale-factor for the block of materials this one is in 
//
    DOMNode *parent = element->getParentNode();
    if (XMLString::compareIString(parent->getNodeName(), translate("materials")) != 0) {
        log << MSG::FATAL << "Asked to make a material for non-material element. Parent element was " << 
                             translate(parent->getNodeName()) << "; error in gmx file; exiting" << endmsg;
        exit(1);
    }
    double scaleFactor(1.0);
    DOMElement *el = dynamic_cast<DOMElement *> (parent);
//    if (el->hasAttribute(translate("densitysf"))) { // Guaranteed by DTD; don't recheck.
        densitySF = translate(el->getAttribute(translate("densitysf")));
        scaleFactor = gmxUtil.evaluate(densitySF);
//    }
//
//    Get my name
//
    name = translate(element->getAttribute(translate("name")));
//
//   Get my density
//
    density = translate(element->getAttribute(translate("density")));
    rho = gmxUtil.evaluate(density) * scaleFactor;
    XMLString::release(&density);
//
//    Create it
//
    GeoMaterial *material = new GeoMaterial(name, rho * g/cm3);
    XMLString::release(&name);
//
//   Add my element contents
//
    DOMNodeList *elRefs = element->getElementsByTagName(translate("elementref"));
    int nElRefs = elRefs->getLength();
    for (int i = 0; i < nElRefs; ++i) {
        DOMElement *elRef = dynamic_cast<DOMElement *>(elRefs->item(i));
        idref = elRef->getAttribute(ref);
        DOMElement *elem = doc->getElementById(idref);

        toRelease = translate(elem->getNodeName());
        string nodeName(toRelease);
        XMLString::release(&toRelease);
        if (nodeName != string("element")) {
            log << MSG::FATAL << "Error in xml/gmx file: An elementref referenced a " << nodeName << " instead of an element." 
                              << endmsg;
            exit(999); // Should do better...
        }

        GeoElement *geoElem = (GeoElement *) gmxUtil.tagHandler.element.process(elem, gmxUtil);

        fracString = translate(elRef->getAttribute(translate("fraction")));
        fraction = gmxUtil.evaluate(fracString);
        XMLString::release(&fracString);
        material->add(geoElem, fraction);
    }
//
//   Add my chemical contents
//
    DOMNodeList *chemRefs = element->getElementsByTagName(translate("chemicalref"));
    int nChemRefs = chemRefs->getLength();
    for (int i = 0; i < nChemRefs; ++i) {
        DOMElement *chemRef = dynamic_cast<DOMElement *>(chemRefs->item(i));
        idref = chemRef->getAttribute(ref);
        DOMElement *chem = doc->getElementById(idref);

        toRelease = translate(chem->getNodeName());
        string nodeName(toRelease);
        XMLString::release(&toRelease);
        if (nodeName != string("chemical")) {
            log << MSG::FATAL << "Error in xml/gmx file: A chemref referenced a " << nodeName << " instead of a chemical." << 
                                  endmsg;
            exit(999); // Should do better...
        }

        fracString = translate(chemRef->getAttribute(translate("fraction")));
        fraction = gmxUtil.evaluate(fracString);
        XMLString::release(&fracString);

        // Loop over chemical contents, adding each element to this material
        DOMNodeList *chemEls = chem->getElementsByTagName(translate("elemcontent"));
        int nChemEls = chemEls->getLength();
        vector<GeoElement *> geoElem;
        vector<double> atomicWeight;
        double molWeight = 0.0;
        vector<double> formula;
        for (int i = 0; i < nChemEls; ++i) {
            DOMElement *chemEl = dynamic_cast<DOMElement *>(chemEls->item(i));
            idref = chemEl->getAttribute(ref);
            DOMElement *elem = doc->getElementById(idref);

            toRelease = translate(elem->getNodeName());
            string nodeName(toRelease);
            XMLString::release(&toRelease);
            if (nodeName != string("element")) {
                log << MSG::FATAL << 
                       "Error in xml/gmx file: An elementref referenced a " << nodeName << " instead of an element." << endmsg;
                exit(999); // Should do better...
            }

            geoElem.push_back((GeoElement *) gmxUtil.tagHandler.element.process(elem, gmxUtil));
            atomicWeight.push_back(geoElem.back()->getA());

            qString = translate(chemEl->getAttribute(translate("quantity")));
            formula.push_back(gmxUtil.evaluate(qString));
            XMLString::release(&qString);

            molWeight += atomicWeight.back() * formula.back();
        }
        for (int i = 0; i < nChemEls; ++i) {
            material->add(geoElem[i], fraction * formula[i] * atomicWeight[i] / molWeight);
        }
    }

//
//   Add my material contents
//
    elRefs = element->getElementsByTagName(translate("materialref"));
    nElRefs = elRefs->getLength();
    for (int i = 0; i < nElRefs; ++i) {
        DOMElement *elRef = dynamic_cast<DOMElement *>(elRefs->item(i));
        idref = elRef->getAttribute(ref);
        DOMElement *elem = doc->getElementById(idref);

        toRelease = translate(elem->getNodeName());
        string nodeName(toRelease);
        XMLString::release(&toRelease);
        if (nodeName != string("material")) {
            log << MSG::FATAL << 
                   "Error in xml/gmx file: A materialref referenced a " << nodeName << " instead of a material." << endmsg;
            exit(999); // Should do better...
        }

        GeoMaterial *geoMaterial = (GeoMaterial *) gmxUtil.tagHandler.material.process(elem, gmxUtil);

        fracString = translate(elRef->getAttribute(translate("fraction")));
        fraction = gmxUtil.evaluate(fracString);
        XMLString::release(&fracString);
        material->add(geoMaterial, fraction);
    }

    material->lock(); // Calculate my params and prevent modification

    return (const RCBase *) material;
}
