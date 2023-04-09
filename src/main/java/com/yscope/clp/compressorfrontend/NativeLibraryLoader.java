package com.yscope.clp.compressorfrontend;

import java.io.*;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

/**
 * Class to load the native library.
 * <p></p>
 * We first try to load from java.library.path. If unsuccessful, then we try
 * loading from the JAR. This allows users to override the native library if
 * desired.
 */
public class NativeLibraryLoader {
  private static final String LIBRARY_NAME = "libclp-ffi-java";
  // Create a timestamp formatter to format with millisecond precision and a
  // timezone offset.
  // Examples of formatted timestamps:
  // - 2023-01-01T00:00:00.000Z
  // - 2023-01-01T00:00:00.000-0500
  private static final DateTimeFormatter logTimestampFormatter =
      DateTimeFormatter.ofPattern("yyyy-MM-dd'T'HH:mm:ss.SSSXX");

  /**
   * Loads the native library
   */
  public static void load () {
    Throwable loadLibraryThrowable;
    try {
      System.loadLibrary(LIBRARY_NAME.substring("lib".length()));
      return;
    } catch (Throwable e) {
      loadLibraryThrowable = e;
      // Move on to try and load from jar
    }

    URL libUrl = getLibUrlInJar();
    if (null == libUrl) {
      logError("Failed to find native library in JAR.");
      logError("System.loadLibrary failed - " + loadLibraryThrowable.getMessage());
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
      logError("Failed to load native library from JAR -");
      e.printStackTrace();
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

  private static void logError(String message) {
    System.err.println(ZonedDateTime.now().format(logTimestampFormatter) + " clp-ffi: " + message);
  }
}
