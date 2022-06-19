/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_BASICCELLBUILDERTOOL_H
#define FASTCALOSIM_BASICCELLBUILDERTOOL_H

/**
 *  @file   CellBuilderTool.cxx
 *  @brief  Building Cells objects from Atlfast
 *  @author Michael Duehrssen
 */

#include "GaudiKernel/AlgTool.h"

#include <math.h>

#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "FastCaloSim/FSmap.h"

class CaloCellContainer ;
class CaloDetDescrManager ;
class CaloCellContainer ;
class AtlasDetectorID ;

class cellinfo_map
{
public:
  typedef std::pair<const CaloDetDescrElement*,float> cellinfo;
  //  typedef std::vector<cellinfo> cellinfo_vec;
  class cellinfo_vec {
  public:
    typedef std::vector<cellinfo> vec;

    cellinfo_vec():m_vol(0) {m_subvol[0]=m_subvol[1]=m_subvol[2]=m_subvol[3]=0;};

    cellinfo& operator[](unsigned int i) {return m_cellinfo_vec[i];};
    const cellinfo& operator[](unsigned int i) const {return m_cellinfo_vec[i];};
    void resize(unsigned int n) {m_cellinfo_vec.resize(n);};
    unsigned int size() const {return m_cellinfo_vec.size();};
    void push_back(const cellinfo & ele,int layernr) {m_cellinfo_vec.push_back(ele);m_vol+=ele.second;m_subvol[layernr]+=ele.second;};
    double vol() const {return m_vol;};
    double vol(int i) const {return m_subvol[i];};

    vec m_cellinfo_vec;
    double m_vol;
    double m_subvol[4];
  };
  typedef std::vector< std::vector<cellinfo_vec> > map_type;

  cellinfo_map(int p_neta=0, int p_nphi=0):
    m_eta_start(-5), m_eta_end(5), m_phi_start(-M_PI), m_phi_end(M_PI), m_neta(p_neta), m_nphi(p_nphi), m_leta(1), m_lphi(1) {

    if(m_neta>0) {
      m_deta=(m_eta_end-m_eta_start)/m_neta;
    }
    else m_deta = 0;
    if(m_nphi>0) {
      m_dphi=(m_phi_end-m_phi_start)/m_nphi;
    }
    else m_dphi = 0;
    m_map.resize(m_neta);
    for(unsigned int i=0;i<m_neta;++i) m_map[i].resize(m_nphi);
  }

  cellinfo_map(double p_eta_start, double p_eta_end, double p_phi_start, double p_phi_end, int p_neta=100, int p_nphi=64, int p_leta=1, int p_lphi=1):
    m_eta_start(p_eta_start), m_eta_end(p_eta_end), m_phi_start(p_phi_start), m_phi_end(p_phi_end), m_neta(p_neta), m_nphi(p_nphi), m_leta(p_leta), m_lphi(p_lphi) {
    if(m_neta>0) m_deta=(m_eta_end-m_eta_start)/m_neta;
    if(m_nphi>0) m_dphi=(m_phi_end-m_phi_start)/m_nphi;

    m_map.resize(m_neta);
    for(unsigned int i=0;i<m_neta;++i) m_map[i].resize(m_nphi);
  }

  void init(double p_eta_start=-5.0, double p_eta_end=+5.0, double p_phi_start=-M_PI, double p_phi_end=-M_PI, int p_neta=100, int p_nphi=64, int p_leta=1, int p_lphi=1) {
    m_eta_start=p_eta_start;
    m_eta_end=p_eta_end;
    m_phi_start=p_phi_start;
    m_phi_end=p_phi_end;
    m_neta=p_neta;
    m_nphi=p_nphi;
    m_leta=p_leta;
    m_lphi=p_lphi;
    if(m_neta>0) m_deta=(m_eta_end-m_eta_start)/m_neta;
    if(m_nphi>0) m_dphi=(m_phi_end-m_phi_start)/m_nphi;

    m_map.resize(m_neta);
    for(unsigned int i=0;i<m_neta;++i) m_map[i].resize(m_nphi);
  }

  double eta_start() const {return m_eta_start;};
  double eta_end() const {return m_eta_end;};
  double phi_start() const {return m_eta_start;};
  double phi_end() const {return m_eta_end;};
  double deta() const {return m_deta;};
  double dphi() const {return m_dphi;};
  unsigned int neta() const {return m_neta;};
  unsigned int nphi() const {return m_nphi;};
  unsigned int leta() const {return m_leta;};
  unsigned int lphi() const {return m_lphi;};

