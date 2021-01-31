package experiment

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterRoadnet request handlers.
func RegisterRoadnet(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/roadnet/loadfromfile/:folder/:graphReaderType", func(c *gin.Context) {
		cq.Add(c, LoadRoadnetFromFile)
	})
}

// LoadRoadnetFromFile loads roadnet from file.
func LoadRoadnetFromFile(c *gin.Context, b interface{}) *util.TaskResult {
	// Only load roadnet in an open experiment.
	if !mod.ExpState.IsOpen {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment first.",
		}
	}

	// Send load roadnet request to core.
	mod.ExpState.StartRefreshRoadnet()
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
	mod.ExpState.EndRefreshRoadnet()

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpState,
	}
}
