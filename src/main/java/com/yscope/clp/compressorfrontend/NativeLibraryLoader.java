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
 * desired.
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

    URL libUrl = getLibUrlInJar();
    if (null == libUrl) {
      logger.error("Failed to find library in JAR");
      return;
    }

    try {
      // Extract library into a temporary file
      File libTempFile = File.createTempFile(LIBRARY_NAME, ".tmp");
      libTempFile.deleteOnExit();
      try (
          InputStream in = libUrl.openStream();
          BufferedOutputStream out =
              new BufferedOutputStream(Files.newOutputStream(libTempFile.toPath()))
      ) {
        byte[] buf = new byte[4096];
        while (true) {
          int numBytesRead = in.read(buf);
          if (-1 == numBytesRead) {
            break;
          }
          out.write(buf, 0, numBytesRead);
        }
      }

      System.load(libTempFile.getAbsolutePath());
    } catch (IOException e) {
      logger.error("Failed to load native library from JAR", e);
    }
  }

  private static URL getLibUrlInJar() {
    // Path in JAR:
    // - Linux: /lib/${osNameWithoutSpaces}/${os.arch}/${LIBRARY_NAME}.so
    // - MacOS: /lib/${osNameWithoutSpaces}/${os.arch}/${LIBRARY_NAME}.dylib
    String osNameWithoutSpaces = System.getProperty("os.name").replace(' ', '-');
    Path libDir = Paths.get("/", "lib", osNameWithoutSpaces, System.getProperty("os.arch"));
    Path[] libPaths = {
        libDir.resolve(LIBRARY_NAME + ".so"),
        libDir.resolve(LIBRARY_NAME + ".dylib")
    };
    for (Path libPath : libPaths) {
      URL libUrl = NativeLibraryLoader.class.getResource(libPath.toString());
      if (null != libUrl) {
        return libUrl;
      }
    }

    return null;
  }
}
