package experiment

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterRoadnet request handlers.
func RegisterRoadnet(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/roadnet/loadfromfile/:folder/:graphReaderType", func(c *gin.Context) {
		cq.Add(c, LoadRoadnetFromFile)
	})
	r.GET("/roadnet", func(c *gin.Context) {
		cq.Add(c, RoadnetGet)
	})
}

// LoadRoadnetFromFile loads roadnet from file.
func LoadRoadnetFromFile(c *gin.Context, b interface{}) *util.TaskResult {
	// Only load roadnet in an open experiment.
	if !mod.ExpCtx.IsExpOpen() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment first.",
		}
	}

	// Send load roadnet request to core.
	mod.ExpCtx.StartRefreshRoadnet()
	util.Core.SendRequest(struct {
		Cmd             string
		Folder          string
		GraphReaderType string
	}{
		Cmd:             "ReadRoadnetFromDataSource",
		Folder:          c.Param("folder"),
		GraphReaderType: c.Param("graphReaderType"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load roadnet: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}
	mod.ExpCtx.EndRefreshRoadnet()

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}

// RoadnetGet gets the roadnet.
func RoadnetGet(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot get roadnet before loaded.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send get roadnet request to core.
	util.Core.SendRequest(struct {
		Cmd string
	}{
		Cmd: "GetRoadnet",
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get roadnet: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret.Data,
	}
}
