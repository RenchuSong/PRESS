package experiment

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterReformatter request handlers.
func RegisterReformatter(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/reformatter/reformat", func(c *gin.Context) {
		cq.Add(c, ReformatToPRESS)
	})
	r.PUT("/reformatter/dumptobinary", func(c *gin.Context) {
		cq.Add(c, DumpPRESSToBinary)
	})
	r.PUT("/reformatter/loadfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadPRESSFromBinary)
	})
	r.PUT("/reformatter/clear", func(c *gin.Context) {
		cq.Add(c, ClearPRESS)
	})
	r.GET("/reformatter/list", func(c *gin.Context) {
		cq.Add(c, ListPRESSTrajectories)
	})
}

// ReformatToPRESS reformts all map matched trajectories to PRESS trajectories.
func ReformatToPRESS(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// DumpPRESSToBinary dumps PRESS trajectories to binary.
func DumpPRESSToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// LoadPRESSFromBinary loads PRESS trajectories from binary.
func LoadPRESSFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// ClearPRESS clears PRESS trajectories.
func ClearPRESS(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// ListPRESSTrajectories lists all PRESS trajectories in binary folder.
func ListPRESSTrajectories(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}
