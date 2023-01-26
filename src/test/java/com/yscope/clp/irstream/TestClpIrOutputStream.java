package com.yscope.clp.irstream;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.ZonedDateTime;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

public class TestClpIrOutputStream {
  @Test
  void testWritingEightByteEncodedIrStream () {
    testWritingIrStream(false);
  }

  @Test
  void testWritingFourByteEncodedIrStream () {
    testWritingIrStream(true);
  }

  private void testWritingIrStream (boolean useFourByteEncoding) {
    String logMessage = " INFO Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3\n";
    ByteBuffer logEvent = ByteBuffer.wrap(logMessage.getBytes(StandardCharsets.ISO_8859_1));

    try {
      String logFilePathString = useFourByteEncoding ? "four-byte.clp" : "eight-byte.clp";
      Path logFilePath = Paths.get(logFilePathString);
      OutputStream fileOutputStream = Files.newOutputStream(logFilePath);
      String timestampPattern = "yyyy-MM-dd HH:mm:ss,SSS";
      String timeZoneId = ZonedDateTime.now().getZone()
          .toString();
      AbstractClpIrOutputStream clpIrOutputStream;
      if (useFourByteEncoding) {
        clpIrOutputStream = new FourByteClpIrOutputStream(timestampPattern, timeZoneId,
                                                          fileOutputStream);
      } else {
        clpIrOutputStream = new EightByteClpIrOutputStream(timestampPattern, timeZoneId,
                                                           fileOutputStream);
      }

      for (int i = 0; i < 100; ++i) {
        clpIrOutputStream.writeLogEvent(System.currentTimeMillis(), logEvent);
      }

      clpIrOutputStream.close();

      File outputFile = new File(logFilePathString);
      assertTrue(outputFile.exists());

      Files.delete(logFilePath);
    } catch (IOException e) {
      fail(e.getMessage());
    }
  }
}
