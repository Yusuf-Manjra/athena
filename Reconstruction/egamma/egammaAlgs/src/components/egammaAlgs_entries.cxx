#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../egammaBuilder.h"
#include "../egammaRecBuilder.h"
#include "../topoEgammaBuilder.h"
#include "../egammaForwardBuilder.h"
#include "../egammaTruthAssociationAlg.h"
#include "../egammaTrackSlimmer.h"
#include "../EMBremCollectionBuilder.h"
#include "../EMVertexBuilder.h"
#include "../egammaTopoClusterCopier.h"
#include "../electronSuperClusterBuilder.h"
#include "../photonSuperClusterBuilder.h"


DECLARE_ALGORITHM_FACTORY( egammaBuilder            )
DECLARE_ALGORITHM_FACTORY( egammaRecBuilder         )
DECLARE_ALGORITHM_FACTORY( topoEgammaBuilder        )
DECLARE_ALGORITHM_FACTORY( egammaForwardBuilder     )
DECLARE_ALGORITHM_FACTORY( egammaTruthAssociationAlg)
DECLARE_ALGORITHM_FACTORY( egammaTrackSlimmer       )
DECLARE_ALGORITHM_FACTORY( EMBremCollectionBuilder  )
DECLARE_ALGORITHM_FACTORY( EMVertexBuilder          )
DECLARE_ALGORITHM_FACTORY( egammaTopoClusterCopier  )
DECLARE_ALGORITHM_FACTORY( electronSuperClusterBuilder  )
DECLARE_ALGORITHM_FACTORY( photonSuperClusterBuilder    )

DECLARE_FACTORY_ENTRIES(egammaAlgs) {
     DECLARE_ALGORITHM( egammaBuilder             )
     DECLARE_ALGORITHM( egammaRecBuilder          )
     DECLARE_ALGORITHM( topoEgammaBuilder         )
     DECLARE_ALGORITHM( egammaForwardBuilder      )
     DECLARE_ALGORITHM( egammaTruthAssociationAlg )
     DECLARE_ALGORITHM( egammaTrackSlimmer        )
     DECLARE_ALGORITHM( EMBremCollectionBuilder   )
     DECLARE_ALGORITHM( EMVertexBuilder           )
     DECLARE_ALGORITHM( egammaTopoClusterCopier   )
     DECLARE_ALGORITHM( electronSuperClusterBuilder  )
     DECLARE_ALGORITHM( photonSuperClusterBuilder    )

}
