package com.tencent.wechat_test.model;
import com.tencent.wcdb.*;

@WCDBTableCoding(multiIndexes = @MultiIndexes(name = "video_status_index", columns = {"status", "downloadtime"}))
public class VideoInfo {
    @WCDBField(isPrimary = true)
    public String filename;
    @WCDBField
    public String clientid;
    @WCDBField
    public int msgsvrid;
    @WCDBField
    public int netoffset;
    @WCDBField
    public int filenowsize;
    @WCDBField
    public int totallen;
    @WCDBField
    public int thumbnetoffset;
    @WCDBField
    public int thumblen;
    @WCDBField
    public int status;
    @WCDBField
    public long createtime;
    @WCDBField
    @WCDBIndex(name = "LastModifyTimeIndex")
    public long lastmodifytime;
    @WCDBField
    public long downloadtime;
    @WCDBField
    public int videolength;
    @WCDBField
    public int msglocalid;
    @WCDBField
    public int nettimes;
    @WCDBField
    public int cameratype;
    @WCDBField
    public String user;
    @WCDBField
    public String human;
    @WCDBField
    public int reserved1;
    @WCDBField
    public int reserved2;
    @WCDBField
    public String reserved3;
    @WCDBField
    public String reserved4;
    @WCDBField
    public int videofuncflag;
    @WCDBField
    @WCDBIndex(name = "massSendIdIndex")
    public long masssendid;
    @WCDBField
    public String masssendlist;
    @WCDBField
    public String videomd5;
    @WCDBField
    public byte[] streamvideo;
    @WCDBField
    public String statextstr;
    @WCDBField
    public int downloadscene;
    @WCDBField
    public byte[] mmsightextinfo;
    @WCDBField
    public int preloadsize;
    @WCDBField
    public int videoformat;
    @WCDBField
    public int forward_msg_local_id;
    @WCDBField
    public String msg_uuid;
    @WCDBField
    public String share_app_info;
    @WCDBField
    public String origin_file_name;
    @WCDBField
    public int had_clicked_video;
    @WCDBField
    @WCDBIndex(name = "videoinfo2_MediaIdIndex")
    public String media_id;
    @WCDBField
    public String media_flag;
}