# WCDB

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/Tencent/wcdb/pulls)
[![Release Version](https://img.shields.io/badge/release-1.0.5-brightgreen.svg)](https://github.com/Tencent/wcdb/releases)
[![WeChat Approved iOS](https://img.shields.io/badge/Wechat_Approved_iOS-1.0.5-brightgreen.svg)](https://github.com/Tencent/wcdb/blob/master/README.md#wcdb-for-iosmacos)
[![WeChat Approved Android](https://img.shields.io/badge/Wechat_Approved_Android-1.0.5-brightgreen.svg)](https://github.com/Tencent/wcdb/blob/master/README.md#wcdb-for-android)
[![Platform](https://img.shields.io/badge/Platform-%20iOS%20%7C%20macOS%20%7C%20Android-brightgreen.svg)](https://github.com/Tencent/wcdb/wiki)

中文版本请参看[这里][wcdb-wiki]

WCDB is an **efficient**, **complete**, **easy-to-use** mobile database framework used in the WeChat application. It's based on SQLite and SQLCipher, and supports three languages: C++, Swift and Objective-C.

# Feature

## **Easy-to-use**

* **ORM** (Object Relational Mapping): WCDB provides a flexible, easy-to-use ORM for creating tables, indices and constraints, as well as CRUD through C++/Swift/Objc objects.
* **WINQ** (WCDB language integrated query): WINQ is a native data querying capability which frees developers from writing glue code to concatenate SQL query strings.

With ORM and WINQ, you can insert, update, query and delete objects from database in one line code: 

```c++
// C++
database.insertObjects<Sample>(Sample(1, "text"), myTable);
database.updateRow("text2", WCDB_FIELD(Sample::content), myTable, WCDB_FIELD(Sample::id) == 1);
auto objects = database.getAllObjects<Sample>(myTable, WCDB_FIELD(Sample::id) > 0);
database.deleteObjects(myTable, WCDB_FIELD(Sample::id) == 1);
```

```swift
// Swift
try database.insert(Sample(id:1, content:"text"), intoTable: myTable)
try database.update(table: myTable,
                    on: Sample.Properties.content,
                    with: "text2"
                    where:Sample.Properties.id == 1)
let objects: [Sample] = try database.getObjects(fromTable: myTable,
                                                where: Sample.Properties.id > 0)
try database.delete(fromTable: myTable where: Sample.Properties.id == 1)
```

```objective-c
// Objc
[database insertObject:sample intoTable:myTable];
[database updateTable:myTable
          setProperty:Sample.content
              toValue:@"text2"
                where:Sample.id == 1];
NSArray* objects = [database getObjectsOfClass:Sample.class
                                     fromTable:myTable
                                         where:Sample.id > 0];
[database deleteFromTable:myTable where:Sample.id == 1];
```

## **Efficient**

Through the framework layer and sqlcipher source optimization, WCDB have more efficient performance.

* **Multi-threaded concurrency**: WCDB supports concurrent read-read and read-write access via connection pooling.
* **Deeply optimized**: WCDB has deeply optimized the source code and configuration of SQLite to adapt to the development scenarios of mobile terminals. At the same time, WCDB has also been optimized for common time-consuming scenarios, such as writing data in batches.

## **Complete**

WCDB summarizes common problems in practice to provide a more complete development experience for database development:

* **Encryption Support**: WCDB supports database encryption via [SQLCipher][sqlcipher].
* **Corruption recovery**: WCDB provides a built-in repair kit for database corruption recovery.
* **Anti-injection**: WCDB provides a built-in protection from SQL injection.
* **Database model upgrade**: The database model is bound to the class definition, so that the addition, deletion and modification of database fields are consistent with the definition of class variables.
* **Full-text search**: WCDB provides an easy-to-use full-text search interface and includes tokenizers for multiple languages.
* **Data Migration**: WCDB supports to migrate data from one databasse to another with simple configuration. And developers don't need to care about the intermediate status and progress of the migration.

## **Compatibility**

WCDB has interfaces in three languages: C++, Swift, and Objc. Interfaces in different languages share the same underlying logic. The code structure of WCDB is shown in the figure below:

<div align=center><img style="display:inline-block" src="https://raw.githubusercontent.com/wiki/Tencent/wcdb/assets/common/CodeStructure-English.png" width = 600/></div>

Under such architecture, WCDB in different languages can have the same interface structure and interface capabilities. In one project, you can write database code in different languages with one WCDB. Database logic in different languages will not conflict. Some global interfaces such as error monitoring can work on database logic in different languages at the same time. 

We will add more language support to WCDB in the near future, such as **Java**, **Kotlin**, **Flutter**, so that WCDB can be applied to more platforms.

# Build and Install

Following wikies contain the detailed instructions about building and installing of WCDB.

* [Building and Installing of WCDB C++][wcdb-cpp-build-and-install]
* [Building and Installing of WCDB Swift][wcdb-swift-build-and-install]
* [Building and Installing of WCDB Objc][wcdb-objc-build-and-install]

# Tutorials

Tutorials of different languages can be found below:

* [Tutorials for WCDB C++][wcdb-cpp-tutorials]
* [Tutorials for WCDB Swift][wcdb-swift-tutorials]
* [Tutorials for WCDB Objc][wcdb-objc-tutorials]

# Contributing

If you are interested in contributing, check out the [CONTRIBUTING.md], also join our [Tencent OpenSource Plan](https://opensource.tencent.com/contribution).

[wcdb-wiki]: https://github.com/Tencent/wcdb/wiki
[wcdb-docs-android]: https://tencent.github.io/wcdb/references/android/index.html
[room]: https://developer.android.com/topic/libraries/architecture/room
[room-codelabs]: https://codelabs.developers.google.com/codelabs/android-room-with-a-view
[wcdb-cpp-build-and-install]: https://github.com/Tencent/wcdb/wiki/C++-%E5%AE%89%E8%A3%85%E4%B8%8E%E5%85%BC%E5%AE%B9%E6%80%A7
[wcdb-objc-build-and-install]:https://github.com/Tencent/wcdb/wiki/Objc-%E5%AE%89%E8%A3%85%E4%B8%8E%E5%85%BC%E5%AE%B9%E6%80%A7
[wcdb-swift-build-and-install]: https://github.com/Tencent/wcdb/wiki/Swift-%E5%AE%89%E8%A3%85%E4%B8%8E%E5%85%BC%E5%AE%B9%E6%80%A7
[wcdb-cpp-tutorials]:https://github.com/Tencent/wcdb/wiki/C++-%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8
[wcdb-swift-tutorials]:https://github.com/Tencent/wcdb/wiki/Swift-%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8
[wcdb-objc-tutorials]:https://github.com/Tencent/wcdb/wiki/Objc-%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8
[Benchmark-iOS]: https://github.com/Tencent/wcdb/wiki/WCDB-iOS-benchmark

