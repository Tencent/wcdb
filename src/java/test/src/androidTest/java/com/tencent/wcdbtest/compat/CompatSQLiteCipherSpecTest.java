package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import com.tencent.wcdb.compat.SQLiteCipherSpec;

import org.junit.Test;

public class CompatSQLiteCipherSpecTest {

    @Test
    public void testDefaultValues() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        assertEquals(0, spec.kdfIteration);
        assertTrue(spec.hmacEnabled);
        assertEquals(SQLiteCipherSpec.HMAC_DEFAULT, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_DEFAULT, spec.kdfAlgorithm);
        assertEquals(4096, spec.pageSize);
    }

    @Test
    public void testCopyConstructor() {
        SQLiteCipherSpec original = new SQLiteCipherSpec();
        original.kdfIteration = 64000;
        original.hmacEnabled = false;
        original.hmacAlgorithm = SQLiteCipherSpec.HMAC_SHA256;
        original.kdfAlgorithm = SQLiteCipherSpec.HMAC_SHA512;
        original.pageSize = 1024;

        SQLiteCipherSpec copy = new SQLiteCipherSpec(original);
        assertEquals(64000, copy.kdfIteration);
        assertFalse(copy.hmacEnabled);
        assertEquals(SQLiteCipherSpec.HMAC_SHA256, copy.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA512, copy.kdfAlgorithm);
        assertEquals(1024, copy.pageSize);
    }

    @Test
    public void testSetKDFIteration() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        SQLiteCipherSpec result = spec.setKDFIteration(64000);
        assertSame(spec, result);
        assertEquals(64000, spec.kdfIteration);
    }

    @Test
    public void testWithHMACEnabled() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        SQLiteCipherSpec result = spec.withHMACEnabled(false);
        assertSame(spec, result);
        assertFalse(spec.hmacEnabled);
    }

    @Test
    public void testSetHmacAlgorithm() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        SQLiteCipherSpec result = spec.setHmacAlgorithm(SQLiteCipherSpec.HMAC_SHA256);
        assertSame(spec, result);
        assertEquals(SQLiteCipherSpec.HMAC_SHA256, spec.hmacAlgorithm);
    }

    @Test
    public void testSetKdfAlgorithm() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        SQLiteCipherSpec result = spec.setKdfAlgorithm(SQLiteCipherSpec.HMAC_SHA512);
        assertSame(spec, result);
        assertEquals(SQLiteCipherSpec.HMAC_SHA512, spec.kdfAlgorithm);
    }

    @Test
    public void testSetPageSize() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        SQLiteCipherSpec result = spec.setPageSize(1024);
        assertSame(spec, result);
        assertEquals(1024, spec.pageSize);
    }

    @Test
    public void testSetSQLCipherVersion1() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        spec.setSQLCipherVersion(1);
        assertFalse(spec.hmacEnabled);
        assertEquals(4000, spec.kdfIteration);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.kdfAlgorithm);
    }

    @Test
    public void testSetSQLCipherVersion2() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        spec.setSQLCipherVersion(2);
        assertTrue(spec.hmacEnabled);
        assertEquals(4000, spec.kdfIteration);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.kdfAlgorithm);
    }

    @Test
    public void testSetSQLCipherVersion3() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        spec.setSQLCipherVersion(3);
        assertTrue(spec.hmacEnabled);
        assertEquals(64000, spec.kdfIteration);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.kdfAlgorithm);
    }

    @Test
    public void testSetSQLCipherVersion4() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec();
        spec.setSQLCipherVersion(4);
        assertTrue(spec.hmacEnabled);
        assertEquals(256000, spec.kdfIteration);
        assertEquals(SQLiteCipherSpec.HMAC_SHA512, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA512, spec.kdfAlgorithm);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetSQLCipherVersionInvalid() {
        new SQLiteCipherSpec().setSQLCipherVersion(5);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetSQLCipherVersionZero() {
        new SQLiteCipherSpec().setSQLCipherVersion(0);
    }

    @Test
    public void testChaining() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec()
                .setKDFIteration(4000)
                .withHMACEnabled(false)
                .setHmacAlgorithm(SQLiteCipherSpec.HMAC_SHA1)
                .setKdfAlgorithm(SQLiteCipherSpec.HMAC_SHA1)
                .setPageSize(1024);

        assertEquals(4000, spec.kdfIteration);
        assertFalse(spec.hmacEnabled);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.hmacAlgorithm);
        assertEquals(SQLiteCipherSpec.HMAC_SHA1, spec.kdfAlgorithm);
        assertEquals(1024, spec.pageSize);
    }

    @Test
    public void testHmacConstants() {
        assertEquals(-1, SQLiteCipherSpec.HMAC_DEFAULT);
        assertEquals(0, SQLiteCipherSpec.HMAC_SHA1);
        assertEquals(1, SQLiteCipherSpec.HMAC_SHA256);
        assertEquals(2, SQLiteCipherSpec.HMAC_SHA512);
    }

    @Test
    public void testVersionOverwritesPreviousSettings() {
        SQLiteCipherSpec spec = new SQLiteCipherSpec()
                .setKDFIteration(100)
                .withHMACEnabled(false);
        assertEquals(100, spec.kdfIteration);
        assertFalse(spec.hmacEnabled);

        spec.setSQLCipherVersion(4);
        assertEquals(256000, spec.kdfIteration);
        assertTrue(spec.hmacEnabled);
    }
}
