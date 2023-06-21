#
#Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
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
    mmMonAlg.cut_pt = 0
    mmMonAlg.doDetailedHists = True

    mmGroup = helper.addGroup(mmMonAlg,'mmMonitor','Muon/MuonRawDataMonitoring/MM/')
    
    # Configure histograms

    # Overview histogram
    mmGroup.defineHistogram('nseg;nseg',  type='TH1F',  title='Number_of_segments; Number_of_segments; Number of Entries', path='Overview', xbins=100, xmin=0, xmax=100., opt='kAlwaysCreate')
    mmGroup.defineHistogram('ntrk;ntrk',  type='TH1F',  title='Number_of_tracks; Number_of_tracks; Number of Entries', path='Overview', xbins=100, xmin=0, xmax=100., opt='kAlwaysCreate')
    mmGroup.defineHistogram('pt_trk;pt',  type='TH1F',  title='pt; pt[GeV]; Number of Entries', path='Overview', xbins=200, xmin=0, xmax=400., opt='kAlwaysCreate')
    mmGroup.defineHistogram('residual;Residuals',  type='TH1F',  title='Residuals; res[mm]; Number of Entries', path='Overview', xbins=200, xmin=-10, xmax=10., opt='kAlwaysCreate')
    mmGroup.defineHistogram('residual,eta_trk;Res_vs_eta', type='TH2F', title="Residuals vs Eta; Residuals [mm]; Eta;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=100, ymin=-3.,ymax=3., opt='kAlwaysCreate')
    mmGroup.defineHistogram('residual,phi_trk;Res_vs_phi', type='TH2F', title="Residuals vs Eta; Residuals [mm]; Phi;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=16, ymin=-3.14,ymax=3.14, opt='kAlwaysCreate')
    mmGroup.defineHistogram('residual,stPhi_mon;Res_vs_stPhi', type='TH2F', title="Residuals vs station Phi; Res; stPhi;", path='Overview', xbins=100, xmin=-10, xmax=10., ybins=16, ymin=.5,ymax=16.5, opt='kAlwaysCreate')
    mmGroup.defineHistogram('charge_all;Charge', type='TH1F', title='Charge; Charge[fC]; Number of Entries', path='Overview', xbins=200, xmin=0., xmax=800., opt='kAlwaysCreate')
    mmGroup.defineHistogram('x_mon, y_mon;Posy_vs_Posx', type='TH2F', title="Posy vs Posx; MM-GlobalX [mm]; MM-GlobalY [mm];", path='Overview', xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000., ymax=5000., opt='kAlwaysCreate')
    mmGroup.defineHistogram('R_mon, z_mon;Posz_vs_R', type='TH2F', title="Posz vs R; MM-GlobalR [mm]; MM-GlobalZ [mm];", path='Overview', xbins=500, xmin=0, xmax=5000., ybins=1000, ymin=-8000 ,ymax=8000, opt='kAlwaysCreate')
    mmGroup.defineHistogram('numberofstrips_percluster;Nstrips_percluster',  type='TH1F', title='Number of strips per cluster; Number of strips; Number of Entries', path='Overview', xbins=20, xmin=0., xmax=20., opt='kAlwaysCreate')
    mmGroup.defineHistogram('strip_times;strip_time',  type='TH1F', title='strip time; time [ns]; Number of Entries', path='Overview',   xbins=800, xmin=-100., xmax=700., opt='kAlwaysCreate')
    mmGroup.defineHistogram('cluster_times;cluster_time',  type='TH1F', title='cluster time; time [ns]; Number of Entries', path='Overview',   xbins=700, xmin=-200., xmax=500., opt='kAlwaysCreate')
    mmGroup.defineHistogram('statEta_strip, strip_number;Strip_Number_vs_StationEta', type='TH2F', title='Strip Numbers vs Station Eta; ; Strip Number;', path='Overview', xbins=5, xmin=-2, xmax=3., xlabels=['#eta-2','#eta-1','','#eta1','#eta2'], ybins=5120, ymin=0., ymax=5120., opt='kAlwaysCreate')
    mmGroup.defineHistogram('nstrips_ontrack;Nstrips_percluster_ontrack', type='TH1F', title='Number of strips per cluster on track; Number of strips; Number of Entries', path='Overview', xbins=20, xmin=0., xmax=20., opt='kAlwaysCreate')
    mmGroup.defineHistogram('charge_ontrack;Charge_ontrack', type='TH1F', title='Charge on track; Charge [fC]; Number of Entries', path='Overview', xbins=200, xmin=0, xmax=800., opt='kAlwaysCreate')
    mmGroup.defineHistogram('strip_time_on_track', type='TH1F', title='strip time on track; time [ns]; Number of Entries', path='Overview', xbins=800, xmin=-100., xmax=700., opt='kAlwaysCreate')
    mmGroup.defineHistogram('cluster_time_on_track', type='TH1F', title='cluster time on track; time [ns]; Number of Entries', path='Overview', xbins=700, xmin=-200, xmax=500., opt='kAlwaysCreate')



    mmGroup.defineHistogram('cluster_size_onseg;Nstrips_percluster_onseg', type='TH1F', title='Number of strips per cluster on segments; Number of strips; Number of Entries', path='Overview', xbins=20, xmin=0., xmax=20., opt='kAlwaysCreate')
    mmGroup.defineHistogram('charge_onseg;Charge_onseg', type='TH1F', title='Charge on segments; Charge[fC]; Number of Entries', path='Overview', xbins=200, xmin=0., xmax=800., opt='kAlwaysCreate')
    mmGroup.defineHistogram('strp_time_onseg;strip_time_onseg',  type='TH1F', title='strip time on segments; time [ns]; Number of Entries', path='Overview',   xbins=800, xmin=-100, xmax=700., opt='kAlwaysCreate')
    
    
    occLabelx=getMMLabelY("x_lab_occ")
    mmGroup.defineHistogram('sector_CSide,stationPhi_CSide;Occupancy_CSide_PCB', type='TH2F', title='Occupancy EC; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5, xlabels=occLabelx, opt='kAlwaysCreate')
    mmGroup.defineHistogram('sector_ASide,stationPhi_ASide;Occupancy_ASide_PCB', type='TH2F', title='Occupancy EA; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5,  xlabels=occLabelx, opt='kAlwaysCreate')
    mmGroup.defineHistogram('sector_CSide_ontrack,stationPhi_CSide_ontrack;Occupancy_CSide_PCB_ontrack', type='TH2F', title='Occupancy EC ontrack; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5, xlabels=occLabelx, opt='kAlwaysCreate')
    mmGroup.defineHistogram('sector_ASide_ontrack,stationPhi_ASide_ontrack;Occupancy_ASide_PCB_ontrack', type='TH2F', title='Occupancy EA ontrack; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5,  xlabels=occLabelx, opt='kAlwaysCreate')
    mmGroup.defineHistogram('sector_CSide_onseg,stationPhi_CSide_onseg;Occupancy_CSide_PCB_onseg', type='TH2F', title='Occupancy EC onseg; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5, xlabels=occLabelx, opt='kAlwaysCreate')
    mmGroup.defineHistogram('sector_ASide_onseg,stationPhi_ASide_onseg;Occupancy_ASide_PCB_onseg', type='TH2F', title='Occupancy EA onseg; ; Sector;', path='Occupancy', xbins=64, xmin=0, xmax=128., ybins=16, ymin=.5, ymax=16.5,  xlabels=occLabelx, opt='kAlwaysCreate')
    
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
            var_layer=f'hitcut,{iside}_eta{eta}'
            title_eff_layer=f'Efficiency_per_layer_Eta{eta}_{iside};  layer Sector = int(layer/8) ; Efficiency;'
            #occLabelx=getMMLabelY("x_layer_in_sector")
            mmSideGroup.defineHistogram(var_layer, type='TEfficiency', title=title_eff_layer, path='Efficiency_perLayer', xbins=128, xmin=0, xmax=128, opt='kAlwaysCreate')
            
        for phi in range(1, phimax+1):
            title_MMSummary=f'Number of strips per cluster E{iside} stPhi {phi}; ; Strip Number'
            var=f'sector_strip_{iside}_phi{phi},strip_number_{iside}_phi{phi};Strip_number_pergap_{iside}_stPhi{phi}'
            mmSideGroup.defineHistogram(var, type='TH2F', title=title_MMSummary, path='Number_of_strips_percluster_perPhiSector', xbins=16, xmin=0, xmax=16, xlabels=thisLabelx11, ybins=5120, ymin=0., ymax=5120., opt='kAlwaysCreate')

            thisLabely=getMMLabelY("x_lab_occ")
            mmSideGroup.defineHistogram(f'lb_mon,sector_lb_{iside}_phi{phi};Occupancy_lb_{iside}_phi{phi}', type='TH2F', title=f'Occupancy wrt lb sector {phi}; LB; PCB FEB', path='Occupancy',  xbins=100, xmin=-0.5, xmax=99.5, opt='kAddBinsDynamically,kAlwaysCreate', ybins=128, ymin=0., ymax=128, ylabels=thisLabely, merge='merge')
            mmSideGroup.defineHistogram(f'lb_ontrack,sector_lb_{iside}_phi{phi}_ontrack;Occupancy_lb_{iside}_phi{phi}_ontrack', type='TH2F', title=f'Occupancy wrt lb sector {phi} ontrack ; LB; PCB FEB', path='Occupancy',  xbins=100, xmin=-0.5, xmax=99.5, opt='kAddBinsDynamically,kAlwaysCreate', ybins=128, ymin=0., ymax=128, ylabels=thisLabely, merge='merge')
            mmSideGroup.defineHistogram(f'lb_onseg,sector_lb_{iside}_phi{phi}_onseg;Occupancy_lb_{iside}_phi{phi}_onseg', type='TH2F', title=f'Occupancy wrt lb sector {phi} onSegms; LB; PCB FEB', path='Segments/Occupancy',  xbins=100, xmin=-0.5, xmax=99.5, opt='kAddBinsDynamically,kAlwaysCreate', ybins=128, ymin=0., ymax=128, ylabels=thisLabely, merge='merge')
                
            for multi in range(multipletmin, multipletmax+1):
                for gas_gap in range(1,5):
                    layer = gas_gap
                    if (multi == 2):
                        layer=gas_gap+4
                    if mmMonAlg.doDetailedHists:
                        title_MMSummary_charge=f'Charge {iside} stPhi{phi} multiplet{multi} layer{layer}; Charge [fC]; Number of Entries'
                        var1=f'charge_perlayer_{iside}_phi{phi}_ml{multi}_gap{gas_gap};Charge_perlayer_{iside}_phi{phi}_ml{multi}_gap{gas_gap}'
                        mmSideGroup.defineHistogram(var1, type='TH1F', title=title_MMSummary_charge, path='Charge_perLayer', xbins=200, xmin=0., xmax=600., opt='kAlwaysCreate')

                        title_MMSummary_charge_ontrack=f'Charge_ontrack_perlayer_{iside}_phi{phi}_ml{multi}_gap{gas_gap}; Charge [fC]; Number of Entries'
                        var2=f'charge_perlayer_ontrack_{iside}_phi{phi}_ml{multi}_gap{gas_gap};Charge_ontrack_perlayer_{iside}_phi{phi}_ml{multi}_gap{gas_gap}'
                        mmSideGroup.defineHistogram(var2, type='TH1F', title=title_MMSummary_charge_ontrack, path='Charge_perLayer', xbins=200, xmin=0., xmax=600., opt='kAlwaysCreate')

                        title_cl_size_layer=f'Cluster size {iside}  phi{phi} mult{multi} layer{layer}; cluster size; Entries'
                        var_clus_size_layer=f'cluster_size_perlayer_{iside}_phi{phi}_ml{multi}_gap{gas_gap};Cluster_size_{iside}_phi{phi}_ml{multi}_gap{gas_gap}'
                        mmSideGroup.defineHistogram(var_clus_size_layer, type='TH1F', title=title_cl_size_layer, path="Cluster_size_perLayer", xbins=20, xmin=0, xmax=20, opt='kAlwaysCreate')
                        title2_cl_size_layer=f'Cluster size ontrack {iside}  phi{phi} mult{multi} layer{layer}; cluster size; Entries'
                        var2_clus_size_layer=f'cluster_size_perlayer_ontrack_{iside}_phi{phi}_ml{multi}_gap{gas_gap};Cluster_size_ontrack_{iside}_phi{phi}_ml{multi}_gap{gas_gap}'
                        mmSideGroup.defineHistogram(var2_clus_size_layer, type='TH1F', title=title2_cl_size_layer, path="Cluster_size_perLayer", xbins=20, xmin=0, xmax=20, opt='kAlwaysCreate')

                        var_residual_layer=f'residuals_{iside}_phi{phi}_layer{layer}'
                        title_residual_layer =f'residuals{iside} stPhi{phi}  layer{layer}; res [mm]; Number of Entries'
                        mmSideGroup.defineHistogram(var_residual_layer, type='TH1F', title=title_residual_layer, path='Residuals_perLayer', xbins=200, xmin=-10, xmax=10, opt='kAlwaysCreate')
                                        

                for eta in etasector:
                    maxpcb=5
                    if(eta == "2"):
                        maxpcb=3
            
                    for gas_gap in range(1,5):
                        #efficiency per pcb
                        if mmMonAlg.doDetailedHists:
                            title_eff=f'Efficiency_per_PCB_Eta{eta}_{iside}_phi{phi-1}_multiplet{multi}_gas_gap{gas_gap}; pcb; Efficiency Eta{eta}'
                            var_pcb=f'hitcut,pcb_eta{eta}_{iside}_phi{phi-1}_multiplet{multi}_gas_gap{gas_gap}'
                            mmSideGroup.defineHistogram(var_pcb, type='TEfficiency', title=title_eff, path='Efficiency', xbins=maxpcb, xmin=0, xmax=maxpcb, opt='kAlwaysCreate')
                        
                            var_residual=f'residuals_{iside}_phi{phi}_stationEta{eta}_multiplet{multi}_gas_gap{gas_gap}'
                            title_residual =f'residuals{iside} stPhi{phi} stEta{eta} multiplet{multi} gap{gas_gap}; res [mm]; Number of Entries'
                            mmSideGroup.defineHistogram(var_residual, type='TH1F', title=title_residual, path='Residuals', xbins=200, xmin=-10, xmax=10, opt='kAlwaysCreate')

                            var_clus_size=f'pcb_mon_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_size_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_size_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_cl_size=f'Cluster size {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap}; PCB; cluster size'
                            mmSideGroup.defineHistogram(var_clus_size, type='TH2F', title=title_cl_size, path="Cluster_size_perPCB", xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=20, ymin=0, ymax=20, opt='kAlwaysCreate')

                            var_strip_time=f'pcb_strip_mon_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},strp_time_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Strip_time_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            var_clus_time=f'pcb_mon_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_time_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_time_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_strp_time=f'Strip time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap}; PCB; strip time [ns]'
                            title_clus_time=f'Cluster time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap}; PCB; cluster time [ns]'
                            mmSideGroup.defineHistogram(var_strip_time, type='TH2F', title=title_strp_time, path='Strip_time_perPCB', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=800, ymin=-100, ymax=700, opt='kAlwaysCreate')
                            mmSideGroup.defineHistogram(var_clus_time, type='TH2F', title=title_clus_time, path='Cluster_time_perPCB', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=700, ymin=-200, ymax=500, opt='kAlwaysCreate')

                            var_charge_perPCB=f'pcb_mon_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},charge_perPCB_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Charge_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_charge_perPCB=f'Charge {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap}; PCB; charge [fC]'
                            mmSideGroup.defineHistogram(var_charge_perPCB, type='TH2F', title=title_charge_perPCB, path='Charge_perPCB', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=200, ymin=0, ymax=800, opt='kAlwaysCreate')

                            var_clus_size_ontrack=f'pcb_mon_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_size_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_size_vs_PCB_ontrack_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_clus_size_ontrack=f'Cluster size {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on track; PCB; cluster size'
                            mmSideGroup.defineHistogram(var_clus_size_ontrack, type='TH2F', title=title_clus_size_ontrack, path='Cluster_size_ontrack', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=20, ymin=0, ymax=20, opt='kAlwaysCreate')

                            var_strip_time_ontrack=f'pcb_strip_mon_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},strp_time_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Strip_time_ontrack_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            var_clus_time_ontrack=f'pcb_mon_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_time_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_time_ontrack_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_strp_time_ontrack=f'Strip time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on track; PCB; strip time [ns]'
                            title_clus_time_ontrack=f'Cluster time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on track; PCB; cluster time [ns]'
                            mmSideGroup.defineHistogram(var_strip_time_ontrack, type='TH2F', title=title_strp_time_ontrack, path='Strip_time_ontrack', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=800, ymin=-100, ymax=700, opt='kAlwaysCreate')
                            mmSideGroup.defineHistogram(var_clus_time_ontrack, type='TH2F', title=title_clus_time_ontrack, path='Cluster_time_ontrack', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=700, ymin=-200, ymax=500, opt='kAlwaysCreate')

                            var_charge_perPCB_ontrack=f'pcb_mon_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},charge_perPCB_ontrack_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Charge_vs_PCB_ontrack_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_charge_perPCB_ontrack=f'Charge {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on track; PCB; charge [fC]'
                            mmSideGroup.defineHistogram(var_charge_perPCB_ontrack, type='TH2F', title=title_charge_perPCB_ontrack, path='Charge_ontrack', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=200, ymin=0, ymax=800, opt='kAlwaysCreate')

                            var_clus_size_onseg=f'pcb_mon_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_size_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_size_vs_PCB_onseg_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_clus_size_onseg=f'Cluster size {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on segms; PCB; cluster size'
                            mmSideGroup.defineHistogram(var_clus_size_onseg, type='TH2F', title=title_clus_size_onseg, path='Segments/Cluster_size_onseg', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=20, ymin=0, ymax=20, opt='kAlwaysCreate')
                            var_strip_time_onseg=f'pcb_strip_mon_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},strp_time_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Strip_time_onseg_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_strp_time_onseg=f'Strip time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on segms; PCB; strip time [ns]'
                            mmSideGroup.defineHistogram(var_strip_time_onseg, type='TH2F', title=title_strp_time_onseg, path='Segments/Strip_time_onseg', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=800, ymin=-100, ymax=700, opt='kAlwaysCreate')
                            title_clus_time_onseg=f'Cluster time {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on segms; PCB; cluster time [ns]'
                            var_clus_time_onseg=f'pcb_mon_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},cluster_time_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Cluster_time_onseg_vs_PCB_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            mmSideGroup.defineHistogram(var_clus_time_onseg, type='TH2F', title=title_clus_time_onseg, path='Segments/Cluster_time_onseg', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=700, ymin=-200, ymax=500, opt='kAlwaysCreate')
                            var_charge_perPCB_onseg=f'pcb_mon_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap},charge_perPCB_onseg_{iside}_phi{phi}_eta{eta}_ml{multi}_gap{gas_gap};Charge_vs_PCB_onseg_{iside}_eta{eta}_phi{phi}_ml{multi}_gap{gas_gap}'
                            title_charge_perPCB_onseg=f'Charge {iside} eta{eta} phi{phi} mult{multi} gasgap{gas_gap} on seg; PCB; charge [fC]'
                            mmSideGroup.defineHistogram(var_charge_perPCB_onseg, type='TH2F', title=title_charge_perPCB_onseg, path='Segments/Charge_onseg', xbins=maxpcb, xmin=.5, xmax=maxpcb+.5, ybins=200, ymin=0, ymax=800, opt='kAlwaysCreate')


        for gas1 in range(1, 5):
            for multi1 in range(1, 3):
                title_ontrack=f'Posy vs Posx E{iside} multiplet{multi1} gap{gas1} ontrack; MM-GlobalX [mm]; MM-GlobalY [mm]'
                var_ontrack=f'x_{iside}_multiplet{multi1}_gas_gap_{gas1}_ontrack,y_{iside}_multiplet{multi1}_gas_gap_{gas1}_ontrack;Posy_vs_Posx_{iside}_multiplet{multi1}_gas_gap_{gas1}_ontrack'
                mmSideGroup.defineHistogram(var_ontrack, type='TH2F', title=title_ontrack, path='PosY_vs_Posx_perLayer_ontrack',xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000.,ymax=5000., opt='kAlwaysCreate')

    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    from AthenaCommon.Constants import DEBUG
    
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    flags = initConfigFlags()
    #flags.Input.Files =['/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuonGenerator_EtaGtr_1p2_Pt10to100.ESD.Run3_2NSW_250322_reco_n270322_EXT0/group.det-muon.28531270.EXT0._000004.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuonGenerator_EtaGtr_1p2_Pt10to100.ESD.Run3_2NSW_250322_reco_n270322_EXT0/group.det-muon.28531270.EXT0._000008.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuonGenerator_EtaGtr_1p2_Pt10to100.ESD.Run3_2NSW_250322_reco_n270322_EXT0/group.det-muon.28531270.EXT0._000013.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuonGenerator_EtaGtr_1p2_Pt10to100.ESD.Run3_2NSW_250322_reco_n270322_EXT0/group.det-muon.28531270.EXT0._000020.ESD.pool.root','/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuonGenerator_EtaGtr_1p2_Pt10to100.ESD.Run3_2NSW_250322_reco_n270322_EXT0/group.det-muon.28531270.EXT0._000022.ESD.pool.root']
    
    flags.Input.Files =[ 
        '/eos/atlas/atlastier0/rucio/data23_13p6TeV/express_express/00451735/data23_13p6TeV.00451735.express_express.recon.ESD.x738/data23_13p6TeV.00451735.express_express.recon.ESD.x738._lb0800._SFO-ALL._0001.1',
        '/eos/atlas/atlastier0/rucio/data23_13p6TeV/express_express/00451735/data23_13p6TeV.00451735.express_express.recon.ESD.x738/data23_13p6TeV.00451735.express_express.recon.ESD.x738._lb0801._SFO-ALL._0001.1',
        '/eos/atlas/atlastier0/rucio/data23_13p6TeV/express_express/00451735/data23_13p6TeV.00451735.express_express.recon.ESD.x738/data23_13p6TeV.00451735.express_express.recon.ESD.x738._lb0802._SFO-ALL._0001.1',
        '/eos/atlas/atlastier0/rucio/data23_13p6TeV/express_express/00451735/data23_13p6TeV.00451735.express_express.recon.ESD.x738/data23_13p6TeV.00451735.express_express.recon.ESD.x738._lb0803._SFO-ALL._0001.1'
        ]

    flags.Output.HISTFileName = 'monitor.root'

    flags.Detector.GeometryMM=True
    flags.DQ.useTrigger=False
    flags.Input.isMC = False
    if not flags.Input.isMC:
        flags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2022-10"


    flags.lock()
    flags.dump()
    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(flags)
    cfg.merge(PoolReadCfg(flags))

    
    mmMonitorAcc  =  MMMonitoringConfig(flags)
    mmMonitorAcc.OutputLevel=DEBUG
    cfg.merge(mmMonitorAcc)
    #cfg.printConfig(withDetails=True, summariseProps = True)
    # number of events selected in the ESD
    cfg.run(-1)
