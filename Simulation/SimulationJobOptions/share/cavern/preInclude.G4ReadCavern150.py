# Modify the hit wrapping action to wrap on 150 ns

from G4AtlasApps.SimFlags import simFlags
# new MT actions. Note that this will only work with one thread.
simFlags.OptionalUserActionList.addAction('G4UA::HitWrapperTool')
simFlags.UserActionConfig.addConfig('G4UA::HitWrapperTool','WrapTime',150)

