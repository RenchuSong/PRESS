package experiment

import (
	"fmt"
	"strconv"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterMapMatcher request handlers.
func RegisterMapMatcher(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/mapmatcher/foldersources", func(c *gin.Context) {
		cq.Add(c, GetGPSFolderSources)
	})
	r.GET("/mapmatcher/filesources/:folder", func(c *gin.Context) {
		cq.Add(c, GetGPSFileSources)
	})
	r.GET("/mapmatcher/readertypes", func(c *gin.Context) {
		cq.Add(c, GetGPSReaderTypes)
	})
	r.PUT("/mapmatcher/add", func(c *gin.Context) {
		cq.Add(c, AddGPSAndMapMatch)
	})
	r.PUT("/mapmatcher/dumpgpstobinary/:folder", func(c *gin.Context) {
		cq.Add(c, DumpGPSToBinary)
	})
	r.PUT("/mapmatcher/dumptobinary/:folder", func(c *gin.Context) {
		cq.Add(c, DumpMapMatchedToBinary)
	})
	r.PUT("/mapmatcher/loadgpsfrombinary/:folder", func(c *gin.Context) {
		cq.Add(c, LoadGPSFromBinary)
	})
	r.PUT("/mapmatcher/loadfrombinary/:folder", func(c *gin.Context) {
		cq.Add(c, LoadMapMatchedFromBinary)
	})
	r.PUT("/mapmatcher/clear", func(c *gin.Context) {
		cq.Add(c, ClearGPSAndMapMatched)
	})
	r.GET("/mapmatcher/gpssamplerates", func(c *gin.Context) {
		cq.Add(c, GetGPSSampleRates)
	})
	r.GET("/mapmatcher/gps/:folder/:id", func(c *gin.Context) {
		cq.Add(c, GetGPSTrajectory)
	})
}

// GetGPSFolderSources gets all GPS folder sources.
func GetGPSFolderSources(c *gin.Context, b interface{}) *util.TaskResult {
	ret, err := getAllGPSFolderSources()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get GPS folder sources: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}

// GetGPSFileSources gets all GPS file sources.
func GetGPSFileSources(c *gin.Context, b interface{}) *util.TaskResult {
	ret, err := getAllGPSFileSources(c.Param("folder"))

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get GPS folder sources: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret,
	}
}

// GetGPSReaderTypes gets all roadnet reader types.
func GetGPSReaderTypes(c *gin.Context, b interface{}) *util.TaskResult {
	// Send get roadnet reader types request to core.
	util.Core.SendRequest(struct {
		Cmd string
	}{
		Cmd: "GetGPSReaderTypes",
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to get GPS reader types: " + err.Error(),
		}
	}

	return &util.TaskResult{
		Code: 200,
		Data: ret.Data,
	}
}

// AddGPSAndMapMatch adds GPS trajectory and map match to roadnet.
func AddGPSAndMapMatch(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot build grid index before roadnet is loaded.
	if !mod.ExpCtx.IsGridIndexReady() || !mod.ExpCtx.IsSPTableReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please build grid index and SP table first.",
		}
	}

	body := b.(map[string]interface{})

	sigmaZ := body["sigmaZ"].(float64)
	if sigmaZ <= 0 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid sigmaZ range (0, +oo): " + fmt.Sprintf("%f", sigmaZ),
		}
	}
	maxGPSBias := body["maxGPSBias"].(float64)
	if maxGPSBias <= 0 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid maxGPSBias range (0, +oo): " + fmt.Sprintf("%f", maxGPSBias),
		}
	}
	maxDistDifference := body["maxDistDifference"].(float64)
	if maxDistDifference <= 0 {
		return &util.TaskResult{
			Code:    400,
			Message: "Out of valid maxDistDifference range (0, +oo): " + fmt.Sprintf("%f", maxDistDifference),
		}
	}

	// Send add GPS trajectory and map match request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd                     string
		SigmaZ                  float64
		MaxGPSBias              float64
		MaxDistDifference       float64
		FileName                string
		GPSTrajectoryReaderType string
	}{
		Cmd:                     "AddGPSTrajectoryAndMapMatch",
		SigmaZ:                  sigmaZ,
		MaxGPSBias:              maxGPSBias,
		MaxDistDifference:       maxDistDifference,
		FileName:                body["fileName"].(string),
		GPSTrajectoryReaderType: body["gpsTrajectoryReaderType"].(string),
	})
	ret, err := util.Core.GetResponse()

	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to add GPS trajectory and map match: " + err.Error(),
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

// DumpGPSToBinary dumps GPS trajectories to binary.
func DumpGPSToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot dump GPS trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send dump GPS request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "DumpGPSTrajectoriesToBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to dump GPS trajectories: " + err.Error(),
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

// DumpMapMatchedToBinary dumps map matched trajectories to binary.
func DumpMapMatchedToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot dump map matched trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send dump map matched request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "DumpMapMatchedTrajectoriesToBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to dump map matched trajectories: " + err.Error(),
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

// LoadGPSFromBinary loads GPS trajectory from binary.
func LoadGPSFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot load GPS trajectories before roadnet and SPTable is ready.
	if !mod.ExpCtx.IsRoadnetReady() || !mod.ExpCtx.IsSPTableReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet and SPTable first.",
		}
	}

	// Send load GPS trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "LoadGPSTrajectoriesFromBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load GPS trajectories: " + err.Error(),
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

// ClearGPSAndMapMatched clears GPS and map matched trajectories from core memory.
func ClearGPSAndMapMatched(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot clear GPS & map matched trajectories before roadnet and SPTable is ready.
	if !mod.ExpCtx.IsRoadnetReady() || !mod.ExpCtx.IsSPTableReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet and SPTable first.",
		}
	}

	// Send clear GPS & map matched trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd string
	}{
		Cmd: "ClearGPSAndMapMatchedTrajectories",
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to clear GPS & map matched trajectories: " + err.Error(),
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

// LoadMapMatchedFromBinary clears GPS and map matched trajectories.
func LoadMapMatchedFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot load map matched trajectories before roadnet is ready.
	if !mod.ExpCtx.IsRoadnetReady() {
		return &util.TaskResult{
			Code:    500,
			Message: "Please load roadnet first.",
		}
	}

	// Send load map matched trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
	}{
		Cmd:    "LoadMapMatchedTrajectoriesFromBinary",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to load map matched trajectories: " + err.Error(),
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

// GetGPSSampleRates gets GPS trajectories sample rates.
func GetGPSSampleRates(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// GetGPSTrajectory gets one GPS trajectory.
func GetGPSTrajectory(c *gin.Context, b interface{}) *util.TaskResult {
	// Cannot show a GPS trajectory before experiment is open.
	if !mod.ExpCtx.IsOpen {
		return &util.TaskResult{
			Code:    500,
			Message: "Please open an experiment first.",
		}
	}

	// Send show GPS trajectory request to core.
	mod.ExpCtx.LockCtxLock()
	defer mod.ExpCtx.UnlockCtxLock()
	util.Core.SendRequest(struct {
		Cmd    string
		Folder string
		ID     string
	}{
		Cmd:    "ShowGPSTrajectory",
		Folder: "Experiment_" + strconv.Itoa(mod.ExpCtx.ID) + "/" + c.Param("folder"),
		ID:     c.Param("id"),
	})

	ret, err := util.Core.GetResponse()
	if err != nil {
		return &util.TaskResult{
			Code:    500,
			Message: "Failed to show GPS trajectory: " + err.Error(),
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
