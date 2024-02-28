package com.example.wcdb.sample.persistence.entity;

import android.arch.persistence.room.Dao;
import android.arch.persistence.room.Delete;
import android.arch.persistence.room.Insert;
import android.arch.persistence.room.Query;
import android.arch.persistence.room.Update;

import java.util.List;

/**
 * Created by johnwhe on 2017/7/12.
 */

@Dao
public interface UserDao {
    @Query("SELECT * FROM user")
    List<User> getAll();

    @Query("SELECT * FROM user WHERE userId = :userId")
    User getById(int userId);

    @Insert
    void insert(User... users);

    @Delete
    void delete(User user);

    @Update
    void update(User user);
}
