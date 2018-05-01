#!/bin/bash

mkdocs build
rm -fR assets tests
mv site/* .
