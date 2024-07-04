#!/usr/bin/env bash

apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential cmake maven openjdk-11-jdk

export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-*
mvn --batch-mode generate-resources validate assembly:single@assemble-lib-dir
