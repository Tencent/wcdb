/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/Assertion.hpp>
#include <WCDB/Data.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryRestorer.hpp>
#include <WCDB/FileHandle.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/FullCrawler.hpp>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Path.hpp>

namespace WCDB {

namespace Repair {

bool FactoryRestorer::work(std::shared_ptr<Assembler> &assembler,
                           const ProgressUpdateCallback &onProgressUpdated)
{
    WCTInnerAssert(assembler->getPath().empty());

    //3. Restore from all archived db. It should be succeed without critical errors.
    //4. Do a deconstructor on restore db.
    //5. Archive current db and use restore db
    //6. Remove all archived dbs.

    FileManager *fileManager = FileManager::shared();
    bool succeed;

    //1. Remove the old restore db
    std::string restoreDirectory = factory.getRestoreDirectory();
    if (!fileManager->removeItem(restoreDirectory)) {
        return false;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    std::string restoreDatabasePath = Path::addComponent(
        restoreDirectory, Path::getFileName(factory.database));
    assembler->setPath(restoreDatabasePath);
    std::string materialPath;
    std::tie(succeed, materialPath) =
        Factory::pickMaterialForRestoring(factory.database);
    if (!succeed) {
        return false;
    }

    //TODO
    if (!materialPath.empty()) {
        size_t fileSize;
        std::tie(succeed, fileSize) = fileManager->getFileSize(materialPath);
        if (!succeed) {
            return false;
        }

        Data materialData(fileSize);
        if (materialData.empty()) {
            return false;
        }

        FileHandle fileHandle(materialPath);
        if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
            return false;
        }
        ssize_t read = fileHandle.read(materialData.buffer(), 0, fileSize);
        if (read < 0) {
            return false;
        }
        Material material;
        //TODO resolved material corruption
        if (!material.deserialize(materialData)) {
            return false;
        }
        Mechanic mechanic(factory.database);
        mechanic.setAssembler(assembler);
        mechanic.setProgressCallback(onProgressUpdated);
        mechanic.work();
    } else {
        //TODO Warning
        FullCrawler fullCrawler(factory.database);
        fullCrawler.setAssembler(assembler);
        fullCrawler.setProgressCallback(onProgressUpdated);
        fullCrawler.work();
    }
    //TODO

    return false;
}

} //namespace Repair

} //namespace WCDB
