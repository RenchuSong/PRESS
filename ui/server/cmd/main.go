package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"net/http"

	log "github.com/sirupsen/logrus"

	"github.com/gin-gonic/gin"
)

func main() {
	var confPath string
	flag.StringVar(&confPath, "config", "./config.json", "the config file path")
	flag.Parse()

	c := readConfig(confPath)
	str, _ := json.Marshal(c)

	log.Infof(string(str))

	r := gin.Default()
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"message": "pong",
		})
	})
	r.Run(fmt.Sprintf(":%v", c.AppServer.Port))
}
