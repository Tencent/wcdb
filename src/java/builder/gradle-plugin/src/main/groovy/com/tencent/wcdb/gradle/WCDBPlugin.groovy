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
import org.gradle.api.file.SourceDirectorySet
import org.gradle.api.internal.file.DefaultSourceDirectorySet
import org.gradle.api.plugins.JavaPlugin
import org.gradle.api.tasks.SourceSet
import org.gradle.api.tasks.SourceSetContainer
import org.gradle.api.tasks.compile.JavaCompile
import org.gradle.util.GradleVersion

class WCDBPlugin implements Plugin<Project> {
    private static final boolean isG8 = GradleVersion.current().baseVersion >= GradleVersion.version("8.0")

    @Override
    void apply(Project project) {
        if (!project.plugins.hasPlugin(JavaPlugin) &&
            !project.plugins.hasPlugin(AppPlugin) &&
            !project.plugins.hasPlugin(LibraryPlugin)) {

            throw new GradleException("WCDBPlugin: Java or Android application/library plugin required.")
        }

        project.configurations.maybeCreate("wcdbOrmClasspath")
        project.dependencies.add("wcdbOrmClasspath", "com.tencent.wcdb:annotation:${pluginVersion}")
        project.dependencies.add("wcdbOrmClasspath", "com.tencent.wcdb:compiler:${pluginVersion}")

        final CommonExtension android = (project.plugins.hasPlugin(AppPlugin) || project.plugins.hasPlugin(LibraryPlugin)) ?
            project.extensions.getByName('android') : null

        if (android != null) {
            android.sourceSets.configureEach {
                if (isG8) {
                    it.extensions.create(SourceDirectorySet, 'wcdbOrm', WCDBOrmSourceDirectorySet, it.name)
                } else {
                    it.extensions.create(SourceDirectorySet, 'wcdbOrm', WCDBOrmSourceDirectorySetG7, it.name)
                }
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
                def destDir = new File(project.buildDir, "generated/sources/wcdb/${vName}")

                def generateTask = project.tasks.register("generate${cvName}WcdbOrm", JavaCompile) {
                    androidSourceSetNames.each {
                        SourceDirectorySet origSs = android.sourceSets.getByName(it).wcdbOrm
                        def ss = new DefaultSourceDirectorySet(origSs)
                        ss.setSrcDirs(origSs.srcDirs)
                        ss.setIncludes(['**/*.java'])
                        source(ss)
                    }
                    classpath = project.configurations.wcdbOrmClasspath
                    options.annotationProcessorPath = project.configurations.wcdbOrmClasspath
                    options.compilerArgs.addAll(["-proc:only", "-implicit:none"])
                    destinationDirectory = destDir
                }

                variant.registerJavaGeneratingTask(generateTask, destDir)
            }
            getNotTestVariants(android).configureEach(variantConfigure)
            android.testVariants.configureEach(variantConfigure)
        } else {
            final SourceSetContainer sourceSets = project.sourceSets
            sourceSets.configureEach {
                if (isG8) {
                    it.extensions.create(SourceDirectorySet, 'wcdbOrm', WCDBOrmSourceDirectorySet, project, it.name)
                } else {
                    it.extensions.create(SourceDirectorySet, 'wcdbOrm', WCDBOrmSourceDirectorySetG7, project, it.name)
                }
            }

            project.afterEvaluate {
                sourceSets.configureEach {
                    java.srcDirs(wcdbOrm.getSrcDirs().toArray())

                    def ssName = SourceSet.isMain(it) ? '' : it.name.capitalize()
                    def destDir = new File(project.buildDir, "generated/sources/wcdb/${ssName}")
                    def generateTask = project.tasks.register("generate${ssName}WcdbOrm", JavaCompile) {
                        SourceDirectorySet origSs = wcdbOrm
                        def ss = new DefaultSourceDirectorySet(origSs)
                        ss.setSrcDirs(origSs.srcDirs)
                        ss.setIncludes(['**/*.java'])
                        source(ss)

                        classpath = project.configurations.wcdbOrmClasspath
                        options.annotationProcessorPath = project.configurations.wcdbOrmClasspath
                        options.compilerArgs.addAll(["-proc:only", "-implicit:none"])
                        destinationDirectory = destDir
                    }
                }
            }
        }
    }

    private static DomainObjectSet<? extends BaseVariant> getNotTestVariants(CommonExtension android) {
        if (android instanceof ApplicationExtension) {
            return ((ApplicationExtension) android).applicationVariants
        } else {
            return ((LibraryExtension) android).libraryVariants
        }
    }

    private static String myVersion = null
    private static String getPluginVersion() {
        if (myVersion == null) {
            def prop = new Properties()
            prop.load(getClassLoader().getResourceAsStream('wcdb-gradle-plugin.properties'))
            myVersion = prop.getProperty('version')
        }
        return myVersion
    }
}