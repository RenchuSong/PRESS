package example

import (
	"time"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
)

// RegisterPing request handlers.
func RegisterPing(r *gin.RouterGroup, cq *util.TaskQueue, oq *util.TaskQueue) {
	r.GET("/ping", func(c *gin.Context) {
		oq.Add(c, PingGet)
	})
	r.POST("/ping", func(c *gin.Context) {
		oq.Add(c, PingPost)
	})
}

// PingGet is get example.
func PingGet(c *gin.Context, b interface{}) *util.TaskResult {
	return &util.TaskResult{
		Code: 200,
		Data: "pong",
	}
}

// PingPost is post example.
func PingPost(c *gin.Context, b interface{}) *util.TaskResult {
	time.Sleep(5 * time.Second)
	return &util.TaskResult{
		Code:    200,
		Message: "pong",
		Data:    b,
	}
}
