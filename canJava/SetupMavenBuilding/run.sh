#! /bin/bash

mvn install:install-file -Dfile=api-1.0.jar -DgroupId=se.sics.sse.fresta -DartifactId=api -Dversion=1.0 -Dpackaging=jar
mvn install:install-file -Dfile=pom.xml -DgroupId=plugins -DartifactId=plugin-parent -Dversion=1.0 -Dpackaging=pom
