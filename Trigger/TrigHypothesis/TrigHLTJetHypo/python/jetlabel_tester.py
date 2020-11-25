# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Utility to test whether a string is a legal jet chain label"""
from __future__ import print_function


from ChainLabelParser import ChainLabelParser
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from  TrigHLTJetHypo.ConditionsToolSetterFastReduction import (
    ConditionsToolSetterFastReduction,)

def compile(label, setter, expand=False, do_dump=False, do_print=False):
    print ('compile:',  label)

    parser = ChainLabelParser(label, debug=False)
    tree = parser.parse()

    tree.set_ids(node_id=0, parent_id=0)
    
    
    if expand:
        visitor = TreeParameterExpander()
        tree.accept(visitor)

        if setter is not None:
            setter.mod(tree)

    print ('compile: tree.scenario', tree.scenario)

        
        
    if do_print:
        print ('\nnode dumping top node only:\n')
        print (tree)

    if do_dump:
        print ('\nnode dump tree:\n')
        print (tree.dump())
        
    return tree

def compile_(label, setter=None, expand=True, do_dump=False, do_print=False):
    compile(label, setter, expand, do_dump)

            
if __name__ == '__main__':


    import sys
    
    from TrigHLTJetHypo.test_cases import test_strings
    c = sys.argv[1]
    index = -1
    try:
        index = int(c)
    except Exception:
        print('expected int in [1,%d] ]on comand line, got %s' % (
            len(test_strings), c))
        sys.exit()

    if(index < 0 or index > len(test_strings) - 1):
        print('index %d not in range [0, %d]' % (index, len(test_strings) -1))
        sys.exit()
                                                 
            
    print('index', index)
    label = test_strings[index]

    setter = ConditionsToolSetterFastReduction('toolSetterFastReduction')
    
    tree = compile(label, setter=setter,  expand=True, do_dump=True)

    print ('tvec: %s' % str(setter.tool))
    print ('svec: %s' % setter.shared)
    print ('conditionsVec [%d]: %s' % (len(setter.conditionsVec),
                                       str(setter.conditionsVec)))
