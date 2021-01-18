package exe

import (
	"fmt"
	"io/ioutil"

	log "github.com/sirupsen/logrus"

	"github.com/gin-gonic/gin"
)

// Task is a request to be handled.
type Task struct {
	id      string
	ctx     *gin.Context
	body    string
	handler func(c *gin.Context, b interface{}) *TaskResult
}

// TaskResult is the execution result of the task.
type TaskResult struct {
	Code    int
	Message string
	Data    interface{}
}

// TaskQueue is the queue for all tasks.
type TaskQueue struct {
	name  string
	tasks chan Task
}

// NewTaskQueue creates a new task queue.
func NewTaskQueue(n string, bs int) *TaskQueue {
	if bs < 1 {
		log.Panicf("Invalid task queue buffer size %v for %s", bs, n)
	}
	return &TaskQueue{
		name:  n,
		tasks: make(chan Task, bs),
	}
}

// Add a task to the queue.
func (t *TaskQueue) Add(
	c *gin.Context,
	handler func(c *gin.Context, b interface{}) *TaskResult,
) {
	fmt.Println("register")
	body := ""
	if b, err := ioutil.ReadAll(c.Request.Body); err == nil && len(b) > 0 {
		body = string(b)
	}
	t.tasks <- Task{
		id:      c.GetHeader("X-Request-Id"),
		ctx:     c.Copy(),
		body:    body,
		handler: handler,
	}
}

// Get a task from the queue.
func (t *TaskQueue) Get() *Task {
	if tsk, ok := <-t.tasks; ok {
		return &tsk
	}
	log.Errorf("Task queue %s is closed, cannot get more tasks.", t.name)
	return nil
}

func respondWithError(c *gin.Context, code int, message interface{}) {
	c.AbortWithStatusJSON(code, gin.H{"error": message})
}
