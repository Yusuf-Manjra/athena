#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

__author__ = "Will Buttinger"
__doc__ = """Extract dataset parameters from AMI, and write them to a text file.\nExamples:\n\n\ngetAMIDatasetParameters.py --inDS="mc15_13TeV.361103%DAOD_TRUTH%" --fields=dataset_number,ldn,nfiles,events,crossSection,genFiltEff,generator_name"""



import logging


#pinched from pandatools!
def readDsFromFile(txtName):
    import re
    dsList = []
    try:
        # read lines
        txt = open(txtName)
        for tmpLine in txt:
            # remove \n
            tmpLine = re.sub('\n','',tmpLine)
            # remove white spaces
            tmpLine = tmpLine.strip()
            # skip comment or empty
            if tmpLine.startswith('#') or tmpLine == '':
                continue
            # append
            dsList += [tmpLine]
        # close file    
        txt.close()
    except:
        errType,errValue = sys.exc_info()[:2]
        logging.error("cannot read datasets from %s due to %s:%s" % (txtName,errType,errValue))
        sys.exit(-1)    
    return dsList



def isfloat(x):
    try:
        a = float(x)
    except ValueError:
        return False
    else:
        return True

def isint(x):
    try:
        a = float(x)
        b = int(a)
    except ValueError:
        return False
    else:
        return a == b




