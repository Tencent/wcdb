// Created by qiuwenchen on 2023/3/30.
//

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
package com.tencent.wcdb.compiler;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.Filer;
import javax.annotation.processing.Messager;
import javax.annotation.processing.ProcessingEnvironment;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedOptions;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.util.Elements;
import javax.tools.Diagnostic;
import javax.lang.model.util.Types;
import javax.tools.JavaFileObject;

import com.tencent.wcdb.*;
import com.tencent.wcdb.compiler.resolvedInfo.ColumnInfo;
import com.tencent.wcdb.compiler.resolvedInfo.MultiIndexesInfo;
import com.tencent.wcdb.compiler.resolvedInfo.MultiPrimaryInfo;
import com.tencent.wcdb.compiler.resolvedInfo.MultiUniqueInfo;
import com.tencent.wcdb.compiler.resolvedInfo.TableConstraintInfo;

@SupportedOptions(value = {"verbose"})
public class JavaAnnotationProcessor extends AbstractProcessor {

    public static final String OPTION_VERBOSE = "verbose";
    Filer filer;
    Messager msg;
    Types typeUtil;
    Elements elementUtil;
    private boolean writerRoundDone = false;
    private int round = 0;
    private boolean verbose = true;
    private int primaryKeyCount = 0;

    private TableConstraintInfo tableConstraintInfo;
    private ArrayList<ColumnInfo> allFieldInfo;

    @Override
    public synchronized void init(ProcessingEnvironment processingEnvironment) {
        super.init(processingEnvironment);
        verbose = Boolean.parseBoolean(processingEnvironment.getOptions().get(OPTION_VERBOSE));
        filer = processingEnvironment.getFiler();
        msg = processingEnvironment.getMessager();
        typeUtil = processingEnvironment.getTypeUtils();
        elementUtil = processingEnvironment.getElementUtils();
    }

    @Override
    public Set<String> getSupportedAnnotationTypes() {
        return Collections.singleton(WCDBTableCoding.class.getCanonicalName());
    }

    @Override
    public SourceVersion getSupportedSourceVersion() {
        return SourceVersion.latest();
    }

