package com.tencent.wcdbtest;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;


public class MyAdapter extends RecyclerView.Adapter<MyAdapter.MyViewHolder> {
    private List<String> mData;

    public MyAdapter(List<String> data) {
        mData = data;
    }


    public void insertItem(String name) {
        mData.add(name);
        notifyDataSetChanged();
    }

    public void refreshData(List<String> dataList) {
        mData.clear();
        mData.addAll(dataList);
        notifyDataSetChanged();
    }

    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        // 创建新的视图，由 XML 布局文件创建
        View itemView = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_list, parent, false);
        return new MyViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(MyViewHolder holder, int position) {
        // 将数据绑定到视图上
        String item = mData.get(position);
        holder.textViewMsgContent.setText(item);
    }

    @Override
    public int getItemViewType(int position) {
        return 2;
    }

    @Override
    public int getItemCount() {
        // 数据项的数量
        return mData.size();
    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {
        public TextView textViewId;
        public TextView textViewMsgUuid;
        public TextView textViewMsgContent;

        public MyViewHolder(View itemView) {
            super(itemView);
            textViewMsgContent = itemView.findViewById(R.id.itemMsgContent);
        }
    }
}