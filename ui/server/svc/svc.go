package svc

import (
	"fmt"
	"net/http"
	"time"

	"github.com/gin-contrib/static"
	"github.com/gin-gonic/gin"

	log "github.com/sirupsen/logrus"
)

// Config for service.
type Config struct {
	Experiments string
	Data        string
	Logs        string
	LogLevel    string
	Port        int
	Static      string
	APIHandlers int
}

// Service for App
type Service struct {
	config Config
	web    *gin.Engine
}

// NewService creates a new service.
func NewService(c Config) *Service {
	r := gin.Default()
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"message": "pong",
		})
	})
	r.Use(static.Serve("/", static.LocalFile(c.Static, true)))
	r.NoRoute(func(ctx *gin.Context) {
		ctx.File(c.Static + "index.html")
	})

	return &Service{
		web:    r,
		config: c,
	}
}

// Run spawns the service.
func (s *Service) Run() {
	log.SetFormatter(&log.TextFormatter{
		FullTimestamp: true,
	})

	go s.web.Run(fmt.Sprintf(":%v", s.config.Port))

	for {
		time.Sleep(time.Second)
	}
}
