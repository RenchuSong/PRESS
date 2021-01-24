package svc

import (
	"fmt"
	"net/http"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/ctr"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/ctr/example"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/ctr/experiment"
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-contrib/static"
	"github.com/gin-gonic/gin"
	"github.com/thinkerou/favicon"
)

// HTTP service.
type HTTP struct {
	port int
	web  *gin.Engine
}

var routes = []func(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue){
	example.RegisterPing,
	experiment.RegisterExperiments,
}

// NewHTTP creates a new HTTP service.
func NewHTTP(
	s *SSEHandler,
	cq *util.TaskQueue,
	oq *util.TaskQueue,
	c *Config,
) *HTTP {
	r := gin.Default()
	r.Use(favicon.New(c.Static + "favicon.ico"))
	r.Use(static.Serve("/", static.LocalFile(c.Static, true)))
	r.Use(requestIDMiddleware())
	r.NoRoute(func(ctx *gin.Context) {
		ctx.File(c.Static + "index.html")
	})
	r.GET("/subscribe", func(c *gin.Context) {
		s.Subscribe(c)
	})
	api := r.Group("/api")
	api.Use(respMiddleware())
	ctr.SetConfig(c.Experiments, c.Data)
	for _, regFn := range routes {
		regFn(api, cq, oq)
	}

	return &HTTP{
		web:  r,
		port: c.Port,
	}
}

// Run spawns the HTTP service.
func (s *HTTP) Run() {
	s.web.Run(fmt.Sprintf(":%v", s.port))
}

func requestIDMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		id := util.NewUUID()
		c.Writer.Header().Set("x-request-id", id)
		c.Next()
	}
}

func respMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		c.Next()
		// TODO: Remove for production.
		c.Writer.Header().Set("access-control-allow-origin", "*")
		c.Writer.Header().Set("Access-Control-Allow-Origin", "*")
		c.Writer.Header().Set("Access-Control-Expose-Headers", "x-request-id")
		c.JSON(http.StatusOK, gin.H{
			"message": "Request received",
		})
	}
}
