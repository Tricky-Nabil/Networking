package Offline_1;

import java.io.Serializable;

public class ClientInformation implements Serializable {
    String clientName;
    Boolean isActive;
    ClientInformation(String str , Boolean activeStatus){
        clientName = str;
        isActive = activeStatus;
    }

    public Boolean getActive() {
        return isActive;
    }

    public void setActive(Boolean active) {
        isActive = active;
    }


}
