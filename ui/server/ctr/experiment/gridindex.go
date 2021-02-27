package experiment

import (
	"strconv"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterGridIndex request handlers.
func RegisterGridIndex(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/gridindex/build/:width/:height", func(c *gin.Context) {
		cq.Add(c, BuildGridIndex)
	})
	r.PUT("/gridindex/dumptobinary", func(c *gin.Context) {
		cq.Add(c, DumpGridIndexToBinary)
	})
	r.PUT("/gridindex/loadfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadGridIndexFromBinary)
	})
}

// BuildGridIndex builds grid index with given width & height.
func BuildGridIndex(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot build grid index before roadnet is loaded.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	width, err := strconv.Atoi(c.Param("width"))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Invalid width: " + err.Error(),
		}
	}
	if width < 50 || width > 1000 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid width range [50, 1000]: ",
		}
	}
	height, err := strconv.Atoi(c.Param("height"))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Invalid height: " + err.Error(),
		}
	}
	if height < 50 || height > 1000 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid height range [50, 1000]: ",
		}
	}

	// Send build grid index request to core.
	util.Core.SendRequest(struct {
		Cmd        string
		CellWidth  int
		CellHeight int
	}{
		Cmd:        "BuildGridIndex",
		CellWidth:  width,
		CellHeight: height,
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to build grid index: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
	}
}

// DumpGridIndexToBinary dumps grid index to binary.
func DumpGridIndexToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return nil
}

// LoadGridIndexFromBinary loads grid index from binary.
func LoadGridIndexFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return nil
}
