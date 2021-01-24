package util

import "io/ioutil"

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
