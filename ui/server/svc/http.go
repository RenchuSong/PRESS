package svc

import (
	"fmt"
	"net/http"

	"github.com/gin-contrib/static"
	"github.com/gin-gonic/gin"
	"github.com/thinkerou/favicon"
)

// HTTP service.
type HTTP struct {
	port int
	web  *gin.Engine
}

// NewHTTP creates a new HTTP service.
func NewHTTP(s *SSEHandler, c *Config) *HTTP {
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

	return &HTTP{
		web:  r,
		port: c.Port,
	}
}

// Run spawns the HTTP service.
func (s *HTTP) Run() {
	s.web.Run(fmt.Sprintf(":%v", s.port))
}
