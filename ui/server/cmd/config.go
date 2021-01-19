package main

import (
	"encoding/json"
	"io/ioutil"

	log "github.com/sirupsen/logrus"
)

// PRESS config.
type config struct {
	Experiments string          `json:"experiments"`
	Data        string          `json:"data"`
	AppServer   appServerConfig `json:"appServer"`
}

// App server config.
type appServerConfig struct {
	Logs             string `json:"logs"`
	LogLevel         string `json:"logLevel"`
	Port             int    `json:"port"`
	Static           string `json:"static"`
	CoreAPIHandlers  int    `json:"coreApiHandlers"`
	OtherAPIHandlers int    `json:"otherApiHandlers"`
}

func readConfig(confPath string) *config {
	data, err := ioutil.ReadFile(confPath)
	if err != nil {
		log.Fatalf(
			"failed to load config file %v: %v",
			confPath,
			err,
		)
	}
	var c config
	if err := json.Unmarshal(data, &c); err != nil {
		log.Fatalf(
			"failed to unmarshal the config file: %v",
			err,
		)
	}
	return &c
}
