package example

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/exe"
	"github.com/gin-gonic/gin"
)

// Register request handlers.
func Register(r *gin.RouterGroup, cq *exe.TaskQueue, oq *exe.TaskQueue) {
	r.GET("/ping", func(c *gin.Context) {
		oq.Add(c, PingGet)
	})
	r.POST("/ping", func(c *gin.Context) {
		oq.Add(c, PingPost)
	})
}

// PingGet is get example.
func PingGet(c *gin.Context, b interface{}) *exe.TaskResult {
	return &exe.TaskResult{
		Code: 200,
		Data: "pong",
	}
}

// PingPost is post example.
func PingPost(c *gin.Context, b interface{}) *exe.TaskResult {
	return &exe.TaskResult{
		Code:    200,
		Message: "pong",
		Data:    b,
	}
}
