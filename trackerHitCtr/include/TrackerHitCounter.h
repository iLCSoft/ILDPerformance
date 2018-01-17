#ifndef TrackerHitCounter_h
#define TrackerHitCounter_h 1

#include "marlin/Processor.h"
#include "DD4hep/DD4hepUnits.h"
#include <string>

/** TrackerHitCounter: Marlin processor that counts SimTrackerHits in tracker detector
 *     elements and reports the number of hits per unit area.
 *
 *  S. Lukić, Jan 2018
 */

class LayerHitCounter {
public:
    LayerHitCounter() :
        _area(1.), _nHits(0),
        _nRuns(0), _nHitsRun(0), _mu2nHitsRun(0),
        _nEvents(0), _nHitsEvent(0), _mu2nHitsEvent(0)

    {}

    LayerHitCounter(double area) :
        _area(area), _nHits(0),
        _nRuns(0), _nHitsRun(0), _mu2nHitsRun(0),
        _nEvents(0), _nHitsEvent(0), _mu2nHitsEvent(0)
    {}

    virtual ~LayerHitCounter() {}

    int getNHits() const { return _nHits; }
    double getHitsPerCm2() const { return double(_nHits)/(_area/dd4hep::cm2); }

    double getHitsPerRun() const { return double(_nHits)/_nRuns; }
    double getStDevHitsPerRun() const { return sqrt(_mu2nHitsRun/_nRuns - pow(_nHits/_nRuns, 2)); }
    double getHitsPerCm2PerRun() const { return getHitsPerRun()/(_area/dd4hep::cm2); }
    double getStDevHitsPerCm2PerRun() const { return getStDevHitsPerRun()/(_area/dd4hep::cm2); }

    double getHitsPerEvent() const { return double(_nHits)/_nEvents; }
    double getStDevHitsPerEvent() const { return sqrt(_mu2nHitsEvent/_nEvents - pow(_nHits/_nEvents, 2)); }
    double getHitsPerCm2PerEvent() const { return getHitsPerEvent()/(_area/dd4hep::cm2); }
    double getStDevHitsPerCm2PerEvent() const { return getStDevHitsPerEvent()/(_area/dd4hep::cm2); }

    bool isAreaAvailable() const { return (_area > 0.); }

    void addHit() { _nHits++; _nHitsRun++; _nHitsEvent++; }

    void markRun() {
        _nRuns++;
        _mu2nHitsRun += _nHitsRun*_nHitsRun;
        _nHitsRun = 0;
        /* streamlog_out(DEBUG) << "LayerHitCounter marking Run #" << _nRuns
                << ". Nhits = " <<  _nHits << ".\n"; */
    }
    void markEvent() {
        _nEvents++;
        _mu2nHitsEvent+= _nHitsEvent*_nHitsEvent;
        _nHitsEvent = 0;
        /* streamlog_out(DEBUG) << "LayerHitCounter marking Run #" << _nRuns
                << ". Nhits = " <<  _nHits << ".\n"; */
    }

private:
    const double _area;
    int _nHits;
    int _nRuns;
    int _nHitsRun;
    double _mu2nHitsRun;
    int _nEvents;
    int _nHitsEvent;
    double _mu2nHitsEvent;
};


typedef std::map<int, LayerHitCounter*> SystemHitCounter;
typedef SystemHitCounter::iterator HitCtrLayerIter;
typedef std::map<int, SystemHitCounter*> HitCtrMap;
typedef HitCtrMap::iterator HitCtrMapIter;


class TrackerHitCounter : public marlin::Processor {

public:

    virtual Processor*  newProcessor() { return new TrackerHitCounter ; }


    TrackerHitCounter() ;

    /** Read and report the areas of tracker elements.
    *  Construct the corresponding hit counters.
    */
    virtual void init() ;

    /// Count runs
    virtual void processRunHeader( LCRunHeader* run ) ;

    /// Count hits in subsystems
    virtual void processEvent( LCEvent * evt ) ;

    /// do nothing
    virtual void check( LCEvent * evt ) ;

    /// report hits and clean up.
    virtual void end() ;


protected:

    void markEvent();
    void markRun();

    HitCtrMap _hitCounters{} ;

    StringVec m_trkHitCollNames{} ;

    int _nRun{} ;
    int _nEvt{} ;
} ;


#endif // TrackerHitCounter_h



