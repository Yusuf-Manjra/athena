/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetector_H
#define AGDDDetector_H

#include "AGDDKernel/AGDDDetectorPositioner.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDDetector {
public:
        AGDDDetector(std::string s):m_small_x(0),m_large_x(0),m_y(0),m_z(0),m_name(s) {}
        AGDDDetector(std::string s,std::string t):m_small_x(0),m_large_x(0),m_y(0),m_z(0),m_detectorType(t),m_name(s) {}
	virtual void SetXYZ(std::vector<double> v) 
	{
		m_small_x=v[0];
		m_large_x=v[1];
		m_y=v[2];
		m_z=v[3];
	}
	
	void small_x(double x) {m_small_x=x;}
	void large_x(double x) {m_large_x=x;}
	void y(double yval) {m_y=yval;}
	void z(double zval) {m_z=zval;}
        void subType(std::string s) {m_sType=s;}
	double small_x() {return m_small_x;}
	double large_x() {return m_large_x;}
	double y() {return m_y;}
	double z() {return m_z;}
	std::string subType() {return m_sType;}
	std::string tech;
	
	std::string GetName() {return m_name;}
	
	std::string DetectorType() {return m_detectorType;}
	std::string DetectorID() {return m_detectorID;}
	
	void SetAddressAndPosition(AGDDDetectorPositioner*) ;
	
	std::vector<AGDDDetectorPositioner*>& GetDetectorPositioners();
	
protected:
	double m_small_x;
	double m_large_x;
	double m_y;
	double m_z;
	
	// detector Type to avoid dynamic casting
	std::string m_detectorType;
	std::string m_detectorID;

	std::string m_sType;
	
	GeoMaterial* GetMMMaterial(std::string);
	
	void AddDetectorPosition(AGDDDetectorPositioner* p);

	virtual void SetDetectorAddress(AGDDDetectorPositioner*) =0;
	std::vector<AGDDDetectorPositioner*> m_detectorPositions;
	
	std::string m_name;
};

#endif
