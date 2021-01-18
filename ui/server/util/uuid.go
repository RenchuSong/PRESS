package util

import "github.com/rs/xid"

// NewUUID returns a new UUID.
func NewUUID() string {
	return xid.New().String()
}
