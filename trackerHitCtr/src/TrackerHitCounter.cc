#include "DD4hep/Detector.h"
#include <DD4hep/DetType.h>
#include <TrackerHitCounter.h>
#include "DDRec/DetectorData.h"
#include <IMPL/LCCollectionVec.h>
#include <EVENT/SimTrackerHit.h>
#include <UTIL/CellIDDecoder.h>
#include <vector>

using namespace marlin ;


TrackerHitCounter aTrackerHitCounter ;


TrackerHitCounter::TrackerHitCounter() : Processor("TrackerHitCounter"),
    _hitCounters(), m_trkHitCollNames()
{

  // modify processor description
  _description = "TrackerHitCounter counts SimTrackerHits in tracker detector elements "
          "and reports the number of hits per unit area." ;

  StringVec defaultTrkHitCollections;
  defaultTrkHitCollections.push_back(std::string("VXDCollection"));
  defaultTrkHitCollections.push_back(std::string("SITCollection"));
  defaultTrkHitCollections.push_back(std::string("FTDCollection"));
  defaultTrkHitCollections.push_back(std::string("TPCCollection"));
  defaultTrkHitCollections.push_back(std::string("SETCollection"));

  registerProcessorParameter("TrkHitCollections" ,
                             "Tracker hit collections that will be analysed",
                             m_trkHitCollNames ,
                             defaultTrkHitCollections ) ;
}

void TrackerHitCounter::init() { 

  // usually a good idea to
  printParameters() ;

  // v01-19-05 namespace
  dd4hep::Detector& theDetector = dd4hep::Detector::getInstance();

  const std::vector< dd4hep::DetElement > &detElements = theDetector.detectors("tracker", true);

  for(dd4hep::DetElement element : detElements) {

    streamlog_out(DEBUG7) << "*********************************************************************\n";
          streamlog_out(DEBUG7) << "Detector element \'" << element.name() << "\' of type \'"
        << element.type() << "\':\n";

    _hitCounters[element.id()] = new SystemHitCounter;

    try {
      streamlog_out(DEBUG) << "Trying ZPlanarData.\n";
      dd4hep::rec::ZPlanarData* layering = element.extension<dd4hep::rec::ZPlanarData>() ;

      // We assume that layer numbering always starts from 0.
      int ilay = 0;
      for ( auto layer : layering->layers ) {
          streamlog_out(DEBUG7) << "  Layer " << ilay+1 << ":\n";
          double ls = layer.zHalfSensitive*2;
          double ws = layer.widthSensitive;
          int nModules = layer.ladderNumber;
          double area = ls*ws*nModules;
          (*_hitCounters.at(element.id()))[ilay] = new LayerHitCounter(area);
          streamlog_out(DEBUG7) << "    Length of sensitive area: " << ls/dd4hep::mm << " mm\n";
          streamlog_out(DEBUG7) << "    Width of sensitive area: " << ws/dd4hep::mm << " mm\n";
          streamlog_out(DEBUG7) << "    Number of ladders: " << nModules << "\n";
          streamlog_out(DEBUG7) << "    Total sensitive area: " << area/dd4hep::cm2 << " cm^2\n";
          //streamlog_out(DEBUG7) << "    Sensors per ladder: " << layer.sensorsPerLadder << "\n";
          ilay++;
      }
    }
    catch ( std::exception &e) {
      streamlog_out(DEBUG7) << "Caught exception " << e.what() << std::endl;
      try {
            streamlog_out(DEBUG7) << "Trying ZDiskPetalsData.\n";
            dd4hep::rec::ZDiskPetalsData *layering = element.extension<dd4hep::rec::ZDiskPetalsData>() ;
            // We assume that layer numbering always starts from 0.
            int ilay = 0;
            for ( auto layer : layering->layers ) {
                streamlog_out(DEBUG7) << "  Layer " << ilay+1 << ":\n";
                double ls = layer.lengthSensitive;
                double wsi = layer.widthInnerSensitive;
                double wso = layer.widthOuterSensitive;
                int nModules = layer.petalNumber;
                double area = ls*(wsi+wso)*nModules/2;
                (*_hitCounters.at(element.id()))[ilay] = new LayerHitCounter(area);
                streamlog_out(DEBUG7) << "    Length of sensitive area: " << ls/dd4hep::mm << " mm\n";
                streamlog_out(DEBUG7) << "    Inner width of sensitive area: " << wsi/dd4hep::mm << " mm\n";
                streamlog_out(DEBUG7) << "    Outer width of sensitive area: " << wso/dd4hep::mm << " mm\n";
                streamlog_out(DEBUG7) << "    Number of petals: " << nModules << "\n";
                streamlog_out(DEBUG7) << "    Total sensitive area: " << area/dd4hep::cm2 << " cm^2\n";
                //streamlog_out(DEBUG7) << "    Sensors per petal: " << layer.sensorsPerPetal << "\n";
                ilay++;
            }
          }
      catch ( std::exception &e1) {
        streamlog_out(DEBUG7) << "Caught exception " << e1.what() << std::endl;
        streamlog_out(DEBUG7) << "  No layering extension in the "
                "detector element \'" << element.name() << "\'.\nTotal hits will be counted.\n";
        (*_hitCounters.at(element.id()))[0] = new LayerHitCounter(-1.);
      }
    }


    streamlog_out(MESSAGE) << "\n    Added " << _hitCounters.at(element.id())->size()
            << " hit counters for subsystem \'" << element.name() << "\'.\n\n";
  }
}


