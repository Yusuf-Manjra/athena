/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationWP.h>

namespace CP {
    IsolationWP::~IsolationWP() {
    }

    std::string IsolationWP::name() const {
        return m_name;
    }
    void IsolationWP::name(const std::string &name) {
        m_name = name;
    }
    const Root::TAccept& IsolationWP::accept(const xAOD::IParticle& p) const {
        m_accept.clear();
        for (const auto& c : m_cuts) {
            if (c->accept(p, m_cutValues)) m_accept.setCutResult(c->name(), true);
        }
        return m_accept;
    }

    const Root::TAccept& IsolationWP::accept(const strObj& p) const {
        m_accept.clear();
        for (const auto& c : m_cuts) {
            if (c->accept(p, m_cutValues)) {
                m_accept.setCutResult(c->name(), true);
            }
        }
        return m_accept;
    }

    
    void IsolationWP::addCut(IsolationCondition* i){
        m_cuts.push_back(std::unique_ptr<IsolationCondition>(i));
        m_accept.addCut(i->name(), i->name());
    }
      
    void IsolationWP::addCut(std::unique_ptr<IsolationCondition>& i) {
        // Push first an empty element back to the vector
        m_cuts.push_back(std::unique_ptr<IsolationCondition>());
        // Steal the owner ship from the old pointer :-P
        i.swap( m_cuts.back());
        m_accept.addCut(i->name(), i->name());
    }
    const Root::TAccept& IsolationWP::getAccept() const {
        return m_accept;
    }
    const std::vector<std::unique_ptr<IsolationCondition>>& IsolationWP::conditions() const{
        return m_cuts;
    }

    IsolationWP::IsolationWP(const std::string &name) :
                            m_name(name),
                            m_cuts(),
                            m_cutValues(nullptr),
                            m_accept(){
                }
}
