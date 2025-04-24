package Offline_1;

import java.io.Serializable;

public class Request implements Serializable {
    public String client_who_requested;
    public String client_who_uploaded;
    public String fileDescription;
    public int reqID;

    public Boolean isUploaded;

    public Request(){
        reqID = Server.reqID;
        Server.reqID++;
        isUploaded = false;
    }


    public String getClient_who_requested() {
        return client_who_requested;
    }

    public void setClient_who_uploaded(String client_who_uploaded) {
        this.client_who_uploaded = client_who_uploaded;
    }

    public void setUploaded(Boolean uploaded) {
        isUploaded = uploaded;
    }


}
