package ctr

// CtrlConfig for controller.
type CtrlConfig struct {
	Experiments string
	Data        string
}

// Config for controller.
var Config CtrlConfig

// SetConfig for controller.
func SetConfig(expDir, dataDir string) {
	Config = CtrlConfig{Experiments: expDir, Data: dataDir}
}
