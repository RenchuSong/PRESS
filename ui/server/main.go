package main

import (
	"encoding/json"
	"flag"
	"io/ioutil"
	"net/http"

	log "github.com/sirupsen/logrus"

	"github.com/gin-gonic/gin"
)

// PRESS config.
type config struct {
	Experiments string          `json:"experiments"`
	Data        string          `json:"data"`
	AppServer   appServerConfig `json:"appServer"`
}

// App server config
type appServerConfig struct {
	Logs           string `json:"logs"`
	LogLevel       string `json:"logLevel"`
	Port           int    `json:"port"`
	StaticHandlers int    `json:"staticHandlers"`
	APIHandlers    int    `json:"apiHandlers"`
}

func main() {
	var confPath string
	flag.StringVar(&confPath, "config", "./config.json", "the config file path")
	flag.Parse()
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
	str, err := json.Marshal(c)

	log.Infof(string(str))

	r := gin.Default()
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"message": "pong",
		})
	})
	r.Run() // listen and serve on 0.0.0.0:8080 (for windows "localhost:8080")
}
