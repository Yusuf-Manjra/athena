# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwClcon_dc2.py
# Created: Nov 2006, sss
# Purpose: Cluster containment corrections, DC2 version.
#
# This correction was derived from DC2 electrons at 50, 100, and 200 GeV.
# Simple Gaussian fits were used to extract the peaks.
# It was designed to be used before the original (`old') longitudinal
# weights correction.  This was used with the `DC2' set of corrections.
#
# The 5x5 version was added in LArClusterRec-02-05-25, in 9.0.3,
# and the other sizes in LArClusterRec-02-05-27, in 9.0.4.
# 


from CaloClusterCorrection.constants import CALOCORR_CLUSTER
from AthenaCommon.SystemOfUnits import GeV


# Energies at which corrections were tabulated.
CaloSw_sample_energies = [50*GeV, 100*GeV, 200*GeV]


# First version, 5x5 clusters.
CaloSw_clcon_first_55 = [
    [0.012500, 1.035514, 1.030185, 1.023728],
    [0.037500, 1.017023, 1.013910, 1.013134],
    [0.062500, 1.015066, 1.011590, 1.010456],
    [0.087500, 1.012692, 1.010001, 1.008765],
    [0.112500, 1.010337, 1.007395, 1.007152],
    [0.137500, 1.009316, 1.006146, 1.006039],
    [0.162500, 1.010676, 1.006880, 1.005249],
    [0.187500, 1.008862, 1.004835, 1.004180],
    [0.212500, 1.008730, 1.005388, 1.004231],
    [0.237500, 1.008265, 1.003867, 1.004087],
    [0.262500, 1.007696, 1.004800, 1.003484],
    [0.287500, 1.007728, 1.004358, 1.002906],
    [0.312500, 1.007843, 1.003792, 1.002157],
    [0.337500, 1.006983, 1.003329, 1.002334],
    [0.362500, 1.005481, 1.002180, 1.001452],
    [0.387500, 1.007179, 1.003720, 1.002256],
    [0.412500, 1.007243, 1.002327, 1.001620],
    [0.437500, 1.007855, 1.003716, 1.001531],
    [0.462500, 1.005129, 1.001004, 1.000701],
    [0.487500, 1.006673, 1.001940, 1.000716],
    [0.512500, 1.008296, 1.002659, 1.000583],
    [0.537500, 1.008347, 1.003710, 1.001691],
    [0.562500, 1.009811, 1.005047, 1.001912],
    [0.587500, 1.008582, 1.003776, 1.001533],
    [0.612500, 1.010099, 1.004789, 1.002059],
    [0.637500, 1.013456, 1.007251, 1.004854],
    [0.662500, 1.019530, 1.012355, 1.007273],
    [0.687500, 1.023487, 1.014084, 1.009242],
    [0.712500, 1.024278, 1.014879, 1.009641],
    [0.737500, 1.027043, 1.016490, 1.009754],
    [0.762500, 1.029041, 1.016800, 1.010714],
    [0.787500, 1.049676, 1.031536, 1.021001],
    [0.812500, 1.057413, 1.035643, 1.028377],
    [0.837500, 1.038151, 1.026575, 1.018428],
    [0.862500, 1.043463, 1.028188, 1.021240],
    [0.887500, 1.042189, 1.029532, 1.020662],
    [0.912500, 1.046237, 1.029863, 1.022809],
    [0.937500, 1.046338, 1.030888, 1.021628],
    [0.962500, 1.045797, 1.029372, 1.022277],
    [0.987500, 1.048155, 1.029865, 1.022117],
    [1.012500, 1.048164, 1.031432, 1.021045],
    [1.037500, 1.048885, 1.032007, 1.021746],
    [1.062500, 1.050757, 1.029104, 1.021361],
    [1.087500, 1.052690, 1.032756, 1.022838],
    [1.112500, 1.050475, 1.031583, 1.021677],
    [1.137500, 1.047564, 1.033386, 1.022139],
    [1.162500, 1.055390, 1.036236, 1.024464],
    [1.187500, 1.052472, 1.031101, 1.023006],
    [1.212500, 1.058102, 1.038227, 1.023740],
    [1.237500, 1.059573, 1.038521, 1.028065],
    [1.262500, 1.061167, 1.043815, 1.030508],
    [1.287500, 1.068512, 1.042734, 1.031791],
    [1.312500, 1.049665, 1.024230, 1.010164],
    [1.337500, 1.046736, 1.019572, 1.007111],
    [1.362500, 1.043596, 1.013778, 0.999030],
    [1.387500, 1.095868, 1.075196, 1.062066],
    [1.412500, 1.101310, 1.077008, 1.065099],
    [1.437500, 1.108857, 1.081700, 1.072879],
    [1.462500, 1.118247, 1.078507, 1.051192],
    [1.487500, 1.127513, 1.072693, 1.038123],
    [1.512500, 1.110449, 1.059505, 1.044913],
    [1.537500, 0.999950, 0.990164, 0.983289],
    [1.562500, 1.060863, 1.051961, 1.050202],
    [1.587500, 1.084023, 1.071180, 1.067544],
    [1.612500, 1.071877, 1.046484, 1.031647],
    [1.637500, 1.080184, 1.051470, 1.027321],
    [1.662500, 1.079239, 1.049039, 1.029752],
    [1.687500, 1.087739, 1.055141, 1.034135],
    [1.712500, 1.049777, 1.030912, 1.024217],
    [1.737500, 1.045786, 1.030601, 1.020388],
    [1.762500, 1.053749, 1.034786, 1.025761],
    [1.787500, 1.055747, 1.035475, 1.023596],
    [1.812500, 1.077158, 1.045784, 1.031477],
    [1.837500, 1.086514, 1.053469, 1.036282],
    [1.862500, 1.080151, 1.053614, 1.035899],
    [1.887500, 1.069624, 1.047961, 1.035295],
    [1.912500, 1.064445, 1.045096, 1.033309],
    [1.937500, 1.057462, 1.041938, 1.030457],
    [1.962500, 1.055789, 1.037340, 1.030124],
    [1.987500, 1.053891, 1.036649, 1.027669],
    [2.012500, 1.051831, 1.036097, 1.028411],
    [2.037500, 1.059887, 1.043711, 1.034435],
    [2.062500, 1.053624, 1.039919, 1.031512],
    [2.087500, 1.057997, 1.041657, 1.034765],
    [2.112500, 1.061659, 1.046037, 1.038245],
    [2.137500, 1.059411, 1.047817, 1.036595],
    [2.162500, 1.060262, 1.046550, 1.038755],
    [2.187500, 1.064186, 1.049101, 1.040172],
    [2.212500, 1.063439, 1.046290, 1.039837],
    [2.237500, 1.059393, 1.047858, 1.039727],
    [2.262500, 1.063883, 1.048058, 1.041470],
    [2.287500, 1.064051, 1.052372, 1.044376],
    [2.312500, 1.069193, 1.055576, 1.048308],
    [2.337500, 1.068652, 1.054089, 1.049113],
    [2.362500, 1.066417, 1.055084, 1.048081],
    [2.387500, 1.064174, 1.050995, 1.043761],
    [2.412500, 1.065184, 1.054187, 1.046795],
    [2.437500, 1.068659, 1.055398, 1.048452],
    [2.462500, 1.080240, 1.066958, 1.060500],
    [2.487500, 1.149999, 1.127871, 1.118679],
    ]

