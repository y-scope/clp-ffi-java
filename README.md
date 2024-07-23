clp-ffi-java is a library to encode log messages with
[CLP](https://github.com/y-scope/clp), and work with the encoded messages using
a foreign function interface (FFI).

# Requirements

* A C++ compiler that supports C++-17
* CMake 3.5.1 or higher
* [Task]

# Setup

* Initialize git submodules
  ```shell
  git submodule update --init --recursive
  ```
  * You should run this command any time you update this repo's submodules.

# Common build commands

Build and test:
```shell
task
# or task package-and-test
```

Build without testing:
```shell
task package
```

Build without any extras:
```shell
task package-basic
```

Generate the JNI headers:
```shell
task jni-headers
```

Build and package the native library:
```shell
task native-lib
```

# Testing

Run all tests:
```shell
task test
```

Run unit tests:
```shell
task unit-tests
```

Run integration tests:
```shell
task integration-tests
```

# Linting

Before submitting a PR, ensure you've run our linting tools and either fixed any violations or
suppressed the warning. If you can't run the linting workflows locally, you can enable and run the
[lint] workflow in your fork.

## Requirements

We currently support running our linting tools on Linux and macOS. If you're developing on another
OS, you can submit a [feature request][feature-req], or use our [lint] workflow in your fork.

To run the linting tools, besides commonly installed tools like `tar`, you'll need:

* `md5sum`
* Python 3.8 or newer
* python3-venv

## Running the linters

```shell
task lint
```

[feature-req]: https://github.com/y-scope/clp-ffi-java/issues/new?assignees=&labels=enhancement&projects=&template=feature-request.yml
[lint]: https://github.com/y-scope/clp-ffi-java/blob/main/.github/workflows/lint.yaml
[Task]: https://taskfile.dev/
