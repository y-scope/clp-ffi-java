#!/usr/bin/env bash

# Exit on error
set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
project_dir="${script_dir}/../../../"

apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential cmake maven

cd "$project_dir"
mvn --batch-mode validate generate-resources test assembly:single@assemble-lib-dir
