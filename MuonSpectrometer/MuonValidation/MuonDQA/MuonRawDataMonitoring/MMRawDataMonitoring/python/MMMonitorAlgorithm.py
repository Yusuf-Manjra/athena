#
#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from MMRawDataMonitoring.MMMonUtils import getMMLabel, getMMLabelY

def MMMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg

    result.merge(MuonGeoModelCfg(inputFlags))

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'MMAthMonitorCfg')
    
    # Adding an algorithm to the helper. 

    mmMonAlg = helper.addAlgorithm(CompFactory.MMRawDataMonAlg,'MMMonAlg')
    mmMonAlg.DoMMESD = True    
    mmMonAlg.do_stereoCorrection=False
    mmMonAlg.cut_pt = 20000

    mmGroup = helper.addGroup(mmMonAlg,'mmMonitor','Muon/MuonRawDataMonitoring/MM/')
    
    # Configure histograms

    # Overview histograms
    mmGroup.defineHistogram('residual;Residuals',  type='TH1F',  title='Residuals; res[mm]; Number of Entries', path='Overview', xbins=200, xmin=-10, xmax=10.)
    mmGroup.defineHistogram('residual,eta_trk;Res_vs_eta', type='TH2F', title="Residuals vs Eta; Residuals [mm]; Eta;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=100, ymin=-3.,ymax=3.)
    mmGroup.defineHistogram('residual,phi_trk;Res_vs_phi', type='TH2F', title="Residuals vs Eta; Residuals [mm]; Phi;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=16, ymin=-3.14,ymax=3.14)
    mmGroup.defineHistogram('residual,stPhi_mon;Res_vs_stPhi', type='TH2F', title="Residuals vs station Phi; Res; stPhi;", path='Overview', xbins=100, xmin=-10, xmax=10., ybins=16, ymin=1,ymax=17)
    mmGroup.defineHistogram('charge_all;Charge', type='TH1F', title='Charge; Charge[fC]; Number of Entries', path='Overview', xbins=120, xmin=0., xmax=1200.)
    mmGroup.defineHistogram('x_mon, y_mon;Posy_vs_Posx', type='TH2F', title="Posy vs Posx; MM-GlobalX [mm]; MM-GlobalY [mm];", path='Overview', xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000., ymax=5000.)
    mmGroup.defineHistogram('R_mon, z_mon;Posz_vs_R', type='TH2F', title="Posz vs R; MM-GlobalR [mm]; MM-GlobalZ [mm];", path='Overview', xbins=500, xmin=0, xmax=5000., ybins=1000, ymin=-8000 ,ymax=8000)
    mmGroup.defineHistogram('numberofstrips_percluster;Number_of_strips_percluster',  type='TH1F', title='Number of strips per cluster; Number of strips; Number of Entries', path='Overview',   xbins=20, xmin=0., xmax=20.)
    mmGroup.defineHistogram('mu_TPC_angle;uTPC_angle', type='TH1F', title='#mu TPC angle; #mu TPC angle [degrees]; Number of Entries', path='Overview',   xbins=2000, xmin=-100, xmax=100)
    mmGroup.defineHistogram('mu_TPC_chi2;uTPC_chi2',  type='TH1F', title='#mu TPC #chi2; #mu TPC #chi2; Number of Entries', path='Overview', xbins=100, xmin=0., xmax=1.)
    mmGroup.defineHistogram('strip_times;time_strip',  type='TH1F', title='strip time; time[ns]; Number of Entries', path='Overview',   xbins=200, xmin=0., xmax=200.)
    mmGroup.defineHistogram('cluster_times;time_cluster',  type='TH1F', title='cluster time; time[ns]; Number of Entries', path='Overview',   xbins=200, xmin=0., xmax=200.)
    mmGroup.defineHistogram('statEta_strip, strip_number;Strip_Number_vs_StationEta', type='TH2F', title='Strip Numbers vs Station Eta; ; Strip Number;', path='Overview', xbins=5, xmin=-2, xmax=3., xlabels=['#eta-2','#eta-1','','#eta1','#eta2'], ybins=5120, ymin=0., ymax=5120.)
    thisLabelx,thisLabely=getMMLabel("x_lab_occ_etaminus1","y_lab_occ_etaminus1")
    mmGroup.defineHistogram('sector_CSide_eta1, stationPhi_CSide_eta1;Occupancy_CSide_eta1_PCB', type='TH2F', title='Occupancy EC eta1; ; Sector;', path='Occupancy', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx)
    mmGroup.defineHistogram('sector_CSide_eta1_ontrack, stationPhi_CSide_eta1_ontrack;Occupancy_CSide_eta1_PCB_ontrack', type='TH2F', title='Occupancy EC eta1 ontrack; ;Sector ;', path='Occupancy_ontrack', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx)
    thisLabelx1,thisLabely1=getMMLabel("x_lab_occ_etaminus2","y_lab_occ_etaminus2")
    mmGroup.defineHistogram('sector_CSide_eta2, stationPhi_CSide_eta2;Occupancy_CSide_eta2_PCB', type='TH2F', title='Occupancy EC eta2; ;Sector;', path='Occupancy', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx1)
    mmGroup.defineHistogram('sector_CSide_eta2_ontrack, stationPhi_CSide_eta2_ontrack;Occupancy_CSide_eta2_PCB_ontrack', type='TH2F', title='Occupancy EC eta2 ontrack; ;Sector;', path='Occupancy_ontrack', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx1)
    thisLabelx2,thisLabely2=getMMLabel("x_lab_occ_eta1","y_lab_occ_eta1")
    mmGroup.defineHistogram('sector_ASide_eta1, stationPhi_ASide_eta1;Occupancy_ASide_eta1_PCB', type='TH2F', title='Occupancy EA eta1; ;Sector;', path='Occupancy', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx2)
    mmGroup.defineHistogram('sector_ASide_eta1_ontrack, stationPhi_ASide_eta1_ontrack;Occupancy_ASide_eta1_PCB_ontrack', type='TH2F', title='Occupancy EA eta1 ontrack; ;Sector;', path='Occupancy_ontrack', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx2)
    thisLabelx3,thisLabely3=getMMLabel("x_lab_occ_eta2","y_lab_occ_eta2")
    mmGroup.defineHistogram('sector_ASide_eta2, stationPhi_ASide_eta2;Occupancy_ASide_eta2_PCB', type='TH2F', title='Occupancy EA eta2; ;Sector;', path='Occupancy', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx3)
    mmGroup.defineHistogram('sector_ASide_eta2_ontrack, stationPhi_ASide_eta2_ontrack;Occupancy_ASide_eta2_PCB_ontrack', type='TH2F', title='Occupancy EA eta2  ontrack; ;Sector;', path='Occupancy_ontrack', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17, xlabels=thisLabelx3)

    side = ["CSide","ASide"]
    etasector  = ["1","2"]
    for iside in side:
        
        if iside=="ASide":
            thisLabelx11,thisLabely11=getMMLabel("x_lab_occ_ASide","y_lab_occ_ASide")
        if iside=="CSide":
            thisLabelx11,thisLabely11=getMMLabel("x_lab_occ_CSide","y_lab_occ_CSide")

        MM_SideGroup="MM_sideGroup{0}".format(iside)
        mmSideGroup=helper.addGroup(mmMonAlg, MM_SideGroup, "Muon/MuonRawDataMonitoring/MM/"+iside)
        
        # Histograms for each sector
        phimax=16
        multipletmin=1
        multipletmax=2
        for eta in etasector:
            maxpcb=5
            if(eta == "2"):
                maxpcb=3
            for multi in range(multipletmin, multipletmax+1):
                for gas_gap in range(1,5):
                    #efficiency per pcb      
                    title_allphi_eff="Efficiency_per_PCB"+"Eta"+str(eta)+"_"+iside+"_allphi"+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                    var_pcb_allphi="hitcut,pcb_eta"+str(eta)+"_allphi_"+iside+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                    mmSideGroup.defineHistogram(var_pcb_allphi, type='TEfficiency', title=title_allphi_eff+"; pcb ;Efficiency Eta1",path='Efficiency', xbins=maxpcb, xmin=0, xmax=maxpcb) 
        for phi in range(1, phimax+1):
            title_MMSummary="Number of strips per cluster, E"+iside+" "+" stPhi "+str(phi)   
            var="sector_strip_"+iside+"_phi"+str(phi)+", strip_number_"+iside+"_phi"+str(phi)+";Strip_number_pergap_"+iside+"_stPhi"+str(phi)
            mmSideGroup.defineHistogram(var, type='TH2F', title=title_MMSummary+"; ;Strip Number", path='Number_of_strips_percluster_perPhiSector', xbins=16, xmin=0, xmax=16, xlabels=thisLabelx11, ybins=5120, ymin=0., ymax=5120.)
            thisLabely=getMMLabelY("y_lab_occ_lb")
            mmSideGroup.defineHistogram('lb_mon,sector_lb_'+iside+'_phi'+str(phi)+';Occupancy_lb_'+iside+'_phi'+str(phi), type='TH2F', title="Occupancy wrt lb sector"+str(phi)+"; LB; PCB", path='Occupancy',  xbins=100, xmin=-0.5, xmax=99.5, opt='kAddBinsDynamically', ybins=64, ymin=0., ymax=64, ylabels=thisLabely)
            mmSideGroup.defineHistogram('lb_ontrack,sector_lb_'+iside+'_phi'+str(phi)+"_ontrack"+';Occupancy_lb_'+iside+'_phi'+str(phi)+"_ontrack", type='TH2F', title="Occupancy wrt lb sector"+str(phi)+" ontrack"+"; LB; PCB", path='Occupancy',  xbins=100, xmin=-0.5, xmax=99.5, opt='kAddBinsDynamically', ybins=64, ymin=0., ymax=64, ylabels=thisLabely)


            for eta in etasector:
                maxpcb=5
                if(eta == "2"):
                    maxpcb=3
                for multi in range(multipletmin, multipletmax+1):
                    for gas_gap in range(1,5):
                        #efficiency per pcb      
                        title_eff="Efficiency_per_PCB"+"Eta"+str(eta)+"_"+iside+"_phi"+str(phi-1)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                        var_pcb="hitcut, pcb_eta"+str(eta)+"_"+iside+"_phi"+str(phi-1)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                        mmSideGroup.defineHistogram(var_pcb, type='TEfficiency', title=title_eff+"; pcb; Efficiency Eta1", path='Efficiency', xbins=maxpcb, xmin=0, xmax=maxpcb)
                        title_MMSummary_charge="Charge "+iside+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                        var1="charge_"+iside+"_sectorphi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)+";Charge_"+iside+"_stPhi"+str(phi)+"_stEta"+str(eta)+"_multiplet"+str(multi)+"_gap"+str(gas_gap)
                        mmSideGroup.defineHistogram(var1, type='TH1F', title=title_MMSummary_charge+';Charge [fC]; Number of Entries', path='Charge_perLayer', xbins=120, xmin=0., xmax=1200.)
                        title_MMSummary_angle="uTPC angle "+iside+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                        var3="mu_TPC_angle_"+iside+"_sectorphi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)+";uTPCangle_"+iside+"_stPhi"+str(phi)+"_stEta"+str(eta)+"_multiplet"+str(multi)+"_gap"+str(gas_gap)
                        mmSideGroup.defineHistogram(var3, type='TH1F', title=title_MMSummary_angle+"; #muTPC angle [degrees];Number of Entries", path='uTPC_angle_perLayer', xbins=2000, xmin=-100, xmax=100)
                        
                        var_residual="residuals_"+iside+"_phi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                        title_residual = "residuals "+iside+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                        mmSideGroup.defineHistogram(var_residual, type='TH1F', title=title_residual+"; res [mm]; Number of Entries", path='Residuals', xbins=200, xmin=-10, xmax=10)

                        var_clus_size="pcb_mon_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) + ",cluster_size_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) +";Cluster_size_vs_PCB_" + iside + "_eta" + str(eta) + "_phi" + str(phi) + "_ml" + str(multi) + "_gap" + str(gas_gap)
                        title_cl_size="Cluster size " + iside + " eta" + str(eta) + " phi" + str(phi) + " mult" + str(multi) + " gasgap" + str(gas_gap)
                        mmSideGroup.defineHistogram(var_clus_size, type='TH2F', title=title_cl_size + "; PCB; cluster size;", path="Cluster_size_perPCB", xbins=maxpcb, xmin=1, xmax=maxpcb+1, ybins=20, ymin=0, ymax=20)

                        var_strip_time="pcb_mon_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) + ",strp_time_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) +";Strip_time_vs_PCB_" + iside + "_eta" + str(eta) + "_phi" + str(phi) + "_ml" + str(multi) + "_gap" + str(gas_gap)
                        var_clus_time="pcb_mon_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) + ",cluster_time_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) +";Cluster_time_vs_PCB_" + iside + "_eta" + str(eta) + "_phi" + str(phi) + "_ml" + str(multi) + "_gap" + str(gas_gap)
                        title_strp_time="Strip time " + iside + " eta" + str(eta) + " phi" + str(phi) + " mult" + str(multi) + " gasgap" + str(gas_gap)
                        title_clus_time="Cluster time " + iside + " eta" + str(eta) + " phi" + str(phi) + " mult" + str(multi) + " gasgap" + str(gas_gap)
                        mmSideGroup.defineHistogram(var_strip_time, type='TH2F', title=title_strp_time + "; PCB; strip time [ns];", path='Strip_time_perPCB', xbins=maxpcb, xmin=1, xmax=maxpcb+1, ybins=200, ymin=0, ymax=200)
                        mmSideGroup.defineHistogram(var_clus_time, type='TH2F', title=title_clus_time + "; PCB; cluster time [ns];", path='Cluster_time_perPCB', xbins=maxpcb, xmin=1, xmax=maxpcb+1, ybins=200, ymin=0, ymax=200)

                        var_charge_perPCB="pcb_mon_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) + ",charge_perPCB_" + iside + "_phi" + str(phi) + "_eta" + str(eta) + "_ml" + str(multi) + "_gap" + str(gas_gap) +";Charge_vs_PCB_" + iside + "_eta" + str(eta) + "_phi" + str(phi) + "_ml" + str(multi) + "_gap" + str(gas_gap)
                        title_charge_perPCB="Charge " + iside + " eta" + str(eta) + " phi" + str(phi) + " mult" + str(multi) + " gasgap" + str(gas_gap)
                        mmSideGroup.defineHistogram(var_charge_perPCB, type='TH2F', title=title_charge_perPCB + "; PCB; charge [fC];", path='Charge_perPCB', xbins=maxpcb, xmin=1, xmax=maxpcb+1, ybins=120, ymin=0, ymax=1200)
        for gas1 in range(1, 5):
            for multi1 in range(1, 3):
                title_ontrack="Posy vs Posx E"+iside+" multiplet"+str(multi1)+" gap"+str(gas1)+" ontrack"
                var_ontrack="x_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack,y_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack;Posy_vs_Posx_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack"
                mmSideGroup.defineHistogram(var_ontrack, type='TH2F', title=title_ontrack+"; MM-GlobalX [mm]; MM-GlobalY [mm];", path='PosY_vs_Posx_perLayer_ontrack',xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000.,ymax=5000.)

    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Constants import DEBUG
    
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files =['/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000001.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000002.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000003.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000004.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000005.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000006.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000007.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000008.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.ParticleGun_DiMu_Pt10to100.ESD.Run3_2NSW_160122a_reco_22050_EXT0/group.det-muon.27853056.EXT0._000009.ESD.pool.root']                           

    #from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    ConfigFlags.Output.HISTFileName = 'monitor.root'

    ConfigFlags.Detector.GeometryMM=True
    ConfigFlags.Muon.doMicromegas = True
    ConfigFlags.DQ.useTrigger=False

    ConfigFlags.lock()
    ConfigFlags.dump()
    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    mmMonitorAcc  =  MMMonitoringConfig(ConfigFlags)
    mmMonitorAcc.OutputLevel=DEBUG
    cfg.merge(mmMonitorAcc)
    #cfg.printConfig(withDetails=True, summariseProps = True)
    # number of events selected in the ESD
    cfg.run(5000)