# First version, 3x3 clusters.
CaloSw_clcon_first_33 = [
[0.012500, 1.083598, 1.075248, 1.068064],
[0.037500, 1.059270, 1.054343, 1.052647],
[0.062500, 1.056698, 1.051454, 1.049965],
[0.087500, 1.054996, 1.050636, 1.048833],
[0.112500, 1.052457, 1.048404, 1.047121],
[0.137500, 1.051575, 1.047163, 1.046339],
[0.162500, 1.052906, 1.047871, 1.045538],
[0.187500, 1.051987, 1.046113, 1.044146],
[0.212500, 1.051182, 1.046550, 1.043959],
[0.237500, 1.050874, 1.044850, 1.044655],
[0.262500, 1.050336, 1.044699, 1.043038],
[0.287500, 1.050450, 1.045925, 1.043018],
[0.312500, 1.050305, 1.044831, 1.042257],
[0.337500, 1.050757, 1.045113, 1.042708],
[0.362500, 1.048325, 1.043472, 1.041999],
[0.387500, 1.049793, 1.045818, 1.042511],
[0.412500, 1.050517, 1.043543, 1.042401],
[0.437500, 1.050384, 1.044771, 1.042003],
[0.462500, 1.048052, 1.042280, 1.041368],
[0.487500, 1.050709, 1.044307, 1.041232],
[0.512500, 1.052541, 1.044204, 1.041916],
[0.537500, 1.052236, 1.046644, 1.043538],
[0.562500, 1.053372, 1.047367, 1.042610],
[0.587500, 1.053131, 1.046388, 1.043197],
[0.612500, 1.054172, 1.047129, 1.043576],
[0.637500, 1.059888, 1.050025, 1.046866],
[0.662500, 1.065455, 1.055513, 1.048951],
[0.687500, 1.071398, 1.058404, 1.052100],
[0.712500, 1.071061, 1.058465, 1.052674],
[0.737500, 1.075573, 1.062227, 1.052835],
[0.762500, 1.078375, 1.060343, 1.051316],
[0.787500, 1.108558, 1.082041, 1.067086],
[0.812500, 1.115436, 1.089854, 1.076757],
[0.837500, 1.097511, 1.080796, 1.069976],
[0.862500, 1.105574, 1.083301, 1.073292],
[0.887500, 1.104842, 1.085276, 1.073090],
[0.912500, 1.107524, 1.084993, 1.075770],
[0.937500, 1.108184, 1.088881, 1.075156],
[0.962500, 1.109826, 1.086638, 1.076036],
[0.987500, 1.114661, 1.086973, 1.076245],
[1.012500, 1.111700, 1.090380, 1.075224],
[1.037500, 1.110955, 1.087224, 1.075020],
[1.062500, 1.118042, 1.083731, 1.076240],
[1.087500, 1.119264, 1.090148, 1.077065],
[1.112500, 1.124156, 1.090173, 1.075934],
[1.137500, 1.116193, 1.092245, 1.077379],
[1.162500, 1.126542, 1.099710, 1.078872],
[1.187500, 1.125817, 1.091988, 1.079315],
[1.212500, 1.138933, 1.096804, 1.079361],
[1.237500, 1.139868, 1.097851, 1.083192],
[1.262500, 1.143223, 1.107220, 1.088119],
[1.287500, 1.155863, 1.107950, 1.089154],
[1.312500, 1.141086, 1.093423, 1.072124],
[1.337500, 1.133941, 1.088635, 1.068301],
[1.362500, 1.132564, 1.081476, 1.062163],
[1.387500, 1.221229, 1.194838, 1.170628],
[1.412500, 1.163183, 1.132167, 1.111349],
[1.437500, 1.159008, 1.127667, 1.115802],
[1.462500, 1.165253, 1.129643, 1.100924],
[1.487500, 1.199532, 1.138201, 1.098659],
[1.512500, 1.164557, 1.111530, 1.092242],
[1.537500, 1.239285, 1.187613, 1.137459],
[1.562500, 1.187843, 1.149365, 1.124999],
[1.587500, 1.210028, 1.166234, 1.144038],
[1.612500, 1.139622, 1.102852, 1.082426],
[1.637500, 1.153102, 1.113284, 1.079862],
[1.662500, 1.151606, 1.108479, 1.081798],
[1.687500, 1.162931, 1.119138, 1.088822],
[1.712500, 1.113660, 1.088693, 1.075994],
[1.737500, 1.107381, 1.084975, 1.073715],
[1.762500, 1.124339, 1.089670, 1.080116],
[1.787500, 1.123101, 1.092109, 1.079271],
[1.812500, 1.149312, 1.108388, 1.089604],
[1.837500, 1.159103, 1.115034, 1.093112],
[1.862500, 1.154565, 1.116041, 1.096007],
[1.887500, 1.144800, 1.109173, 1.092903],
[1.912500, 1.136341, 1.108075, 1.091679],
[1.937500, 1.138459, 1.105476, 1.090769],
[1.962500, 1.139065, 1.103637, 1.091598],
[1.987500, 1.127967, 1.104228, 1.091782],
[2.012500, 1.128535, 1.104028, 1.093636],
[2.037500, 1.135324, 1.113625, 1.099948],
[2.062500, 1.133196, 1.112406, 1.098147],
[2.087500, 1.139143, 1.112251, 1.102785],
[2.112500, 1.143870, 1.118498, 1.105640],
[2.137500, 1.141986, 1.119992, 1.106307],
[2.162500, 1.144888, 1.122039, 1.109291],
[2.187500, 1.147715, 1.124179, 1.113318],
[2.212500, 1.147369, 1.121398, 1.111672],
[2.237500, 1.146021, 1.124735, 1.113158],
[2.262500, 1.150536, 1.125037, 1.115445],
[2.287500, 1.150609, 1.131717, 1.120368],
[2.312500, 1.156826, 1.135885, 1.124357],
[2.337500, 1.152683, 1.134392, 1.126578],
[2.362500, 1.155483, 1.135921, 1.126032],
[2.387500, 1.153915, 1.132719, 1.122183],
[2.412500, 1.153612, 1.136881, 1.125996],
[2.437500, 1.158028, 1.139757, 1.128729],
[2.462500, 1.156985, 1.139050, 1.131086],
[2.487500, 1.218083, 1.194113, 1.173903],
    ]