    @Override
    public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment env) {
        try {
            round++;
            verboseLog("WCDB Processing round " + round + ", new annotations: " +
                    !annotations.isEmpty() + ", processingOver: " + env.processingOver());
            if (env.processingOver()) {
                if (!annotations.isEmpty()) {
                    msg.printMessage(Diagnostic.Kind.ERROR,
                            "Unexpected WCDB processing state: annotations still available after processing over");
                    return false;
                }
            }

            if (annotations.isEmpty()) {
                return false;
            }

            if (writerRoundDone) {
                msg.printMessage(Diagnostic.Kind.ERROR,
                        "Unexpected WCDB processing state: annotations still available after writing.");
            }

            Set<? extends Element> elements = env.getElementsAnnotatedWith(WCDBTableCoding.class);
            for (Element element : elements) {
                if (!checkClassElement(element)) {
                    return false;
                }

                primaryKeyCount = 0;
                WCDBTableCoding config = element.getAnnotation(WCDBTableCoding.class);
                assert config != null;
                tableConstraintInfo = TableConstraintInfo.Companion.resolve(config);
                allFieldInfo = new ArrayList<>();

                verboseLog("WCDB Processing: " + element);

                for(Element enclosedElement : element.getEnclosedElements()) {
                    if(enclosedElement.getKind() != ElementKind.FIELD) {
                        continue;
                    }
                    WCDBField fieldAnnotation = enclosedElement.getAnnotation(WCDBField.class);
                    if(fieldAnnotation == null) {
                        continue;
                    }
                    WCDBDefault defaultValueAnnotation = enclosedElement.getAnnotation(WCDBDefault.class);
                    WCDBIndex indexAnnotation = enclosedElement.getAnnotation(WCDBIndex.class);
                    if (!checkFieldElement(enclosedElement, fieldAnnotation, indexAnnotation, defaultValueAnnotation)) {
                        allFieldInfo.clear();
                        break;
                    }
                    verboseLog("Found WCDBField: " + enclosedElement
                            + " " + enclosedElement.asType().toString());
                    allFieldInfo.add(ColumnInfo.Companion.resolve(enclosedElement, fieldAnnotation, indexAnnotation, defaultValueAnnotation));
                }

                if(!checkColumnInTableConstraint(element)) {
                    return false;
                }

                if(allFieldInfo.size() > 0) {
                    createORMFile((TypeElement) element);
                }
            }
            writerRoundDone = true;

        } catch (RuntimeException e) {
            // IntelliJ does not handle exceptions nicely, so log and print a message
            e.printStackTrace();
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "Unexpected error in WCDBAnnotationProcessor: " + e);
        }
        return true;
    }

    private void verboseLog(String content) {
        if(verbose) {
            msg.printMessage(Diagnostic.Kind.NOTE, content);
        }
    }

    private boolean checkClassElement(Element element) {
        if(element.getKind() != ElementKind.CLASS) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "@WCDBTableCoding is only valid for classes", element);
            return false;
        }
        Set<Modifier> modifiers = element.getModifiers();
        if(modifiers.contains(Modifier.PRIVATE) || modifiers.contains(Modifier.PROTECTED)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The class with annotation @WCDBTableCoding can not be private or protected", element);
            return false;
        }
        if(modifiers.contains(Modifier.ABSTRACT)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The class with annotation @WCDBTableCoding can not be abstract", element);
            return false;
        }
        return true;
    }

    private boolean checkFieldElement(Element element, WCDBField fieldAnnotation, WCDBIndex indexAnnotation, WCDBDefault defaultValueAnnotation) {
        if(element.getModifiers().contains(Modifier.PRIVATE) || element.getModifiers().contains(Modifier.PROTECTED)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The field with annotation @WCDBField can not be private or protected", element);
            return false;
        }
        String type = element.asType().toString();
        if(!JavaFieldORMInfo.allTypes.contains(type)) {
            String info = "The type " + type + " of field " +  element.toString() +
                    " in " + element.getEnclosingElement().getSimpleName() + " is Unsupported!\n"
                    + "WCDB only supports Java basic types and byte[].";
            msg.printMessage(Diagnostic.Kind.ERROR, info, element);
            return false;
        }
        if(fieldAnnotation.isPrimary()) {
            primaryKeyCount++;
            if(primaryKeyCount > 1) {
                String info = "@WCDBField can only configure one primary key in " + element.getEnclosingElement().getSimpleName()
                        + ". If multiple primary keys are required, configure multiPrimaries in @WCDBTableCoding. ";
                msg.printMessage(Diagnostic.Kind.ERROR, info, element);
                return false;
            }
            if(fieldAnnotation.isAutoIncrement()) {
                if(!JavaFieldORMInfo.allInfo.get(type).columnType.equals("Integer")) {
                    String info = "Auto-increment field must be integer";
                    msg.printMessage(Diagnostic.Kind.ERROR, info, element);
                    return false;
                }
            }
            if (indexAnnotation != null) {
                String info = element.getEnclosingElement().getSimpleName() +
                        " is confined as primary key, so it does not need to configure @WCDBIndex";
                msg.printMessage(Diagnostic.Kind.ERROR, info, element);
                return false;
            }
        } else if (fieldAnnotation.isAutoIncrement()) {
            String info = "Only the primary key can be configured as auto-increment";
            msg.printMessage(Diagnostic.Kind.ERROR, info, element);
            return false;
        }
        if (defaultValueAnnotation != null) {
            int valueCount = 0;
            boolean typeMissMatch = false;
            String columnType = JavaFieldORMInfo.allInfo.get(type).columnType;
            if (defaultValueAnnotation.intValue() != 0) {
                valueCount++;
                if (!columnType.equals("Integer")) {
                    typeMissMatch = true;
                }
            }
            if (defaultValueAnnotation.doubleValue() != 0) {
                valueCount++;
                if (!columnType.equals("Float")) {
                    typeMissMatch = true;
                }
            }
            if (defaultValueAnnotation.textValue().length() > 0) {
                valueCount++;
                if (!columnType.equals("Text")) {
                    typeMissMatch = true;
                }
            }
            if (valueCount > 1) {
                msg.printMessage(Diagnostic.Kind.ERROR, "Only one default value can be configured for a field", element);
                return false;
            } else if (typeMissMatch) {
                if (columnType.equals("BLOB") ) {
                    msg.printMessage(Diagnostic.Kind.ERROR, "Assigning a default value to BLOB is unsupported", element);
                } else {
                    msg.printMessage(Diagnostic.Kind.ERROR, "Default value should be a " + columnType, element);
                }
                return false;
            }
        }
        return true;
    }

    private boolean checkColumnInTableConstraint(Element element) {
        if (tableConstraintInfo.getMultiIndexes().isEmpty() &&
                tableConstraintInfo.getMultiPrimaries().isEmpty() &&
                tableConstraintInfo.getMultiUnique().isEmpty()) {
            return true;
        }
        Set<String> allColumns = new HashSet<>();
        for(ColumnInfo columnInfo : allFieldInfo) {
            allColumns.add(columnInfo.getColumnName().isEmpty() ? columnInfo.getPropertyName() : columnInfo.getColumnName());
        }

        for(MultiIndexesInfo multiIndexes : tableConstraintInfo.getMultiIndexes()) {
            for(String column : multiIndexes.getColumns()) {
                if (!allColumns.contains(column)) {
                    msg.printMessage(Diagnostic.Kind.ERROR, "Can't find column \""+ column + "\" in class orm config.", element);
                    return false;
                }
            }
        }

        for(MultiPrimaryInfo multiPrimary : tableConstraintInfo.getMultiPrimaries()) {
            for(String column : multiPrimary.getColumns()) {
                if (!allColumns.contains(column)) {
                    msg.printMessage(Diagnostic.Kind.ERROR, "Can't find column \""+ column + "\" in class orm config.", element);
                    return false;
                }
            }
        }

        for(MultiUniqueInfo multiUnique : tableConstraintInfo.getMultiUnique()) {
            for(String column : multiUnique.getColumns()) {
                if (!allColumns.contains(column)) {
                    msg.printMessage(Diagnostic.Kind.ERROR, "Can't find column \""+ column + "\" in class orm config.", element);
                    return false;
                }
            }
        }

        return true;
    }

    private void createORMFile(TypeElement element) throws RuntimeException {
        String packageName = elementUtil.getPackageOf(element).toString();
        String className = String.valueOf(element.getSimpleName());
        String ormClassName = "DB" + element.getSimpleName();

        JavaCodeGenerator codeGenerator = new JavaCodeGenerator();
        codeGenerator.packageName = packageName;
        codeGenerator.className = className;
        codeGenerator.ormClassName = ormClassName;
        codeGenerator.tableConstraintInfo = tableConstraintInfo;
        codeGenerator.allColumnInfo = allFieldInfo;

        String javaCode = codeGenerator.generate();

        BufferedWriter writer = null;
        try {
            JavaFileObject outputFile = filer.createSourceFile(packageName + "." + ormClassName);
            writer = new BufferedWriter(outputFile.openWriter());
            writer.write(javaCode);
        } catch (IOException e) {
            throw new RuntimeException("Could not write source for " + element.getSimpleName(), e);
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    //Silent
                }
            }
        }
    }
}