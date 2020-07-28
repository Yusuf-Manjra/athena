#include "TrigEgammaHypo/TrigL2CaloHypo.h"
#include "TrigEgammaHypo/TrigL2ElectronFex.h"
#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "TrigEgammaHypo/TrigL2ElectronHypo.h"
#include "../TrigEgammaFastPhotonFexMT.h"
#include "TrigEgammaHypo/TrigL2PhotonHypo.h"
#include "TrigEgammaHypo/TrigEFDielectronMassHypo.h"
#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"
#include "TrigEgammaHypo/TrigEFMtAllTE.h"
#include "TrigEgammaHypo/TrigEFCaloCalibFex.h"
#include "TrigEgammaHypo/TrigEFCaloHypo.h"
#include "TrigEgammaHypo/TrigEFElectronHypo.h"
#include "TrigEgammaHypo/TrigEFPhotonHypo.h"
#include "TrigEgammaHypo/TrigEFTrackHypo.h"
#include "TrigEgammaHypo/TrigL2CaloLayersHypo.h"
#include "../TrigEgammaFastCaloHypoAlgMT.h"
#include "../TrigEgammaFastCaloHypoToolInc.h"
#include "../TrigEgammaPrecisionCaloHypoToolInc.h"
#include "../TrigEgammaPrecisionPhotonHypoToolInc.h"
#include "../TrigEgammaPrecisionElectronHypoToolInc.h"
#include "../TrigEgammaFastCaloHypoToolMult.h"
#include "../TrigEgammaFastElectronHypoTool.h"
#include "../TrigEgammaFastElectronFexMT.h"
#include "../TrigEgammaFastElectronHypoAlgMT.h"
#include "../TrigEgammaFastPhotonHypoAlgMT.h"
#include "../TrigEgammaFastPhotonHypoTool.h"
#include "../TrigEgammaPrecisionCaloHypoAlgMT.h"
#include "../TrigEgammaPrecisionCaloHypoToolMult.h"
#include "../TrigEgammaPrecisionPhotonHypoAlgMT.h"
#include "../TrigEgammaPrecisionElectronHypoAlgMT.h"
#include "../TrigEgammaPrecisionPhotonHypoToolMult.h"
#include "../TrigEgammaPrecisionElectronHypoToolMult.h"

DECLARE_COMPONENT( TrigL2CaloHypo )
DECLARE_COMPONENT( TrigL2ElectronFex )
DECLARE_COMPONENT( TrigL2PhotonFex )
DECLARE_COMPONENT( TrigL2ElectronHypo )
DECLARE_COMPONENT( TrigEgammaFastPhotonFexMT )
DECLARE_COMPONENT( TrigL2PhotonHypo )
DECLARE_COMPONENT( TrigEFDielectronMassHypo )
DECLARE_COMPONENT( TrigEFDielectronMassFex )
DECLARE_COMPONENT( TrigEFMtAllTE )
DECLARE_COMPONENT( TrigEFCaloCalibFex )
DECLARE_COMPONENT( TrigEFCaloHypo )
DECLARE_COMPONENT( TrigEFElectronHypo )
DECLARE_COMPONENT( TrigEFPhotonHypo )
DECLARE_COMPONENT( TrigEFTrackHypo )
DECLARE_COMPONENT( TrigL2CaloLayersHypo )
DECLARE_COMPONENT( TrigEgammaFastCaloHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaFastElectronHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaFastPhotonHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaFastCaloHypoToolInc )
DECLARE_COMPONENT( TrigEgammaPrecisionCaloHypoToolInc )
DECLARE_COMPONENT( TrigEgammaPrecisionPhotonHypoToolInc )
DECLARE_COMPONENT( TrigEgammaPrecisionElectronHypoToolInc )
DECLARE_COMPONENT( TrigEgammaFastCaloHypoToolMult )
DECLARE_COMPONENT( TrigEgammaFastElectronHypoTool )
DECLARE_COMPONENT( TrigEgammaFastElectronFexMT )
DECLARE_COMPONENT( TrigEgammaFastPhotonHypoTool )
DECLARE_COMPONENT( TrigEgammaPrecisionCaloHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaPrecisionCaloHypoToolMult )
DECLARE_COMPONENT( TrigEgammaPrecisionPhotonHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaPrecisionElectronHypoAlgMT )
DECLARE_COMPONENT( TrigEgammaPrecisionPhotonHypoToolMult )
DECLARE_COMPONENT( TrigEgammaPrecisionElectronHypoToolMult )