# First version, 3x5 clusters.
CaloSw_clcon_first_35 = [
[0.012500, 1.057771, 1.051770, 1.044844],
[0.037500, 1.038613, 1.034856, 1.033667],
[0.062500, 1.035245, 1.031613, 1.030391],
[0.087500, 1.034293, 1.031001, 1.029634],
[0.112500, 1.031192, 1.028381, 1.027655],
[0.137500, 1.031044, 1.027011, 1.026569],
[0.162500, 1.031720, 1.027965, 1.026176],
[0.187500, 1.030527, 1.026152, 1.024889],
[0.212500, 1.030301, 1.026532, 1.024991],
[0.237500, 1.029958, 1.025091, 1.024998],
[0.262500, 1.029131, 1.025653, 1.023885],
[0.287500, 1.029385, 1.025513, 1.024148],
[0.312500, 1.029428, 1.024974, 1.022888],
[0.337500, 1.028608, 1.024929, 1.023130],
[0.362500, 1.027023, 1.023458, 1.022376],
[0.387500, 1.028624, 1.025306, 1.023322],
[0.412500, 1.028839, 1.023637, 1.022785],
[0.437500, 1.029642, 1.024784, 1.022523],
[0.462500, 1.027227, 1.022634, 1.021755],
[0.487500, 1.028633, 1.023766, 1.021845],
[0.512500, 1.030293, 1.023981, 1.021748],
[0.537500, 1.030636, 1.025680, 1.023265],
[0.562500, 1.031709, 1.026756, 1.022962],
[0.587500, 1.030916, 1.025574, 1.023049],
[0.612500, 1.032310, 1.026478, 1.023394],
[0.637500, 1.036244, 1.029491, 1.026659],
[0.662500, 1.042702, 1.034534, 1.028714],
[0.687500, 1.047179, 1.036723, 1.031342],
[0.712500, 1.047508, 1.037218, 1.031266],
[0.737500, 1.050630, 1.039436, 1.031875],
[0.762500, 1.051231, 1.037896, 1.031056],
[0.787500, 1.075635, 1.055885, 1.044406],
[0.812500, 1.081691, 1.059589, 1.051826],
[0.837500, 1.066734, 1.053592, 1.044820],
[0.862500, 1.071659, 1.055208, 1.047097],
[0.887500, 1.071079, 1.056172, 1.046841],
[0.912500, 1.074623, 1.056650, 1.048940],
[0.937500, 1.075422, 1.058316, 1.048302],
[0.962500, 1.076191, 1.057415, 1.048987],
[0.987500, 1.077208, 1.057993, 1.049381],
[1.012500, 1.077198, 1.059950, 1.047581],
[1.037500, 1.078684, 1.059730, 1.048098],
[1.062500, 1.080395, 1.057014, 1.047608],
[1.087500, 1.082590, 1.061071, 1.050116],
[1.112500, 1.080967, 1.059171, 1.048116],
[1.137500, 1.078530, 1.061144, 1.049502],
[1.162500, 1.085649, 1.064245, 1.051423],
[1.187500, 1.083301, 1.059578, 1.050386],
[1.212500, 1.088333, 1.066472, 1.050369],
[1.237500, 1.092388, 1.066534, 1.055875],
[1.262500, 1.094308, 1.073947, 1.058703],
[1.287500, 1.100385, 1.072403, 1.058492],
[1.312500, 1.085213, 1.056040, 1.041059],
[1.337500, 1.081545, 1.051547, 1.037191],
[1.362500, 1.076692, 1.043041, 1.031335],
[1.387500, 1.181167, 1.157334, 1.138840],
[1.412500, 1.128040, 1.101770, 1.087642],
[1.437500, 1.135979, 1.108339, 1.098151],
[1.462500, 1.148735, 1.110341, 1.087271],
[1.487500, 1.188464, 1.125140, 1.085067],
[1.512500, 1.142431, 1.089932, 1.069303],
[1.537500, 1.201690, 1.153754, 1.108873],
[1.562500, 1.145796, 1.116656, 1.099140],
[1.587500, 1.170029, 1.135717, 1.116934],
[1.612500, 1.098155, 1.070681, 1.055173],
[1.637500, 1.109505, 1.078892, 1.051020],
[1.662500, 1.111054, 1.076480, 1.055717],
[1.687500, 1.123063, 1.085283, 1.061468],
[1.712500, 1.076432, 1.057935, 1.049550],
[1.737500, 1.073528, 1.056544, 1.046331],
[1.762500, 1.082861, 1.062382, 1.052105],
[1.787500, 1.085142, 1.062673, 1.051884],
[1.812500, 1.109218, 1.075748, 1.058462],
[1.837500, 1.119654, 1.085112, 1.065936],
[1.862500, 1.112399, 1.083583, 1.065821],
[1.887500, 1.101926, 1.077778, 1.064905],
[1.912500, 1.097440, 1.076663, 1.062716],
[1.937500, 1.091316, 1.073204, 1.060974],
[1.962500, 1.091605, 1.069227, 1.060753],
[1.987500, 1.088260, 1.070131, 1.060432],
[2.012500, 1.085996, 1.069421, 1.060692],
[2.037500, 1.094529, 1.076621, 1.066982],
[2.062500, 1.089318, 1.074125, 1.064569],
[2.087500, 1.094394, 1.076948, 1.068212],
[2.112500, 1.100145, 1.081730, 1.071111],
[2.137500, 1.100231, 1.082401, 1.071424],
[2.162500, 1.099443, 1.083144, 1.074324],
[2.187500, 1.103373, 1.086635, 1.076364],
[2.212500, 1.102624, 1.083799, 1.076090],
[2.237500, 1.099225, 1.085625, 1.076502],
[2.262500, 1.104298, 1.086953, 1.079190],
[2.287500, 1.106200, 1.091937, 1.082867],
[2.312500, 1.111650, 1.095150, 1.087399],
[2.337500, 1.110515, 1.094913, 1.089539],
[2.362500, 1.110085, 1.096608, 1.088554],
[2.387500, 1.108400, 1.093071, 1.085528],
[2.412500, 1.109246, 1.096243, 1.087997],
[2.437500, 1.113268, 1.098362, 1.089963],
[2.462500, 1.112369, 1.098194, 1.092492],
[2.487500, 1.178236, 1.160151, 1.141596],
    ]

