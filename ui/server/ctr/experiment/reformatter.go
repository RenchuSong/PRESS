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

// RegisterReformatter request handlers.
func RegisterReformatter(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.PUT("/reformatter/reformat", func(c *gin.Context) {
		cq.Add(c, ReformatToPRESS)
	})
	r.PUT("/reformatter/dumptobinary/:folder", func(c *gin.Context) {
		cq.Add(c, DumpPRESSToBinary)
	})
	r.PUT("/reformatter/loadfrombinary/:folder", func(c *gin.Context) {
		cq.Add(c, LoadPRESSFromBinary)
	})
	r.PUT("/reformatter/clear", func(c *gin.Context) {
		cq.Add(c, ClearPRESS)
	})
	r.GET("/reformatter/list", func(c *gin.Context) {
		cq.Add(c, ListTrajectories)
	})
	r.GET("/reformatter/press/:folder/:id", func(c *gin.Context) {
		cq.Add(c, GetPRESSTrajectory)
	})
}

// ReformatToPRESS reformts all map matched trajectories to PRESS trajectories.
func ReformatToPRESS(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot reformat GPS to PRESS trajectories before roadnet and SPTable are loaded.
	if !mod.ExpCtx.IsRoadnetReady() || !mod.ExpCtx.IsSPTableReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet and SP table first.",
		}
	}

	// Send reformat trajectories request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd string
	}{
		Cmd: "ReformatTrajectories",
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to reformat to PRESS trajectories: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
	}
}

// DumpPRESSToBinary dumps PRESS trajectories to binary.
func DumpPRESSToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot dump PRESS trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send dump PRESS request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "DumpPRESSTrajectoriesToBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to dump PRESS trajectories: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
	}
}

// LoadPRESSFromBinary loads PRESS trajectories from binary.
func LoadPRESSFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot load PRESS trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send load PRESS trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "LoadPRESSTrajectoriesFromBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load PRESS trajectories: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}

// ClearPRESS clears PRESS trajectories.
func ClearPRESS(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot clear PRESS trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send clear PRESS trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd string
	}{
		Cmd: "ClearPRESSTrajectories",
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to clear PRESS trajectories: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return &util.TaskResult{
		Code:    200,
		Message: ret.Message,
		Data:    mod.ExpCtx,
	}
}

// ListTrajectories lists all GPS/PRESS trajectories in binary folder.
func ListTrajectories(c *gin.Context, b interface{}) *util.TaskResult {
	if !mod.ExpCtx.IsOpen {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment first.",
		}
	}

	fd := path.Join(
		ctr.Config.Experiments,
		"Experiment_"+strconv.Itoa(mod.ExpCtx.ID),
		"gps_trajectories",
	)
	if fe, _ := util.FileExists(fd); !fe {
		return &util.TaskResult{
			Code: 200,
			Data: make([]string, 0),
		}
	}
	gps, err := util.ListDir(fd)
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get all GPS trajectories: " + err.Error(),
		}
	}
	gpsRet := make([]string, 0, len(gps))
	for _, n := range gps {
		if !strings.HasPrefix(n, ".") {
			gpsRet = append(gpsRet, n)
		}
	}

	press, err := util.ListDir(path.Join(
		ctr.Config.Experiments,
		"Experiment_"+strconv.Itoa(mod.ExpCtx.ID),
		"press_trajectories",
	))
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get all PRESS trajectories: " + err.Error(),
		}
	}
	pressRet := make([]string, 0, len(gps))
	for _, n := range press {
		if !strings.HasPrefix(n, ".") {
			pressRet = append(pressRet, n)
		}
	}

	if len(gpsRet) != len(pressRet) {
		return &util.TaskResult{
			Code:    500,
			Message: "GPS trajectory and PRESS trajectory number mismatch.",
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: pressRet,
	}
}

// GetPRESSTrajectory gets one GPS trajectory.
func GetPRESSTrajectory(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot show a PRESS trajectory before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send show PRESS trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
		ID     string
	}{
		Cmd:    "ShowPRESSTrajectory",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
		ID:     c.Param("id"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to show PRESS trajectory: " + err.Error(),
		}
	}
	if !ret.Success {
		return &util.TaskResult{
			Code:    500,
			Message: ret.Message,
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret.Data,
	}
}
