package com.tencent.wcdb.gradle

import com.android.build.gradle.AppPlugin
import com.android.build.gradle.LibraryPlugin
import org.gradle.api.GradleException
import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.tasks.compile.JavaCompile

class WCDBPlugin implements Plugin<Project> {
    @Override
    void apply(Project project) {
        if (!project.plugins.hasPlugin(AppPlugin) && !project.plugins.hasPlugin(LibraryPlugin)) {
            throw new GradleException("WCDBPlugin: Android application or library plugin required.")
        }

        project.sourceSets {
            wcdb { java { srcDir 'src/wcdb' } }
        }

        project.task("generateWcdbOrm", type: JavaCompile) {
            source = sourceSets.wcdb.allJava
            classpath = project.buildscript.configurations.classpath
            options.annotationProcessorPath = project.buildscript.configurations.classpath
            options.compilerArgs.addAll(["-proc:only", "-implicit:none"])
            destinationDir = new File(project.buildDir, "generated/source/wcdb")
        }

        def android = project.extensions.android

        android.sourceSets {
            main {
                java { srcDir 'src/wcdb' }
            }
        }

        if (project.plugins.hasPlugin(AppPlugin)) {
            android.applicationVariants.all { variant ->
                variant.registerJavaGeneratingTask(generateWcdbOrm, new File(project.buildDir, "generated/source/wcdb"))
            }
        } else {
            android.libraryVariants.all { variant ->
                variant.registerJavaGeneratingTask(generateWcdbOrm, new File(project.buildDir, "generated/source/wcdb"))
            }
        }
    }
}