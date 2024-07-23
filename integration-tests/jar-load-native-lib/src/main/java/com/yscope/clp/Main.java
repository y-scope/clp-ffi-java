package com.yscope.clp;

import com.yscope.clp.compressorfrontend.BuiltInVariableHandlingRuleVersions;
import com.yscope.clp.compressorfrontend.EncodedMessage;
import com.yscope.clp.compressorfrontend.MessageDecoder;
import com.yscope.clp.compressorfrontend.MessageEncoder;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class Main {
    public static void main(String[] args) {
        // To trigger loading the native library, we do a basic encode-decode test.
        MessageEncoder messageEncoder = new MessageEncoder(
                BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1
        );
        EncodedMessage encodedMessage = new EncodedMessage();
        MessageDecoder messageDecoder =
                new MessageDecoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                        BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
        try {
            String message = "Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3";
            messageEncoder.encodeMessage(message, encodedMessage);
            String decodedMessage = messageDecoder.decodeMessage(
                    encodedMessage.getLogTypeAsString(),
                    encodedMessage.getDictionaryVarsAsStrings(),
                    encodedMessage.encodedVars
            );
            if (false == message.equals(decodedMessage)) {
                throw new RuntimeException("Failed to encode message.");
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