# First version, 3x7 clusters.
CaloSw_clcon_first_37 = [
[0.012500, 1.050621, 1.044233, 1.040079],
[0.037500, 1.033092, 1.029734, 1.028721],
[0.062500, 1.029960, 1.026500, 1.025391],
[0.087500, 1.028764, 1.025986, 1.024493],
[0.112500, 1.025790, 1.022884, 1.022537],
[0.137500, 1.025021, 1.021584, 1.021543],
[0.162500, 1.026142, 1.022802, 1.021080],
[0.187500, 1.024745, 1.020880, 1.019966],
[0.212500, 1.024634, 1.021255, 1.019876],
[0.237500, 1.024389, 1.019610, 1.019742],
[0.262500, 1.023316, 1.020385, 1.019010],
[0.287500, 1.023289, 1.020066, 1.019184],
[0.312500, 1.023398, 1.019271, 1.017674],
[0.337500, 1.022994, 1.019666, 1.017977],
[0.362500, 1.021465, 1.017778, 1.017103],
[0.387500, 1.022593, 1.019827, 1.017842],
[0.412500, 1.022636, 1.018430, 1.017391],
[0.437500, 1.023150, 1.019780, 1.017256],
[0.462500, 1.021140, 1.016733, 1.016206],
[0.487500, 1.022658, 1.018167, 1.016475],
[0.512500, 1.023595, 1.018378, 1.016447],
[0.537500, 1.024602, 1.019695, 1.017571],
[0.562500, 1.025860, 1.020838, 1.017464],
[0.587500, 1.024387, 1.019801, 1.017434],
[0.612500, 1.026307, 1.020738, 1.017953],
[0.637500, 1.029686, 1.023178, 1.021120],
[0.662500, 1.035484, 1.028036, 1.022818],
[0.687500, 1.039001, 1.030212, 1.025397],
[0.712500, 1.039284, 1.029910, 1.025118],
[0.737500, 1.040777, 1.031832, 1.025537],
[0.762500, 1.040902, 1.030503, 1.024384],
[0.787500, 1.063765, 1.046594, 1.037358],
[0.812500, 1.070050, 1.048577, 1.041888],
[0.837500, 1.056046, 1.044349, 1.037015],
[0.862500, 1.059867, 1.046288, 1.039082],
[0.887500, 1.060138, 1.047567, 1.038752],
[0.912500, 1.063053, 1.048045, 1.040495],
[0.937500, 1.062487, 1.047783, 1.039927],
[0.962500, 1.062367, 1.047890, 1.040280],
[0.987500, 1.063508, 1.048745, 1.040552],
[1.012500, 1.062834, 1.049318, 1.038936],
[1.037500, 1.063573, 1.049312, 1.039493],
[1.062500, 1.064223, 1.047629, 1.038899],
[1.087500, 1.066497, 1.050029, 1.039974],
[1.112500, 1.065750, 1.048124, 1.039168],
[1.137500, 1.064151, 1.048833, 1.040076],
[1.162500, 1.069473, 1.051740, 1.041324],
[1.187500, 1.066715, 1.047580, 1.040766],
[1.212500, 1.069402, 1.052320, 1.040502],
[1.237500, 1.072276, 1.052941, 1.043757],
[1.262500, 1.074131, 1.057597, 1.046027],
[1.287500, 1.076661, 1.056116, 1.047281],
[1.312500, 1.059972, 1.040509, 1.028714],
[1.337500, 1.056507, 1.034399, 1.023292],
[1.362500, 1.048957, 1.022941, 1.015474],
[1.387500, 1.153410, 1.135781, 1.123152],
[1.412500, 1.100139, 1.083033, 1.072330],
[1.437500, 1.110341, 1.089631, 1.083889],
[1.462500, 1.120638, 1.091128, 1.070399],
[1.487500, 1.156143, 1.101527, 1.068220],
[1.512500, 1.119004, 1.068781, 1.052869],
[1.537500, 1.182724, 1.129512, 1.092640],
[1.562500, 1.124959, 1.102615, 1.088650],
[1.587500, 1.147676, 1.120546, 1.105768],
[1.612500, 1.080720, 1.057671, 1.044660],
[1.637500, 1.086858, 1.059718, 1.040915],
[1.662500, 1.088204, 1.060791, 1.043980],
[1.687500, 1.103229, 1.071128, 1.050663],
[1.712500, 1.064293, 1.047986, 1.041126],
[1.737500, 1.061543, 1.047777, 1.038057],
[1.762500, 1.071896, 1.053861, 1.043120],
[1.787500, 1.073395, 1.055622, 1.042640],
[1.812500, 1.097101, 1.066033, 1.050396],
[1.837500, 1.106184, 1.074995, 1.056359],
[1.862500, 1.099924, 1.072793, 1.055604],
[1.887500, 1.091189, 1.068336, 1.054960],
[1.912500, 1.085963, 1.067009, 1.053871],
[1.937500, 1.081466, 1.064286, 1.052065],
[1.962500, 1.078320, 1.060121, 1.051573],
[1.987500, 1.076612, 1.060670, 1.050403],
[2.012500, 1.074725, 1.059428, 1.050860],
[2.037500, 1.082622, 1.065278, 1.056241],
[2.062500, 1.078303, 1.062808, 1.053946],
[2.087500, 1.081170, 1.066565, 1.057175],
[2.112500, 1.085947, 1.069811, 1.060725],
[2.137500, 1.086852, 1.071644, 1.061084],
[2.162500, 1.086800, 1.071676, 1.063372],
[2.187500, 1.089546, 1.075309, 1.065068],
[2.212500, 1.088566, 1.072922, 1.064810],
[2.237500, 1.087811, 1.073237, 1.065754],
[2.262500, 1.089465, 1.074094, 1.066863],
[2.287500, 1.092274, 1.079409, 1.070980],
[2.312500, 1.097847, 1.082533, 1.075826],
[2.337500, 1.097035, 1.082219, 1.077722],
[2.362500, 1.096554, 1.084642, 1.076996],
[2.387500, 1.094445, 1.080021, 1.073264],
[2.412500, 1.094718, 1.083884, 1.076027],
[2.437500, 1.098914, 1.085178, 1.077745],
[2.462500, 1.098906, 1.084760, 1.080213],
[2.487500, 1.163979, 1.148510, 1.135051],
    ]

