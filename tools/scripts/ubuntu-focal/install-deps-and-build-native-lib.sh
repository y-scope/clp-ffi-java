#!/usr/bin/env bash

apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential cmake maven openjdk-11-jdk

mvn --batch-mode generate-resources validate assembly:single@assemble-lib-dir
