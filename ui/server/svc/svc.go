package svc

import (
	"fmt"
	"net/http"
	"time"

	"github.com/gin-contrib/static"
	"github.com/gin-gonic/gin"
	"github.com/thinkerou/favicon"

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

// Service for App.
type Service struct {
	config Config
	web    *gin.Engine
	sse    *SSEHandler
}

// NewService creates a new service.
func NewService(c Config) *Service {
	s := NewSSEHandler()

	r := gin.Default()
	r.Use(favicon.New(c.Static + "favicon.ico"))
	r.GET("/ping", func(c *gin.Context) {
		s.Send("123")
		c.JSON(http.StatusOK, gin.H{
			"message": "pong",
		})
	})
	r.GET("/api/subscribe", func(c *gin.Context) {
		s.Subscribe(c)
	})
	r.Use(static.Serve("/", static.LocalFile(c.Static, true)))
	r.NoRoute(func(ctx *gin.Context) {
		ctx.File(c.Static + "index.html")
	})

	return &Service{
		web:    r,
		sse:    s,
		config: c,
	}
}

// Run spawns the service.
func (s *Service) Run() {
	log.SetFormatter(&log.TextFormatter{
		FullTimestamp: true,
	})
	log.SetLevel(log.DebugLevel)

	go s.sse.Run()
	go s.web.Run(fmt.Sprintf(":%v", s.config.Port))

	for {
		time.Sleep(time.Second)
	}
}
