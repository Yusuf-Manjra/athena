/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCL1RAWDATA_H
#define MUONRDO_TGCL1RAWDATA_H


#include <inttypes.h>
#include <ostream>

class MsgStream;

/**
  An unit object of TGC SROD output.
  This class is supposed to hold Coincidence-information.
  For this purpose, two full-constructors have been prepared.
  @ author Toshi Sumida
*/
class TgcL1RawData
{
public:
    friend class TgcL1RawDataCnv_p1;

    enum DataType
    {
        TYPE_HIPT,
        TYPE_NSL_ROI,
        TYPE_NSL_NSW,
        TYPE_NSL_RPC,
        TYPE_NSL_EIFI,
        TYPE_NSL_TMDB,
        TYPE_UNKNOWN
    };

    // Default constructor
    TgcL1RawData()
    {
        clear(0, 0, 0, 0, 0);
    }

    // P1 ------------------------
    // Constructor for Trigger Coincidence
    TgcL1RawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t srodId,
               uint16_t l1Id,
               uint16_t bcId,
               uint16_t type,
               uint16_t index,
               uint16_t pos,
               int16_t delta);

    // P2 ------------------------
    // High pT
    TgcL1RawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t srodId,
               uint16_t l1Id,
               uint16_t bcId,
               bool strip,
               bool forward,
               uint16_t sector,
               uint16_t chip,
               uint16_t index,
               bool hipt,
               uint16_t hitId,
               uint16_t sub,
               int16_t delta,
               uint16_t inner);

    // New Sector logic
    // RoI
    TgcL1RawData(uint16_t bcTag,
                 uint16_t subDetectorId,
                 uint16_t srodId,
                 uint16_t l1Id,
                 uint16_t bcId,
                 bool forward,
                 uint16_t sector,
                 uint16_t innerflag,
                 uint16_t coinflag,
                 bool muplus,
                 uint16_t threshold,
                 uint16_t roi);

    // NSW
    TgcL1RawData(uint16_t bcTag,
                 uint16_t subDetectorId,
                 uint16_t srodId,
                 uint16_t l1Id,
                 uint16_t bcId,
                 bool forward,
                 uint16_t sector,
                 uint16_t nsweta,
                 uint16_t nswphi,
                 uint16_t nswcand,
                 uint16_t nswdtheta,
                 uint16_t nswphires,
                 uint16_t nswlowres,
                 uint16_t nswid);

// RPC BIS78
    TgcL1RawData(uint16_t bcTag,
                 uint16_t subDetectorId,
                 uint16_t srodId,
                 uint16_t l1Id,
                 uint16_t bcId,
                 bool forward,
                 uint16_t sector,
                 uint16_t rpceta,
                 uint16_t rpcphi,
                 uint16_t rpcflag,
                 uint16_t rpcdeta,
                 uint16_t rpcdphi);

// EIFI
      TgcL1RawData(uint16_t bcTag,
                   uint16_t subDetectorId,
                   uint16_t srodId,
                   uint16_t l1Id,
                   uint16_t bcId,
                   bool forward,
                   uint16_t sector,
                   uint16_t ei,
                   uint16_t fi,
                   uint16_t cid); // chamber ID

