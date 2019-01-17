

# How to run read_channels_test_java


```bash
curdir=$(pwd)
cd ../../..
source sourceme.sh
cd ${AIO_LIB_DIR} && make 
cd ${AIO_LIB_DIR}/wrappers/java && make -f GNUMakefile inplace_java
cd ${curdir}
make jar
java -jar *.jar  -N 10000
```


or using Gradle

```bash
gradle fatJar
java -jar  build/libs/read_channels_test-all-1.0.jar  -N 1000000
```
