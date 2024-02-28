package com.example.wcdb.sample.persistence.entity;

import android.arch.persistence.room.Entity;
import android.arch.persistence.room.PrimaryKey;

/**
 * Created by johnwhe on 2017/7/12.
 */

@Entity
public class User {
    @PrimaryKey(autoGenerate = true)
    public int userId;

    public String firstName;
    public String lastName;
}
