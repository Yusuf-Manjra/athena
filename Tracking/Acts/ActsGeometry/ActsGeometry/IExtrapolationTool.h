#ifndef GEOMACTS_ACTS_IEXTRAPOLATIONTOOL_H
#define GEOMACTS_ACTS_IEXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"
#include <memory>

namespace Acts {

class ExtrapolationCode;
class Surface;
class BoundaryCheck;
class IExtrapolationEngine;

static const InterfaceID IID_IACTSExtrapolationTool("IACTSExtrapolationTool", 1, 0);

class IExtrapolationTool : public AthAlgTool
{

public:

  static const InterfaceID& interfaceID(){return IID_IACTSExtrapolationTool;}

  //DeclareInterfaceID(IExtrapolationTool, 1, 0);
  IExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent) {;}
  
  virtual ExtrapolationCode
  extrapolate(ExCellCharged&       ecCharged,
              const Surface*       sf     = 0,
              const BoundaryCheck& bcheck = true) const = 0;

  virtual ExtrapolationCode
  extrapolate(ExCellNeutral&       ecNeutral,
              const Surface*       sf     = 0,
              const BoundaryCheck& bcheck = true) const = 0;
  
  virtual
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine() const = 0;

  virtual
  void
  prepareAlignment() const = 0;

};

}


#endif
