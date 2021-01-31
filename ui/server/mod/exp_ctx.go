package mod

import (
	"errors"
	"sync"
)

var expCtxLock sync.RWMutex

// ExperimentContext holds the state of doing an experiment.
type ExperimentContext struct {
	IsOpen       bool   `json:"isOpen"`
	ID           int    `json:"id"`
	Name         string `json:"name"`
	RoadnetReady bool   `json:"roadnetReady"`
}

// ExpCtx is the experiment state.
var ExpCtx = ExperimentContext{
	IsOpen:       false,
	ID:           -1,
	Name:         "",
	RoadnetReady: false,
}

// IsExpOpen is experiment open.
func (es *ExperimentContext) IsExpOpen() bool {
	expCtxLock.RLock()
	defer expCtxLock.RUnlock()
	return es.IsOpen
}

// IsRoadnetReady is roadnet ready.
func (es *ExperimentContext) IsRoadnetReady() bool {
	expCtxLock.RLock()
	defer expCtxLock.RUnlock()
	return es.RoadnetReady
}

// Open an experiment.
func (es *ExperimentContext) Open(id int, name string) error {
	expCtxLock.Lock()
	defer expCtxLock.Unlock()

	if ExpCtx.IsOpen {
		return errors.New("only one experiment can be opened")
	}
	ExpCtx.IsOpen = true
	ExpCtx.ID = id
	ExpCtx.Name = name
	return nil
}

// Close the experiment.
func (es *ExperimentContext) Close() {
	expCtxLock.Lock()
	defer expCtxLock.Unlock()

	ExpCtx.IsOpen = false
	ExpCtx.ID = -1
	ExpCtx.Name = ""
	ExpCtx.RoadnetReady = false
}

// StartRefreshRoadnet start fresh roadnet.
func (es *ExperimentContext) StartRefreshRoadnet() {
	expCtxLock.Lock()
	ExpCtx.RoadnetReady = false
}

// EndRefreshRoadnet end fresh roadnet.
func (es *ExperimentContext) EndRefreshRoadnet() {
	defer expCtxLock.Unlock()
	ExpCtx.RoadnetReady = true
}
