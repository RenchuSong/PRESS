package svc

import (
	"encoding/json"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
)

// Exe is an executor.
type Exe struct {
	tskQueue *util.TaskQueue
	sse      *SSEHandler
}

// NewExe creates a new executor.
func NewExe(q *util.TaskQueue, s *SSEHandler) *Exe {
	return &Exe{
		tskQueue: q,
		sse:      s,
	}
}

// Run spawns an executor.
func (e *Exe) Run() {
	for {
		if t, ok := <-e.tskQueue.Tasks; ok {
			var b interface{}
			if err := json.Unmarshal([]byte(t.Body), &b); err != nil {
				e.sse.Send(&util.TaskResult{
					ID:      t.ID,
					Code:    503,
					Message: "Failed to parse payload.",
				})
				return
			}
			resp := t.Handler(t.Ctx, b)
			resp.ID = t.ID
			e.sse.Send(resp)
		} else {
			break
		}
	}
}
