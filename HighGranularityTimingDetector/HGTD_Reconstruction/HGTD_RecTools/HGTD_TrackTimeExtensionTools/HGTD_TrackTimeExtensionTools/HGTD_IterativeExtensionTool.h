/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TrackTimeExtensionTools/HGTD_IterativeExtensionTool.h
 *
 * @author Noemi Calace <noemi.calace@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date August, 2021
 *
 * @brief The HGTD_IterativeExtensionTool uses an iterative Kalman filter
 * approach to find hits (HGTD_Cluster) in HGTD that are compatible with a track
 * extrapolated to the surfaces of HGTD. During this process, the track itself
 * is not altered, the found hits are returned as an array of
 * HGTD_ClusterOnTrack objects.
 *
 * TODO:
 *  - exchange chi2/ndof with TMath::Prob?
 *  - add chi2 for the time -> get an estimate at layer 0 (TOF +- beamspot),
 *    decorate the TrackParameters with time and timeres before!
 */

#ifndef HGTD_ITERATIVEEXTENTSIONTOOL_H
#define HGTD_ITERATIVEEXTENTSIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "HGTD_RecToolInterfaces/IHGTD_TrackTimeExtensionTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RecToolInterfaces/IHGTD_TOFcorrectionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"

#include <memory>

class HGTD_ID;
class HGTD_DetectorManager;

class HGTD_IterativeExtensionTool : virtual public IHGTD_TrackTimeExtensionTool,
                                    public AthAlgTool {

public:
  HGTD_IterativeExtensionTool(const std::string&, const std::string&,
                              const IInterface*);

  virtual ~HGTD_IterativeExtensionTool() = default;

  virtual StatusCode initialize();

  /**
   * @brief Finds the (up to) four measurements in HGTD that can be associated
   * to the track. The Track itself is not altered during this process.
   * Since HGTD contains four active layers per endcap, four hits is the
   * maximum. An extension can be of hole-type
   *
   * @param [in] track Track reconstructed in ITk.
   *
   * @return Array of compatible HGTD hits in the form of HGTD_ClusterOnTrack.
   */
  virtual std::array<std::unique_ptr<const Trk::TrackStateOnSurface>, 4>
  extendTrackToHGTD(const Trk::Track& track,
                    const HGTD_ClusterContainer* container) override;

  MsgStream& dump(MsgStream& out) const;
  std::ostream& dump(std::ostream& out) const;

private:
  /**
   * @brief  Retrieve the last hit on track stored in the Trk::Track.
   *
   * @param [in] track Track reconstructed in ITk.
   *
   * @return The last hit on track, i.e. the closest one to HGTD.
   *
   * FIXME: returns a const*, but the Track manages this TSOS. Could I return
   * rather const&? But: I want to check if type is OK... should I return an
   * "empty" TSOS if nothing passes the requirements?
   */
  const Trk::TrackStateOnSurface* getLastHitOnTrack(const Trk::Track& track);

  /**
   * @brief  Select all within the vincinity of the point of extrapolation.
   *
   * @param [in] param The already extrapolated track parameter.
   * @param [in] layer Target layer of the extrapolation.
   *
   * @return Vector of compatible surfaces that represent modules.
   *
   * FIXME exchange the loop with the neighbour functionality (wherever that is)
   */
  std::vector<const Trk::Surface*>
  getCompatibleSurfaces(const Trk::TrackParameters& param,
                        const Trk::Layer* layer);

  /**
   * @brief After the compatible surfaces have been selected, the extrapolation
   * is repeated to each of them.
   *
   * @param [in] param The last track parameter before extrapolation.
   * @param [in] surfaces Target layer of the extrapolation.
   *
   * @return Vector of the updated track parameters.
   *
   * FIXME Is this really necessary to call the extrapolation on each target
   * surface?
   */
  std::vector<std::unique_ptr<const Trk::TrackParameters>>
  extrapolateToSurfaces(const Trk::TrackParameters& param,
                        const std::vector<const Trk::Surface*>& surfaces);

  /**
   * @brief Finds the overall best fitting cluster by keeping the one that gave
   * the lowest chi2 after testing each surface and returning it.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  updateStateWithBestFittingCluster(
      const std::vector<std::unique_ptr<const Trk::TrackParameters>>& params);

  /**
   * @brief Find the cluster on a given surface that has the best chi2 passing
   * the cut. Is called within updateStateWithBestFittingCluster.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  findBestCompatibleCluster(const Trk::TrackParameters* param);

  /**
   * @brief Calls the TOF correction tool to build an HGTD_ClusterOnTrack with a
   * calibrated time and resolution and then calls the Kalman updator tools to
   * update the track state.
   */
  std::unique_ptr<const Trk::TrackStateOnSurface>
  updateState(const Trk::TrackParameters* param,
              const HGTD_Cluster* cluster);

  // extrapolation tool
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  // kalman updator tool
  ToolHandle<Trk::IUpdator> m_updator;

  ToolHandle<IHGTD_TOFcorrectionTool> m_tof_corr_tool;

  // keep a pointer to the currently used track, but does not own it!
  // FIXME: this is needed for the TOF correction. Maybe there is a smarter way
  // without keeping this pointer and without pushing the Trk::Track object
  // through everything ==>> move to a TOF corr. accepting just the perigee??
  const Trk::Track* m_track;

  /**
   * @brief Keep pointer to current cluster container.
   */
  const HGTD_ClusterContainer* m_cluster_container;

  const HGTD_DetectorManager* m_hgtd_det_mgr;
  const HGTD_ID* m_hgtd_id_helper;

  /**
   * @brief Track extensions are only kept if the chi2/ndof is lower than the
   * defined cut.
   */
  float m_chi2_cut;

  /**
   * @brief Particle hypothesis used for the extrapolation.
   */
  int m_particle_hypot;
};

#endif // HGTD_ITERATIVEEXTENTSIONTOOL_H
