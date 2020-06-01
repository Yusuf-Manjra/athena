#include "TrigT1ResultByteStream/L1TriggerResultByteStreamCnv.h"
#include "TrigT1ResultByteStream/RoIBResultByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamCnv.h"

#include "TrigT1ResultByteStream/MuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/CTPByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecMuCTPIByteStreamCnv.h"
#include "TrigT1ResultByteStream/RecCTPByteStreamCnv.h"

#include "TrigT1ResultByteStream/CTPByteStreamTool.h"
#include "TrigT1ResultByteStream/MuCTPIByteStreamTool.h"
#include "TrigT1ResultByteStream/RecCTPByteStreamTool.h"
#include "TrigT1ResultByteStream/RecMuCTPIByteStreamTool.h"
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.h"
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"

#include "../L1TriggerByteStreamDecoderAlg.h"
#include "../ExampleL1TriggerByteStreamTool.h"

// ROBF for offline
typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF; 

// converter for offline 
typedef RoIBResultByteStreamCnv<ROBF> RoIBResultByteStreamCnvT ;
typedef RecRoIBResultByteStreamCnv<ROBF> RecRoIBResultByteStreamCnvT ;

// declare 
DECLARE_CONVERTER( L1TriggerResultByteStreamCnv )
DECLARE_CONVERTER( RoIBResultByteStreamCnvT )
DECLARE_CONVERTER( RecRoIBResultByteStreamCnvT )
DECLARE_CONVERTER( MuCTPIByteStreamCnv )
DECLARE_CONVERTER( CTPByteStreamCnv )
DECLARE_CONVERTER( RecMuCTPIByteStreamCnv )
DECLARE_CONVERTER( RecCTPByteStreamCnv )

DECLARE_COMPONENT( CTPByteStreamTool )
DECLARE_COMPONENT( MuCTPIByteStreamTool )
DECLARE_COMPONENT( RecCTPByteStreamTool )
DECLARE_COMPONENT( RecMuCTPIByteStreamTool )
DECLARE_COMPONENT( RecRoIBResultByteStreamTool )
DECLARE_COMPONENT( RoIBResultByteStreamTool )

DECLARE_COMPONENT( L1TriggerByteStreamDecoderAlg )
DECLARE_COMPONENT( ExampleL1TriggerByteStreamTool )
