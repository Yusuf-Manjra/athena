from PowhegControl import PowhegConfig_VBF_H

# Use the Powheg_VBF_H configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_VBF_H( runArgs=transform_runArgs, opts=transform_opts )