  double index_to_eta_center(const int i) const {return m_eta_start+(i+0.5)*m_deta;};
  double index_to_phi_center(const int i) const {return m_phi_start+(i+0.5)*m_dphi;};

  int    eta_to_index(const double eta) const {return (int)floor( (eta-m_eta_start)/m_deta );};
  int    phi_to_index_cont(const double phi) const {
    return (int)floor( (phi-m_phi_start)/m_dphi );
  };
  int    phi_to_index(const double phi) const {
    int iphi=phi_to_index_cont(phi);
    if(iphi<0) iphi+=m_nphi;
    if(iphi>=(int)m_nphi) iphi-=m_nphi;
    return iphi;
  };

  void eta_range(const int ieta,double& eta1,double& eta2) { eta1=m_eta_start+ieta*m_deta; eta2=eta1+m_leta*m_deta; };
  void phi_range(const int iphi,double& phi1,double& phi2) { phi1=m_phi_start+iphi*m_dphi; phi2=phi1+m_lphi*m_dphi; };

  cellinfo_vec& vec(int ieta,int iphi) {return m_map[ieta][iphi];};
  const cellinfo_vec& vec(int ieta,int iphi) const {return m_map[ieta][iphi];};

  const std::string& name() const {return m_name;};
  void setname(const std::string& name) {m_name=name;};

private:
  double m_eta_start;
  double m_eta_end;
  double m_phi_start;
  double m_phi_end;
  unsigned int m_neta;
  unsigned int m_nphi;
  unsigned int m_leta;
  unsigned int m_lphi;
  double m_deta;
  double m_dphi;

  std::string m_name;

  map_type m_map;
};

class CellInfoContainer
{
 public:
  CellInfoContainer() = default;
  ~CellInfoContainer() = default;

  friend class BasicCellBuilderTool;
  friend class CellInfoContainerCondAlg;
  friend class FastShowerCellBuilderTool;

  inline const cellinfo_map& getCellistMap(int sample) const {return m_celllist_maps[sample];}
  inline const cellinfo_map& getEmCellistMap() const {return m_em_celllist_map;}
  inline const cellinfo_map& getEmMap() const {return m_em_map;}
  inline const cellinfo_map& getEmFineMap() const {return m_em_fine_map;}
  inline const cellinfo_map& getHadMap() const {return m_had_map;}

 protected:
  inline cellinfo_map& getCellistMap(int sample) {return m_celllist_maps[sample];}
  inline cellinfo_map& getEmCellistMap() {return m_em_celllist_map;}
  inline cellinfo_map& getEmMap() {return m_em_map;}
  inline cellinfo_map& getEmFineMap() {return m_em_fine_map;}
  inline cellinfo_map& getHadMap() {return m_had_map;}

 private:
  cellinfo_map m_celllist_maps[CaloCell_ID_FCS::MaxSample];
  cellinfo_map m_em_celllist_map;
  cellinfo_map m_em_map;
  cellinfo_map m_em_fine_map;
  cellinfo_map m_had_map;
};

class BasicCellBuilderTool: public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  //  typedef std::vector< std::vector<cellinfo_vec> > cellinfo_map;
  typedef cellinfo_map::cellinfo cellinfo;
  typedef cellinfo_map::cellinfo_vec cellinfo_vec;

  BasicCellBuilderTool(
                       const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~BasicCellBuilderTool();


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

protected:
  void init_all_maps(const CaloDetDescrManager* caloDDM);
  void init_cell(cellinfo_map& map,const CaloDetDescrElement* theDDE);
  void init_volume(cellinfo_map& map);
  void find_phi0(const CaloDetDescrManager* caloDDM);

  //  void init_map(cellinfo_map& map, int layer); //layer 1=EM 3=HAD
  void findCells(const CaloDetDescrManager* caloDDM,
                 cellinfo_vec & cell_vec, double eta_min, double eta_max, double phi_min, double phi_max, int layer);


  void addCell(CaloCellContainer * theCellContainer, int etabin, int phibin, double energy, cellinfo_map& map );

  Identifier m_id;
  double m_phi0_em;
  double m_phi0_had;

  CellInfoContainer m_cellinfoCont;

  double                   m_min_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  double                   m_max_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_rmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_rent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]


  double deta(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  void   minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta) const;
  double rzmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rzent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double zmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double zent(CaloCell_ID_FCS::CaloSample sample,double eta) const;

  const CaloCell_ID*  m_caloCID;

  bool m_isCaloBarrel[CaloCell_ID_FCS::MaxSample];
  bool isCaloBarrel(CaloCell_ID_FCS::CaloSample sample) const {return m_isCaloBarrel[sample];};
};

#endif
