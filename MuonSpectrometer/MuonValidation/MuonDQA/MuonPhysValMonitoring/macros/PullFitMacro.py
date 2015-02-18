# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#author: Dan Mori
#Add gaussian curve to pull plots
#if plotdir argument given, will output pdfs in that directory of all pull plots
#usage: python PullFitMacro.py physval_file.root [plotdir]

import ROOT
import sys
import os
import itertools

#---------------------------------------------------------------------------

def AddGaussian( infile, HistDir, HistName ):
    hist = infile.GetDirectory(HistDir).Get(HistName)
    if hist.GetEntries() < 10:
        print('INFO Skipping Fit. NumEntries = {0}: '.format(hist.GetEntries()) + HistName)
        return
    f = ROOT.TF1( "gaus", "gaus", -5, 5 )
    hist.Fit(f,"RQ")
    f1 = hist.GetFunction("gaus")
    f1.SetLineColor(ROOT.kRed)
    print('INFO Overwriting histogram: ' + HistDir+'/'+HistName )
    hist.GetDirectory().WriteTObject( hist, HistName, "Overwrite" )

#---------------------------------------------------------------------------

def main( args ):
    if len( args ) > 1:
        filename = args[1]
        if len( args ) > 2:
            PlotDir = os.path.dirname( args[2] )
            print( 'INFO Plots will be saved to {0}'.format( PlotDir ) )
            if not os.path.exists(PlotDir):
                os.makedirs(PlotDir)
        else:
            PlotDir = ''

    else:
        print( 'Usage: python {0} filename [plot directory]'.format( args[0] ) )
        return

    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        return

    print( 'Opening file: ' + filename )
    infile = ROOT.TFile.Open( filename, 'update' )

    MuonTypes = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    PullTypes = [ 'Pulls', 'PullsID', 'PullsMS' ]
    Variables = [ 'phi0', 'theta', 'qOverP', 'P', 'd0', 'z0' ]

    for MuonType in MuonTypes:
        if not infile.Get( 'Muons/' + MuonType ):
#            print( 'INFO TDirectory not found: Muons/' + MuonType )
            continue
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( MuonType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for Author in Authors:
            if not infile.Get( 'Muons/{0}/matched/{1}'.format( MuonType, Author ) ):
#                print( 'INFO TDirectory not found: ' + 'Muons/{0}/matched/{1}'.format( MuonType, Author ) )
                continue
            for PullType in PullTypes:
                HistDir = 'Muons/{0}/matched/{1}/{2}'.format( MuonType, Author, PullType )
                FileDir = infile.Get( HistDir )
                if not FileDir:
                    print( 'INFO TDirectory not found: ' + HistDir )
                    continue
                for var in Variables:
                    HistName = '_'.join(HistDir.split('/')) + '_Pull_' + var
                    FileHist = FileDir.Get( HistName )
                    if not FileHist:
#                        print( 'INFO Histogram not found: ' + HistName )
                        continue
                    AddGaussian( infile, HistDir, HistName )

    infile.Close()

    if PlotDir != '':
        infile = ROOT.TFile.Open( filename )
        can = ROOT.TCanvas()
        infile.GetDirectory(HistDir).Get(HistName).Draw()
        can.SaveAs( PlotDir + '/' + HistName + '.pdf' )
        infile.Close()

#---------------------------------------------------------------------------

if __name__ == "__main__":
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main( sys.argv )
