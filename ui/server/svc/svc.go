package svc

import (
	"fmt"
	"net/http"
	"os"
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

func setLogger(c *Config) {
	log.SetFormatter(&log.TextFormatter{
		FullTimestamp: true,
	})
	switch c.LogLevel {
	case "ERROR":
		log.SetLevel(log.ErrorLevel)
	case "WARN":
		log.SetLevel(log.WarnLevel)
	case "INFO":
		log.SetLevel(log.InfoLevel)
	case "DEBUG":
		log.SetLevel(log.DebugLevel)
	default:
		log.SetLevel(log.InfoLevel)
	}
	f, err := os.OpenFile(
		c.Logs+"access.log",
		os.O_APPEND|os.O_CREATE|os.O_WRONLY,
		0644,
	)
	if err != nil {
		log.Fatal("Failed to open log file, ", err)
	}
	log.SetOutput(f)
}

// Run spawns the service.
func (s *Service) Run() {
	setLogger(&s.config)

	go s.sse.Run()
	go s.web.Run(fmt.Sprintf(":%v", s.config.Port))

	for {
		time.Sleep(time.Second)
	}
}
