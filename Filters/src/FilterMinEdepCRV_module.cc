// Pass events with at least one hit satisfying a min momentum cut.
//
// Andrei Gaponenko, 2013

#include <string>
#include <map>
#include <sstream>

// art includes.
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "MCDataProducts/inc/StepPointMC.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"

namespace mu2e {

  //================================================================
  class FilterStepPointMomentum : public art::EDFilter {
    typedef std::vector<art::InputTag> InputTags;
    InputTags inputs_;
    double cutEDepMin_;
    double cutEDepMax_;

    // statistics counters
    unsigned numInputEvents_;
    unsigned numPassedEvents_;
  public:
    explicit FilterStepPointMomentum(const fhicl::ParameterSet& pset);
    virtual bool filter(art::Event& event) override;
    virtual void endJob() override;
  };

  //================================================================
  FilterStepPointMomentum::FilterStepPointMomentum(const fhicl::ParameterSet& pset)
    : cutEDepMin_(pset.get<double>("cutEDepMin", std::numeric_limits<double>::min()))
    , cutEDepMax_(pset.get<double>("cutEDepMax", std::numeric_limits<double>::max()))
    , numInputEvents_(0)
    , numPassedEvents_(0)
  {
    typedef std::vector<std::string> VS;
    const VS in(pset.get<VS>("inputs"));
    for(const auto& i : in) {
      inputs_.emplace_back(i);
    }
  }

  //================================================================
  bool FilterStepPointMomentum::filter(art::Event& event) {
    bool passed = false;
    double totalEDep = 0;
    for(const auto& cn : inputs_) {
      auto ih = event.getValidHandle<StepPointMCCollection>(cn);
      for(const auto& hit : *ih) {
	totalEDep = totalEDep + hit.totalEDep();
      }
    }

    if(totalEDep > cutEDepMin_ && totalEDep < cutEDepMax_) 
      passed = true;

    ++numInputEvents_;
    if(passed) { ++numPassedEvents_; }
    return passed;
  }

  //================================================================
  void FilterStepPointMomentum::endJob() {
    mf::LogInfo("Summary")
      <<"FilterStepPointMomentum_module: passed "
      <<numPassedEvents_<<" / "<<numInputEvents_<<" events\n";
  }

  //================================================================
} // namespace mu2e

DEFINE_ART_MODULE(mu2e::FilterStepPointMomentum);
