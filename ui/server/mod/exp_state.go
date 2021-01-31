package mod

import (
	"errors"
	"sync"
)

var expStateLock sync.RWMutex

// ExperimentState holds the state of doing an experiment.
type ExperimentState struct {
	IsOpen       bool   `json:"isOpen"`
	ID           int    `json:"id"`
	Name         string `json:"name"`
	RoadnetReady bool   `json:"roadnetReady"`
}

// ExpState is the experiment state.
var ExpState = ExperimentState{
	IsOpen: false,
}

// Open an experiment.
func (es *ExperimentState) Open(id int, name string) error {
	expStateLock.Lock()
	defer expStateLock.Unlock()

	if ExpState.IsOpen {
		return errors.New("only one experiment can be opened")
	}
	ExpState.IsOpen = true
	ExpState.ID = id
	ExpState.Name = name
	return nil
}

// Close the experiment.
func (es *ExperimentState) Close() {
	expStateLock.Lock()
	defer expStateLock.Unlock()

	ExpState.IsOpen = false
	ExpState.RoadnetReady = false
}

// StartRefreshRoadnet start fresh roadnet.
func (es *ExperimentState) StartRefreshRoadnet() {
	expStateLock.Lock()
	ExpState.RoadnetReady = false
}

// EndRefreshRoadnet end fresh roadnet.
func (es *ExperimentState) EndRefreshRoadnet() {
	defer expStateLock.Unlock()
	ExpState.RoadnetReady = true
}
