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

@SupportedOptions(value = {"verbose"})
public class AnnotationProcessor extends AbstractProcessor {

    public static final String OPTION_VERBOSE = "verbose";
    private static final String TAB = "    ";
    Filer filer;
    Messager msg;
    Types typeUtil;
    Elements elementUtil;
    private boolean writerRoundDone = false;
    private int round = 0;
    private boolean verbose = true;
    private int primaryKeyCount = 0;
    Element autoIncrementElement = null;

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
            if (verbose) {
                msg.printMessage(Diagnostic.Kind.NOTE,
                        "WCDB Processing round " + round + ", new annotations: " +
                        !annotations.isEmpty() + ", processingOver: " + env.processingOver());
            }
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
                if (verbose) {
                    msg.printMessage(Diagnostic.Kind.NOTE, "WCDB Processing: " + element.toString());
                }
                ArrayList<Element> fields = new ArrayList<>();
                primaryKeyCount = 0;
                autoIncrementElement = null;
                for(Element enclosedElement : element.getEnclosedElements()) {
                    if(enclosedElement.getKind() != ElementKind.FIELD) {
                        continue;
                    }
                    WCDBField fieldAnnotation = enclosedElement.getAnnotation(WCDBField.class);
                    if(fieldAnnotation == null) {
                        continue;
                    }
                    if (!checkFieldElement(enclosedElement, fieldAnnotation)) {
                        fields.clear();
                        break;
                    }
                    if (verbose) {
                        msg.printMessage(Diagnostic.Kind.NOTE,
                                "Found WCDBField: " + enclosedElement.toString()
                                        + " " + enclosedElement.asType().toString());
                    }
                    fields.add(enclosedElement);
                }
                if(fields.size() > 0) {
                    createORMFile((TypeElement) element, fields);
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

    private boolean checkClassElement(Element element) {
        if(element.getKind() != ElementKind.CLASS) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "@WCDBTableCoding is only valid for classes", element);
            return false;
        }
        if(element.getModifiers().contains(Modifier.PRIVATE) ||
                element.getModifiers().contains(Modifier.PROTECTED)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The class with annotation @WCDBTableCoding can not be private or protected", element);
            return false;
        }
        if(element.getModifiers().contains(Modifier.ABSTRACT)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The class with annotation @WCDBTableCoding can not be abstract", element);
            return false;
        }
        return true;
    }

    private boolean checkFieldElement(Element element, WCDBField fieldAnnotation) {
        if(element.getModifiers().contains(Modifier.PRIVATE) || element.getModifiers().contains(Modifier.PROTECTED)) {
            msg.printMessage(Diagnostic.Kind.ERROR,
                    "The field with annotation @WCDBField can not be private or protected", element);
            return false;
        }
        String type = element.asType().toString();
        if(!FieldORMInfo.allTypes.contains(type)) {
            String info = "The type " + type + " of field " +  element.toString() +
                    " in " + element.getEnclosingElement().getSimpleName() + " is Unsupported!\n"
                    + "WCDB only supports Java basic types and byte[], ByteBuffer.";
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
                if(!FieldORMInfo.allInfo.get(type).columnType.equals("Integer")) {
                    String info = "Auto-increment field must be integer";
                    msg.printMessage(Diagnostic.Kind.ERROR, info, element);
                    return false;
                }
                autoIncrementElement = element;
            }
        } else if (fieldAnnotation.isAutoIncrement()) {
            String info = "Only the primary key can be configured as auto-increment";
            msg.printMessage(Diagnostic.Kind.ERROR, info, element);
            return false;
        }
        return true;
    }

    private void createORMFile(TypeElement element, ArrayList<Element> fields) throws RuntimeException {
        BufferedWriter writer = null;
        try {
            String packageName = elementUtil.getPackageOf(element).toString();
            String className = String.valueOf(element.getSimpleName());
            String ormClassName = "DB" + element.getSimpleName();
            JavaFileObject outputFile = filer.createSourceFile(packageName + "." + ormClassName);
            writer = new BufferedWriter(outputFile.openWriter());

            WCDBTableCoding config = element.getAnnotation(WCDBTableCoding.class);
            assert config != null;

            writer.write("package " + packageName + ";\n\n");

            writeImport(writer);

            writer.write("public class " + ormClassName + " implements TableBinding<" + className + "> {\n");
            writer.write(TAB + "private static Binding baseBinding;\n");
            writer.write(TAB + "public static " + ormClassName + " INSTANCE;\n\n");

            writeFields(writer, className, fields);

            writer.write(TAB + "\nstatic {\n\n");
            writer.write(TAB + TAB + "baseBinding = new Binding();\n");
            writer.write(TAB + TAB + "INSTANCE = new " + ormClassName + "();\n\n");
            writeColumns(writer, fields);
            writeTableConfig(writer, config);
            writer.write(TAB + "}\n\n");

            writeBindingType(writer, className);
            writeBindingFields(writer, className, fields);
            writeBaseBinding(writer);

            writeExtractObject(writer, className, fields);
            writeBindObject(writer, className, fields);

            writeAutoIncrementConfig(writer, className);

            writer.write("}\n\n");

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

    private void writeImport(BufferedWriter writer) throws IOException {
        writer.write("import com.tencent.wcdb.core.PreparedStatement;\n");
        writer.write("import com.tencent.wcdb.orm.*;\n");
        writer.write("import com.tencent.wcdb.winq.Column;\n");
        writer.write("import com.tencent.wcdb.winq.ColumnConstraint;\n");
        writer.write("import com.tencent.wcdb.winq.ColumnDef;\n");
        writer.write("import com.tencent.wcdb.winq.ColumnType;\n");
        writer.write("import com.tencent.wcdb.winq.StatementCreateIndex;\n");
        writer.write("import com.tencent.wcdb.winq.TableConstraint;\n\n");
    }

    private void writeFields(BufferedWriter writer, String className, ArrayList<Element> fields) throws IOException {
        for(Element field : fields) {
            writer.write(TAB + "public static Field<" + className + "> " + field + ";\n");
        }
    }

    private void writeColumns(BufferedWriter writer, ArrayList<Element> fields) throws IOException {
        int fieldId = 1;
        for (Element field : fields){

            WCDBField config = field.getAnnotation(WCDBField.class);
            assert config != null;
            FieldORMInfo info = FieldORMInfo.allInfo.get(field.asType().toString());
            assert info != null;

            String columnName = config.columnName();
            if( columnName == null || columnName.length() == 0 ) {
                columnName = field.toString();
            }
            writer.write(TAB + TAB + field + " = new Field<>(\"" + columnName + "\", INSTANCE, " + fieldId + ", " + (config.isPrimary() && config.isAutoIncrement()) + ");\n");
            fieldId++;
            writer.write(TAB + TAB + "ColumnDef " + field + "Def = new ColumnDef(" + field + ", ColumnType." + info.columnType + ");\n");

            String constraintPrefix = TAB + TAB + field + "Def.constraint(new ColumnConstraint()";

            if(config.isPrimary()) {
                writer.write(constraintPrefix + ".primaryKey()" + (config.isAutoIncrement() ? ".autoIncrement());\n" : ");\n"));
            }

            if(config.defaultIntValue() != 0) {
                writer.write( constraintPrefix + ".defaultTo(" + config.defaultIntValue() + "));\n");
            } else if (config.defaultDoubleValue() != 0) {
                writer.write( constraintPrefix + ".defaultTo(" + config.defaultDoubleValue() + "));\n");
            } else if (config.defaultStringValue() != null && config.defaultStringValue().length() > 0) {
                writer.write( constraintPrefix + ".defaultTo(\"" + config.defaultStringValue() + "\"));\n");
            }

            if(config.isUnique()) {
                writer.write(constraintPrefix + ".unique()" + ");\n");
            }

            if(config.isNotNull()) {
                writer.write(constraintPrefix + ".notNull()" + ");\n");
            }

            writer.write(TAB + TAB + "baseBinding.addColumnDef(" + field + "Def);\n\n");

            WCDBIndex index = field.getAnnotation(WCDBIndex.class);
            if(index == null) {
                continue;
            }

            writer.write(TAB + TAB + "baseBinding.addIndex(\"" + index.nameSuffix() + "\", new StatementCreateIndex().ifNotExist()");
            if(index.isUnique()) {
                writer.write(".unique()");
            }
            writer.write(".indexedBy(" + field + "));\n\n");
        }
    }

    private void writeTableConfig(BufferedWriter writer, WCDBTableCoding config) throws IOException {
        if(config.multiIndexes() != null) {
            for(MultiIndexes indexes : config.multiIndexes()) {
                writer.write(TAB + TAB + "baseBinding.addIndex(\"" + indexes.nameSuffix()
                        + "\", new StatementCreateIndex().ifNotExist().indexedBy(new Column[]{");
                for(String column : indexes.columns()) {
                    writer.write(column + ", ");
                }
                writer.write("}));\n");
            }
        }

        if(config.multiPrimaries() != null) {
            for(MultiPrimary primaries : config.multiPrimaries()) {
                writer.write(TAB + TAB + "baseBinding.addTableConstraint(new TableConstraint().primaryKey().indexedBy(new Column[]{");
                for(String column : primaries.columns()) {
                    writer.write(column + ", ");
                }
                writer.write("}));\n");
            }
        }

        if(config.multiUnique() != null) {
            for(MultiUnique uniques : config.multiUnique()) {
                writer.write(TAB + TAB + "baseBinding.addTableConstraint(new TableConstraint().unique().indexedBy(new Column[]{");
                for(String column : uniques.columns()) {
                    writer.write(column + ", ");
                }
                writer.write("}));\n");
            }
        }
    }

    private void writeBindingType(BufferedWriter writer, String className) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public Class<" + className + "> bindingType() {\n");
        writer.write(TAB + TAB + "return " + className + ".class;\n");
        writer.write(TAB + "}\n\n");
    }

    private void writeBindingFields(BufferedWriter writer, String className, ArrayList<Element> fields) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public Field<" + className + ">[] allBindingFields() {\n");
        writer.write(TAB + TAB + "return new Field[]{");
        for (Element field : fields) {
            writer.write(field + ", ");
        }
        writer.write("};\n" + TAB + "}\n\n");

        writer.write(TAB + "public static Field<" + className + ">[] allFields() {\n");
        writer.write(TAB + TAB + "return new Field[]{");
        for (Element field : fields) {
            writer.write(field + ", ");
        }
        writer.write("};\n" + TAB + "}\n\n");
    }

    private void writeBaseBinding(BufferedWriter writer) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public Binding baseBinding() {\n");
        writer.write(TAB + TAB + "return baseBinding;\n");
        writer.write(TAB + "}\n\n");
    }

    private void writeExtractObject(BufferedWriter writer, String className, ArrayList<Element> fields) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public " + className + " extractObject(Field<" + className
                + ">[] fields, PreparedStatement preparedStatement) {\n");
        writer.write(TAB + TAB + className + " newOne = new " + className + "();\n");
        writer.write(TAB + TAB + "int index = 0;\n");
        writer.write(TAB + TAB + "for(Field<" + className + "> field : fields) {\n");
        writer.write(TAB + TAB + TAB + "switch (field.getFieldId()) {\n");
        int index = 1;
        for(Element field : fields) {
            FieldORMInfo info = FieldORMInfo.allInfo.get(field.asType().toString());
            assert info != null;

            writer.write(TAB + TAB + TAB + TAB + "case " + index + ":\n");
            writer.write(TAB + TAB + TAB + TAB + TAB + "newOne." + field + " = " + info.fieldSetter + "(index);\n");
            writer.write(TAB + TAB + TAB + TAB + TAB + "break;\n");

            index++;
        }

        writer.write(TAB + TAB + TAB + TAB + "default:\n");
        writer.write(TAB + TAB + TAB + TAB+ TAB + "assert false : \"Invalid id \" + field.getFieldId() + \" of field \" + field.getDescription() + \" in "
                + className + ".\";\n");
        writer.write(TAB + TAB + TAB + "}\n");
        writer.write(TAB + TAB + TAB + "index++;\n");
        writer.write(TAB + TAB + "}\n");
        writer.write(TAB + TAB + "return newOne;\n");
        writer.write(TAB + "}\n\n");
    }

    private void writeBindObject(BufferedWriter writer, String className, ArrayList<Element> fields) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public void bindField(" + className + " object, Field<" + className
                + "> field, int index, PreparedStatement preparedStatement) {\n");
        writer.write(TAB + TAB + "switch (field.getFieldId()) {\n");
        int index = 1;
        for(Element field : fields) {
            String type = field.asType().toString();
            FieldORMInfo info = FieldORMInfo.allInfo.get(type);
            assert info != null;

            writer.write(TAB + TAB + TAB + "case " + index + ":\n");
            if(type.startsWith("java.lang.")) {
                writer.write(TAB + TAB + TAB + TAB + "if ( object." + field + " != null ) {\n");
                writer.write(TAB + TAB + TAB + TAB + TAB + "preparedStatement." + info.fieldGetter + "(object." + field + ", index);\n");
                writer.write(TAB + TAB + TAB + TAB + "} else {\n");
                writer.write(TAB + TAB + TAB + TAB + TAB + "preparedStatement.bindNull(index);\n");
                writer.write(TAB + TAB + TAB + TAB + "}\n");
            } else {
                writer.write(TAB + TAB + TAB + TAB + "preparedStatement." + info.fieldGetter + "(object." + field + ", index);\n");
            }
            writer.write(TAB + TAB + TAB + TAB + "break;\n");

            index++;
        }

        writer.write(TAB + TAB + TAB + "default:\n");
        writer.write(TAB + TAB + TAB+ TAB + "assert false : \"Invalid id \" + field.getFieldId() + \" of field \" + field.getDescription() + \" in "
                + className + ".\";\n");
        writer.write(TAB + TAB + "}\n");
        writer.write(TAB + "}\n\n");
    }

    private void writeAutoIncrementConfig(BufferedWriter writer, String className) throws IOException {
        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public boolean isAutoIncrement(" + className + " object) {\n");
        if(autoIncrementElement != null) {
            writer.write(TAB + TAB + "return object." + autoIncrementElement + " == 0;\n");
        } else {
            writer.write(TAB + TAB + "return false;\n");
        }
        writer.write(TAB + "}\n\n");

        writer.write(TAB + "@Override\n");
        writer.write(TAB + "public void setLastInsertRowId(" + className + " object, long lastInsertRowId) {\n");
        if(autoIncrementElement != null) {
            writer.write(TAB + TAB + "object." + autoIncrementElement + " = (" + autoIncrementElement.asType().toString() + ") lastInsertRowId;\n");
        }
        writer.write(TAB + "}\n\n");
    }
}