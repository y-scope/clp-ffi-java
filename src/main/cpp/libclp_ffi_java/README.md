Since Java prefixes methods in the JNI headers with "Java_", we also use this 
convention to make it more explicit where an object/variable has a direct
counterpart in the JVM. For instance, the "Java_MessageEncoder.cpp" file 
contains the implementation of the native methods of the Java MessageEncoder
class.
