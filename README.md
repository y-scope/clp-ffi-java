clp-ffi-java is a library to encode log messages with 
[CLP](https://github.com/y-scope/clp), and work with the encoded messages using
a foreign function interface (FFI).

# Requirements

* A C++ compiler that supports C++-17
* CMake 3.5.1 or higher

# Common build commands
* Initialize git submodules
  ```shell
  git submodule update --init --recursive
  ```

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
