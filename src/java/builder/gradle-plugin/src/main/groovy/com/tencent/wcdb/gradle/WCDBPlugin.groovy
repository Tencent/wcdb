package com.tencent.wcdb.gradle

import com.android.build.api.dsl.ApplicationExtension
import com.android.build.api.dsl.CommonExtension
import com.android.build.api.dsl.LibraryExtension
import com.android.build.gradle.AppPlugin
import com.android.build.gradle.LibraryPlugin
import com.android.build.gradle.api.BaseVariant
import org.gradle.api.DomainObjectSet
import org.gradle.api.GradleException
import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.internal.file.DefaultSourceDirectorySet
import org.gradle.api.model.ObjectFactory
import org.gradle.api.tasks.compile.JavaCompile

class WCDBPlugin implements Plugin<Project> {
    @Override
    void apply(Project project) {
        if (!project.plugins.hasPlugin(AppPlugin) && !project.plugins.hasPlugin(LibraryPlugin)) {
            throw new GradleException("WCDBPlugin: Android application or library plugin required.")
        }

        final CommonExtension android = (project.plugins.hasPlugin(AppPlugin) || project.plugins.hasPlugin(LibraryPlugin)) ?
            project.extensions.getByName('android') : null

        if (android != null) {
            android.sourceSets.configureEach {
                it.extensions.create('wcdbOrm', WCDBOrmSourceDirectorySet, it.name)
            }

            project.afterEvaluate {
                android.sourceSets.configureEach {
                    // Add wcdbOrm sourceSet to 'java'
                    java.srcDirs(wcdbOrm.getSrcDirs().toArray())
                }
            }

            def variantConfigure = { BaseVariant variant ->
                def vName = variant.name
                def cvName = vName.capitalize()
                def androidSourceSetNames = variant.sourceSets*.name
                def destDir = new File(project.buildDir, "generated/source/wcdb/${vName}")

                def generateTask = project.tasks.register("generate${cvName}WcdbOrm", JavaCompile) {
                    androidSourceSetNames.each {
                        WCDBOrmSourceDirectorySet origSs = android.sourceSets.getByName(it).wcdbOrm
                        def ss = new DefaultSourceDirectorySet(origSs)
                        ss.setSrcDirs(origSs.srcDirs)
                        ss.setIncludes(['**/*.java'])
                        source(ss)
                    }
                    classpath = project.buildscript.configurations.classpath
                    options.annotationProcessorPath = classpath
                    options.compilerArgs.addAll(["-proc:only", "-implicit:none"])
                    destinationDirectory = destDir
                }

                variant.registerJavaGeneratingTask(generateTask, destDir)
            }
            getNotTestVariants(android).configureEach(variantConfigure)
            android.testVariants.configureEach(variantConfigure)
        } else {
            // TODO
        }
    }

    private static DomainObjectSet<? extends BaseVariant> getNotTestVariants(CommonExtension android) {
        if (android instanceof ApplicationExtension) {
            return ((ApplicationExtension) android).applicationVariants
        } else {
            return ((LibraryExtension) android).libraryVariants
        }
    }
}