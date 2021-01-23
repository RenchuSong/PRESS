package util

import (
	"io/ioutil"

	log "github.com/sirupsen/logrus"

	"github.com/gin-gonic/gin"
)

// Task is a request to be handled.
type Task struct {
	ID      string
	Ctx     *gin.Context
	Body    string
	Handler func(c *gin.Context, b interface{}) *TaskResult
}

// TaskResult is the execution result of the task.
type TaskResult struct {
	ID      string      `json:"requestID"`
	Code    int         `json:"code"`
	Message string      `json:"message"`
	Data    interface{} `json:"data"`
}

// TaskQueue is the queue for all tasks.
type TaskQueue struct {
	Name  string
	Tasks chan Task
}

// NewTaskQueue creates a new task queue.
func NewTaskQueue(n string, bs int) *TaskQueue {
	if bs < 1 {
		log.Panicf("Invalid task queue buffer size %v for %s", bs, n)
	}
	return &TaskQueue{
		Name:  n,
		Tasks: make(chan Task, bs),
	}
}

// Add a task to the queue.
func (t *TaskQueue) Add(
	c *gin.Context,
	handler func(c *gin.Context, b interface{}) *TaskResult,
) {
	body := "{}"
	if b, err := ioutil.ReadAll(c.Request.Body); err == nil && len(b) > 0 {
		body = string(b)
	}
	t.Tasks <- Task{
		ID:      c.Writer.Header().Get("x-request-id"),
		Ctx:     c.Copy(),
		Body:    body,
		Handler: handler,
	}
}

// Get a task from the queue.
func (t *TaskQueue) Get() *Task {
	if tsk, ok := <-t.Tasks; ok {
		return &tsk
	}
	log.Errorf("Task queue %s is closed, cannot get more tasks.", t.Name)
	return nil
}

func respondWithError(c *gin.Context, code int, message interface{}) {
	c.AbortWithStatusJSON(code, gin.H{"error": message})
}