def main():
    logging.basicConfig(format='%(levelname)s:%(message)s')

    import time,datetime
    from pytz import timezone
    import argparse

    try:
      import pyAMI.client
      import pyAMI.atlas.api as AtlasAPI
      import pyAMI.config
    except ImportError:
       logging.error("Unable to find pyAMI client. Please try this command first: lsetup pyAMI")
       return -1

    
    extraFieldDefaults = {} #{"approx_crossSection":None,"approx_GenFiltEff":1.0}

    fieldDefaults = {"subprocessID":0,"dataset_number":0}
    #populate the fieldDefaults ... for all, assume 'None'
    for field in pyAMI.config.tables['datasets'].keys():
        if str(field) == "cross_section": continue #special exception because this field only present in 
        if str(field) in fieldDefaults.keys(): continue
        if str(field).startswith("@"): continue
        fieldDefaults[str(field)] = None
        

    import commands
    #check the voms proxy 
    status,out = commands.getstatusoutput("voms-proxy-info -fqan -exists")
    if status!=0:
        logging.error("Please renew your certificate with this command: voms-proxy-init -voms atlas");
        return -1

    try:
        client = pyAMI.client.Client('atlas')
        AtlasAPI.init()
    except:
        logging.error("Could not establish pyAMI session. Are you sure you have a valid certificate? Do: voms-proxy-init -voms atlas")
        return -1

    #need to collect the ami dataset parameter defaults
    paramExplains = [] #for the help message only

    paramDefaults = {}
    res = client.execute('ListPhysicsParameterDefs',format='dom_object')
    for r in res.get_rows() : #r is OrderedDict
        explainString = "%s: %s" % (r[u'PARAMNAME'],r[u'DESCRIPTION']);
        if r[u'UNITS']!=u'NULL': explainString += " (units: %s)" % r[u'UNITS']
        if r[u'HASDEFAULT']==u'N' : paramDefaults[str(r[u'PARAMNAME'])] = None
        else: 
            explainString += " (default value = %s)" % r[u'DEFAULTVALUE']
            if r[u'PARAMTYPE']==u'number': paramDefaults[str(r[u'PARAMNAME'])] = float(r[u'DEFAULTVALUE']) #FIXME: Assumes all parameters are floats
            elif r[u'PARAMTYPE']==u'string': paramDefaults[str(r[u'PARAMNAME'])] = str(r[u'DEFAULTVALUE'])
        paramExplains += [explainString]


    cern_time = timezone('UCT')
    current_time = datetime.datetime.fromtimestamp(time.time(),cern_time).strftime('%Y-%m-%d %H:%M:%S')

    from argparse import RawTextHelpFormatter
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--inDS',nargs='+',default=[""],help="List of datasets to retrieve parameters for")
    parser.add_argument('--inDsTxt',default="",help="Alternative to --inDS, can specify the datasets from an input file")
    parser.add_argument('--fields',nargs='+',help="List of parameters to extract. Available parameters are: \n\n  %s\n\nYou can also include any from:\n  %s\nYou can also do keyword_xxx to add a bool branch for keywords" % ("\n  ".join(paramExplains),", ".join(fieldDefaults.keys()+extraFieldDefaults.keys())),default=["dataset_number","ldn","events"])
    parser.add_argument('--timestamp',default=current_time,help="The timestamp to query parameters at, specified in Universal Central Time (UCT). If left blank, will take the current time")
    parser.add_argument('--physicsGroups',nargs='+',default=["PMG,MCGN"],help="Physics group from which to retrieve parameters, listed in order of priority (highest first). Default value is 'PMG,MCGN' (i.e. try to use PMG values, fallback on MCGN values if unavailable). Allowed groups are:\n   PMG (this is the PMG's group name), BPHY, COSM, DAPR, EGAM, EXOT, FTAG, HIGG, HION, IDET, IDTR, JETM, LARG, MCGN (this is the AMI default group name), MDET, MUON, PHYS, REPR, SIMU, STDM, SUSY, TAUP, TCAL, TDAQ, THLT, TOPQ, TRIG, UPGR, VALI")
    parser.add_argument('--explainFields',nargs='+',default=[],help="The fields you would like explained .. will appear as comment lines after each row in the output")
    parser.add_argument('--explainInfo',nargs='+',default=[],help="Properties of the parameter you want to show in the explanation. Can list from: explanation, insert_time, end_time, physicsGroup, createdby")
    parser.add_argument('--outFile',default=sys.stdout,type=argparse.FileType('w'),help="Where to print the output to. Leave blank to print to stdout")
    parser.add_argument('--delim',default="\t",help="The delimiter character. Defaults to tab")
    parser.add_argument('-v',action='store_true',help="Verbose output for debugging")

    args = parser.parse_args()

    if args.timestamp=="the dawn of time": 
        logging.error("Unfortunately we don't know any parameters from this time period... but we're working on it!")
        return 9999

    #split elements of fields by comma to get full list 
    args.fields = sum((y.split(',') for y in args.fields),[])
    args.fields = [x.strip() for x in args.fields] #strips whitespace
    #look for keyword_ fields, these are special ...
    args.keywords=[]
    for f in args.fields:
        if f.startswith("keyword_"):
            k = f[8:]
            #and then add each keyword to the extraFieldDefaults so it is recognised thusly 
            extraFieldDefaults["keyword_%s"%k]=bool(False)
            args.keywords += [k]

    #same for physics groups
    args.physicsGroups = sum((y.split(',') for y in args.physicsGroups),[])
    args.physicsGroups = [x.strip() for x in args.physicsGroups] #strips whitespace


    #same for explainFields and explainInfo
    args.explainFields = sum((y.split(',') for y in args.explainFields),[])
    args.explainFields = [x.strip() for x in args.explainFields] #strips whitespace
    args.explainInfo = sum((y.split(',') for y in args.explainInfo),[])
    args.explainInfo = [x.strip() for x in args.explainInfo] #strips whitespace

    if args.inDsTxt != '': args.inDS = readDsFromFile(args.inDsTxt)
    
    #and same for inDS
    args.inDS = sum((y.split(',') for y in args.inDS),[])
    args.inDS = [x.strip() for x in args.inDS] #strips whitespace

    if args.v: logging.getLogger().setLevel(logging.DEBUG)
    else: logging.getLogger().setLevel(logging.INFO)
    logging.debug(args.inDS)
    logging.debug(args.fields)
    logging.debug(args.timestamp)
    
    #1. check field values are allowed, we obtain default field values at same time..
    #2. For each entry in inDS, if contains wildcard we obtain list of DS, otherwise check DS exists. During this time we obtain the datasetid and numEvents properties, incase we need them
    #3.  For each of these DS, get parameters from ami matching the timestamp. Organize into fields and index by subprocessID
    #4.  Output a line to our output file




    #1.
    #before adding all the ami parameters, identify which of provided fields are: 1). Obtained from list_datasets command (dsFields) 2). actual parameters
    dsFields = [ x for x in args.fields if x in fieldDefaults.keys() and x not in ["subprocessID","ldn"] ]
    extraFields = [ x for x in args.fields if x in extraFieldDefaults.keys() ]
    paramFields = [ x for x in args.fields if x in paramDefaults.keys() ]

    if len(paramFields)>0 and args.physicsGroups==[""]:
        logging.error("You must specify at least one physics group. See -h for allowed groups")
        return -1;

    #combine paramDefaults with fieldDefaults
    fieldDefaults.update(paramDefaults)
    #and with extra fields
    fieldDefaults.update(extraFieldDefaults)
    
    for field in args.fields:
        if field not in fieldDefaults:
            logging.error("%s is not a recognised field. Allowed fields are:" % field)
            logging.error(fieldDefaults.keys())
            return -1;
        

    #2.
    #replace all '*' with '%' and strip "/"
    args.inDS = [ds.replace("*","%") for ds in args.inDS]
    args.inDS = [ds.rstrip("/") for ds in args.inDS]
  

    logging.info("Fetching list of datasets from AMI (this may take a few minutes)...")


    #obtain list of datasets 
    res = AtlasAPI.list_datasets(client,patterns=args.inDS,fields=dsFields+['ldn'],ami_status="VALID") #changed status from %, to only catch valid now: wb 08/2015
    
    #NOTE: Should we allow retrieval of the extra information: keyword, genfiltereff, approx crossection, .. these all come from GetDatasetInfo ami command

    dataset_values = dict()
    for r in res:
        mydict = dict()
        dataset_values[str(r['ldn'])] = mydict
        for field in r.items():
            if str(field[0]) == "ldn": continue
            if str(field[0]) not in args.fields: continue
            mydict[str(field[0])] = str(field[1])
        #also if we have the 'extra fields or keywords' we will need to execute AtlasAPI.get_dataset_info ..
        if len(extraFields)>0 or len(args.keywords)>0:
            info_res = AtlasAPI.get_dataset_info(client,str(r['ldn']))
            #print(info_res)
            if len(info_res)==0: logging.error("Unable to retrieve dataset info for %s" % str(r['ldn']));return -1
            for field in extraFields:
                #ignore the keyword_ fields 
                if field.startswith("keyword_"): continue
                mydict[field] = float(info_res[0][unicode(field)]) if isfloat(info_res[0][unicode(field)]) else extraFieldDefaults[field]
            for k in args.keywords:
                mydict["keyword_%s" % k] = int( (k in str(info_res[0][unicode('keyword')]).split(",")) )
            
    
    logging.debug(dataset_values)

    #res = client.execute(['GetDatasetInfo

    for ds in args.inDS:
        if '%' not in ds and ds not in dataset_values.keys():
            logging.warning("Unknown dataset: %s" % ds)

    logging.info("Obtaining physics parameters for selected datasets...")

    headerString = ""
    doneHeader=False
    for ds in dataset_values.keys():
        #obtain list of parameters for this dataset
        if(args.timestamp==current_time):
            res = client.execute(['GetPhysicsParamsForDataset',"--logicalDatasetName=%s"% ds,"--timestamp='%s'"%args.timestamp], format='dom_object')
        else:
             res = client.execute(['GetPhysicsParamsForDataset',"--logicalDatasetName=%s"% ds,"--timestamp='%s'"%args.timestamp,"--history=true"], format='dom_object')

        #first we have to determine how many subprocesses this ds has 
        dsSubprocesses = [0] #always have the 0 subprocess 
        for r in res.get_rows():
            sp = int(r[u'subprocessID'])
            if sp not in dsSubprocesses: dsSubprocesses += [sp]

        #now for each subprocess we have to locate each required field value (in paramFields)
        #rank by physicsGroup
        for sp in dsSubprocesses:
            paramVals = dict()
            groupsWithVals = dict() #held for helpful output
            #need to keep explanations for requested fields
            explainInfo = dict()
            for i in args.explainFields: explainInfo[i] = dict()

            for param in paramFields:
                groupsWithVals[param] = []
                bestGroupIndex = len(args.physicsGroups)
                paramVals[param] = fieldDefaults[param]
                for r in res.get_rows():
                    if int(r[u'subprocessID']) != sp: continue
                    if str(r[u'paramName']) != param: continue
                    if str(r[u'physicsGroup']) not in args.physicsGroups: 
                        groupsWithVals[param] += [(str(r[u'physicsGroup']),str(r[u'paramValue']))]
                        continue
                    if args.physicsGroups.index(str(r[u'physicsGroup'])) > bestGroupIndex : continue
                    if args.physicsGroups.index(str(r[u'physicsGroup'])) == bestGroupIndex : logging.warning("Duplicate parameter %s for group %s in dataset %s (subprocess %d). Please report this!" % (param,str(r[u'physicsGroup']),ds,sp))
                    paramVals[param] = str(r[u'paramValue'])
                    bestGroupIndex=args.physicsGroups.index(str(r[u'physicsGroup']))
                    #keep the explanation info 
                    for e in args.explainInfo: 
                        #print(r.keys())
                        if unicode(e) not in r:
                            logging.error("Unrecognised explainInfo field: %s" % e)
                            return -1
                        explainInfo[param][e]=str(r[unicode(e)])
            #at this stage, parameters reside in paramVals dict or dataset_values[ds] dict
            #print them in the requested order .. if any is "None" then stop, because it doesn't have a default value and didn't find a value for it either 
            rowString = ""
            for param in args.fields:
                val = None
                if param == "ldn": val = ds
                elif param == "subprocessID": val = sp
                elif param in dataset_values[ds].keys(): val = dataset_values[ds][param]
                else: val = paramVals.get(param,None)
                if val == None:
                    logging.warning("dataset %s (subprocess %d) does not have parameter %s, which has no default." % (ds,sp,param))
                    if len(groupsWithVals.get(param,[]))>0:
                        logging.warning("The follow physicsGroups have defined that parameter though:")
                        logging.warning(groupsWithVals[param])
                    val = "#UNKNOWN#"
                    #return -1
                if rowString != "": rowString += args.delim
                rowString += str(val)
                #inspect the type of str(val) to build up the header
                if not doneHeader:
                    headerString += param
                    if type(fieldDefaults[param])==bool: headerString += "/O:"
                    elif type(fieldDefaults[param])==int: headerString += "/I:"
                    elif type(fieldDefaults[param])==float: headerString += "/D:"
                    elif isfloat(str(val)): headerString += "/D:"
                    #elif isint(str(val)): headerString += "/I:" TO BE SAFE WE MAKE ALL NUMERIC FIELDS FLOATS, EXCEPT if the default value is type int
                    else: headerString += "/C:"
                
            if not doneHeader:
                doneHeader=True
                print(headerString[:-1],file=args.outFile)
            print(rowString,file=args.outFile)
            #also print the required explanations
            for (field,expl) in explainInfo.items():
                outString = "#%s: { " % field
                doneFirst=False
                for eField in args.explainInfo:
                    if doneFirst: outString += " , "
                    if not eField in expl.keys(): outString += " %s: <NONE .. value is default>"%eField
                    else: outString += "%s: %s" % (eField,expl[eField])
                    doneFirst=True
                outString += " }"
                print(outString,file=args.outFile)

    #print the footer, which is the command to reproduce this output
    import os
    print("#lsetup  \"asetup %s,%s\" pyAMI" % (os.environ.get('AtlasProject','UNKNOWN!'),os.environ.get('AtlasVersion','UNKNOWN!')))
    print("#getAMIDatasetParameters.py --timestamp=\"%s\" --physicsGroups=\"%s\" --fields=\"%s\" --inDS=\"%s\"" % (args.timestamp,",".join(args.physicsGroups),",".join(args.fields),",".join(dataset_values.keys())),file=args.outFile )

    args.outFile.close()


if __name__ == "__main__":
    import sys
    sys.exit(main())