// Performed at the end of each run (file)
void TrackerHitCounter::processRunHeader( LCRunHeader*) {
  _nRun++;
  streamlog_out(DEBUG7) << "Processing run " << _nRun << "\n";
  markRun();
  streamlog_out(DEBUG7) << std::endl;
}


void TrackerHitCounter::processEvent( LCEvent * evt) {

  _nEvt++;

  for (auto collname : m_trkHitCollNames) {

    streamlog_out(DEBUG) << "Looking into collection " << collname << "\n";
    LCCollectionVec *col = dynamic_cast<LCCollectionVec*>(evt->getCollection(collname));
    CellIDDecoder<SimTrackerHit> decoder(col);

    for (auto lcobj : (*col)) {

      SimTrackerHit* hit = dynamic_cast<SimTrackerHit*>(lcobj);

      if(!hit) {
        streamlog_out(WARNING) << "Collection " << collname << " contains objects "
            "of type other than SimTrackerHit!\nSkipping collection.\n";
        break;
      }

      int nsys = decoder(hit)["system"];
      streamlog_out(DEBUG) << "Found hit belonging to system #" << nsys << "\n";
      HitCtrMapIter hcmit = _hitCounters.find(nsys);

      if (hcmit == _hitCounters.end()) {
        streamlog_out(WARNING) << "Hit belongs to a system that is not analysed.\n";
      }
      else {
        SystemHitCounter *shc = hcmit->second;
        if (shc->size() == 1) {
          shc->at(0)->addHit();
        }
        else {
          int nLayer = decoder(hit)["layer"];
          HitCtrLayerIter hclit = shc->find(nLayer);
          if (hclit == shc->end()) {
            streamlog_out(ERROR) << "Hit in system ID=" << hcmit->first
                 << " belongs to layer number " << nLayer << ". Out of range!\n"
                    "  This should only happen if the xml detector description\n"
                    "  is different than the one used in the simulation.\n";
          }
          else {
            hclit->second->addHit();
          }
        }
      }

    } // Loop over hits in collection

  } // Loop over collections

  markEvent();

}


void TrackerHitCounter::check( LCEvent *  ) { }


void TrackerHitCounter::end() {

  streamlog_out(MESSAGE) << "******************************************************\n";
  streamlog_out(MESSAGE) << "REPORT: \n";

  dd4hep::Detector& theDetector = dd4hep::Detector::getInstance();
  const std::vector< dd4hep::DetElement > &detElements = theDetector.detectors("tracker", true);

  for(dd4hep::DetElement element : detElements) {

    streamlog_out(MESSAGE) << "-----------------------------------------\n"
        "Subsystem : " << element.name() << "\n";
    auto hitctr = _hitCounters.at(element.id());


    if (hitctr->size() == 1) {
      streamlog_out(MESSAGE) << "  Reporting total hits in the detector element:\n";
    }

    for (auto layerpair : *hitctr ) {

        auto layerctr = layerpair.second;
        streamlog_out(MESSAGE) << "  Layer " << layerpair.first+1 << ": "
            << layerctr->getNHits() << " hits.\n";
        streamlog_out(MESSAGE) << "    (" << layerctr->getHitsPerRun() << " +- "
                  << layerctr->getStDevHitsPerRun() << ") hits/run.\n";
        streamlog_out(MESSAGE) << "    (" << layerctr->getHitsPerEvent() << " +- "
                  << layerctr->getStDevHitsPerEvent() << ") hits/event.\n";

        if (layerctr->isAreaAvailable()) {
          streamlog_out(MESSAGE) << "    (" << layerctr->getHitsPerCm2PerRun() << " +- "
                  << layerctr->getStDevHitsPerCm2PerRun() << ") hits/cm^2/run.\n";
          streamlog_out(MESSAGE) << "    (" << layerctr->getHitsPerCm2PerEvent() << " +- "
                  << layerctr->getStDevHitsPerCm2PerEvent() << ") hits/cm^2/event.\n";
        }
    }

    streamlog_out(MESSAGE) << "\n";
  }
  streamlog_out(MESSAGE) << "Analysed a total of " << _nEvt << " events in " << _nRun << " runs.\n";
  streamlog_out(MESSAGE) << "******************************************************\n";


  // Clean up
  for (auto systempair : _hitCounters) {
        for (auto layerpair : *(systempair.second) ) {
            delete layerpair.second;
        }
        delete systempair.second;
    }
    _hitCounters.clear();

}


void TrackerHitCounter::markRun() {
    for (auto sysCtr : _hitCounters) {
        for (auto layerCtr : *sysCtr.second) {
            layerCtr.second->markRun();
        }
    }
}

void TrackerHitCounter::markEvent() {
    for (auto sysCtr : _hitCounters) {
        for (auto layerCtr : *sysCtr.second) {
            layerCtr.second->markEvent();
        }
    }
}

