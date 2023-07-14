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
        }
    }
}