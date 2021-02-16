package util

import (
	"io/ioutil"
	"os"
)

// ListDir lists all directories in given path.
func ListDir(path string) ([]string, error) {
	files, err := ioutil.ReadDir(path)
	if err != nil {
		return []string{}, err
	}
	ret := make([]string, 0, len(files))
	for _, f := range files {
		ret = append(ret, f.Name())
	}
	return ret, nil
}

// FileExists check if file exists.
func FileExists(name string) (bool, error) {
	_, err := os.Stat(name)
	if os.IsNotExist(err) {
		return false, nil
	}
	return err == nil, err
}

// FileSize returns the size of the file.
func FileSize(name string) (int64, error) {
	s, err := os.Stat(name)
	if err != nil {
		return 0, err
	}
	return s.Size(), nil
}

// RemoveFile removes a file.
func RemoveFile(name string) error {
	e, err := FileExists(name)
	if err != nil {
		return err
	}
	if !e {
		return nil
	}
	return os.Remove(name)
}
