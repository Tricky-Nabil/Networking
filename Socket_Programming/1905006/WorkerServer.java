package Offline_1;

import java.io.*;
import java.util.Date;
import java.util.Map;
import java.util.Random;

public class WorkerServer implements Runnable{
    private NetworkUtil networkUtil;
    public String clientName;
    private Thread thr;
    WorkerServer(NetworkUtil n , String str){
        networkUtil = n;
        clientName = str;
        thr = new Thread(this);
        thr.start();
    }

    @Override
    public void run() {
        ClientInformation clientInformation = null;

        if(Server.clientInformationMap.get(clientName) == null){
            clientInformation = new ClientInformation(clientName , (!networkUtil.getSocket().isClosed()));
            Server.clientInformationMap.put(clientName , clientInformation);
            try {
                networkUtil.write("Successful");
            } catch (IOException e) {
                e.printStackTrace();
            }
            String filePath = "src/ServerDirectories/" + clientName;
            File directory_for_unique_client = new File(filePath);
            Server.unreadMessagesMap.put(clientName,new Message(clientName));
            directory_for_unique_client.mkdirs();
            System.out.println("Directory created at " + filePath + " for client " + clientName);

        }
        else{
            if(Server.clientInformationMap.get(clientName).getActive()){
                try {
                    networkUtil.write("Unsuccessful");
                } catch (IOException e) {
                    e.printStackTrace();
                }
                System.out.println("User already exists");
                try {
                    networkUtil.getSocket().close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                return;
            }
            else{
                try {
                    networkUtil.write("Successful");
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Server.clientMap.put(clientName , networkUtil);
                Server.clientInformationMap.get(clientName).isActive = (!networkUtil.getSocket().isClosed());

            }
        }

        while(true) {
            String received_options_from_client = "";
            try {
                received_options_from_client = (String) networkUtil.read();
                if(received_options_from_client.equalsIgnoreCase("No_1")){
                    //System.out.println(Server.clientInformationMap.size());
                    for(var i : Server.clientInformationMap.keySet()){
                        //System.out.println("In Loop");
                        if(Server.clientInformationMap.get(i).getActive()){
                            networkUtil.write("Client " + i + " is online now");
                           // System.out.println("Client " + i + " is online now");
                        }
                        else{
                            networkUtil.write("Client " + i + " is offline now");
                        }
                    }
                    networkUtil.write("NO_MORE_CLIENTS_LEFT");
                }
                else if(received_options_from_client.equalsIgnoreCase("No_2")){
                    for(var i : Server.fileMap.keySet()){
                        var temp_file_description = Server.fileMap.get(i);
                        if(temp_file_description.getClientName().equalsIgnoreCase(clientName) && temp_file_description.fileType){
                           networkUtil.write("File ID : " + temp_file_description.fileID + "-- File Name : " + temp_file_description.fileName + "-- Type - Public");
                        }
                        else if(temp_file_description.getClientName().equalsIgnoreCase(clientName) && (!temp_file_description.fileType)){
                            networkUtil.write("File ID : " + temp_file_description.fileID + "-- File Name : " + temp_file_description.fileName + "-- Type - Private");
                        }
                    }
                    networkUtil.write("NO_MORE_FILES_LEFT");
                    networkUtil.write("Do you want to download any file?");
                    var download_file_status = (String) networkUtil.read();
                    if(download_file_status.equalsIgnoreCase("NO"))
                        continue;

                    networkUtil.write("How many files do you wanna download");
                    var number_of_file_to_be_downloaded = (Integer)networkUtil.read();
                    for(int i = 0 ; i  < number_of_file_to_be_downloaded ; i++){
                        server_sendFile();
                    }

                    //server_sendFile();

                }
                else if(received_options_from_client.equalsIgnoreCase("No_3")){
                    for(var i : Server.fileMap.keySet()){
                        var temp_file_description = Server.fileMap.get(i);
                        if((!temp_file_description.getClientName().equalsIgnoreCase(clientName)) && temp_file_description.fileType){
                            networkUtil.write("File ID : " + temp_file_description.fileID + "-- File Name : " + temp_file_description.fileName + "-- Type - Public");
                        }
                    }
                    networkUtil.write("NO_MORE_PUBLIC_FILES_LEFT");
                    networkUtil.write("Do you want to download any file?");
                    var download_file_status = (String) networkUtil.read();
                    if(download_file_status.equalsIgnoreCase("NO"))
                        continue;

                    networkUtil.write("How many files do you wanna download");
                    var number_of_file_to_be_downloaded = (Integer)networkUtil.read();
                    for(int i = 0 ; i  < number_of_file_to_be_downloaded ; i++){
                        server_sendFile();
                    }


                    //server_sendFile();
                }
                else if(received_options_from_client.equalsIgnoreCase("No_4")){
                    networkUtil.write("Give the file description");
                    var str_fileDescription = (String)networkUtil.read();
                    Request request = new Request();
                    request.client_who_requested = clientName;
                    request.fileDescription = str_fileDescription;
                    Server.requestMap.put(request.reqID, request);
                    for(var i : Server.unreadMessagesMap.keySet()){
                        String str = "Request ID : " + request.reqID + "\nRequested By : " + clientName + "\nFile Description : " + str_fileDescription + "\nTime : " + new Date();
                        Server.unreadMessagesMap.get(i).getUnreadMessages().add(str);
                    }
                }
                else if(received_options_from_client.equalsIgnoreCase("No_5")){
                    var unread_messages = Server.unreadMessagesMap.get(clientName);
                    var str = unread_messages.getUnreadMessages();
                    for(var i : str){
                        networkUtil.write(i);
                    }
                    Server.unreadMessagesMap.get(clientName).getUnreadMessages().clear();
                    networkUtil.write("NO_UNREAD_MESSAGE_LEFT");

                }
                else if(received_options_from_client.equalsIgnoreCase("No_6")){
                    networkUtil.write("Give the file name you want to upload");
                    String right_file_given = (String) networkUtil.read();
                    //System.out.println(right_file_given);
                    if(right_file_given.equalsIgnoreCase("Wrong"))
                        continue;

                    String file_name = (String) networkUtil.read();
                    networkUtil.write("Do you wanna make it public or private?");
                    String file_type = (String) networkUtil.read();
                    server_receiveFile(file_name, clientName, file_type, -1);
                }
                else if(received_options_from_client.equalsIgnoreCase("No_7")){
                    networkUtil.write("Give me the request Id");
                    Integer reqId = (Integer) networkUtil.read();
                    if(Server.requestMap.get(reqId) == null){
                        networkUtil.write(false);
                        continue;
                    }
                    networkUtil.write(true);
                    networkUtil.write("Give me the file name you want to upload");
                    String right_file_given = (String) networkUtil.read();
                    //System.out.println(right_file_given);
                    if(right_file_given.equalsIgnoreCase("Wrong"))
                        continue;
                    String file_name = (String) networkUtil.read();
                    server_receiveFile(file_name, clientName, "Public", reqId);

                }
                else if(received_options_from_client.equalsIgnoreCase("No_8")){
                    networkUtil.write("Logout Successful");
                    Server.clientInformationMap.get(clientName).setActive(false);
                    Server.clientMap.get(clientName).getSocket().close();

                }
                else{
                    //Bla Bla Bla
                }
            } catch (Exception e) { }
        }
    }

    private void server_sendFile(){
        try {
            var file_id = (Integer)networkUtil.read();
            if(Server.fileMap.get(file_id) == null){
                networkUtil.write(false);
                return;
            }
            networkUtil.write(true);
            var fileDescription = Server.fileMap.get(file_id);

            networkUtil.write(fileDescription.getFileName());

            FileInputStream fileInputStream = new FileInputStream(new File(fileDescription.getFilePath()));
            BufferedInputStream bufferedInputStream = new BufferedInputStream(fileInputStream);
            byte[] chunk = new byte[Server.MAX_CHUNK_SIZE];
            int bytesRead;
            int chunkNumber = 1;

            while ((bytesRead = bufferedInputStream.read(chunk)) != -1){
                System.out.println(bytesRead + " bytes read");
                networkUtil.writeInVast(chunk, 0, bytesRead);
                networkUtil.flush();
                System.out.println(chunkNumber + "no chunk is sent successfully!!");
                chunkNumber++;
            }

            networkUtil.write("File sent successfully!!");
            System.out.println("File sent successfully!!");

        } catch (IOException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    private void server_receiveFile(String file_name, String clientName, String file_type, int reqID){
        try{
            networkUtil.write("Give me the size of the file you want to upload");
            long fileSize = (long) networkUtil.read();
            System.out.println("File Size : " + fileSize);
            if(Server.used_buffer_size + fileSize > Server.MAX_BUFFER_SIZE){
                networkUtil.write("BUFFER_SIZE_OVERFLOWED");
                System.out.println("Not enough space in buffer");
                return;
            }
            else{
                Server.used_buffer_size += fileSize;
                networkUtil.write("Okay");
            }
            Random rand = new Random();
            int chunkSize = rand.nextInt( Server.MAX_CHUNK_SIZE - Server.MIN_CHUNK_SIZE + 1) + Server.MIN_CHUNK_SIZE;
            networkUtil.write(chunkSize);
            int chunkNumber = 1;
            byte[] buffer = new byte[chunkSize];
            int bytesRead;

            String file_path = "src/ServerDirectories/" + clientName + "/" + file_name;
            File file = new File(file_path);
            if(file.exists()){
                int checkIndex = -1;
                for(var keys : Server.fileMap.keySet()){
                    if(Server.fileMap.get(keys).fileName.equals(file_name) && Server.fileMap.get(keys).getClientName().equals(clientName)){
                        checkIndex = keys;
                        break;
                    }
                }
                if(checkIndex != -1){
                    Server.fileMap.remove(checkIndex);
                }
                file.delete();
            }
            FileOutputStream fileOutputStream = new FileOutputStream(file_path);
            BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(new FileOutputStream(file_path));
            int totalBytesRead = 0;
            while ((bytesRead = networkUtil.readByte(buffer)) != -1) {
                totalBytesRead += bytesRead;
                bufferedOutputStream.write(buffer, 0, bytesRead);
                bufferedOutputStream.flush();
                System.out.println(chunkNumber + " no chunk received from server side\n" + bytesRead + " bytes received");
                chunkNumber++;
                //Thread.sleep(40000);
                networkUtil.write("CHUNKS_RECEIVED");
            }



            String upload_status = (String) networkUtil.read();
            if(upload_status.equalsIgnoreCase("FINISHED")){
                if(totalBytesRead == fileSize){
                    networkUtil.write(true);
                    System.out.println("File Uploaded Successfully!!");
                    bufferedOutputStream.close();
                    Server.used_buffer_size -= totalBytesRead;

                    FileDescription fileDescription = new FileDescription();
                    fileDescription.setClientName(clientName);
                    fileDescription.setFileName(file_name);
                    fileDescription.setFilePath(file_path);
                    if(file_type.equalsIgnoreCase("public"))
                        fileDescription.setFileType(true);
                    else if(file_type.equalsIgnoreCase("private"))
                        fileDescription.setFileType(false);
                    Server.fileMap.put(fileDescription.getFileID(),fileDescription);
                    if(reqID != -1){
                        Request request = Server.requestMap.get(reqID);
                        request.setClient_who_uploaded(clientName);
                        request.setUploaded(true);
                        String str = "Requested ID : " + request.reqID + "\nUploaded By : " + clientName + "\nFile name : " + file_name + "\nTime : " + new Date();
                        Server.unreadMessagesMap.get(request.getClient_who_requested()).getUnreadMessages().add(str);
                    }
                }
                else{
                    System.out.println("Upload operation failed!!");
                    networkUtil.write(false);
                    file = new File(file_path);
                    file.delete();
                }
            }
            else{

                System.out.println(upload_status);
                file = new File(file_path);
                file.delete();
                System.out.println("Sorry client " + clientName + "\nYour upload is failed!!");
            }

        }
        catch(Exception e){

        }

    }
}
