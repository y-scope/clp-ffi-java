package com.yscope.clp.compressorfrontend;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Class to load the native library.
 * <p></p>
 * We first try to load from java.library.path. If unsuccessful, then we try
 * loading from the JAR. This allows users to override the native library if
 * desired. See
 * https://github.com/zeromq/jzmq/blob/master/jzmq-jni/src/main/java/org/zeromq/EmbeddedLibraryTools.java
 * for an example.
 */
public class NativeLibraryLoader {
  private static final Logger logger = LoggerFactory.getLogger(NativeLibraryLoader.class);

  private static final String LIBRARY_NAME = "libclp-ffi-java";

  /**
   * Loads the native library
   */
  public static void load () {
    try {
      System.loadLibrary(LIBRARY_NAME.substring("lib".length()));
      return;
    } catch (Throwable e) {
      // Move on to try and load from jar
    }

    // Path in JAR: /lib/${os.name}/${os.arch}/${LIBRARY_NAME}
    Path libPath = Paths.get("/", "lib", System.getProperty("os.name"),
        System.getProperty("os.arch"), LIBRARY_NAME + ".so");
    URL libUrl = MessageEncoder.class.getResource(libPath.toString());
    if (null == libUrl) {
      logger.error("Failed to find library in JAR: {}", libPath);
      return;
    }

    try {
      final File libFile = File.createTempFile(LIBRARY_NAME, ".tmp");
      libFile.deleteOnExit();
      try (
          InputStream in = libUrl.openStream();
          BufferedOutputStream out =
              new BufferedOutputStream(Files.newOutputStream(libFile.toPath()))
      ) {
        byte[] buf = new byte[8192];
        while (true) {
          int len = in.read(buf);
          if (-1 == len) {
            break;
          }
          out.write(buf, 0, len);
        }
      }
      System.load(libFile.getAbsolutePath());
    } catch (IOException e) {
      logger.error("Failed to load native library from JAR", e);
    }
  }
}
