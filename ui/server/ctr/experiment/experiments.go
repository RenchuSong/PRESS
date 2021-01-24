package experiment

import (
	"encoding/json"
	"io/ioutil"
	"path"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/ctr"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterExperiments request handlers.
func RegisterExperiments(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/experiments", func(c *gin.Context) {
		oq.Add(c, ExperimentsGet)
	})
}

// ExperimentsGet gets all experiments.
func ExperimentsGet(c *gin.Context, b interface{}) *util.TaskResult {
	exp, err := util.ListDir(ctr.Config.Experiments)
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get all experiments: " + err.Error(),
		}
	}
	ret := make([]mod.ExperimentMeta, 0, len(exp))
	for _, n := range exp {
		metaStr, err := ioutil.ReadFile(
			path.Join(ctr.Config.Experiments, n, "meta.json"),
		)
		if err == nil {
			var meta mod.ExperimentMeta
			if err := json.Unmarshal(metaStr, &meta); err == nil {
				ret = append(ret, meta)
			}
		}
	}
	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}
