clp-ffi-java is a library to encode log messages with 
[CLP](https://github.com/y-scope/clp), and work with the encoded messages using
a foreign function interface (FFI).

# Requirements

* A C++ compiler that supports C++-17
* CMake 3.5.1 or higher

# Setup

* Initialize git submodules
  ```shell
  git submodule update --init --recursive
  ```
  * You should run this command any time you update this repo's submodules.

# Common build commands

* Build and test
  ```shell
  mvn package
  ```
* Build without any extras
  ```shell
  mvn package -DskipTests -Dmaven.javadoc.skip=true -Dmaven.source.skip
  ```
* Generate the JNI headers
  ```shell
  mvn generate-sources
  ```
* Build the native library
  ```shell
  mvn generate-resources
  ```
* Build and package the native library
  ```shell
  mvn generate-resources validate assembly:single@assemble-lib-dir
  ```
  * The validate phase is necessary to remove spaces from the OS name

# Testing

```shell
mvn test
```

# Linting

Before submitting a PR, ensure you've run our linting tools and either fixed any violations or
suppressed the warning. If you can't run the linting workflows locally, you can enable and run the
[lint] workflow in your fork.

## Requirements

We currently support running our linting tools on Linux and macOS. If you're developing on another
OS, you can submit a [feature request][feature-req], or use our [lint] workflow in your fork.

To run the linting tools, besides commonly installed tools like `tar`, you'll need:

* `curl`
* `md5sum`
* Python 3.8 or newer
* python3-venv
* [Task]

## Running the linters

```shell
task lint
```

[clp-lint]: https://github.com/y-scope/clp-ffi-java/blob/main/.github/workflows/lint.yaml
[feature-req]: https://github.com/y-scope/clp-ffi-java/issues/new?assignees=&labels=enhancement&projects=&template=feature-request.yml
[Task]: https://taskfile.dev/
