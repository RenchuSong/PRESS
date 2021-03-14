package experiment

import (
	"strconv"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterSPTable request handlers.
func RegisterSPTable(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/sptable/build/:distance", func(c *gin.Context) {
		cq.Add(c, BuildSPTable)
	})
	r.PUT("/sptable/dumptobinary", func(c *gin.Context) {
		cq.Add(c, DumpSPTableToBinary)
	})
	r.PUT("/sptable/loadfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadSPTableFromBinary)
	})
}

// BuildSPTable builds SPTable with given distance.
func BuildSPTable(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot build SPTable before roadnet is loaded.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	distance, err := strconv.Atoi(c.Param("distance"))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Invalid distance: " + err.Error(),
		}
	}
	if distance < 2000 || distance > 10000 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid distance range [2000, 10000]: ",
		}
	}

	// Send build SPTable request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	mod.ExpCtx.StartBuildSPTable()
	util.Core.SendRequest(struct {
		Cmd     string
		MaxDist int
	}{
		Cmd:     "BuildSPTable",
		MaxDist: distance,
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to build SPTable: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}
	mod.ExpCtx.EndBuildSPTable()

	if err := resetSPTable(mod.ExpCtx.ID); err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: err.Error(),
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}

// DumpSPTableToBinary dumps SPTable to binary.
func DumpSPTableToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot dump SPTable before it's been loaded.
	if !mod.ExpCtx.IsOpen || !mod.ExpCtx.SPTableReady {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment and build SPTable first.",
		}
	}

	// Send dump SPTable request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "DumpSPTableToBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to dump SPTable: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return Auxiliaries(c, b)
}

// LoadSPTableFromBinary loads SPTable from binary.
func LoadSPTableFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Only load SPTable in an open experiment with roadnet loaded.
	if !mod.ExpCtx.IsExpOpen() || !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment and load roadnet first.",
		}
	}

	// Send load SPTable request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	mod.ExpCtx.StartBuildSPTable()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "LoadSPTableFromBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load SPTable: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}
	mod.ExpCtx.EndBuildSPTable()

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}
