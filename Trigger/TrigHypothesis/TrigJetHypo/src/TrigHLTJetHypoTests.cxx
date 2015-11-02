/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include "./TrigHLTJetHypoHelpers/FlowEdge.h"
#include "./TrigHLTJetHypoHelpers/FlowNetwork.h"
#include "./TrigHLTJetHypoHelpers/FordFulkerson.h"

int main(int argc, char* argv[]) {

  if (argc != 2){
    std::cerr << "Usage: " << argv[0] <<" <filename>" << '\n';
    return 1;
  }

  std::fstream in(argv[1]);
  FlowNetwork G(in);
  std::cout<< G << '\n';

  int s = 0;
  int t = G.V() - 1;
  // compute maximum flow and minimum cut
  FordFulkerson maxflow = FordFulkerson(G, s, t);
  std::cout <<"Max flow from " << s <<  " to " <<  t << '\n';
  for (int v = 0; v < G.V(); v++) {
    for (auto e : G.adj(v)) {
      if ((v == e->from()) && e->flow() > 0)
        std::cout << "   " << *e << '\n';
    }
  }
        
  // print min-cut
  std::cout << "Min cut: " << '\n';
  for (int v = 0; v < G.V(); v++) {
    if (maxflow.inCut(v)) {std::cout << v <<  " " ;}
  }
  std::cout << '\n';
  
  std::cout << "Max flow value = " <<  maxflow.value() << '\n';
}
