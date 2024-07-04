package com.yscope.clp;

import com.yscope.clp.compressorfrontend.BuiltInVariableHandlingRuleVersions;
import com.yscope.clp.compressorfrontend.EncodedMessage;
import com.yscope.clp.compressorfrontend.MessageEncoder;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class Main {
    public static void main(String[] args) {
        // To trigger loading the native library, we encode a message and print its logtype
        MessageEncoder messageEncoder = new MessageEncoder(
                BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1
        );
        EncodedMessage encodedMessage = new EncodedMessage();
        try {
            String message = "Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3";
            messageEncoder.encodeMessage(message, encodedMessage);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        System.out.println(new String(encodedMessage.logtype, StandardCharsets.ISO_8859_1));
    }
}
