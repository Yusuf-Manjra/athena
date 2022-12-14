
#ifndef ANALYSISTRUTHTAGRESULTS_H
#define ANALYSISTRUTHTAGRESULTS_H

#include <map>
#include <vector>
#include <string>

namespace Analysis {

    class TruthTagResults {

    public:

    //map from systematic name to vector of event weights for different number of tagged jets
    // for examples: map_trf_weight_ex["Nominal"].at(3) is the event weight for exactly 3 tagged jets
    // map_trf_weight_in["Nominal"].at(2) is the event weight for 2 or more tagged jets
    std::map<std::string,std::vector<float> > map_trf_weight_ex;
    std::map<std::string,std::vector<float> > map_trf_weight_in;

    //map from systematic name to vector of SF weights for different number of tagged jets
    // for examples: map_SF_ex["Nominal"].at(3) is the SF event weight for exactly 3 tagged jets
    // map_SF_ex["Nominal"].at(2) is the SF event weight for 2 or more tagged jets
    std::map<std::string,std::vector<float> > map_SF_ex;
    std::map<std::string,std::vector<float> > map_SF_in;

    //chosen permutation (does not depend on the systematic variation)
    // trf_chosen_perm_ex.at(3) is the chosen permutation for exactly 3 tagged jets
    std::vector<std::vector<bool> > trf_chosen_perm_ex;
    std::vector<std::vector<bool> > trf_chosen_perm_in;

    //the tag weight bin, similar configuration to the chosen permutation vectors.
    std::vector<std::vector<int> > trf_bin_ex;
    std::vector<std::vector<int> > trf_bin_in;

    //random tag weights generated for the chosen quantiles.
    std::vector<std::vector<float> > trf_bin_score_ex;
    std::vector<std::vector<float> > trf_bin_score_in;

    //random c-tag weights generated for the chosen quantiles.
    std::vector<std::vector<float> > trf_ctag_bin_score_ex;
    std::vector<std::vector<float> > trf_ctag_bin_score_in;

    //direct tagging results
    std::vector<bool> is_tagged;
    std::map<std::string,float > map_SF;

    std::vector<std::string> syst_names;

    void clear(){

        map_trf_weight_ex.clear();
        map_trf_weight_in.clear();
        trf_chosen_perm_ex.clear();
        trf_chosen_perm_in.clear();
        trf_bin_ex.clear();
        trf_bin_in.clear();

    }

    float getEvtDirectTagSF(std::string syst_name="Nominal")
    {
        return(map_SF[syst_name]);
    }

    std::vector<bool> getDirectTaggedJets()
    {
        return(is_tagged);

    }

    float getEventWeight(int nbtag,bool Ex,std::string syst_name="Nominal")
    {
        if(Ex)
        {
            return map_trf_weight_ex[syst_name].at(nbtag);
        }
        else
        {
            return map_trf_weight_in[syst_name].at(nbtag);
        }
    }

    std::vector<bool> getEventPermutation(int nbtag,bool Ex )
    {
        if(Ex)
        {
            return(trf_chosen_perm_ex.at(nbtag));
        }

        else
        {
            return(trf_chosen_perm_in.at(nbtag));
        }
    }

    std::vector<int> getEventQuantiles(int nbtag,bool Ex )
    {
        if(Ex)
        {
            return(trf_bin_ex.at(nbtag));
        }

        else
        {
            return(trf_bin_in.at(nbtag));
        }
    }


    std::vector<float> getRandomTaggerScores(int nbtag,bool Ex )
    {
        if(Ex)
        {
            return(trf_bin_score_ex.at(nbtag));
        }
        else
        {
            return(trf_bin_score_in.at(nbtag));
        }
    }


    };


}

#endif // ANALYSISTRUTHTAGRESULTS_H