// TMDB
      TgcL1RawData(uint16_t bcTag,
                   uint16_t subDetectorId,
                   uint16_t srodId,
                   uint16_t l1Id,
                   uint16_t bcId,
                   bool forward,
                   uint16_t sector,
                   uint16_t mod,
                   uint16_t bcid);

    // Destructor
    virtual ~TgcL1RawData() = default;
    // return Information Type Hit:Coincidence
    bool isCoincidence() const
    {
        return m_type == TYPE_HIPT || m_type == TYPE_NSL_ROI;
    }

    /*********** common accessors
     */
    void setOnlineId(uint16_t sId, uint16_t rId)
    {
        m_subDetectorId = sId;
        m_srodId = rId;
    }

    uint16_t bcTag() const
    {
        return m_bcTag;
    }
    uint16_t l1Id() const
    {
        return m_l1Id;
    }
    uint16_t bcId()  const
    {
        return m_bcId;
    }

    uint16_t subDetectorId() const
    {
        return m_subDetectorId;
    }
    uint16_t srodId() const
    {
        return m_srodId;
    }
    DataType type() const
    {
        return m_type;
    }
    void setType(uint16_t type);
    bool isForward() const
    {
        return m_forward;
    }
    uint16_t bitpos() const
    {
        return m_bitpos;
    }
    uint16_t channel() const
    {
        return m_bitpos;
    }
    uint16_t tracklet() const
    {
        return m_tracklet;
    }
    bool isAdjacent() const
    {
        return m_adjacent;
    }
    uint16_t index() const
    {
        return m_index;
    }
    uint16_t position() const
    {
        return m_pos;
    }
    int16_t delta() const
    {
        return m_delta;
    }
    uint16_t inner() const
    {
        return m_inner;
    }
    uint16_t segment() const
    {
        return m_segment;
    }
    uint16_t subMatrix() const
    {
        return m_subMatrix;
    }
    uint16_t sector() const
    {
        return m_sector;
    }
    uint16_t chip() const
    {
        return m_chip;
    }
    bool isHipt() const
    {
        return m_hipt;
    }
    uint16_t hitId() const
    {
        return m_hitId;
    }
    uint16_t hsub() const
    {
        return m_hsub;
    }
    bool isStrip() const
    {
        return m_strip;
    }
    bool cand3plus() const
    {
        return m_cand3plus;
    }
    bool isMuplus() const
    {
        return m_muplus;
    }
    uint16_t threshold() const
    {
        return m_threshold;
    }
    bool isOverlap() const
    {
        return m_overlap;
    }
    bool isVeto() const
    {
        return m_veto;
    }
    uint16_t roi() const
    {
        return m_roi;
    }

    static std::string typeName(DataType type);

    std::string typeName() const
    {
        return typeName(m_type);
    }

private:
    void clear(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t srodId,
               uint16_t l1Id,
               uint16_t bcId);

    /********** common attributes
     */

    // BC tag (Central, Next, Previous)
    uint16_t m_bcTag;

    // online Identifiers
    uint16_t m_subDetectorId;
    uint16_t m_srodId;

    // BCID and L1ID on SLB
    uint16_t m_l1Id;
    uint16_t m_bcId;

    /********** Hit attributes
     */

    // bitmap position [40..199]
    uint16_t m_bitpos;

    // Index of assocaitated coincidence (if any) [0..63]
    uint16_t m_tracklet;

    bool m_adjacent;

    /********** Coincidence attributes
     */

    //  Coincidence type (L-Pt, H-Pt, SL)
    DataType m_type;
    bool m_forward;

    // Coincidence index
    uint16_t m_index;

    // Coincidence Data
    uint16_t m_pos;
    int16_t m_delta;
    uint16_t m_segment;
    uint16_t m_subMatrix;

    // HipT data
    uint16_t m_sector;
    uint16_t m_chip;
    bool m_hipt;
    uint16_t m_hitId;
    uint16_t m_hsub;
    bool m_strip;
    uint16_t m_inner;

    // New Sector Logic
    // RoI
    bool m_cand3plus;
    bool m_muplus;
    uint16_t m_threshold;
    bool m_overlap;
    bool m_veto;
    uint16_t m_roi;
    uint16_t m_innerflag;
    uint16_t m_coinflag;

    // NSW
    uint16_t m_nsweta;
    uint16_t m_nswphi;
    uint16_t m_nswsl;
    uint16_t m_nswcand;
    uint16_t m_nswdtheta;
    uint16_t m_nswphires;
    uint16_t m_nswlowres;
    uint16_t m_nswid;
    
    // RPC BIS78
    uint16_t m_rpceta;
    uint16_t m_rpcphi;
    uint16_t m_rpcflag;
    uint16_t m_rpcdeta;
    uint16_t m_rpcdphi;
    
    // EIFI
    uint16_t m_ei;
    uint16_t m_fi;
    uint16_t m_cid;

    // TMDB
    uint16_t m_tmdbmod;
    uint16_t m_tmdbbcid;
    
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator<<(MsgStream& sl, const TgcL1RawData& coll);

/**Overload of << operator for std::ostream for debug output*/
std::ostream& operator<<(std::ostream& sl, const TgcL1RawData& coll);

#endif

