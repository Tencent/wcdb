package com.tencent.wcdbtest.orm.testclass;

import com.tencent.wcdbtest.base.RandomTool;

import java.util.Random;

public class AllTypeObjectHelper {
    static public AllTypeObject maxObject() {
        AllTypeObject max = new AllTypeObject();
        max.type = "max";
        max.aBoolean = true;
        max.aBoolean2 = true;
        max.aByte = Byte.MAX_VALUE;
        max.aByte2 = Byte.MAX_VALUE;
        max.aChar = Character.MAX_VALUE;
        max.character = Character.MAX_VALUE;
        max.aShort = Short.MAX_VALUE;
        max.aShort2 = Short.MAX_VALUE;
        max.anInt = Integer.MAX_VALUE;
        max.integer = Integer.MAX_VALUE;
        max.aLong = Long.MAX_VALUE;
        max.aLong2 = Long.MAX_VALUE;
        max.aFloat = Float.MAX_VALUE;
        max.aFloat2 = Float.MAX_VALUE;
        max.aDouble = Double.MAX_VALUE;
        max.aDouble2 = Double.MAX_VALUE;
        max.string = RandomTool.string();
        max.bytes = RandomTool.bytes();
        return max;
    }

    static public AllTypeObject minObject() {
        AllTypeObject min = new AllTypeObject();
        min.type = "min";
        min.aBoolean = false;
        min.aBoolean2 = false;
        min.aByte = Byte.MIN_VALUE;
        min.aByte2 = Byte.MIN_VALUE;
        min.aChar = Character.MIN_VALUE;
        min.character = Character.MIN_VALUE;
        min.aShort = Short.MIN_VALUE;
        min.aShort2 = Short.MIN_VALUE;
        min.anInt = Integer.MIN_VALUE;
        min.integer = Integer.MIN_VALUE;
        min.aLong = Long.MIN_VALUE;
        min.aLong2 = Long.MIN_VALUE;
        min.aFloat = Float.MIN_VALUE;
        min.aFloat2 = Float.MIN_VALUE;
        min.aDouble = Double.MIN_VALUE;
        min.aDouble2 = Double.MIN_VALUE;
        min.string = RandomTool.string();
        min.bytes = RandomTool.bytes();
        return min;
    }

    static public AllTypeObject randomObject() {
        Random random = new Random();
        AllTypeObject object = new AllTypeObject();
        object.type = "random";
        object.aBoolean = random.nextBoolean();
        object.aBoolean2 = random.nextBoolean();
        object.aByte = (byte) random.nextInt();
        object.aByte2 = (byte) random.nextInt();
        object.aChar = (char) random.nextInt();
        object.character = (char) random.nextInt();
        object.aShort = (short) random.nextInt();
        object.aShort2 = (short) random.nextInt();
        object.anInt = random.nextInt();
        object.integer = random.nextInt();
        object.aLong = random.nextLong();
        object.aLong2 = random.nextLong();
        object.aFloat = random.nextFloat();
        object.aFloat2 = random.nextFloat();
        object.aDouble = random.nextDouble();
        object.aDouble2 = random.nextDouble();
        object.string = RandomTool.string();
        object.bytes = RandomTool.bytes();
        return object;
    }

    static public AllTypeObject emptyObject() {
        AllTypeObject empty = new AllTypeObject();
        empty.type = "empty";
        return empty;
    }
}
