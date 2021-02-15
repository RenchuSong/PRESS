package experiment

import (
	"path"
	"strconv"
	"strings"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/ctr"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterExperiment request handlers.
func RegisterExperiment(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/experiment/open/:id", func(c *gin.Context) {
		oq.Add(c, Open)
	})
	r.PUT("/experiment/close", func(c *gin.Context) {
		oq.Add(c, Close)
	})
	r.GET("/experiment/auxiliaries/:id", func(c *gin.Context) {
		oq.Add(c, Auxiliaries)
	})
}

// Open an experiment.
func Open(c *gin.Context, b interface{}) *util.TaskResult {
	id, err := strconv.Atoi(c.Param("id"))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Invalid experiment ID: " + err.Error(),
		}
	}
	exp, err := getExperimentByID(id)
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to check experiments " + err.Error(),
		}
	}
	if exp == nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Experiment doesn't exist.",
		}
	}

	if err := mod.ExpCtx.Open(id, exp.Name); err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to open experiment: " + err.Error(),
		}
	}
	return &util.TaskResult{
		Code: 200,
		Data: mod.ExpCtx,
	}
}

// Close the experiment.
func Close(c *gin.Context, b interface{}) *util.TaskResult {
	mod.ExpCtx.Close()
	return &util.TaskResult{
		Code: 200,
		Data: mod.ExpCtx,
	}
}

// Auxiliaries of binary files in the experiment folder.
func Auxiliaries(c *gin.Context, b interface{}) *util.TaskResult {
	_, err := strconv.Atoi(c.Param("id"))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Invalid experiment ID: " + err.Error(),
		}
	}
	bfs, err := util.ListDir(
		path.Join(ctr.Config.Experiments, "Experiment_"+c.Param("id")),
	)
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to list all auxiliary binary files: " + err.Error(),
		}
	}
	ret := make([]string, 0, len(bfs))
	for _, f := range bfs {
		if strings.HasSuffix(f, ".bin") {
			ret = append(ret, f)
		}
	}
	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}
