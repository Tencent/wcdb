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

#ifndef WCTBinding_hpp
#define WCTBinding_hpp

#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTConstraintBinding.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTIndexBinding.h>

class WCTBinding {
public:
    WCTBinding(Class cls);

#pragma mark - Column
    template <typename T>
    std::shared_ptr<WCTColumnBinding>
    addColumnBinding(const std::string &propertyName,
                     const std::string &columnName)
    {
        std::shared_ptr<WCTColumnBinding> columnBinding(new WCTColumnBinding(
            m_cls, propertyName, columnName, (T *) nullptr));
        _addColumnBinding(columnName, columnBinding);
        return columnBinding;
    }

    std::shared_ptr<WCTColumnBinding>
    getColumnBinding(const WCTProperty &property) const;

    std::shared_ptr<WCTIndexBinding>
    getOrCreateIndexBinding(const std::string &indexSubfixName);

    template <typename T>
    typename std::enable_if<std::is_base_of<WCTConstraintBindingBase, T>::value,
                            T *>::type
    getOrCreateConstraintBinding(const std::string &constraintName)
    {
        T *constraint = (T *) getConstraint(constraintName, T::type);
        if (!constraint) {
            std::shared_ptr<T> constraintBinding(new T(constraintName));
            addConstraintBinding(
                constraintName,
                std::static_pointer_cast<WCTConstraintBindingBase>(
                    constraintBinding));
            constraint = constraintBinding.get();
        }
        return constraint;
    }

    const std::shared_ptr<WCTIndexBindingMap> getIndexBindingMap() const;
    const WCTColumnBindingList &getColumnBindingList() const;
    const WCTColumnBindingMap &getColumnBindingMap() const;

    const std::shared_ptr<std::list<std::pair<std::string, std::string>>>
    getVirtualTableArgumentList() const;

    void addVirtualTableArgument(const std::string &left,
                                 const std::string &right);
    void addVirtualTableArgument(const std::string &left, NSString *right);
    void setVirtualTableModule(const std::string &moduleName);
    void setVirtualTableModule(NSString *moduleName);

    WCDB::StatementCreateTable
    generateCreateTableStatement(const std::string &tableName) const;
    WCDB::StatementCreateVirtualTable
    generateVirtualCreateTableStatement(const std::string &tableName) const;

protected:
    void
    _addColumnBinding(const std::string &propertyName,
                      const std::shared_ptr<WCTColumnBinding> &columnBinding);
    WCTConstraintBindingBase *getConstraint(const std::string &constraintName,
                                            WCTConstraintBindingType type);
    void addConstraintBinding(
        const std::string &constraintName,
        const std::shared_ptr<WCTConstraintBindingBase> &constraintBinding);

    Class m_cls;

    WCTColumnBindingMap m_columnBindingMap;   //columnName->column binding
    WCTColumnBindingList m_columnBindingList; //ordered column binding

    void lazyInitIndexBinding();
    std::shared_ptr<WCTIndexBindingMap>
        m_indexBindingMap; //indexName->index binding

    void lazyInitConstraintBinding();
    std::shared_ptr<WCTConstraintBindingMap>
        m_constraintBindingMap; //constraintName->constraint binding
    std::shared_ptr<WCTConstraintBindingList>
        m_constraintBindingList; //ordered constraint binding

    void lazyInitVirtualTableArgumentList();
    std::shared_ptr<std::list<std::pair<std::string, std::string>>>
        m_virtualTableArgumentList; //for Virtual Table
    std::string m_virtualTableModuleName;
};

#endif /* WCTBinding_hpp */
