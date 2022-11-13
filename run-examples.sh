#!/bin/bash

set -u

for f in Examples/* ; do
	bash run-example.sh $f
done
