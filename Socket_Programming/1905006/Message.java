package Offline_1;

import java.io.Serializable;
import java.util.ArrayList;

public class Message implements Serializable {
    String clientName;
    ArrayList<String> unreadMessages;

    Message(String str){
        clientName  =str;
        unreadMessages = new ArrayList<>();
    }

    public ArrayList<String> getUnreadMessages() {
        return unreadMessages;
    }

}
