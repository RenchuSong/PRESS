package experiment

import (
	"fmt"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/mod"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterMapMatcher request handlers.
func RegisterMapMatcher(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/mapmatcher/datasources", func(c *gin.Context) {
		cq.Add(c, GetGPSFileSources)
	})
	r.PUT("/mapmatcher/add", func(c *gin.Context) {
		cq.Add(c, AddGPSAndMapMatch)
	})
	r.PUT("/mapmatcher/dumpgpstobinary", func(c *gin.Context) {
		cq.Add(c, DumpGPSToBinary)
	})
	r.PUT("/mapmatcher/dumptobinary", func(c *gin.Context) {
		cq.Add(c, DumpMapMatchedToBinary)
	})
	r.PUT("/mapmatcher/loadgpsfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadFromBinary)
	})
	r.PUT("/mapmatcher/loadfrombinary", func(c *gin.Context) {
		cq.Add(c, LoadMapMatchedFromBinary)
	})
	r.PUT("/mapmatcher/clear", func(c *gin.Context) {
		cq.Add(c, ClearGPSAndMapMatched)
	})
	r.GET("/mapmatcher/gpssamplerates", func(c *gin.Context) {
		cq.Add(c, GetGPSSampleRates)
	})
}

// GetGPSFileSources gets all GPS folder sources.
func GetGPSFileSources(c *gin.Context, b interface{}) *util.TaskResult {
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
		Data:    mod.ExpCtx,
	}
}

// DumpGPSToBinary dumps GPS trajectories to binary.
func DumpGPSToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// DumpMapMatchedToBinary dumps map matched trajectories to binary.
func DumpMapMatchedToBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// LoadFromBinary loads GPS trajectory from binary.
func LoadFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// ClearGPSAndMapMatched loads map matched trajectory from binary.
func ClearGPSAndMapMatched(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// LoadMapMatchedFromBinary clears GPS and map matched trajectories.
func LoadMapMatchedFromBinary(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}

// GetGPSSampleRates gets GPS trajectories sample rates.
func GetGPSSampleRates(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code:    500,
		Message: "Not implemented.",
	}
}
