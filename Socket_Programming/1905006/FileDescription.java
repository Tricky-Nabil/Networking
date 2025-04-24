package Offline_1;

import java.io.Serializable;

public class FileDescription implements Serializable {
    public int fileID;
    public String fileName;
    public Boolean fileType;
    public String filePath;
    public String clientName;


    FileDescription(){
        fileID = Server.fileID;
        Server.fileID++;
    }

    public void setClientName(String clientName) {
        this.clientName = clientName;
    }

    public String getClientName() {
        return clientName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileType(Boolean fileType) {
        this.fileType = fileType;
    }

    public Boolean getFileType() {
        return fileType;
    }

    public int getFileID() {
        return fileID;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public String getFilePath() {
        return filePath;
    }
}