# First version, 5x7 clusters.
CaloSw_clcon_first_57 = [
[0.012500, 1.025697, 1.020228, 1.016519],
[0.037500, 1.009999, 1.006999, 1.006756],
[0.062500, 1.008181, 1.005069, 1.004263],
[0.087500, 1.005614, 1.003333, 1.002306],
[0.112500, 1.003224, 1.000443, 1.000677],
[0.137500, 1.002029, 0.999108, 0.999649],
[0.162500, 1.003579, 1.000265, 0.998834],
[0.187500, 1.001911, 0.998211, 0.997744],
[0.212500, 1.001929, 0.998856, 0.997814],
[0.237500, 1.001278, 0.997358, 0.997675],
[0.262500, 1.000176, 0.997847, 0.997050],
[0.287500, 1.000386, 0.997607, 0.996542],
[0.312500, 1.000445, 0.996732, 0.995603],
[0.337500, 1.000091, 0.996652, 0.995787],
[0.362500, 0.998202, 0.995189, 0.994728],
[0.387500, 0.999768, 0.996909, 0.995432],
[0.412500, 0.999300, 0.995868, 0.994795],
[0.437500, 1.000004, 0.996800, 0.994860],
[0.462500, 0.997572, 0.993857, 0.994008],
[0.487500, 0.998905, 0.995015, 0.993695],
[0.512500, 1.000099, 0.995193, 0.994058],
[0.537500, 1.000648, 0.996532, 0.994603],
[0.562500, 1.002423, 0.997660, 0.995034],
[0.587500, 1.000294, 0.996282, 0.994423],
[0.612500, 1.002320, 0.997705, 0.995225],
[0.637500, 1.005287, 0.999362, 0.997872],
[0.662500, 1.010783, 1.004418, 0.999679],
[0.687500, 1.013887, 1.005855, 1.001766],
[0.712500, 1.014171, 1.006218, 1.001752],
[0.737500, 1.016006, 1.007522, 1.002028],
[0.762500, 1.017432, 1.007536, 1.002341],
[0.787500, 1.036144, 1.020700, 1.012020],
[0.812500, 1.041956, 1.022509, 1.017341],
[0.837500, 1.025730, 1.015605, 1.008675],
[0.862500, 1.029507, 1.017614, 1.011407],
[0.887500, 1.028772, 1.018531, 1.010444],
[0.912500, 1.032071, 1.018656, 1.012594],
[0.937500, 1.031521, 1.018007, 1.011613],
[0.962500, 1.030460, 1.018383, 1.011596],
[0.987500, 1.031373, 1.018774, 1.011429],
[1.012500, 1.031975, 1.019799, 1.010383],
[1.037500, 1.032011, 1.019356, 1.010668],
[1.062500, 1.033225, 1.018190, 1.010090],
[1.087500, 1.034482, 1.019566, 1.011071],
[1.112500, 1.033428, 1.018564, 1.010242],
[1.137500, 1.031084, 1.018548, 1.010689],
[1.162500, 1.035748, 1.020992, 1.012364],
[1.187500, 1.033464, 1.016269, 1.010896],
[1.212500, 1.036233, 1.021456, 1.011473],
[1.237500, 1.038176, 1.021275, 1.013419],
[1.262500, 1.039794, 1.025882, 1.015865],
[1.287500, 1.042124, 1.025722, 1.017849],
[1.312500, 1.025180, 1.009111, 0.998702],
[1.337500, 1.021905, 1.003423, 0.994401],
[1.362500, 1.018516, 0.997512, 0.991176],
[1.387500, 1.066293, 1.049895, 1.042496],
[1.412500, 1.066953, 1.052434, 1.045874],
[1.437500, 1.080939, 1.060715, 1.055434],
[1.462500, 1.083241, 1.053830, 1.033156],
[1.487500, 1.086213, 1.040659, 1.013678],
[1.512500, 1.079446, 1.041078, 1.024846],
[1.537500, 1.156673, 1.108701, 1.064808],
[1.562500, 1.096902, 1.074887, 1.062588],
[1.587500, 1.117406, 1.092181, 1.078589],
[1.612500, 1.050980, 1.029718, 1.018574],
[1.637500, 1.054562, 1.032609, 1.014305],
[1.662500, 1.054558, 1.030504, 1.016295],
[1.687500, 1.066195, 1.036940, 1.019236],
[1.712500, 1.034532, 1.018160, 1.012372],
[1.737500, 1.029874, 1.018315, 1.009057],
[1.762500, 1.038773, 1.023343, 1.013451],
[1.787500, 1.040644, 1.024065, 1.011882],
[1.812500, 1.061736, 1.033540, 1.019204],
[1.837500, 1.070089, 1.041234, 1.023857],
[1.862500, 1.064281, 1.039147, 1.023204],
[1.887500, 1.055161, 1.034150, 1.021846],
[1.912500, 1.049465, 1.031838, 1.020553],
[1.937500, 1.043447, 1.029451, 1.018115],
[1.962500, 1.041662, 1.024806, 1.017019],
[1.987500, 1.039006, 1.023264, 1.015252],
[2.012500, 1.036753, 1.023230, 1.014881],
[2.037500, 1.043690, 1.028493, 1.019660],
[2.062500, 1.039165, 1.024667, 1.017147],
[2.087500, 1.040758, 1.027530, 1.019268],
[2.112500, 1.044648, 1.030405, 1.022856],
[2.137500, 1.043965, 1.031082, 1.021947],
[2.162500, 1.043629, 1.030760, 1.023866],
[2.187500, 1.046107, 1.033075, 1.024338],
[2.212500, 1.045076, 1.030962, 1.023999],
[2.237500, 1.043007, 1.031696, 1.024382],
[2.262500, 1.044718, 1.030704, 1.025468],
[2.287500, 1.046088, 1.035234, 1.028181],
[2.312500, 1.051777, 1.038923, 1.032580],
[2.337500, 1.051174, 1.037652, 1.033255],
[2.362500, 1.048477, 1.038478, 1.032171],
[2.387500, 1.045689, 1.033769, 1.027730],
[2.412500, 1.045631, 1.036906, 1.030228],
[2.437500, 1.048886, 1.037854, 1.031619],
[2.462500, 1.062800, 1.050698, 1.046061],
[2.487500, 1.134717, 1.110494, 1.099414],
    ]

