package experiment

import (
	"sync"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

var expListLock sync.RWMutex

// RegisterExperiments request handlers.
func RegisterExperiments(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/experiments", func(c *gin.Context) {
		oq.Add(c, ExperimentsGet)
	})
	r.POST("/experiments", func(c *gin.Context) {
		oq.Add(c, ExperimentsPost)
	})
	r.DELETE("/experiments/:id", func(c *gin.Context) {
		oq.Add(c, ExperimentsDelete)
	})
}

// ExperimentsGet gets all experiments.
func ExperimentsGet(c *gin.Context, b interface{}) *util.TaskResult {
	expListLock.RLock()
	defer expListLock.RUnlock()

	ret, err := getAllExperiments()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get all experiments: " + err.Error(),
		}
	}
	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}

// ExperimentsPost creates a new experiment.
func ExperimentsPost(c *gin.Context, b interface{}) *util.TaskResult {
	body := b.(map[string]interface{})

	expListLock.Lock()
	defer expListLock.Unlock()

	ret, err := createExperiment(body["name"].(string), body["image"])

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to create experiment: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}

// ExperimentsDelete deletes an experiment.
func ExperimentsDelete(c *gin.Context, b interface{}) *util.TaskResult {
	expListLock.Lock()
	defer expListLock.Unlock()

	if err := deleteExperiment(c.Param("id")); err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to delete experiment: " + err.Error(),
		}
	}

	ret, err := getAllExperiments()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get all experiments: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}
