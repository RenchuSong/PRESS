package experiment

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterMapMatcher request handlers.
func RegisterMapMatcher(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.POST("/mapmatcher/add", func(c *gin.Context) {
		cq.Add(c, AddGPSTrajectoryAndMapMatch)
	})
	r.PUT("/mapmatcher/dumpgpstobinary", func(c *gin.Context) {
		cq.Add(c, DumpGPSToBinary)
	})
	r.PUT("/mapmatcher/dumptobinary", func(c *gin.Context) {
		cq.Add(c, DumpMapMatchedToBinary)
	})
	r.PUT("/mapmatcher/loadgpsfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadFromBinary)
	})
	r.PUT("/mapmatcher/loadfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadMapMatchedFromBinary)
	})
}

// AddGPSTrajectoryAndMapMatch adds GPS trajectory and map match to roadnet.
func AddGPSTrajectoryAndMapMatch(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// DumpGPSToBinary dumps GPS trajectories to binary.
func DumpGPSToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// DumpMapMatchedToBinary dumps map matched trajectories to binary.
func DumpMapMatchedToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// LoadFromBinary loads GPS trajectory from binary.
func LoadFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// LoadMapMatchedFromBinary loads map matched trajectory from binary.
func LoadMapMatchedFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}
