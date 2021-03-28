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
			Message: "Out of valid width range [50, 1000]: " + strconv.Itoa(width),
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
			Message: "Out of valid height range [50, 1000]: " + strconv.Itoa(height),
		}
	}

	// Send build grid index request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	mod.ExpCtx.StartBuildGridIndex()
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
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}
	mod.ExpCtx.EndBuildGridIndex()

	if err := resetGridIndex(mod.ExpCtx.ID); err != nil {
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

// DumpGridIndexToBinary dumps grid index to binary.
func DumpGridIndexToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot dump grid index before it's been loaded.
	if !mod.ExpCtx.IsOpen || !mod.ExpCtx.GridIndexReady {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment and build grid index first.",
		}
	}

	// Send dump grid index request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "DumpGridIndexToBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to dump grid index: " + err.Error(),
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

// LoadGridIndexFromBinary loads grid index from binary.
func LoadGridIndexFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Only load grid index in an open experiment with roadnet loaded.
	if !mod.ExpCtx.IsExpOpen() || !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment and load roadnet first.",
		}
	}

	// Send load grid index request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	mod.ExpCtx.StartBuildGridIndex()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "LoadGridIndexFromBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load grid index: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}
	mod.ExpCtx.EndBuildGridIndex()

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}
