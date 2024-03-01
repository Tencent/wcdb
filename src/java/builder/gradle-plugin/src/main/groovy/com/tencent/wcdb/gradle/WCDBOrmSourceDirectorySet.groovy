package com.tencent.wcdb.gradle

import org.gradle.api.Project
import org.gradle.api.internal.file.DefaultSourceDirectorySet
import org.gradle.api.internal.file.FileCollectionFactory
import org.gradle.api.internal.file.collections.DefaultDirectoryFileTreeFactory
import org.gradle.api.internal.tasks.TaskDependencyFactory
import org.gradle.api.model.ObjectFactory
import org.gradle.api.tasks.util.PatternSet
import org.gradle.internal.Factory

import javax.inject.Inject
/**
 * The backing class of the autogen extension added to sourceSets, e.g., sourceSets.main.autogen
 */
class WCDBOrmSourceDirectorySet extends DefaultSourceDirectorySet {

    private static final Factory<PatternSet> sPatternSetFactory = new Factory<PatternSet>() {
        @Override
        PatternSet create() {
            return new PatternSet()
        }
    }

    @Inject
    WCDBOrmSourceDirectorySet(Project project, String name) {
        super(name, "$name WCDB ORM source", sPatternSetFactory,
                project.services.get(TaskDependencyFactory),
                project.services.get(FileCollectionFactory),
                new DefaultDirectoryFileTreeFactory(), project.services.get(ObjectFactory))

        srcDir("src/${name}/wcdbOrm")
        include('**/*.java')
        include('**/*.kt')
    }
}