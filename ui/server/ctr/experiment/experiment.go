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
	r.GET("/experiment/auxiliaries", func(c *gin.Context) {
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
	if !mod.ExpCtx.IsOpen {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment first.",
		}
	}
	ef := path.Join(
		ctr.Config.Experiments, "Experiment_"+strconv.Itoa(mod.ExpCtx.ID),
	)
	bfs, err := util.ListDir(ef)
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to list all auxiliary binary files: " + err.Error(),
		}
	}
	ret := make([]*AuxiliaryInfo, 0, len(bfs))
	for _, f := range bfs {
		if strings.HasSuffix(f, ".bin") {
			s, err := util.FileSize(path.Join(ef, f))
			if err != nil {
				return &util.TaskResult{
					Code:    500,
					Message: "Failed to get file size: " + f,
				}
			}
			ret = append(ret, &AuxiliaryInfo{
				Filename: f,
				Size:     s,
			})
		}
	}
	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}
