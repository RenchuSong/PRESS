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
	experiment.RegisterExperiment,
	experiment.RegisterRoadnet,
	experiment.RegisterGridIndex,
	experiment.RegisterSPTable,
	experiment.RegisterMapMatcher,
	experiment.RegisterReformatter,
}

// NewHTTP creates a new HTTP service.
func NewHTTP(
	s *SSEHandler,
	cq *util.TaskQueue,
	oq *util.TaskQueue,
	c *Config,
) *HTTP {
	r := gin.Default()
	r.Use(CORSMiddleware())
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

	util.NewCore(c.Pipe)

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

		if c.Request.Method == "OPTIONS" {
			c.AbortWithStatus(204)
		}

		c.JSON(http.StatusOK, gin.H{
			"message": "Request received",
		})
	}
}

// CORSMiddleware get CORS work during development.
// TODO: Remove for production.
func CORSMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		c.Writer.Header().Set("Access-Control-Allow-Origin", "*")
		c.Writer.Header().Set("Access-Control-Allow-Credentials", "true")
		c.Writer.Header().Set("Access-Control-Allow-Methods", "POST, OPTIONS, GET, PUT, DELETE")
		c.Writer.Header().Set("Access-Control-Expose-Headers", "x-request-id")
		c.Writer.Header().Set("Access-Control-Allow-Headers", "Content-Type, Content-Length, Accept-Encoding, X-CSRF-Token, Authorization, accept, origin, Cache-Control, X-Requested-With")
		if c.Request.Method == "OPTIONS" {
			c.AbortWithStatus(204)
		}

		c.Next()
	}
}
