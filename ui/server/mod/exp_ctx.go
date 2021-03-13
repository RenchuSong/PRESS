package mod

import (
	"errors"
	"sync"
)

var expCtxLock sync.RWMutex

// ExperimentContext holds the state of doing an experiment.
type ExperimentContext struct {
	IsOpen         bool   `json:"isOpen"`
	ID             int    `json:"id"`
	Name           string `json:"name"`
	RoadnetReady   bool   `json:"roadnetReady"`
	GridIndexReady bool   `json:"gridIndexReady"`
	SPTableReady   bool   `json:"spTableReady"`
}

// ExpCtx is the experiment state.
var ExpCtx = ExperimentContext{
	IsOpen:         false,
	ID:             -1,
	Name:           "",
	RoadnetReady:   false,
	GridIndexReady: false,
	SPTableReady:   false,
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

// IsGridIndexReady is grid index ready.
func (es *ExperimentContext) IsGridIndexReady() bool {
	expCtxLock.RLock()
	defer expCtxLock.RUnlock()
	return es.GridIndexReady
}

// IsSPTableReady is SPTable ready.
func (es *ExperimentContext) IsSPTableReady() bool {
	expCtxLock.RLock()
	defer expCtxLock.RUnlock()
	return es.SPTableReady
}

// Open an experiment.
func (es *ExperimentContext) Open(id int, name string) error {
	expCtxLock.Lock()
	defer expCtxLock.Unlock()

	if ExpCtx.IsOpen && ExpCtx.ID != id {
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
	ExpCtx.GridIndexReady = false
	ExpCtx.SPTableReady = false
}

// StartRefreshRoadnet start fresh roadnet.
func (es *ExperimentContext) StartRefreshRoadnet() {
	ExpCtx.RoadnetReady = false
}

// EndRefreshRoadnet end fresh roadnet.
func (es *ExperimentContext) EndRefreshRoadnet() {
	ExpCtx.RoadnetReady = true
}

// StartBuildGridIndex start fresh grid index.
func (es *ExperimentContext) StartBuildGridIndex() {
	ExpCtx.GridIndexReady = false
}

// EndBuildGridIndex end fresh grid index.
func (es *ExperimentContext) EndBuildGridIndex() {
	ExpCtx.GridIndexReady = true
}

// StartBuildSPTable start fresh SPTable.
func (es *ExperimentContext) StartBuildSPTable() {
	ExpCtx.SPTableReady = false
}

// EndBuildSPTable end fresh grid index.
func (es *ExperimentContext) EndBuildSPTable() {
	ExpCtx.SPTableReady = true
}

// LockCtxLock locks the ctx lock.
func (es *ExperimentContext) LockCtxLock() {
	expCtxLock.Lock()
}

// UnlockCtxLock unlocks the ctx lock.
func (es *ExperimentContext) UnlockCtxLock() {
	expCtxLock.Unlock()
}