# First version, 7x7 clusters.
CaloSw_clcon_first_77 = [
[0.012500, 1.019721, 1.013573, 1.009922],
[0.037500, 1.003013, 0.999978, 1.000044],
[0.062500, 1.001665, 0.998624, 0.998070],
[0.087500, 0.999232, 0.997072, 0.996197],
[0.112500, 0.996890, 0.994165, 0.994525],
[0.137500, 0.995336, 0.992977, 0.993277],
[0.162500, 0.997078, 0.994101, 0.992484],
[0.187500, 0.995563, 0.991729, 0.991524],
[0.212500, 0.995262, 0.992376, 0.991406],
[0.237500, 0.994805, 0.991116, 0.991343],
[0.262500, 0.993745, 0.991471, 0.990520],
[0.287500, 0.993832, 0.991343, 0.990368],
[0.312500, 0.993799, 0.990208, 0.989159],
[0.337500, 0.993336, 0.990310, 0.989432],
[0.362500, 0.991484, 0.988605, 0.988272],
[0.387500, 0.992917, 0.990464, 0.989140],
[0.412500, 0.992574, 0.989182, 0.988611],
[0.437500, 0.993627, 0.990460, 0.988631],
[0.462500, 0.990936, 0.987459, 0.987480],
[0.487500, 0.992132, 0.988353, 0.987391],
[0.512500, 0.993522, 0.988827, 0.987365],
[0.537500, 0.993796, 0.989872, 0.988024],
[0.562500, 0.995427, 0.991174, 0.988477],
[0.587500, 0.993520, 0.989722, 0.987870],
[0.612500, 0.995399, 0.990677, 0.988564],
[0.637500, 0.998580, 0.992592, 0.991474],
[0.662500, 1.003376, 0.997468, 0.993109],
[0.687500, 1.006654, 0.998723, 0.995192],
[0.712500, 1.006959, 0.999342, 0.995103],
[0.737500, 1.008803, 1.000854, 0.995875],
[0.762500, 1.009789, 0.999999, 0.995032],
[0.787500, 1.028597, 1.012833, 1.004188],
[0.812500, 1.032791, 1.015504, 1.010037],
[0.837500, 1.017573, 1.007522, 1.001138],
[0.862500, 1.020222, 1.008553, 1.002847],
[0.887500, 1.020125, 1.009684, 1.002219],
[0.912500, 1.023216, 1.009755, 1.004095],
[0.937500, 1.021918, 1.009485, 1.003163],
[0.962500, 1.021135, 1.008662, 1.002997],
[0.987500, 1.021884, 1.009898, 1.002640],
[1.012500, 1.021881, 1.010238, 1.001512],
[1.037500, 1.022427, 1.010629, 1.001892],
[1.062500, 1.023175, 1.008469, 1.001467],
[1.087500, 1.023976, 1.010419, 1.002352],
[1.112500, 1.024020, 1.008886, 1.001504],
[1.137500, 1.020972, 1.008910, 1.001655],
[1.162500, 1.025626, 1.011437, 1.003306],
[1.187500, 1.023514, 1.007424, 1.002046],
[1.212500, 1.026136, 1.011465, 1.002095],
[1.237500, 1.028531, 1.012173, 1.004454],
[1.262500, 1.029725, 1.016158, 1.006691],
[1.287500, 1.032701, 1.015931, 1.009156],
[1.312500, 1.014781, 0.999414, 0.989940],
[1.337500, 1.011585, 0.995132, 0.987085],
[1.362500, 1.002422, 0.982121, 0.974997],
[1.387500, 1.056590, 1.041213, 1.034184],
[1.412500, 1.056115, 1.042419, 1.035585],
[1.437500, 1.069645, 1.049957, 1.046193],
[1.462500, 1.072015, 1.041256, 1.021664],
[1.487500, 1.080305, 1.032888, 1.005876],
[1.512500, 1.088097, 1.047530, 1.022539],
[1.537500, 1.144251, 1.097166, 1.059430],
[1.562500, 1.088718, 1.067452, 1.054971],
[1.587500, 1.108701, 1.084678, 1.070927],
[1.612500, 1.042361, 1.021171, 1.010552],
[1.637500, 1.044935, 1.021864, 1.005121],
[1.662500, 1.043180, 1.020265, 1.006957],
[1.687500, 1.054075, 1.026478, 1.010214],
[1.712500, 1.024707, 1.009226, 1.003792],
[1.737500, 1.020274, 1.008720, 1.000144],
[1.762500, 1.029872, 1.013473, 1.004171],
[1.787500, 1.030367, 1.014129, 1.002666],
[1.812500, 1.049716, 1.022992, 1.009586],
[1.837500, 1.059269, 1.030106, 1.014040],
[1.862500, 1.052397, 1.028403, 1.013067],
[1.887500, 1.043361, 1.022951, 1.011259],
[1.912500, 1.035487, 1.020933, 1.009614],
[1.937500, 1.032210, 1.017783, 1.007004],
[1.962500, 1.030084, 1.013304, 1.005618],
[1.987500, 1.027722, 1.011868, 1.003815],
[2.012500, 1.024316, 1.011412, 1.003317],
[2.037500, 1.030239, 1.016478, 1.007915],
[2.062500, 1.025651, 1.012468, 1.004998],
[2.087500, 1.027130, 1.015003, 1.007200],
[2.112500, 1.031447, 1.017365, 1.010087],
[2.137500, 1.030105, 1.017934, 1.008818],
[2.162500, 1.029508, 1.017105, 1.010475],
[2.187500, 1.031979, 1.019819, 1.010841],
[2.212500, 1.029825, 1.016950, 1.010157],
[2.237500, 1.028244, 1.017778, 1.010382],
[2.262500, 1.029367, 1.017014, 1.011327],
[2.287500, 1.031258, 1.020830, 1.013547],
[2.312500, 1.035904, 1.023196, 1.018105],
[2.337500, 1.035629, 1.022343, 1.018222],
[2.362500, 1.031974, 1.022447, 1.016995],
[2.387500, 1.029765, 1.018095, 1.012742],
[2.412500, 1.029618, 1.021431, 1.015112],
[2.437500, 1.036448, 1.026186, 1.020301],
[2.462500, 1.054524, 1.042935, 1.037817],
[2.487500, 1.125280, 1.100120, 1.089035],
    ]


class CaloSwClcon_dc2_parms:
    region = CALOCORR_CLUSTER
    etamax = 2.5
    degree = 3
    energy_degree = 2
    energies = CaloSw_sample_energies
    use_raw_eta = False
    correction = {'ele55' : CaloSw_clcon_first_55,
                  'ele33' : CaloSw_clcon_first_33,
                  'ele35' : CaloSw_clcon_first_35,
                  'ele37' : CaloSw_clcon_first_37,
                  'ele57' : CaloSw_clcon_first_57,
                  'ele77' : CaloSw_clcon_first_77,

                  'gam55' : CaloSw_clcon_first_55,
                  'gam33' : CaloSw_clcon_first_33,
                  'gam35' : CaloSw_clcon_first_35,
                  'gam37' : CaloSw_clcon_first_37,
                  'gam57' : CaloSw_clcon_first_57,
                  'gam77' : CaloSw_clcon_first_77,
                  }
